.. _Browser_Simulation:

==================
Browser Simulation
==================

Compile the HoneyGUI engine to WebAssembly and run it in a browser. No compiler to install, no board: open a web page and the UI is there.

Advantages
----------

**No compiling C code**

The engine contains the HML runtime and parses the designer's ``ui/*.xml`` directly. Changing the UI compiles nothing; save and the result is on screen.

**Nothing for the user to install**

A browser is all it takes. The engine can be prebuilt and shipped with a tool or a website, so users need no MinGW, Python, SCons or SDL2.

**Identical on every platform**

Windows, Linux and macOS load the same prebuilt wasm, so no "works on my machine" differences.

**Cheap to distribute**

A ``gui.js`` plus one asset file on a static server, with no toolchain download for the user.

**Embeddable in any page**

``startHoneyGUI()`` puts a GUI preview window inside your own page, documentation or VSCode extension.

Limitations
-----------

- No debugger: no breakpoints, no single stepping
- Project C code (callbacks, business functions) does not run in preview mode
- Single-threaded cooperative scheduling, slower than native
- No LetterShell: no console for commands that change runtime data

  The desktop simulator comes with a command line where an application registers its own commands, so parameters can be changed at run time without a rebuild. A browser target has no equivalent mechanism, so there is no terminal in the browser simulator.

So the browser simulator is for looking at the UI, demoing it, and handing the preview to someone else; debug C code with the desktop simulator.

Quick start
-----------

emsdk must already be installed and activated, otherwise ``emcmake`` does not exist. See :ref:`Install emsdk`.

The reason it is needed: the repository ships no prebuilt engine, so ``gui.js`` has to be built locally. Only the `HoneyDesigner <https://realmcu.github.io/HoneyDesigner/>`_ online site carries a prebuilt one, which works without touching the local environment.

In that activated shell, run from the repository root:

.. code-block:: shell

   > emcmake cmake -S simulator/web -B simulator/web/build -G Ninja
   > cmake --build simulator/web/build
   > python simulator/web/pack_files.py

Then double-click ``simulator/web/index.html``. That is it.

This uses the checked-in default configuration, which builds the engine and the HML runtime with **no demo application**, so the page opens showing an empty screen. That is the engine the designer preview runs on; to see a compiled demo instead, select one in `Choosing the demo application`_. Previewing an exported designer project is in `Designer UI Preview`_; embedding in your own page is in `Embed in Your Own Page`_.

.. _Install emsdk:

Build and Run
-------------

Setup
~~~~~

What you need: Python, CMake 3.13+, Ninja, and emsdk.

Python comes with :ref:`Get Started`; install CMake and Ninja from your package manager, or use the ones emsdk ships. Only emsdk needs a separate download:

1. Download emsdk: `https://github.com/emscripten-core/emsdk <https://github.com/emscripten-core/emsdk>`_
2. Install and activate:

.. code-block:: shell

   > cd emsdk
   > emsdk.bat install latest      :: Linux/macOS: ./emsdk install latest
   > emsdk.bat activate latest     :: Linux/macOS: ./emsdk activate latest
   > emsdk_env.bat                 :: Linux/macOS: source ./emsdk_env.sh

3. Check it:

.. code-block:: shell

   > emcc --version

Run ``emsdk_env`` again in every new shell.

.. note::

   Windows uses ``emsdk.bat`` / ``emsdk_env.bat``; Linux and macOS use ``./emsdk`` / ``source emsdk_env.sh``. The commands below use the Windows form; on Linux/macOS just replace ``>`` with ``$``.

Build
~~~~~

From the repository root:

.. code-block:: shell

   > emcmake cmake -S simulator/web -B simulator/web/build -G Ninja
   > cmake --build simulator/web/build

The output is in ``simulator/web/build/``, where ``gui.js`` is the engine.

.. note::

   Use ``emcmake cmake``, not plain ``cmake``; the latter fails with "must be configured with the Emscripten toolchain".

Choosing the demo application
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``Kconfig.gui`` is shared with the desktop simulator:

.. code-block:: shell

   > cd simulator/web
   > menuconfig ../../Kconfig.gui

The generated ``.config`` decides which widgets the engine contains and which demo application, if any, is built in. The default ``.config`` is checked in and builds all widgets plus the HML runtime with no demo application: the page opens on an empty screen, which is what the designer preview runs on.

A demo application is one Kconfig choice away. Any of the CI configurations selects one, for example the 466x466 demo:

.. code-block:: shell

   > emcmake cmake -S simulator/web -B simulator/web/build -G Ninja \
       -DWEB_SIM_CONFIG=example/ci_build_scripts/configs/application/.config_realtek_build_gui_466_466_demo
   > cmake --build simulator/web/build

A demo reads its resources from a romfs image, so ``simulator/web/files.json`` has to list that image before ``pack_files.py`` is run. It is empty by default.

Rebuilding after ``menuconfig`` is the same command, since the default ``.config`` is read automatically:

.. code-block:: shell

   > emcmake cmake -S simulator/web -B simulator/web/build -G Ninja
   > cmake --build simulator/web/build

Opening it in a browser
~~~~~~~~~~~~~~~~~~~~~~~

``index.html`` loads the engine through ``<script>`` tags, so double-clicking it works; no HTTP server needed. Assets are packed into a ``files.js``:

.. code-block:: shell

   > python simulator/web/pack_files.py

Then double-click ``simulator/web/index.html``.

With the default configuration there is no demo, so the canvas stays empty and the page reports the screen size; that is the expected result, not a failure. The page is useful once a demo is selected or a project is loaded (see `Designer UI Preview`_).

In the window: drag acts as a swipe, click as a touch, the wheel scrolls lists, and the Home / Back / Menu / Power buttons act as hardware keys.

.. note::

   ``files.js`` is packed from the files listed in ``files.json``, which is empty by default. A selected demo needs its romfs image listed there; update ``files.json`` and re-run ``pack_files.py`` after changing the demo. ``files.js`` is not checked in.

Packaging a distributable engine
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To ship the preview inside a tool so that users need no emsdk, prebuild the engine:

.. code-block:: shell

   > emcmake cmake -S simulator/web -B simulator/web/build -G Ninja \
       -DWEB_SIM_MAIN_MODULE=ON
   > cmake --build simulator/web/build
   > cmake --install simulator/web/build --prefix <install dir>

The installed layout:

.. code-block:: text

   engine/
       gui.js        the engine (global createHoneyGUI)
       gui.wasm
       web_sim.js    page script (global startHoneyGUI)
   include/
       web_sim.h
       other engine headers

Put the three files from ``engine/`` wherever the web page can reach them.

.. note::

   ``WEB_SIM_MAIN_MODULE=ON`` exports every engine symbol and is noticeably larger. Leave it off for UI preview only, with no compiled project.

Screen size
~~~~~~~~~~~

A prebuilt engine is shared by many projects, so the screen size is not fixed at build time; the page sets it at start-up (see `Rendering and scaling`_). The bundled ``index.html`` uses the default from the configuration.

Troubleshooting
~~~~~~~~~~~~~~~

``emcmake`` is not found
  emsdk is not activated, or a new shell was opened without activating it again.

``must be configured with the Emscripten toolchain``
  ``cmake`` was used instead of ``emcmake cmake``.

The page reports ``build/gui.js is missing``
  The engine has not been built, or was built elsewhere. Confirm ``simulator/web/build/gui.js`` exists.

The page reports ``files.js is missing``
  ``python simulator/web/pack_files.py`` has not been run.

The page reports ``Failed to start`` or stays black
  Page encoding. The wasm inside ``gui.js`` is UTF-8 text, so the page must declare ``<meta charset="UTF-8">``.

``This engine was built without the HML runtime``
  The configuration has the HML runtime off. Build with the default ``simulator/web/.config``, which has it.

The engine runs but the screen is blank
  Expected with the default configuration, which has no demo application. Select one in `Choosing the demo application`_ to see a compiled demo. If a demo is selected, check that the paths in ``files.json`` exist, then re-run ``pack_files.py``.

Designer UI Preview
-------------------

Use case: the UI has been designed with :doc:`HoneyDesigner <../VSCode/index>` and needs to be viewed in a browser, without compiling the project's C code for that.

The browser simulator's engine has the HML runtime built in, so it parses and renders the designer's ``ui/*.xml`` directly. Changing the UI requires neither a rebuild nor a compiler download.

Procedure
~~~~~~~~~

**Step 1: prepare the engine**

The default ``simulator/web/.config`` already is this engine: all widgets, the HML runtime and no demo application.

.. code-block:: shell

   > emcmake cmake -S simulator/web -B simulator/web/build -G Ninja \
       -DWEB_SIM_MAIN_MODULE=ON
   > cmake --build simulator/web/build
   > cmake --install simulator/web/build --prefix out

The installed layout:

.. code-block:: text

   out/engine/    gui.js, gui.wasm, web_sim.js
   out/include/   headers

**Step 2: put the project assets in place**

The converted asset directory of the designer project (``<project>/build/assets``) has this structure:

.. code-block:: text

   assets/
       ui/
           Main.xml            screen files
           Settings.xml
       resource/
           map/map_00.bin      images, keeping the designer's subdirectories
       HarmonyOS_Sans_Bold..._bitmap.bin    fonts
       project.json

Place this directory where the web page can reach it, referred to below as ``sim/assets``.

**Step 3: start the preview from the page**

Call ``startHoneyGUI()``, write the assets into the engine filesystem, and point it at the HML:

.. code-block:: javascript

   const files = { /* 'assets/ui/Main.xml': Uint8Array, ... */ };

   const sim = await startHoneyGUI({
       createModule: globalThis.createHoneyGUI,
       canvas,
       files,
       screen: { width: 410, height: 502, pixelBits: 32 },
       hml: { dir: 'assets' },        // HML is under assets/ui
   });

``hml.dir`` is relative to the module working directory. The engine scans the ``ui/`` below it, registers every file as a view, and shows the entry view.

To open a specific view, pass ``hml: { dir: 'assets', startView: 'Settings' }``. An unknown name keeps the entry view.

**Step 4 (optional): refresh after saving**

Saving an HML file requires neither a rebuild nor another ``pack_files.py`` run:

.. code-block:: javascript

   const bytes = new TextEncoder().encode('<hml>...</hml>');
   sim.module.FS.writeFile('/assets/ui/Main.xml', bytes);   // path matching hml.dir
   sim.reloadHml();

The engine rebuilds the current view on the next frame. A file that fails to parse keeps its previous version.

.. note::

   This refresh is **triggered by the page** through ``reloadHml()``; the engine does not poll for file changes. The ``hml_watch()`` interface in the engine is not yet wired into any project.

.. warning::

   Every refresh retains one more copy of the replaced document in the engine heap (a hidden view may still reference old nodes). The engine heap is 10 MB, so reloading one large file several dozen times can exhaust it. A page driving the refresh itself should restart the engine after about 3 MB of accumulated reloads.

UI only, no C code
~~~~~~~~~~~~~~~~~~

Business functions referenced by name from the HML (``functionName``, timer ``callback``) are project C code and do not run in the preview. When that logic is needed, simulate from the VSCode extension, or compile the project into ``project.wasm`` and load it (see `Load a compiled project`_).

The same assets on the desktop simulator
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Select ``HML Designer Application`` in ``menuconfig``, build, and pass the asset directory as an argument:

.. code-block:: shell

   > .\simulator\win32\gui.exe ..\my_project\build\assets

Troubleshooting
~~~~~~~~~~~~~~~

``This engine was built without the HML runtime``
  The engine does not have ``CONFIG_REALTEK_BUILD_XML_LOADER``. Build with the default ``simulator/web/.config``, which has it.

Elements are misplaced or images are missing
  HML asset paths resolve as ``assets/resource/xxx.png`` -> ``/resource/xxx.bin``. Check that the designer converted the images to ``.bin`` and that the subdirectories match.

Nothing changes after editing the UI
  Check that ``sim.reloadHml()`` is called and that the written path matches ``hml.dir`` (``assets/ui/xxx.xml`` is ``/assets/ui/xxx.xml`` in MEMFS).

Some widgets are missing from the screen
  The HML runtime and the designer's code generator do not support exactly the same components. For content missing from the preview, trust the VSCode extension simulation.

Embed in Your Own Page
----------------------

Use case: the engine is already packaged (see `Packaging a distributable engine`_) and a GUI preview window is needed inside your own page, documentation or extension.

Minimal working page
~~~~~~~~~~~~~~~~~~~~

.. code-block:: html

   <!DOCTYPE html>
   <html>
   <head><meta charset="UTF-8"><title>HoneyGUI</title></head>
   <body>
   <canvas id="screen"></canvas>
   <script src="engine/gui.js"></script>
   <script src="engine/web_sim.js"></script>
   <script>
       (async () => {
           const canvas = document.getElementById('screen');
           const sim = await startHoneyGUI({
               createModule: globalThis.createHoneyGUI,
               canvas,
               files: {},                                  // runtime assets
               screen: { width: 410, height: 502, pixelBits: 32 },
           });
           canvas.style.width  = `${sim.width * 2}px`;     // scale as needed
           canvas.style.height = `${sim.height * 2}px`;
       })();
   </script>
   </body>
   </html>

Three points to note:

1. **The page must declare UTF-8**: ``<meta charset="UTF-8">``. ``gui.js`` embeds the wasm as UTF-8 text, and a wrong charset makes the start-up fail
2. **Load order**: load ``gui.js`` first (defines ``createHoneyGUI``), then ``web_sim.js`` (defines ``startHoneyGUI``)
3. **Both are plain scripts**, not ES modules, which is what lets the page run from ``file://``

Three common configurations
~~~~~~~~~~~~~~~~~~~~~~~~~~~

**1. Run only the demo application built into the engine**

``files: {}``, with ``screen`` set to the resolution of that configuration. With the bundled ``index.html`` the assets are already packed into ``files.js``.

**2. Preview a designer project**

.. code-block:: javascript

   await startHoneyGUI({ createModule, canvas, files, screen,
                         hml: { dir: 'assets' } });

See `Designer UI Preview`_.

**3. Load a compiled project**

.. code-block:: javascript

   await startHoneyGUI({ createModule, canvas, files, screen,
                         project: projectWasmBytes });

The project must first be compiled as an Emscripten side module (``-sSIDE_MODULE``, with ``-sASYNCIFY``), and the engine must be built with ``WEB_SIM_MAIN_MODULE=ON``.

.. _Load a compiled project:

The ``files`` parameter
~~~~~~~~~~~~~~~~~~~~~~~

``{ relative path: Uint8Array }``, written into the in-memory filesystem before the engine starts; directories are created as needed. It supplies the resources the project needs at run time (romfs image, images, fonts). In the bundled page this step is handled by the ``files.js`` that ``pack_files.py`` generates.

.. _Rendering and scaling:

Rendering and scaling
~~~~~~~~~~~~~~~~~~~~~

``startHoneyGUI()`` already runs the per-frame refresh loop, so the page only needs to:

- set the canvas ``width`` / ``height`` attributes to ``sim.width`` / ``sim.height`` (engine pixels)
- set the display size through the CSS ``style.width`` / ``style.height``

.. code-block:: javascript

   canvas.width  = sim.width;
   canvas.height = sim.height;
   canvas.style.width  = `${sim.width * 2}px`;
   canvas.style.height = `${sim.height * 2}px`;
   canvas.style.imageRendering = 'pixelated';   // keep pixels crisp when scaled

Pointer, wheel and key listeners are bound by ``startHoneyGUI()``, which also converts coordinates, so scaling needs no extra handling.

No response, or offset coordinates
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 40 60

   * - Symptom
     - Cause
   * - Touch does not respond
     - The page uses ``click`` instead of ``pointerdown`` / ``pointerup``, or the canvas is covered by another element that captures pointer events
   * - Touch coordinates are offset
     - The displayed canvas size differs from the ``width`` / ``height`` attributes and the page converts coordinates once more. Leave the conversion to the built-in listeners
   * - Image is cropped or stretched
     - Only the CSS size was changed, without the ``width`` / ``height`` attributes, or ``screen`` does not match the canvas size
   * - Starting again after ``stop()`` fails
     - An Emscripten module instance cannot be reused; create a new one (refreshing an iframe is the usual approach)

The full interface list is in the comments of ``simulator/web/port/web_sim.h`` and ``simulator/web/web_sim.js``.

Deployment and distribution
---------------------------

The HoneyGUI repository provides the engine and the page host script, not a hosted site. There are three ways to get the UI into a browser:

.. list-table::
   :header-rows: 1
   :widths: 26 30 44

   * - Way
     - How
     - Good for
   * - Open it locally
     - Build, then double-click ``simulator/web/index.html``
     - Checking the result yourself; no deployment
   * - Use the online designer
     - Open `HoneyDesigner <https://realmcu.github.io/HoneyDesigner/>`_
     - A ready-made web version, without touching the HoneyGUI repository
   * - Host it yourself
     - See the full steps below
     - Embedding the preview in your own tool, documentation or intranet; also the way to go when github.io is unreachable

Hosting it yourself, step by step
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Step 1: build the engine**

.. code-block:: shell

   > emcmake cmake -S simulator/web -B simulator/web/build -G Ninja
   > cmake --build simulator/web/build

The output is a single ``simulator/web/build/gui.js``\ (the wasm is embedded), used together with ``simulator/web/web_sim.js``.

.. note::

   The default ``.config`` has the HML runtime, which this needs. If ``-DWEB_SIM_CONFIG`` points at a configuration without it, the page reports ``This engine was built without the HML runtime``.

This step only has to be redone when the engine changes. **Changing the UI does not need a rebuild**, but the steps below have to be repeated.

**Step 2: prepare the project assets**

Open the project in the VSCode extension and click "simulate" once; the assets are converted into ``<project>/build/assets``. List every file from there in the repository's ``simulator/web/files.json``:

.. code-block:: json

   [
     "my_project/build/assets/ui/Main.xml",
     "my_project/build/assets/resource/map/map_00.bin",
     "my_project/build/assets/HarmonyOS_Sans_Bold_size30_bits4_bitmap.bin"
   ]

Two constraints:

- Paths are relative to the repository root and must not contain ``..``. The packing script uses the path verbatim as the file name inside the engine, and ``..`` keeps the file out of the in-memory filesystem
- An image referenced as ``src="assets/xxx.png"`` in the HML is looked up as ``xxx.bin``, so list the converted ``.bin``

.. note::

   Pack the build output ``build/assets``, not the project directory you edit in the designer.

**Step 3: pack**

.. code-block:: shell

   > python simulator/web/pack_files.py

It prints ``packed N file(s), ...`` and writes ``simulator/web/files.js``.

After changing the HML, go through steps 2 to 4 again: simulate -> update ``files.json``\ (when files were added or removed) -> re-run ``pack_files.py`` -> copy the new ``files.js`` into the site directory.

**Step 4: assemble the site**

Four files in one directory:

.. code-block:: text

   mysite/
       index.html     copied from simulator/web/index.html
       gui.js         copied from simulator/web/build/
       web_sim.js     copied from simulator/web/
       files.js       generated in the step above

Three changes in ``index.html``:

.. code-block:: html

   <!-- 1. script path: was build/gui.js -->
   <script src="gui.js"></script>

   <!-- 2. select the screen; the three values come from the project's project.json:
           resolution / pixelMode / cornerRadius (ARGB8888 means 32 bits,
           a negative cornerRadius means a round screen) -->
   screen: { width: 410, height: 502, pixelBits: 32 },

   <!-- 3. add the hml option, pointing at the directory holding the packed assets -->
   startHoneyGUI({ createModule: globalThis.createHoneyGUI, canvas, files,
                   screen, hml: { dir: 'my_project/build/assets' } });

``files`` comes from ``files.js`` (the page decodes ``HONEYGUI_WEB_SIM_FILES``, so there is no loading code to write). ``hml.dir`` must match the prefix used in ``files.json``; the engine scans the ``ui/`` below it.

**Step 5: serve it**

Browsers do not let a page ``fetch()`` local files, so double-clicking will not work; run an HTTP server:

.. code-block:: shell

   > cd mysite
   > python -m http.server 8000

Then open ``http://localhost:8000/``.

For real deployment, hand the directory to any static server (nginx, IIS, GitHub Pages, an intranet file server). There is no backend and no runtime to install.

.. note::

   **The online designer runs HML preview**: it parses and renders ``ui/*.xml`` and does not execute project C code (``callFunction``, callbacks). For that logic, use the VSCode extension or the desktop simulator.
