Build and Simulation
====================

This chapter explains how to build and run a HoneyDesigner project on the :term:`PC` and how the HoneyGUI and LVGL build flows differ.

Overview
--------

The extension creates a ``build/`` directory in the project root and automatically converts resources, generates code, compiles, and starts the simulator. HoneyGUI uses SCons; LVGL uses CMake.

Requirements
------------

Common software:

* **VSCode**: Code editor

  * Recommended version: 1.80.0 or later
  * Download: https://code.visualstudio.com/
  * Install the HoneyDesigner extension

* **Python**: 3.9.7 or later

  * Download: https://www.python.org/downloads/

* **SCons**: Build tool

  .. code-block:: bash

     pip install scons==4.4.0

Common optional software:

* **CMake**: 3.31.2 or later (required for LVGL projects)

  * Download: https://cmake.org/download/

* **FFmpeg**: Required for video conversion

  * Download: https://ffmpeg.org/download.html

* **Git**: Required when creating projects from templates

  * Download: https://git-scm.com/downloads

Windows
~~~~~~~

Required software:

* **MinGW-w64**: C/C++ compiler

  * Recommended version: 8.1.0 or later
  * Installation path: ``C:\mingw64`` (default)
  * Download: https://sourceforge.net/projects/mingw-w64/

* **SDL2**: Bundled with the repository; no separate installation is required

Linux/WSL
~~~~~~~~~~

.. code-block:: bash

   # Ubuntu/Debian
   sudo apt-get update
   sudo apt-get install build-essential python3 python3-pip libsdl2-dev
   pip3 install scons==4.4.0

Linux/WSL uses system ``gcc``/``g++``; SDL2 is provided by ``libsdl2-dev``. WSL needs WSLg or an X server to display the simulator window.

Environment Variables
~~~~~~~~~~~~~~~~~~~~~

**Windows**

The build script ``menu_config.py`` resolves ``gcc`` through ``PATH`` and fails if it is not found, so ``C:\mingw64\bin`` must be on ``PATH``. To add it temporarily in the current CMD window, run:

.. code-block:: bat

   set PATH=C:\mingw64\bin;%PATH%

For a permanent change, edit the user or system ``PATH`` under **System Properties > Environment Variables**, then reopen the terminal and VSCode. Verify the configuration with:

.. code-block:: bash

   gcc --version
   g++ --version
   mingw32-make --version

**Linux/WSL**

System GCC is installed by ``build-essential`` and normally needs no extra configuration. Verify it with:

.. code-block:: bash

   gcc --version
   pkg-config --libs sdl2

If SDL2 is not in the system default location, CMake can use ``SDL2_INCLUDE_DIR`` and ``SDL2_LIB_DIR`` to locate the headers and libraries.

**FFmpeg**

When video conversion is needed, add FFmpeg's ``bin`` directory to ``PATH``; otherwise video assets are skipped.

Environment Check
~~~~~~~~~~~~~~~~~

1. Open the **ENVIRONMENT** view in the HoneyDesigner sidebar.
2. Check Python, SCons, GCC, SDL2, and FFmpeg.
3. Click refresh or run ``HoneyDesigner: Refresh Environment Check``.
4. Install missing items from the view.

Project Structure
-----------------

.. code-block:: text

   my_project/
   ├── project.json
   ├── ui/
   │   └── MyProjectMain.hml
   ├── src/
   │   ├── MyProjectEntry.c
   │   ├── SConscript
   │   ├── ui/
   │   ├── callbacks/
   │   ├── user/
   │   └── lvgl/                # LVGL projects
   ├── assets/
   └── build/                   # Generated build directory

``win32_sim/`` is not a user project directory. The extension copies a fixed PC simulator project skeleton into ``build/``, including the SCons/CMake build scripts, the ``main.c`` entry point, the ``port/`` platform layer (display, input, audio, filesystem, and OS), linker scripts, and the SDL2 static library used on Windows. You only maintain ``ui/``, ``src/``, ``assets/``, and ``project.json``. ``build/`` is generated and can be deleted and regenerated at any time.

Project Configuration
---------------------

Example ``project.json``:

.. code-block:: json

   {
     "$schema": "HoneyGUI",
     "type": "Designer",
     "name": "MyWatchApp",
     "appId": "com.example.mywatch",
     "version": "1.0.0",
     "resolution": "410x502",
     "cornerRadius": 20,
     "targetEngine": "honeygui",
     "pixelMode": "RGB565",
     "minSdk": "1.0",
     "mainHmlFile": "ui/MyWatchAppMain.hml",
     "romfsBaseAddr": "0x704D1000",
     "aiAssets": true,
     "uart": {
       "port": "COM3",
       "baudRate": 115200,
       "chipType": "RTL87X3E"
     }
   }

Common fields:

* ``resolution``: screen size in ``WxH`` form.
* ``cornerRadius``: ``0`` rectangle, ``-1`` circle, positive value for corner radius.
* ``targetEngine``: ``honeygui`` or ``lvgl``.
* ``pixelMode``: ``RGB565``, ``RGB888``, or ``ARGB8888``.
* ``romfsBaseAddr``: Flash base address for romfs.
* ``uart``: UART download settings.

Build Flow
----------

Build Project
~~~~~~~~~~~~~

**Build from the extension:**

1. Open an HML file in the project.
2. Click :guilabel:`Simulate` in the toolbar, or run ``HoneyDesigner: Compile & Simulate`` from the Command Palette.
3. By default, the extension converts resources, generates code, compiles, and starts the simulator in that order.
4. To skip a stage, open the dropdown to the right of :guilabel:`Simulate` and clear the corresponding item under **Simulation Flow**.

Extension commands:

* ``HoneyDesigner: Debug Simulation`` starts a debug session.
* ``HoneyDesigner: Run Simulation Without Code Generation`` skips code generation.
* ``HoneyDesigner: Stop Simulation`` stops the simulator.
* The toolbar :guilabel:`Clean` action or **Quick** view :guilabel:`Clean` removes build output.

**Manual build:**

The toolbar and Command Palette are the recommended paths. For manual investigation, generate code first and then run SCons from ``build/``:

.. code-block:: bash

   cd build
   scons

LVGL projects use CMake:

.. code-block:: bash

   cd lvgl-pc
   cmake -S . -B build
   cmake --build build -j4

Build Output
~~~~~~~~~~~~

Whether you build from the extension or manually, the main outputs are:

HoneyGUI:

* Windows: ``build/gui.exe``
* Linux/WSL: ``build/gui``
* Assets: ``build/assets/``
* romfs: ``build/app_romfs_<baseAddr>.bin``

LVGL:

* Project: ``lvgl-pc/``
* Executable: ``lvgl-pc/build/lvgl_pc.exe`` or ``lvgl-pc/build/lvgl_pc``

Debugging
---------

* Use ``HoneyDesigner: Debug Simulation`` to start a debug session.
* Windows requires GDB, usually installed with MinGW.
* You can also add a ``cppdbg`` configuration to ``launch.json`` and point ``program`` at ``build/gui.exe``.

Example:

.. code-block:: json

   {
     "version": "0.2.0",
     "configurations": [
       {
         "name": "Debug HoneyGUI",
         "type": "cppdbg",
         "request": "launch",
         "program": "${workspaceFolder}/build/gui.exe",
         "cwd": "${workspaceFolder}",
         "MIMode": "gdb",
         "miDebuggerPath": "C:/mingw64/bin/gdb.exe"
       }
     ]
   }

Running the Simulator
---------------------

Start the Simulator
~~~~~~~~~~~~~~~~~~~

From the extension:

1. Click :guilabel:`Simulate` in the toolbar, or run ``HoneyDesigner: Compile & Simulate``.
2. If the code has already been generated, run ``HoneyDesigner: Run Simulation Without Code Generation`` to start without regenerating code.

You can also start it manually:

.. code-block:: bash

   # Windows
   ./build/gui.exe

   # Linux/WSL
   ./build/gui

For LVGL projects, run ``lvgl-pc/build/lvgl_pc.exe`` or ``lvgl-pc/build/lvgl_pc``.

Interaction
~~~~~~~~~~~

* **Left mouse button**: touch click.
* **Mouse drag**: swipe gesture.
* **Mouse long press**: long press.
* **Esc**: exit the simulator.
* **Close the window**: exit the simulator.

Common Issues
-------------

**gcc not found**

.. code-block:: text

   'gcc' is not recognized as an internal or external command

Solutions:

* Install MinGW-w64 or GCC.
* Add the compiler ``bin`` directory to ``PATH``.
* Restart the terminal and VSCode.

**SDL2 missing**

.. code-block:: text

   fatal error: SDL2/SDL.h: No such file or directory

Solution:

.. code-block:: bash

   sudo apt-get install libsdl2-dev

**SCons module missing**

.. code-block:: bash

   pip install scons==4.4.0

**Simulator fails to start**

* Windows: verify the graphics/OpenGL environment.
* Linux: verify SDL2 is installed.
* WSL: configure an X server or use WSLg.

**Resource file not found**

* Run resource conversion first.
* Verify that image paths start with ``assets/`` and font paths start with ``/``.
* Run a full build again.

Performance
-----------

* Use incremental builds when only a few files change.
* Use appropriate image and video dimensions, formats, and compression.
* Avoid long-running work in frequently triggered callbacks.
* Use optimized builds for release.
* Adjust CMake build parallelism for LVGL projects.

Next Steps
----------

* Read :doc:`deployment` for embedded deployment solutions
