# **Welcome to the RTKIOT GUI wiki!!** 
## Description

- There are two ways to develop an application, SaaA(Script as a APP) and Native C API.
- SaaA(Script as a APP) consists of JavaScript and XML. Recommended ```RVisualDesigner``` as a  PC designer to do low-code development.
- Native C API development using C/C++ language, directly call the API of GUI engine.
- Also can directly call the API of [LVGL](https://lvgl.io/).
- The GUI framework has good portability, which can run on a variety of chips and OS.  PC Windows version is provided.
## Software Architecture

![sa](https://foruda.gitee.com/images/1698215110757026070/a3648515_10088396.png "sa")

## Installation

### Install compiler
1.  [Mingw64 Download](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/Personal%20Builds/mingw-builds/8.1.0/threads-posix/sjlj/x86_64-8.1.0-release-posix-sjlj-rt_v6-rev0.7z)
2.  Unzip and copy to directory:C:/mingw64
3.  Add a environment variable:/mingw64/bin
### Install scons
1.  ```pip install scons==4.4.0```

### Startup by CMD 
```shell
> cd win32_sim
> scons
> cd ..
> .\win32_sim\gui.exe
```

### Startup by GUI


#### Install VSCODE
1.  [Install VSCODE](https://code.visualstudio.com/)
2.  [Install C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
#### Open project
1.  Click ```HoneyGUI.code-workspace``` file
#### Run project
   ![Image description](https://foruda.gitee.com/images/1673343789273016802/9126e625_10088396.png "png")
#### Display the watchface. You can swipe and long press to interact
![Image description](https://foruda.gitee.com/images/1698286583110259632/b48ad0af_10088396.png "watchface.PNG")
