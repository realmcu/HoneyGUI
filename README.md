# **Welcome to the RTKIOT GUI wiki!!**

<br>
<div style="text-align: center"><img src="https://foruda.gitee.com/images/1699931734086261442/f0690691_10641540.png" width = "300" /></div>
<br>
<div style="text-align: center"><img src="https://foruda.gitee.com/images/1723621216978994553/76b62adc_10737458.gif" width = "300" /></div>
<br>
<div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/Calculator/calculator.gif" width = "300" /></div>
<br>
<div style="text-align: center"><img src="https://foruda.gitee.com/images/1725848489900237181/97911fc4_13408154.gif" width = "300" /></div>
<br>
<div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/GUI-APP/Switch_APP/switch_app.gif" width = "300" /></div>
<br>

## Description

Here are several common methods for APP development.
- Develop applications using the RealGUI display engine by calling C/C++ APIs.
- Directly call [LVGL](https://lvgl.io/) APIs to develop applications.
- Directly call [ARM-2D](https://github.com/ARM-software/Arm-2D) APIs to develop applications.
- SaaA (Script as a APP) consists of JavaScript and XML. Recommended ```RVisualDesigner``` as a  PC designer to do low-code development.

The GUI framework has good portability, which can run on a variety of chips and OS. PC Windows version is provided.

## Software Architecture


<div align=center><img width= "600" src ="https://foruda.gitee.com/images/1721196540166827807/b601c3a5_13408154.png"/></div><br/>


## Installation

### Install compiler
Download the mingw-w64 compiler suite, unzip it to drive C, and add the environment variable.
1.  [Mingw64 Download](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/Personal%20Builds/mingw-builds/8.1.0/threads-posix/sjlj/x86_64-8.1.0-release-posix-sjlj-rt_v6-rev0.7z)
2.  Unzip and copy to directory: `C:/mingw64`
3.  Add a environment variable: `C:/mingw64/bin`

### Install scons
Open a CMD window and execute the following commands to install the Python scons library.
```
> pip install scons==4.4.0
```

After installing the `mingw-w64` compiler environment and `scons` library, you can launch the application in two ways: stratup by CMD or startup by GUI.

### Startup by CMD
Open a CMD window in the `HoneyGUI` folder, and then run the following command to start the application.

```shell
> cd win32_sim
> scons
> cd ..
> .\win32_sim\gui.exe
```
The `scons` command to perform the build process and then execute `gui.exe` to run it.


<div align=center><img width= "600"  src ="https://foruda.gitee.com/images/1718704649306452668/282ac763_13408154.png"/></div><br/>


### Startup by VSCode

#### Install VSCode
   - [```Install VSCODE```](https://code.visualstudio.com/)
   - [```Install C/C++```](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)

#### Open project
   - Click ```HoneyGUI.code-workspace``` file

#### Run project
You can select the ```Run and Debug``` options after entering the vscode interface, and then click the ```Run``` button.


<div align=center><img width="300" src ="https://foruda.gitee.com/images/1699582639386992543/b2078d27_13671125.png"/></div><br/>


### Display the watchface
You can interact with it by swiping and long pressing.


<div style="text-align: center"><img width="300" src ="https://foruda.gitee.com/images/1721095451242922178/f43e885b_1860080.png"/></div><br/>
