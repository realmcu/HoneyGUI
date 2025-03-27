.. _Library_Porting_EN:

================
HoneyGUI Porting
================

HoneyGUI is a lightweight embedded GUI system optimized for Realtek chips. This document will guide you through compiling the HoneyGUI library on different Realtek chip platforms, including configuration and usage methods for both Armclang and Armcc compiler environments.

Important Notes
----------------

Environment Setup
~~~~~~~~~~~~~~~~~~

- Ensure Keil MDK development environment is properly installed.
- Verify CMake (version 3.15 or above) is installed and added to system PATH.
- Verify all dependent libraries are correctly installed before compilation.

Compiler Selection
~~~~~~~~~~~~~~~~~~~

- Keil AC5 projects: Must use armcc compiler to build library.
- Keil AC6 projects: Must use armclang compiler to build library.
- Ensure compiler version matches project settings exactly.

Path Configuration
~~~~~~~~~~~~~~~~~~~

- Armcc compiler path verification:
    - Default location: ``C:/Keil_v5/ARM/ARMCC/bin``.
    - If installation location differs, update path in CMake configuration.
- Armclang compiler path verification:
    - Default location: ``C:/Keil_v5/ARM/ArmCompilerforEmbedded6.22/bin``.
    - If installation location differs, update path in CMake configuration.

Common Issues
~~~~~~~~~~~~~~

- When compilation errors occur, first check:
    - Chip model is correctly specified.
    - Compiler path is properly configured.
    - Compiler version matches project requirements.
- Ensure all necessary environment variables are correctly set

Environment Requirements
------------------------
- CMake 3.15 or above.
- Keil MDK 5 or above.
- Windows operating system

Armcc Compilation
------------------
Supported Chips:
  - RTL8773E (default)
  - RTL8763E
  - RTL8762G
  - RTL8763D

Compilation Steps:

1. Open cmd window in the armcc directory of project path, generate build files by running :kbd:`cmake -G "MinGW Makefiles" -DSOC=RTL8763D -B "./temp"`:

.. code-block:: console
    :emphasize-lines: 1

    E:\HoneyGUI\lib\armcc>cmake -G "MinGW Makefiles" -DSOC=RTL8763D -B "./temp"
    soc = RTL8763D
    -- The C compiler identification is ARMCC 5.6.960
    -- The CXX compiler identification is ARMCC 5.6.960
    -- Detecting C compiler ABI info
    -- Detecting C compiler ABI info - done
    ...
    -- Configuring done (2.7s)
    -- Generating done (0.9s)
    -- Build files have been written to: E:/HoneyGUI/lib/armcc/temp

.. note::
    If chip model is not specified, defaults to RTL8773E.

2. Enter temp directory and compile project by running :kbd:`cd temp` :kbd:`cmake --build .`:

.. code-block:: console
    :emphasize-lines: 1,2

    E:\HoneyGUI\lib\armcc>cd temp
    E:\HoneyGUI\lib\armcc\temp>cmake --build .
    [  1%] Building C object CMakeFiles/gui.dir/E_/HoneyGUI/realgui/3rd/cJSON/cJSON.o
    [  2%] Building C object CMakeFiles/gui.dir/E_/HoneyGUI/realgui/3rd/ezXML/ezxml.o
    [  3%] Building C object CMakeFiles/gui.dir/E_/HoneyGUI/realgui/3rd/nanovg/base/nanovg.o
    ...
    [100%] Linking C static library gui.lib
    [100%] Built target gui

3. Install resources by running :kbd:`cmake --build . --target install`:

.. code-block:: console
    :emphasize-lines: 1

    E:\HoneyGUI\lib\armcc\temp>cmake --build . --target install
    [100%] Built target gui
    Install the project...
    -- Install configuration: ""
    -- Installing: E:/HoneyGUI/lib/armcc/install/lib/gui.lib
    ...

4. Generated resource file locations:
    - Header files: ``<your HoneyGUI dir>/lib/armcc/install/include``.
    - Library file: ``<your HoneyGUI dir>/lib/armcc/install/lib/gui.lib``.

Armclang Compilation
---------------------
Supported Chips:
  - RTL8762G (default)
  - RTL8762D
  - RTL8773E
  - RTL8773G

Compilation Steps:

1. Open cmd window in the armclang directory of project path, generate build files by running :kbd:`cmake -G "MinGW Makefiles" -DSOC=RTL8762G -B "./temp"`:

.. code-block:: console
    :emphasize-lines: 1

    E:\HoneyGUI\lib\armclang>cmake -G "MinGW Makefiles" -DSOC=RTL8762G -B "./temp"
    soc = RTL8762G
    -- The C compiler identification is ARMClang
    -- The CXX compiler identification is ARMClang
    ...
    -- Configuring done
    -- Generating done
    -- Build files have been written to: E:/HoneyGUI/lib/armclang/temp

.. note::
    If chip model is not specified, defaults to RTL8762G.

2. Enter temp directory and compile project by running :kbd:`cd temp` :kbd:`cmake --build .`:

.. code-block:: console
    :emphasize-lines: 1,2

    E:\HoneyGUI\lib\armclang>cd temp
    E:\HoneyGUI\lib\armclang\temp>cmake --build .
    [  0%] Building C object CMakeFiles/gui.dir/...
    ...
    [100%] Built target gui

3. Install resources by running :kbd:`cmake --build . --target install`:

.. code-block:: console
    :emphasize-lines: 1

    E:\HoneyGUI\lib\armclang\temp>cmake --build . --target install
    [100%] Built target gui
    Install the project...
    -- Installing: E:/HoneyGUI/lib/armclang/install/lib/gui.lib
    ...

4. Generated resource file locations:
    - Header files: ``<your HoneyGUI dir>/lib/armclang/install/include``.
    - Library file: ``<your HoneyGUI dir>/lib/armclang/install/lib/gui.lib``.

Project Porting Example
------------------------

Using RTL8773GWP dashboard project as an example.

1. Copy generated resource files to project directory:

   - Copy header files (.h) to project resource directory.
   - Copy library file (gui.lib) to project resource directory.
   
2. Project Configuration:

   - Add header file paths in Keil MDK.
   - Link gui.lib library in project settings.

.. figure:: https://foruda.gitee.com/images/1741674292411402494/07c72a64_13406851.jpeg
   :align: center
   :width: 800px

   Link header file paths to project directory

.. figure:: https://foruda.gitee.com/images/1741657914099825115/216c4d69_13406851.jpeg
   :align: center
   :width: 800px

   Link library file to project directory