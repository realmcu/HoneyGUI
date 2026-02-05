Quick Start
===========

This chapter introduces how to install the HoneyGUI Visual Designer plugin and create your first GUI application.

Installation Requirements
--------------------------

Software Environment
~~~~~~~~~~~~~~~~~~~~

* **VSCode**: Version 1.60.0 or higher
* **Python**: 3.9.7 or higher
* **SCons**: 4.4.0 (install via ``pip install scons==4.4.0``)
* **kconfiglib**: Install via ``pip install kconfiglib``

**Windows Platform**:

* MinGW-w64 (recommended version 8.1.0), installed at ``C:\mingw64``
* CMake 3.31.2+ (optional, for CMake build)

**Linux/WSL Platform**:

* GCC toolchain
* SDL2 development libraries: ``sudo apt-get install libsdl2-dev``

Installing the Plugin
----------------------

Method 1: Install from VSCode Marketplace
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

1. Open VSCode
2. Click the Extensions icon in the activity bar (or press ``Ctrl+Shift+X``)
3. Type **"HoneyGUI Visual Designer"** in the search box
4. Click the **Install** button

Method 2: Install from VSIX File
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

1. Download the plugin's ``.vsix`` file
2. In VSCode, open the Extensions panel
3. Click the **...** menu in the top right corner
4. Select **Install from VSIX...**
5. Choose the downloaded VSIX file

Environment Check
------------------

After installation, the plugin will automatically check the development environment:

1. Open VSCode Command Palette (``Ctrl+Shift+P``)
2. Type and execute ``HoneyGUI: Check Environment``
3. View the terminal output to confirm all dependencies are installed

.. note::
   If the environment check fails, install missing dependencies according to the error messages.

Creating Your First Project
----------------------------

Using Project Templates
~~~~~~~~~~~~~~~~~~~~~~~

1. Click the HoneyGUI icon in the VSCode activity bar
2. Click the **New Project** button in the sidebar
3. Select a project template:

   * **Blank Project**: Design interface from scratch
   * **Watch Project**: Smartwatch interface template (410x502)
   * **Dashboard Project**: Dashboard interface template (800x480)

4. Enter the project name
5. Choose the project save location
6. The plugin will automatically create the project structure and initialize it

.. figure:: resource/create-project.png
   :align: center
   :width: 600px

   Create New Project

Project Structure
~~~~~~~~~~~~~~~~~

The created project contains the following structure:

.. code-block:: text

   my-project/
   ├── project.json          # Project configuration file
   ├── views/                # HML interface files directory
   │   └── main.hml          # Main interface
   ├── src/                  # Generated C code
   │   ├── ui/               # UI structure code (auto-generated)
   │   ├── callbacks/        # Callback function framework (generated once)
   │   └── user/             # User code (manually written)
   ├── assets/               # Resource files directory
   │   ├── images/           # Image resources
   │   ├── fonts/            # Font resources
   │   └── videos/           # Video resources
   └── SConstruct            # SCons build script

Designing Interface
--------------------

Opening the Designer
~~~~~~~~~~~~~~~~~~~~

1. Double-click an ``.hml`` file in the VSCode file explorer
2. The designer will open in the editor area, displaying the visual design interface

.. figure:: resource/design-ui.png
   :align: center
   :width: 800px

   Visual Designer Interface

Designer Interface Overview
~~~~~~~~~~~~~~~~~~~~~~~~~~~

* **Toolbar**: Provides common operations like save, undo, redo
* **Component Library** (left): Contains all available HoneyGUI components
* **Canvas** (center): Design area showing the actual interface
* **Component Tree** (top right): Shows the component hierarchy of the current interface
* **Property Panel** (bottom right): Edit properties of the selected component

Adding Components
~~~~~~~~~~~~~~~~~

1. Select a component from the left component library (e.g., ``hg_button``)
2. Drag it to the canvas center
3. Release the mouse, and the component will be added to the interface
4. Modify component properties in the right property panel:

   * Position and size
   * Text content
   * Colors and styles
   * Event bindings

Basic Operations
~~~~~~~~~~~~~~~~

* **Select component**: Click on a component in the canvas
* **Move component**: Drag the selected component
* **Resize**: Drag the control points on the component edges
* **Delete component**: Press ``Delete`` key after selection
* **Copy/Paste**: ``Ctrl+C`` / ``Ctrl+V``
* **Undo/Redo**: ``Ctrl+Z`` / ``Ctrl+Y``

Generating Code
----------------

Automatic Code Generation
~~~~~~~~~~~~~~~~~~~~~~~~~~

After completing the design, the plugin will automatically generate C code:

1. Click the **Generate Code** button in the toolbar, or use the command palette:

   .. code-block:: text

      Ctrl+Shift+P → HoneyGUI: Generate Code

2. The plugin will scan all HML files in the ``views/`` directory
3. Automatically generate the following files:

   * ``src/ui/*.c`` - UI structure code (regenerated each time)
   * ``src/callbacks/*.c`` - Callback function framework (generated only once)
   * ``src/user/*.c`` - User code (manually written, never overwritten)

Code Protection Zones
~~~~~~~~~~~~~~~~~~~~~

Generated code contains protection zone markers where users can add custom logic:

.. code-block:: c

   // @protected start
   // Add your code here
   void on_button_click(void *obj, gui_event_t *event) {
       printf("Button clicked!\n");
   }
   // @protected end

.. warning::
   Code outside protection zones will be overwritten during regeneration. Always write custom code inside protection zones.

Build and Simulation
---------------------

Building the Project
~~~~~~~~~~~~~~~~~~~~

1. Click the **▶ Build and Simulate** button in the toolbar
2. Or use the command palette: ``HoneyGUI: Build and Simulate``
3. The plugin will automatically:

   * Convert resource files (images, fonts, etc.)
   * Compile C code (using SCons)
   * Launch the simulator

.. figure:: resource/compile.png
   :align: center
   :width: 600px

   Build and Simulation

Running the Simulator
~~~~~~~~~~~~~~~~~~~~~

After successful compilation, the simulator window will pop up automatically, displaying the designed interface.

* **Mouse operations**: Simulate touchscreen
* **Keyboard operations**: Support some shortcuts
* **Real-time debugging**: Debug via GDB or printf

Common Issues
~~~~~~~~~~~~~

**Build Failure**:

1. Check if environment variables are configured correctly
2. Verify MinGW/GCC path is correct
3. Review error messages in the terminal output

**Simulator Won't Start**:

1. Check if SDL2 library is installed
2. On Linux: ``sudo apt-get install libsdl2-dev``
3. On Windows: Ensure MinGW includes SDL2

Next Steps
----------

* Learn :doc:`Designer Usage <designer>` for more design techniques
* Understand :doc:`HML File Format <hml_format>` for advanced customization
* Master :doc:`Event System <event_system>` to implement interactive features
* Explore :doc:`Resource Management <resource_management>` to optimize resource usage
