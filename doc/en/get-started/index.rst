.. _Get Started:

=================
Get Started
=================

Source Project Download
-----------------------

- Download on GitHub: `https://github.com/realmcu/HoneyGUI <https://github.com/realmcu/HoneyGUI>`_
- Download on Gitee: `https://gitee.com/realmcu/HoneyGUI <https://gitee.com/realmcu/HoneyGUI>`_

Description
-----------

HoneyGUI is a graphics display framework independently developed by Realtek. It is an open-source embedded graphical user interface (:term:`GUI`) library specifically designed for resource-constrained microcontrollers and embedded systems. HoneyGUI is lightweight, feature-rich, and highly customizable, making it widely used in consumer electronics, home appliances, medical devices, and smartwatches.

HoneyGUI provides a :term:`PC`-based simulation environment, allowing developers to quickly develop and debug applications without relying on embedded hardware platforms. Furthermore, HoneyGUI can be used in conjunction with Realtek's proprietary front-end design tool, :term:`RVD`, to achieve visual programming.

HoneyGUI currently primarily supports the following two efficient :term:`APP` development methods:

1. C/C++ :term:`API` development

   - Developers can directly utilize the C/C++ API based on the RealGUI display engine to construct applications.
   - This approach offers the highest level of flexibility and system performance control, suitable for complex applications with high performance requirements.

2. Front-end low-code development

   - Develop front-end applications using JavaScript and XML.
   - It is recommended to use ``RVisualDesigner`` to quickly build beautiful :term:`UI` interfaces through dragging and dropping components and visual configuration, enabling low-code development and significantly improving development efficiency.
   - The low-code development approach for the front end offers advantages in rapid development and iteration, making it particularly suitable for rapid prototyping and small-scale application development.

The HoneyGUI framework has good portability, which can run on a variety of chips and :term:`OS`. PC Windows version is provided.

Software Architecture
-----------------------

.. figure:: https://foruda.gitee.com/images/1753340174900062921/e20848cb_10737458.png
   :align: center
   :width: 800

   HoneyGUI Software Architecture


Installation For Windows
-------------------------


Install Compiler
^^^^^^^^^^^^^^^^

Download the MinGW-w64 toolchain, unzip it to drive C, and add it to the system environment variable ``Path``.

1.  `MinGW-w64 Download <https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/Personal%20Builds/mingw-builds/8.1.0/threads-posix/sjlj/x86_64-8.1.0-release-posix-sjlj-rt_v6-rev0.7z>`_
2.  Unzip and copy to directory: ``C:\mingw64``
3.  Add a environment variable: ``C:\mingw64\bin``

   - Open the Start Menu and search for ``Advanced system setting``.
   - Show ``System Properties`` and then go to the ``Advanced`` tab.
   - Click on the ``Environment Variables`` button.
   - In the ``User variables`` section, find and select the Path variable and click ``Edit``.
   - Click ``New`` and add ``C:\mingw64\bin``.
   - Click ``OK`` to close all dialogs.


Install Python
^^^^^^^^^^^^^^^^

Python 3.9.7 is tested.

Install Scons
^^^^^^^^^^^^^^^^

Open a CMD window and execute the following commands to install the Python scons library.

.. code-block:: shell

   > pip install scons==4.4.0

Install Kconfiglib
^^^^^^^^^^^^^^^^^^^

Install kconfiglib for configuration management:

.. code-block:: shell

   > pip install kconfiglib

Configure Application (menuconfig)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Before compiling, you can use the ``menuconfig`` graphical configuration tool to select the example application to run. This configuration is common to both **SCons** and **CMake** build systems.

Using menuconfig for Configuration
"""""""""""""""""""""""""""""""""""

Execute the following commands in the ``win32_sim`` directory:

.. code-block:: shell

   > cd win32_sim
   > menuconfig ../Kconfig.gui

After executing the command, a graphical configuration interface will open:

.. figure:: https://foruda.gitee.com/images/1764320806391041617/417b0f55_13406851.png
   :align: center
   :width: 600

   menuconfig Configuration Interface

Operation Instructions
""""""""""""""""""""""

1. Use **arrow keys** to move up and down to select items
2. Use **space bar** or **enter key** to cancel/select configuration items
3. Select ``Select HoneyGUI Demo (Watch Turnkey Demo 410x502)`` to enter application configuration
4. Select the example application to run:

   - ``Watch Turnkey Demo 410x502``: Watch application example (410x502 resolution)
   - ``GUI Demo 800x480``: Dashboard application example (800x480 resolution)
   - Other available example applications

.. figure:: https://foruda.gitee.com/images/1764321915931205772/1248fc4d_13406851.png
   :align: center
   :width: 600

   Menuconfig APP Configuration Interface

5. Press **ESC** key to return to the previous menu
6. Select ``< s >`` to save the configuration to the ``.config`` file
7. Select ``< q >`` to exit the configuration interface

Configuration File Description
""""""""""""""""""""""""""""""

After configuration, the ``.config`` file will be automatically generated in the ``HoneyGUI\win32_sim`` directory. The build system (SCons or CMake) will automatically read this configuration file and compile the corresponding application based on the configuration.

.. note::

   If menuconfig is not used for configuration, the build system will use the default configuration or an existing ``.config`` file.

After installing the ``MinGW-w64`` toolchain and ``scons`` library, you can start the application in several ways.

Startup by CMD (Scons)
^^^^^^^^^^^^^^^^^^^^^^^^
Open a CMD window in the ``HoneyGUI`` folder, and then run the following command to start the application.

.. code-block:: shell

   > cd win32_sim
   > scons
   > cd ..
   > .\win32_sim\gui.exe

The ``scons`` command to perform the build process and then execute ``gui.exe`` to run it.


.. figure:: https://foruda.gitee.com/images/1718704649306452668/282ac763_13408154.png
   :align: center
   :width: 700

   Run Application


Startup by CMD (CMake)
^^^^^^^^^^^^^^^^^^^^^^^

Dependencies
""""""""""""
    
**CMake** (tested with version 3.31.2): `https://cmake.org/download/ <https://cmake.org/download/>`_

**MinGW-w64**: As mentioned earlier

Initializing the Build Environment
"""""""""""""""""""""""""""""""""""

In the ``HoneyGUI`` folder, execute:

.. code-block:: shell

   > cd win32_sim
   > mkdir build
   > cd build
   > cmake -G "MinGW Makefiles" ..

Configuring the Application
""""""""""""""""""""""""""""

If you need to change the application configuration, refer to the previous ``Configure Application (menuconfig)`` section and execute the ``menuconfig ../Kconfig.gui`` command in the ``win32_sim`` directory.

Compiling the Project
"""""""""""""""""""""

In the ``HoneyGUI\win32_sim\build`` folder, execute:

.. code-block:: shell
      
   > cmake -G "MinGW Makefiles" ..
   > mingw32-make -j 32

During compilation, CMake will automatically:

- Read the configuration from the ``.config`` file
- Select the corresponding application source code based on the configuration
- Automatically process the binary resource files (``.bin`` files) required by the application
- Generate the executable file ``gui.exe``

Running the Program
"""""""""""""""""""

In the ``HoneyGUI`` folder, execute:

.. code-block:: shell
   
   > .\win32_sim\build\gui.exe



Startup by VSCode
^^^^^^^^^^^^^^^^^^^^

Install VSCode
"""""""""""""""

- `Download VSCode <https://code.visualstudio.com/>`_
- `Install C/C++ plug-in <https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools>`_

Open Project
"""""""""""""""

- Click ``HoneyGUI.code-workspace`` file

Run Project
"""""""""""""""

You can select the ``Run and Debug`` options after entering the vscode interface, and then click the ``Run`` button.


.. figure:: https://foruda.gitee.com/images/1699582639386992543/b2078d27_13671125.png
   :align: center
   :width: 400

   VSCode Run Project


Display
^^^^^^^^

Watch Project
"""""""""""""""

The watchface is displayed in the window, and you can interact with it by swiping and clicking.


.. figure:: https://docs.realmcu.com/HoneyGUI/image/gui_watch.gif
   :align: center
   :width: 400

   Watch Project Example

Dashboard Project
"""""""""""""""""""

The dashboard is displayed in the window.

.. figure:: https://foruda.gitee.com/images/1753340204000227545/39228f04_10737458.png
   :align: center
   :width: 400

   Dashboard Project Example
