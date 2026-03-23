# HoneyGUI Development Guidelines

**Generated:** 2026-03-23 | **v2.1.1.0** | **Branch:** master

## Overview

Embedded GUI framework for Realtek microcontrollers (RTL8762, RTL8773). C library with PC simulator + embedded ports. Supports C/C++ API and HML/XML low-code development.

## Structure

    realgui/           # Core GUI engine
    +-- core/          # Event system, object model (def_*.h)
    +-- widget/        # 21 widget components (gui_*.c)
    +-- engine/        # Rendering, fonts, codecs
    +-- dc/            # Display controller
    +-- input/         # Touch/keyboard handling
    +-- 3rd/           # Third-party (Arm2D, box2d, stb, h264bsd)
    +-- server/        # GUI server components

    example/          # Demo applications by screen resolution
    win32_sim/        # Windows PC simulator
    lib/              # Prebuilt toolchain libraries
    tool/             # Development tools (image/video converters)

## Build

    # SCons (primary)
    cd win32_sim && scons

    # CMake (alternative)
    cd win32_sim && mkdir build && cmake -G "MinGW Makefiles" .. && mingw32-make

    # Configure app
    cd win32_sim && menuconfig ../Kconfig.gui

## Key Conventions

- **Logging**: gui_log() not printf() - include gui_api_os.h
- **File I/O**: VFS API - include gui_vfs.h
- **Shell commands**: realgui/3rd/letter_shell/ with SHELL_EXPORT_CMD()
- **Platform macros**: #if defined(_WIN32) || defined(__linux__)
- **Widget prefix**: gui_ for implementations, gui_ headers
- **Event types**: GUI_EVENT_* in realgui/core/def_event.h
- **HML tags**: hg_ prefix (e.g., <hg_view>, <hg_button>)

## Git

- **Commit prefix**: GUI: (max 48 chars title, 60 chars body)
- **Gerrit push**: git push origin HEAD:refs/for/master

## Anti-Patterns (DO NOT)

- Define __ARM_2D_CFG_FORCED_FIXED_POINT_TRANSFORM__ in arm_2d_features.h:158
- Use virtual screen flag in arm_2d_types.h:636
- Free cached geometry buffers in gui_circle.c:923, gui_rect.c:1047
- Edit auto-generated files (cconfig.h, scons-tool/*.py)
- Use stb_truetype on untrusted font files (no security guarantee)

## Testing

    cd example/test && python run_all_tests.py    # All tests
    python test_widgets.py                          # Widget demos
    python test_gui_lib_build.py                    # Cross-compile tests

## Platform Ports

- win32_sim/: MinGW-w64, SDL2, Windows
- keil_sim/ac5/ac6/: Keil ARM MDK
- zephyr/: Zephyr RTOS integration
- lib/: Prebuilt for gcc, armcc, armclang, arm-none-eabi-gcc