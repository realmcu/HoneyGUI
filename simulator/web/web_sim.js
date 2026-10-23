/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * Browser host for the HoneyGUI web simulator.
 *
 * Loaded with a plain <script> tag (no ES module), so the page also works
 * when opened from disk (file://). Defines the global startHoneyGUI().
 * The page must be UTF-8 (<meta charset="UTF-8">): gui.js embeds the wasm
 * as UTF-8 text.
 *
 * Usage:
 *   const sim = await startHoneyGUI({
 *       createModule: createHoneyGUI,   // factory from gui.js
 *       canvas: document.querySelector('canvas'),
 *       files: { 'example/foo.bin': uint8Array },  // written to MEMFS before start
 *       screen: { width: 480, height: 320, pixelBits: 32, cornerRadius: 40 },
 *       // Only for an engine built with WEB_SIM_MAIN_MODULE=ON:
 *       project: uint8Array,   // a project compiled as -sSIDE_MODULE
 *       // Or, for an engine built with the HML runtime, run designer HML
 *       // with no compiled project. dir is relative to the working
 *       // directory and holds the converted assets with the HML in ui/;
 *       // startView (optional) is shown instead of the entry view:
 *       hml: { dir: 'assets', startView: '' },
 *   });
 *   sim.reloadHml();   // after rewriting files under hml.dir
 *   sim.stop();
 *
 * Files are written relative to the module's working directory, which is
 * what the POSIX VFS mount "/pc" -> "." resolves against (same layout as
 * running simulator/win32 from the repository root).
 */

/* Touch event codes from realgui/input/tp_algo.h */
const TOUCH_UP = 1;
const TOUCH_DOWN = 2;
/* Wheel event codes from realgui/core/def_input.h */
const WHEEL_SCROLL = 3;

const KEY_CODES = { Home: 0, Escape: 1, Backspace: 1, KeyM: 2, KeyP: 3 };

function mkdirp(FS, dir) {
    let current = '';
    for (const part of dir.split('/').filter(Boolean)) {
        current += '/' + part;
        try {
            FS.mkdir(current);
        } catch (e) {
            /* already exists */
        }
    }
}

async function startHoneyGUI({ createModule, canvas, files = {}, screen, project, hml, print, printErr }) {
    const module = await createModule({
        print: print || (text => console.log(text)),
        printErr: printErr || (text => console.error(text)),
        locateFile: (path, prefix) => prefix + path,
    });

    const { FS } = module;
    const cwd = FS.cwd();
    for (const [relPath, data] of Object.entries(files)) {
        const full = `${cwd}/${relPath}`.replace(/\/+/g, '/');
        mkdirp(FS, full.slice(0, full.lastIndexOf('/')));
        FS.writeFile(full, data);
    }

    if (screen) {
        module._web_sim_configure(screen.width, screen.height, screen.pixelBits, screen.cornerRadius || 0);
    }

    if (project) {
        /* The project registers its init functions from constructors, so it
         * must be loaded before main() runs the engine. */
        FS.writeFile('/project.wasm', project);
        await module.loadDynamicLibrary('/project.wasm', { loadAsync: true, global: true, nodelete: true });
    } else if (hml) {
        if (typeof module._web_sim_load_hml !== 'function') {
            throw new Error('This engine was built without the HML runtime (CONFIG_REALTEK_BUILD_XML_LOADER)');
        }
        module.ccall('web_sim_load_hml', null, ['string', 'string'], [hml.dir, hml.startView || '']);
    }

    const width = module._web_sim_screen_width();
    const height = module._web_sim_screen_height();
    canvas.width = width;
    canvas.height = height;
    const ctx = canvas.getContext('2d');
    const image = ctx.createImageData(width, height);

    let running = true;
    let pressed = false;

    /* Round the canvas like the desktop simulator's screen mask. The radius is
     * in screen pixels, so it is given as a percentage to follow CSS scaling. */
    const cornerRadius = module._web_sim_screen_corner_radius();
    if (cornerRadius > 0) {
        canvas.style.borderRadius = `${cornerRadius * 100 / width}% / ${cornerRadius * 100 / height}%`;
    }

    /* Clamped: with pointer capture, events keep arriving outside the canvas. */
    const clamp = (v, max) => Math.min(Math.max(v, 0), max - 1);
    const toScreen = event => {
        const rect = canvas.getBoundingClientRect();
        return [
            clamp(Math.floor((event.clientX - rect.left) * width / rect.width), width),
            clamp(Math.floor((event.clientY - rect.top) * height / rect.height), height),
        ];
    };
    const onPointerDown = event => {
        pressed = true;
        canvas.setPointerCapture(event.pointerId);
        module._web_sim_touch(TOUCH_DOWN, ...toScreen(event));
    };
    const onPointerMove = event => {
        if (pressed) {
            module._web_sim_touch(0, ...toScreen(event));
        }
    };
    const onPointerUp = event => {
        if (pressed) {
            pressed = false;
            module._web_sim_touch(TOUCH_UP, ...toScreen(event));
        }
    };
    const onWheel = event => {
        event.preventDefault();
        module._web_sim_wheel(WHEEL_SCROLL, event.deltaY < 0 ? 1 : -1);
    };
    const onKey = down => event => {
        const key = KEY_CODES[event.code];
        if (key !== undefined) {
            event.preventDefault();
            module._web_sim_key(key, down ? 1 : 0);
        }
    };
    const onKeyDown = onKey(true);
    const onKeyUp = onKey(false);

    canvas.addEventListener('pointerdown', onPointerDown);
    canvas.addEventListener('pointermove', onPointerMove);
    canvas.addEventListener('pointerup', onPointerUp);
    canvas.addEventListener('pointercancel', onPointerUp);
    canvas.addEventListener('wheel', onWheel, { passive: false });
    canvas.tabIndex = 0;
    canvas.addEventListener('keydown', onKeyDown);
    canvas.addEventListener('keyup', onKeyUp);

    /* The engine writes converted RGBA pixels into a buffer in WASM memory;
     * copy it to the canvas once per animation frame. */
    const paint = () => {
        if (!running) {
            return;
        }
        const ptr = module._web_sim_screen_buffer();
        if (ptr) {
            image.data.set(module.HEAPU8.subarray(ptr, ptr + width * height * 4));
            ctx.putImageData(image, 0, 0);
        }
        requestAnimationFrame(paint);
    };

    /* main() runs the GUI server loop, which yields through Asyncify and
     * never returns while the simulator is running. */
    module.callMain([]);
    requestAnimationFrame(paint);

    return {
        module,
        width,
        height,
        /* HML runs only: rebuild the current view from the files on the next frame. */
        reloadHml() {
            module._web_sim_reload_hml?.();
        },
        currentView() {
            return module._web_sim_current_view ? module.UTF8ToString(module._web_sim_current_view()) : '';
        },
        stop() {
            running = false;
            canvas.removeEventListener('pointerdown', onPointerDown);
            canvas.removeEventListener('pointermove', onPointerMove);
            canvas.removeEventListener('pointerup', onPointerUp);
            canvas.removeEventListener('pointercancel', onPointerUp);
            canvas.removeEventListener('wheel', onWheel);
            canvas.removeEventListener('keydown', onKeyDown);
            canvas.removeEventListener('keyup', onKeyUp);
            try {
                module.abort?.('stopped');
            } catch (e) {
                /* abort() throws by design */
            }
        },
    };
}

globalThis.startHoneyGUI = startHoneyGUI;
