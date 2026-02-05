Build and Simulation
====================

This document describes how to build and run HoneyGUI Design projects, including development environment setup, build process, and simulation testing.

Overview
--------

HoneyGUI Design supports PC simulation, allowing rapid GUI application development and testing without embedded hardware. Projects use the SCons build system and support Windows, Linux, and WSL environments.

Development Environment Requirements
-------------------------------------

Windows
~~~~~~~

Required software:

* **MinGW-w64**: C/C++ compiler

  * Recommended version: 8.1.0 or higher
  * Installation path: ``C:\mingw64`` (default)
  * Download: https://sourceforge.net/projects/mingw-w64/

* **Python**: 3.9.7 or higher

  * Download: https://www.python.org/downloads/

* **SCons**: Build tool

  .. code-block:: bash

     pip install scons==4.4.0

* **kconfiglib**: Configuration tool

  .. code-block:: bash

     pip install kconfiglib

* **VSCode**: Code editor

  * Download: https://code.visualstudio.com/
  * Install HoneyGUI Design plugin

Optional software:

* **CMake**: 3.31.2 or higher (if using CMake build)

  .. code-block:: bash

     pip install cmake

Linux/WSL
~~~~~~~~~

Required software:

.. code-block:: bash

   # Ubuntu/Debian
   sudo apt-get update
   sudo apt-get install build-essential python3 python3-pip libsdl2-dev

   # Install Python tools
   pip3 install scons==4.4.0 kconfiglib

Environment Variable Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Windows**:

Ensure MinGW-w64 ``bin`` directory is in PATH:

.. code-block:: bash

   # Check compiler
   gcc --version
   g++ --version

   # If not found, add to PATH
   set PATH=C:\mingw64\bin;%PATH%

**Linux/WSL**:

.. code-block:: bash

   # Check compiler
   gcc --version

   # Check SDL2
   pkg-config --libs sdl2

Project Structure
-----------------

Standard HoneyGUI Design project structure:

.. code-block:: text

   my_project/
   ├── project.json          # Project configuration file
   ├── ui/                   # HML design files
   │   ├── main/
   │   │   └── main.hml
   │   └── settings/
   │       └── settings.hml
   ├── assets/               # Resource files
   │   ├── images/          # Image resources
   │   ├── fonts/           # Font resources
   │   └── models/          # 3D models
   ├── src/                  # Generated C code
   │   ├── {ProjectName}Entry.c
   │   ├── SConscript
   │   ├── ui/              # UI code
   │   ├── callbacks/       # Callback code
   │   └── user/            # User code
   └── win32_sim/           # Simulator directory (after build)

Project Configuration File
--------------------------

``project.json`` defines basic project information:

.. code-block:: json

   {
     "name": "MyWatchApp",
     "version": "1.0.0",
     "description": "My smart watch application",
     "resolution": "410x502",
     "author": "Your Name",
     "license": "MIT"
   }

Supported resolutions:

* ``240x240`` - Small round watch
* ``410x502`` - Standard smartwatch
* ``466x466`` - Large screen watch
* ``480x272`` - Landscape device
* ``800x480`` - Tablet/car display

Build Process
-------------

Build Using VSCode Plugin
~~~~~~~~~~~~~~~~~~~~~~~~~~

1. **Open Project**: Open HoneyGUI Design project folder in VSCode

2. **Generate Code**:

   * Click "Generate Code" button in toolbar
   * Or use command palette: ``Ctrl+Shift+P`` → ``HoneyGUI: Generate Code``

3. **Configure Application**:

   * Use command: ``HoneyGUI: Configure Application``
   * Select application to run

4. **Build Project**:

   * Use command: ``HoneyGUI: Build Project``
   * Or execute in terminal:

     .. code-block:: bash

        cd win32_sim
        scons

5. **Run Simulator**:

   * Use command: ``HoneyGUI: Run Simulator``
   * Or execute manually:

     .. code-block:: bash

        # Windows
        ./win32_sim/gui.exe

        # Linux/WSL
        ./win32_sim/gui

Build Using Command Line
~~~~~~~~~~~~~~~~~~~~~~~~~

**Complete build workflow**:

.. code-block:: bash

   # 1. Enter simulator directory
   cd win32_sim

   # 2. Configure application (optional)
   menuconfig ../Kconfig.gui

   # 3. Build project
   scons

   # 4. Run
   cd ..
   ./win32_sim/gui.exe  # Windows
   ./win32_sim/gui      # Linux/WSL

**Clean build**:

.. code-block:: bash

   cd win32_sim
   scons -c

**Debug mode**:

.. code-block:: bash

   # Skip strict checks for development debugging
   set DEBUG_MODE=1        # Windows
   export DEBUG_MODE=1     # Linux/WSL
   scons

Build Configuration
-------------------

Application Configuration (Menuconfig)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Configure application before building:

.. code-block:: bash

   cd win32_sim
   menuconfig ../Kconfig.gui

Configuration interface allows selection of:

* Preset applications (watch demo, GUI demo, etc.)
* Screen resolution
* Feature toggles

Configuration is saved in ``.config`` file, read by both SCons and CMake.

Compiler Flags
~~~~~~~~~~~~~~

**Default compiler flags** (strict mode):

* ``-Wall``: Enable all warnings
* ``-Werror``: Treat warnings as errors
* ``-O2``: Optimization level 2

**Debug mode flags** (``DEBUG_MODE=1``):

* ``-g``: Generate debug information
* ``-O0``: No optimization, easier debugging
* Don't use ``-Werror``

Build with CMake (Optional)
----------------------------

HoneyGUI also supports CMake build system:

.. code-block:: bash

   cd win32_sim
   mkdir build
   cd build

   # Windows (MinGW)
   cmake -G "MinGW Makefiles" ..
   mingw32-make -j 32

   # Linux
   cmake ..
   make -j 8

   # Run
   cd ../..
   ./win32_sim/build/gui.exe  # Windows
   ./win32_sim/build/gui      # Linux

Running the Simulator
---------------------

Start Simulator
~~~~~~~~~~~~~~~

.. code-block:: bash

   # Windows
   ./win32_sim/gui.exe

   # Linux/WSL
   ./win32_sim/gui

The simulator window will display your designed GUI interface.

Interaction Methods
~~~~~~~~~~~~~~~~~~~

* **Mouse Left Click**: Simulate touch tap
* **Mouse Drag**: Simulate swipe gesture
* **Mouse Long Press**: Simulate long press
* **Keyboard ESC**: Exit simulator
* **Window Close**: Exit simulator

Debugging Tips
~~~~~~~~~~~~~~

1. **View Logs**: Simulator outputs debug information to console

2. **Use GDB Debugging** (Linux):

   .. code-block:: bash

      gdb ./win32_sim/gui
      (gdb) run
      (gdb) break main
      (gdb) continue

3. **Visual Studio Code Debugging** (Windows):

   Configure ``launch.json``:

   .. code-block:: json

      {
        "version": "0.2.0",
        "configurations": [
          {
            "name": "Debug HoneyGUI",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/win32_sim/gui.exe",
            "cwd": "${workspaceFolder}",
            "MIMode": "gdb",
            "miDebuggerPath": "C:/mingw64/bin/gdb.exe"
          }
        ]
      }

Common Issues
-------------

Build Errors
~~~~~~~~~~~~

**Error: gcc not found**

.. code-block:: text

   'gcc' is not recognized as an internal or external command

Solutions:

* Confirm MinGW-w64 is installed
* Check PATH environment variable
* Restart terminal/VSCode

**Error: SDL2 library missing**

.. code-block:: text

   fatal error: SDL2/SDL.h: No such file or directory

Solution (Linux):

.. code-block:: bash

   sudo apt-get install libsdl2-dev

**Error: Python module missing**

.. code-block:: text

   ModuleNotFoundError: No module named 'SCons'

Solution:

.. code-block:: bash

   pip install scons==4.4.0 kconfiglib

Runtime Errors
~~~~~~~~~~~~~~

**Error: Cannot start simulator**

.. code-block:: text

   Failed to initialize SDL

Solutions:

* Windows: Confirm system supports OpenGL
* Linux: Check SDL2 library installation
* WSL: Need to configure X Server (e.g., VcXsrv)

**Error: Resource file not found**

.. code-block:: text

   Error loading image: assets/logo.bin

Solutions:

* Confirm resource files have been converted to .bin format
* Check file path is correct
* Rebuild project

Performance Optimization
------------------------

Build Speed Optimization
~~~~~~~~~~~~~~~~~~~~~~~~

1. **Parallel Compilation**:

   .. code-block:: bash

      # SCons
      scons -j 8  # Use 8 threads

      # CMake
      make -j 8

2. **Incremental Build**: Build system automatically recompiles only changed files after partial modifications

3. **Use ccache** (Linux):

   .. code-block:: bash

      sudo apt-get install ccache
      export CC="ccache gcc"
      export CXX="ccache g++"

Runtime Performance Optimization
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

1. **Resource Optimization**: Use appropriate image formats and resolutions

2. **Code Optimization**: Avoid time-consuming operations in frequently called callbacks

3. **Compilation Optimization**: Use ``-O2`` or ``-O3`` optimization level for release builds

Continuous Integration
----------------------

Build HoneyGUI projects in CI/CD pipelines:

.. code-block:: yaml

   # GitHub Actions example
   name: Build HoneyGUI
   on: [push, pull_request]

   jobs:
     build:
       runs-on: ubuntu-latest
       steps:
         - uses: actions/checkout@v2

         - name: Install dependencies
           run: |
             sudo apt-get update
             sudo apt-get install -y build-essential libsdl2-dev python3-pip
             pip3 install scons==4.4.0 kconfiglib

         - name: Build project
           run: |
             cd win32_sim
             scons

         - name: Archive artifacts
           uses: actions/upload-artifact@v2
           with:
             name: gui-binary
             path: win32_sim/gui

Next Steps
----------

* Learn :doc:`code_generation` to understand code generation process
* See :doc:`event_system` to implement interaction logic
* Refer to :doc:`resource_management` to manage project resources
* Read :doc:`deployment` for embedded deployment solutions
