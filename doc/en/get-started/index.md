# **Welcome to the RTKIOT GUI wiki!**

## Source project download

- Download on GitHub: <https://github.com/realmcu/HoneyGUI>
- Download on Gitee: <https://gitee.com/realmcu/HoneyGUI>

## Description

HoneyGUI is a graphics display framework independently developed by Realtek. It is an open-source embedded graphical user interface (GUI) library specifically designed for resource-constrained microcontrollers and embedded systems. HoneyGUI is lightweight, feature-rich, and highly customizable, making it widely used in consumer electronics, home appliances, medical devices, and smartwatches.

As a comprehensive display framework, HoneyGUI not only includes Realtek's self-developed display engine development tools but also supports direct calls to external APIs such as LVGL and ARM2D for application development. Additionally, HoneyGUI provides a PC-based simulation environment, allowing developers to quickly develop and debug applications without relying on embedded hardware platforms. Furthermore, HoneyGUI can be used in conjunction with Realtek's proprietary front-end design tool, [RVD](/Glossary.rst#RVD), to achieve visual programming.

Here are several common methods for APP development:

- Develop applications using the RealGUI display engine by calling C/C++ APIs.
- Directly call [LVGL](https://lvgl.io/) APIs to develop applications.
- Directly call [ARM-2D](https://github.com/ARM-software/Arm-2D) APIs to develop applications.
- Front-end development using JavaScript and XML. It is recommended to use ```RVisualDesigner``` as a [PC](/Glossary.rst#term-PC)-based design tool for low-code development.

The [GUI](/Glossary.rst#term-GUI) framework has good portability, which can run on a variety of chips and [OS](/Glossary.rst#term-OS). PC Windows version is provided.

## Software Architecture


<div style="text-align: center"><img width= "600" src ="https://foruda.gitee.com/images/1721196540166827807/b601c3a5_13408154.png"/></div><br/>


## Installation

{#Install_compiler_en}
### Install compiler
Download the MinGW-w64 toolchain, unzip it to drive C, and add it to the system environment variable `Path`.
1.  [MinGW-w64 Download](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/Personal%20Builds/mingw-builds/8.1.0/threads-posix/sjlj/x86_64-8.1.0-release-posix-sjlj-rt_v6-rev0.7z)
2.  Unzip and copy to directory: `C:/mingw64`
3.  Add a environment variable: `C:/mingw64/bin`

### Install scons
Open a CMD window and execute the following commands to install the Python scons library.
```
> pip install scons==4.4.0
```

After installing the `MinGW-w64` toolchain and `scons` library, you can launch the application in two ways: stratup by CMD or startup by GUI.

### Startup by CMD
Open a CMD window in the `HoneyGUI` folder, and then run the following command to start the application.

```shell
> cd win32_sim
> scons
> cd ..
> .\win32_sim\gui.exe
```
The `scons` command to perform the build process and then execute `gui.exe` to run it.


<div style="text-align: center"><img width= "600"  src ="https://foruda.gitee.com/images/1718704649306452668/282ac763_13408154.png"/></div><br/>


### Startup by VSCode

#### Install VSCode
   - [Download VSCode](https://code.visualstudio.com/)
   - [Install C/C++ plug-in](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)

#### Open project
   - Click `HoneyGUI.code-workspace` file

#### Run project
You can select the `Run and Debug` options after entering the vscode interface, and then click the `Run` button.


<div style="text-align: center"><img width="300" src ="https://foruda.gitee.com/images/1699582639386992543/b2078d27_13671125.png"/></div><br/>


### Display the watchface
You can interact with it by swiping and long pressing.


<div style="text-align: center"><img width="300" src ="https://foruda.gitee.com/images/1721095451242922178/f43e885b_1860080.png"/></div><br/>
