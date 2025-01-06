=================
Get Started
=================

Source Project Download
-----------------------

- Download on GitHub: <https://github.com/realmcu/HoneyGUI>
- Download on Gitee: <https://gitee.com/realmcu/HoneyGUI>

Description
-----------

HoneyGUI is a graphics display framework independently developed by Realtek. It is an open-source embedded graphical user interface (GUI) library specifically designed for resource-constrained microcontrollers and embedded systems. HoneyGUI is lightweight, feature-rich, and highly customizable, making it widely used in consumer electronics, home appliances, medical devices, and smartwatches.

As a comprehensive display framework, HoneyGUI not only includes Realtek's self-developed display engine but also supports direct calls to external APIs such as LVGL and ARM2D for application development. Additionally, HoneyGUI provides a PC-based simulation environment, allowing developers to quickly develop and debug applications without relying on embedded hardware platforms. Furthermore, HoneyGUI can be used in conjunction with Realtek's proprietary front-end design tool, [RVD](/GUI-APP/SaaA/index.rst#RVD), to achieve visual programming.

Here are several common methods for APP development:

- Develop applications using the RealGUI display engine by calling C/C++ APIs.
- Directly call [LVGL](https://lvgl.io/) APIs to develop applications.
- Directly call [ARM-2D](https://github.com/ARM-software/Arm-2D) APIs to develop applications.
- Front-end development using JavaScript and XML. It is recommended to use ```RVisualDesigner``` as a [PC](/Glossary.rst#term-PC)-based design tool for low-code development.

The [GUI](/Glossary.rst#term-GUI) framework has good portability, which can run on a variety of chips and [OS](/Glossary.rst#term-OS). PC Windows version is provided.

Software Architecture
-----------------------


<div style="text-align: center"><img width= "800" src ="https://foruda.gitee.com/images/1721196540166827807/b601c3a5_13408154.png"/></div><br/>


Installation For Windows
-------------------------


Install Compiler
^^^^^^^^^^^^^^^^

Download the MinGW-w64 toolchain, unzip it to drive C, and add it to the system environment variable `Path`.
1.  [MinGW-w64 Download](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/Personal%20Builds/mingw-builds/8.1.0/threads-posix/sjlj/x86_64-8.1.0-release-posix-sjlj-rt_v6-rev0.7z)
2.  Unzip and copy to directory: `C:\mingw64`
3.  Add a environment variable: `C:\mingw64\bin`:
      - Open the Start Menu and search for `Advanced system setting`.
      - Show `System Properties` and then go to the `Advanced` tab.
      - Click on the `Environment Variables` button.
      - In the `User variables` section, find and select the Path variable and click `Edit`.
      - Click `New` and add `C:\mingw64\bin`.
      - Click `OK` to close all dialogs.


Install Python
^^^^^^^^^^^^^^^^

Python 3.9.7 is tested.

Install Scons
^^^^^^^^^^^^^^^^

Open a CMD window and execute the following commands to install the Python scons library.
```
> pip install scons==4.4.0
```

After installing the `MinGW-w64` toolchain and `scons` library, you can launch the application in two ways: stratup by CMD or startup by GUI.

### Startup by CMD (scons)
Open a CMD window in the `HoneyGUI` or `gui` folder, and then run the following command to start the application.

.. code-block:: shell

   > cd win32_sim
   > scons
   > cd ..
   > .\win32_sim\gui.exe

The `scons` command to perform the build process and then execute `gui.exe` to run it.


.. image:: https://foruda.gitee.com/images/1718704649306452668/282ac763_13408154.png
   :align: center
   :width: 700

Startup by CMD (CMake)
^^^^^^^^^^^^^^^^^^^^^^^

- Dependency Software
    
     **CMake** (tested with version 3.31.2): [Download here](https://cmake.org/download/)
    
     **MinGW-w64**: mentioned before
- Initialization: In the ```HoneyGUI``` folder

.. code-block:: shell

   > cd win32_sim
   > mkdir build
   > cd build
   > cmake -G "MinGW Makefiles" ..

- Compilation: In the ```HoneyGUI/win32_sim/build``` folder

.. code-block:: shell
      
   > cmake -G "MinGW Makefiles" ..
   > mingw32-make -j 32

- Configuration: In the ```HoneyGUI/win32_sim/build``` folder

.. code-block:: shell
   > cmake --build . --target menuconfig

- Run: In the ```HoneyGUI``` folder

.. code-block:: shell
   > .\win32_sim\gui.exe



Startup by VSCode
^^^^^^^^^^^^^^^^^^^^

Install VSCode
"""""""""""""""

- `Download VSCode <https://code.visualstudio.com/>`_
- `Install C/C++ plug-in <https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools>`_

Open Project
"""""""""""""""

   - Click `HoneyGUI.code-workspace` file

Run Project
"""""""""""""""

You can select the `Run and Debug` options after entering the vscode interface, and then click the `Run` button.


<div style="text-align: center"><img width="400" src ="https://foruda.gitee.com/images/1699582639386992543/b2078d27_13671125.png"/></div><br/>




Display
^^^^^^^^

Watch Project
"""""""""""""""

The watchface is displayed in the window, and you can interact with it by swiping and long pressing.


<div style="text-align: center"><img width="400" src ="https://foruda.gitee.com/images/1721095451242922178/f43e885b_1860080.png"/></div><br/>

Dashboard Project
"""""""""""""""

The dashboard is displayed in the window.


<div style="text-align: center"><img width="400" src ="https://foruda.gitee.com/images/1731649605724335535/042f1dcf_10088396.png"/></div><br/>
