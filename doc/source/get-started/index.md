# **Welcome to the RTKIOT GUI wiki!!**
## Description

There are two ways to develop an application, SaaA(Script as a APP) and Native C API.
- SaaA(Script as a APP) consists of JavaScript and XML. Recommended ```RVisualDesigner``` as a  PC designer to do low-code development.
- Native C API development using C/C++ language, directly call the API of GUI engine.
- Also can directly call the API of [LVGL](https://lvgl.io/).

The GUI framework has good portability, which can run on a variety of chips and OS.  PC Windows version is provided.

## Software Architecture
<div align=center><img src ="https://foruda.gitee.com/images/1698215110757026070/a3648515_10088396.png"/></div>

## Installation

### <small>Install compiler</small>
Download the mingw-w64 compiler suite, unzip it to drive C, and add the environment variable.
1.  [Mingw64 Download](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/Personal%20Builds/mingw-builds/8.1.0/threads-posix/sjlj/x86_64-8.1.0-release-posix-sjlj-rt_v6-rev0.7z)
2.  Unzip and copy to directory:C:/mingw64
3.  Add a environment variable:/mingw64/bin

### <small>Install scons</small>
Open a CMD window and execute the following commands to install the Python scons library.
- ```pip install scons==4.4.0```

After installing the `mingw-w64` compiler environment and `scons` library, you can launch the application in two ways: stratup by CMD or startup by GUI.

### <small>Startup by CMD</small>
Open a CMD window in the `HoneyGUI` folder, and then run the following command to start the application.

```shell
> cd win32_sim
> scons
> cd ..
> .\win32_sim\gui.exe
```
The `scons` command to perform the build process and then execute `gui.exe` to run it.

<div align=center><img src ="https://foruda.gitee.com/images/1699429582680903862/9a73ac31_13671125.png"/></div>

### <small>Startup by GUI</small>

1. Install VSCODE
   - [```Install VSCODE```](https://code.visualstudio.com/)
   - [```Install C/C++```](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
   
2. Open project
   - Click ```HoneyGUI.code-workspace``` file

3. Run project
You can select the Run and Debug options after entering the vscode interface, and then click the `Run button`.
<div align=center><img width= "625" height="474" src ="https://foruda.gitee.com/images/1699345834874402209/42615323_13671125.png"/></div>

### Display the watchface
You can interact with it by swiping and long pressing.
<div align=center><img src ="https://foruda.gitee.com/images/1698286583110259632/b48ad0af_10088396.png"/></div>