# **Welcome to the RTKIOT GUI wiki!!**

## Source project download

- Download on GitHub: <https://github.com/realmcu/HoneyGUI>
- Download on Gitee: <https://gitee.com/realmcu/HoneyGUI>

## Description

There are two ways to develop an application, SaaA(Script as a APP) and Native C-APP.
- SaaA(Script as a APP) consists of JavaScript and XML. Recommended ```RVisualDesigner``` as a  PC designer to do low-code development.
- Native C-APP development using C/C++ language, directly call the API of GUI engine.
- Also can directly call the API of [LVGL](https://lvgl.io/).

The GUI framework has good portability, which can run on a variety of chips and OS. PC Windows version is provided.

## Software Architecture


<div style="text-align: center"><img width= "600" src ="https://foruda.gitee.com/images/1698215110757026070/a3648515_10088396.png"/></div><br/>


## Installation

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


<div style="text-align: center"><img width= "600"  src ="https://foruda.gitee.com/images/1699429582680903862/9a73ac31_13671125.png"/></div><br/>


### Startup by GUI

#### 1. Install VSCode
   - [Download VSCode](https://code.visualstudio.com/)
   - [Install C/C++ plug-in](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)

#### 2. Open project
   - Click `HoneyGUI.code-workspace` file

#### 3. Run project
You can select the `Run and Debug` options after entering the vscode interface, and then click the `Run button`.


<div style="text-align: center"><img width="300" src ="https://foruda.gitee.com/images/1699582639386992543/b2078d27_13671125.png"/></div><br/>


### Display the watchface
You can interact with it by swiping and long pressing.


<div style="text-align: center"><img width="300" src ="https://foruda.gitee.com/images/1698286583110259632/b48ad0af_10088396.png"/></div><br/>
