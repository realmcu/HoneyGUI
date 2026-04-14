# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

HoneyGUI is an embedded graphical user interface (GUI) library developed by Realtek for resource-constrained microcontrollers. It provides both C/C++ API development and JavaScript/XML low-code development approaches. This SDK includes a PC-based simulation environment for rapid development without embedded hardware.

Current version: v2.1.1.0

**Online Documentation:** https://docs.realmcu.com/gui/en/latest/index.html

## Versioning Scheme

HoneyGUI uses a four-part semantic versioning scheme:

**Format:** `v{MAJOR}.{MINOR}.{PATCH}.{BUILD}`

- **MAJOR**: Major architectural changes or product-level updates
- **MINOR**: API changes (including breaking API changes)
- **PATCH**: Bug fixes and minor improvements
- **BUILD**: Build number (typically 0)

Special releases may include `.RVD` suffix for RVisualDesigner-specific builds.

Version information is auto-generated via `tool/git_generate_version.py`.

## Build System

HoneyGUI supports two build systems:

### SCons Build (Primary)

```shell
cd win32_sim
scons                    # Build the project
scons -c                 # Clean build artifacts
cd ..
./win32_sim/gui.exe      # Run on Windows
./win32_sim/gui          # Run on Linux/WSL
```

**Build Configuration:**
- Compiler flags are strict by default (treats warnings as errors)
- Set `DEBUG_MODE=1` environment variable to skip strict checks for development
- Build artifacts: `gui.exe`/`gui`, `gui.map`, `gui.bin`
- Clean command removes build artifacts but preserves `.config` file

### CMake Build (Alternative)

```shell
cd win32_sim
mkdir build && cd build
cmake -G "MinGW Makefiles" ..
mingw32-make -j 32       # Or use 'make' on Linux
cd ../..
./win32_sim/build/gui.exe
```

### Application Configuration (Menuconfig)

Before building, configure which application to run:

```shell
cd win32_sim
menuconfig ../Kconfig.gui
```

This generates a `.config` file that both build systems read. Available applications include:
- Watch Turnkey Demo (410x502) - default watch application
- GUI Demo (800x480) - dashboard application
- Various screen-specific applications (240x240, 466x466, etc.)

## Architecture

### Directory Structure

- **`realgui/`** - Core RealGUI engine
  - `core/` - Core event system, object management (`def_event.h` defines event types)
  - `engine/` - Rendering engine (blit operations, codecs, font rendering, post-processing)
  - `dc/` - Display controller layer
  - `input/` - Input device handling
  - `widget/` - Widget library (see Widgets section)
  - `server/` - GUI server components
  - `3rd/` - Third-party libraries (Lite3D, nanosvg, box2d, particle systems, etc.)
  - `misc/` - Utilities (VFS, TLSF memory allocator)

- **`example/`** - Sample applications
  - `application/` - Full application examples organized by screen resolution
  - `widget/` - Individual widget examples
  - `tmp/` - Temporary/experimental examples
  - `test/` - Test suite

- **`lib/`** - Prebuilt libraries for different toolchains
  - `sim/` - Simulation platform libraries
  - `gcc/`, `armcc/`, `armclang/`, `arm-none-eabi-gcc/` - Embedded toolchain libraries

- **`tool/`** - Development tools
  - `image-convert-tool/` - Convert images to HoneyGUI formats with compression
  - `3D-tool/` - 3D model conversion tools
  - `D2XML/` - Design-to-XML conversion (for RVisualDesigner integration)
  - `video-convert-tool/` - Video format conversion
  - `mkromfs/` - ROM filesystem creation
  - `scons-tool/` - SCons build utilities

- **`win32_sim/`** - Windows/Linux simulation environment
- **`keil_sim/`** - Keil simulator environment

### Widget System

All widgets inherit from `gui_obj_t` base object. Major widget types:

- `gui_img` - Image rendering (supports various formats and compression)
- `gui_text` - Text rendering with font support
- `gui_view` - Container for screen layouts using view descriptors
- `gui_win` - Window management
- `gui_list` - Scrollable lists
- `gui_canvas` - Custom drawing canvas
- `gui_svg` - SVG vector graphics
- `gui_lottie` - Lottie animation
- `gui_lite3d` - 3D rendering integration
- `gui_particle` - Particle effects
- `gui_glass` - Glass/blur effects
- `gui_video` - Video playback
- `gui_geometry` - Geometric shapes
- `gui_soccer` - 2.5D soccer demo

### Application Structure

Applications use a **view-based architecture** with view descriptors:

```c
static gui_view_descriptor_t const descriptor = {
    .name = "view_name",
    .pView = &current_view,
    .on_switch_in = design_ui_callback,
    .on_switch_out = cleanup_callback,
    .keep = false,  // Whether to keep in memory when switched out
};
```

Views are registered using `gui_view_descriptor_register()` and retrieved with `gui_view_descriptor_get()`.

### Event System

Events are defined in `realgui/core/def_event.h`:
- Touch events: `GUI_EVENT_TOUCH_CLICKED`, `GUI_EVENT_TOUCH_PRESSED`, `GUI_EVENT_TOUCH_LONG`, etc.
- Keyboard events: `GUI_EVENT_KB_SHORT_PRESSED`, `GUI_EVENT_KB_LONG_PRESSED`
- System events: `GUI_EVENT_SYSTEM_RESET`, `GUI_EVENT_SYSTEM_SLEEP`, etc.

Event callbacks use: `void (*gui_event_cb_t)(void *obj, gui_event_t *e)`

### 3D Rendering

3D functionality is provided by Lite3D (in `realgui/3rd/Lite3D/`):
- Supports glTF model loading via cgltf
- Hardware acceleration via litegfx/vglite
- Integration through `gui_lite3d` widget

### HML (HoneyGUI Markup Language) Low-Code Development

HML provides XML-based UI development through the ezXML loader (in `realgui/3rd/ezXML/loader/`):

**Supported HML Components:**
- `<hg_view>`, `<hg_window>`, `<hg_win>` - Container views
- `<hg_image>` - Image display
- `<hg_text>`, `<hg_label>` - Text rendering
- `<hg_button>` - Interactive buttons
- `<hg_list>`, `<hg_list_item>` - Scrollable lists
- `<hg_canvas>` - Custom drawing
- `<hg_arc>`, `<hg_circle>` - Geometric shapes
- `<hg_video>` - Video playback
- `<hg_time_label>` - Time display

**Event Binding:**
- `onClick` - Touch click events
- `onLongPress` - Long press events
- Callbacks registered via `hml_register_callback(name, func)`

**RVisualDesigner Integration:**
- Use RVD tool for visual UI design with drag-and-drop
- Generates HML/XML files compatible with ezXML loader
- Located in `tool/D2XML/` for design-to-XML conversion

## Platform Macros

- `_HONEYGUI_SIMULATOR_` - Defined for PC simulation builds
- Various config options controlled via menuconfig/Kconfig

## Resource Management

- Binary resources are typically stored in `root_image/` directories within applications
- Use `mkromfs` tool to create ROM filesystems from resource directories
- Image formats support compression (RLE, FastLZ, adaptive) via image-convert-tool

## Testing

```shell
cd example/test
python run_all_tests.py        # Run all tests
python test_widgets.py         # Test widget functionality
python test_gui_lib_build.py   # Test library build
```

Test results are written to `test_report.txt` and errors to `test_errors.log`.

## Toolchain Requirements

**Windows:**
- MinGW-w64 (tested: 8.1.0) installed at `C:\mingw64`
- Python 3.9.7+
- SCons 4.4.0: `pip install scons==4.4.0`
- kconfiglib: `pip install kconfiglib`
- CMake 3.31.2+ (for CMake builds)

**Linux/WSL:**
- GCC toolchain
- SDL2 development libraries
- Same Python dependencies as Windows

## Development Workflow

### Command Line Workflow

1. Configure application: `cd win32_sim && menuconfig ../Kconfig.gui`
2. Build: `scons` (or use CMake)
3. Run: `../win32_sim/gui.exe` (or `gui` on Linux)
4. Interact with the GUI window using mouse for touch simulation

### VSCode Workflow

1. Open `HoneyGUI.code-workspace` in VSCode
2. Install C/C++ Extension if not already installed
3. Use "Run and Debug" panel (Ctrl+Shift+D)
4. Click Run button to build and launch
5. Configuration is read from `.config` file generated by menuconfig

## Development Approaches

HoneyGUI supports three development approaches:

1. **C/C++ API Development** - Direct widget API usage for maximum performance
   - Best for complex applications with high performance requirements
   - Full control over rendering and event handling
   - Examples in `example/application/` and `example/widget/`

2. **HML/XML Low-Code Development** - Declarative UI with XML markup
   - Fast prototyping with RVisualDesigner tool
   - Event handlers registered via `hml_register_callback()`
   - Component files in `realgui/3rd/ezXML/loader/components/`

3. **JavaScript Development** - Front-end scripting approach
   - Rapid iteration for UI logic
   - Works with RVisualDesigner for low-code workflow
   - Suitable for rapid prototyping and small applications

## Git Commit Guidelines

HoneyGUI uses a structured commit message format. All commits must follow the template in `.git-commit-template`.

### Commit Message Format

```
<MODULE>: <short description (max 72 chars)>


[JIRA] <JIRA-ID or NA>

[JIRA TYPE] Bug-Native-Minor | Bug-Native-Critical | Bug-Introduced-Minor | Bug-Introduced-Critical | Feature-*

[Introduced by] <Change-ID or NA>

[Issue description]
<Describe the issue>

[Issue reproduction]
<Steps to reproduce>

[Cause]
<Root cause analysis>

[Solution]
<Description of fix>

[Validation] <Validation method or NA>
<Test results>

[Need Cherry Pick to] default | branch-name | NA
```

### Guidelines

- **Module prefix**: Use `GUI:` prefix for all commits (per AGENTS.md)
  - Use `GUI-BypassTest:` when explicitly requested to skip CI/CD testing
  - Bypass testing only for: config changes, doc updates, non-functional changes
  - Example: `GUI-BypassTest: update VSCode debug configuration`
- **Subject line**: Max 72 characters, capitalize first letter, no period at end
- **Language**: Commit messages must be in English only - no Chinese characters allowed
- **JIRA**: Include JIRA ticket ID or `NA` if not applicable
- **All sections required**: Issue description, reproduction, cause, solution, validation
- **Validation**: Always include test results or validation method
- **Cherry pick**: Default is `default` for main branch integration

### Common Commit Types

- `GUI: fix <issue>` - Bug fixes
- `GUI: add <feature>` - New features
- `GUI: update <component>` - Enhancements
- `GUI: refactor <module>` - Code refactoring

### Example

```
GUI: fix memory leak in gui_text widget

[JIRA] GUI-1234
[JIRA TYPE] Bug-Native-Minor
[Introduced by] NA

[Issue description]
Text widget leaks memory when rendering long strings repeatedly.

[Issue reproduction]
1. Create gui_text widget
2. Update text content 1000+ times
3. Observe memory usage increasing

[Cause]
Font glyph cache not freed when text content changes.

[Solution]
Add cache cleanup in gui_text_set() before allocating new cache.

[Validation] Verified on RTL8762D
- Memory usage stable after 10000 text updates
- No performance regression

[Need Cherry Pick to] default
```

### Pushing Commits

This project uses Gerrit for code review:

```bash
git push origin HEAD:refs/for/master
```

## Important Conventions

- Event types recently renamed from `gui_event_t` to `gui_event_code_t` (event struct still named `gui_event_t`)
- View descriptors must be registered before use
- Memory management uses TLSF allocator in constrained environments
- Applications are organized by target screen resolution
- HML component tags use `hg_` prefix (e.g., `<hg_view>`, `<hg_button>`)
