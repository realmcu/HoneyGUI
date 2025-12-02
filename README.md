# **Welcome to HoneyGUI !**

[EN](README.md)   |  [中文](README_zh.md)

For detailed information, please refer to [HoneyGUI Online Documentation](https://docs.realmcu.com/gui/en/latest/index.html).

## Examples
<table>
  <tr>
    <td align="center"><img src="https://foruda.gitee.com/images/1699931734086261442/f0690691_10641540.png"width = "200" height = "250"/><br><sub><b>Dynamic Dial</b></sub>
    <td align="center"><img src="https://foruda.gitee.com/images/1723621216978994553/76b62adc_10737458.gif"width = "200" height = "250"/><br><sub><b>Fruit Ninja</b></sub>
    <td align="center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/Calculator/calculator.gif"width = "200" height = "250"/><br><sub><b>Calculator</b></sub>
  </tr>
  <tr>
    <td align="center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/watchface.gif" width = "200" height = "250"/><br><sub><b>Watch Face</b></sub>
    <td align="center"><img src="https://foruda.gitee.com/images/1734073855265837536/f8a3808e_13408154.gif"width = "200" height = "250"/><br><sub><b>Timer</b></sub>
    <td align="center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/musicPlayer.gif"width = "200" height = "250"/><br><sub><b>Music Player</b></sub>
  </tr>
  <tr>
    <td align="center"><img src="https://foruda.gitee.com/images/1725848489900237181/97911fc4_13408154.gif"width = "200" height = "250"/><br><sub><b>Vector Graphics</b></sub>
    <td align="center"><img src="https://foruda.gitee.com/images/1734070660330786955/61e4ff9d_13408154.gif"width = "200" height = "250"/><br><sub><b>3D Butterfly</b></sub>
    <td align="center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/soccer.gif"width = "200" height = "250"/><br><sub><b>2.5D Soccer</b></sub>
    
  </tr>
  
</table>


## Introduction

HoneyGUI is a graphics display framework independently developed by Realtek. It is an open-source embedded graphical user interface (GUI) library specifically designed for resource-constrained microcontrollers and embedded systems. HoneyGUI is lightweight, feature-rich, and highly customizable, making it widely used in consumer electronics, home appliances, medical devices, and smartwatches.

HoneyGUI provides a `PC`-based simulation environment, allowing developers to quickly develop and debug applications without relying on embedded hardware platforms. Furthermore, HoneyGUI can be used in conjunction with Realtek's proprietary front-end design tool `RVD` to achieve visual programming.

HoneyGUI currently supports the following two efficient APP development methods:

1. C/C++ API development

   - Developers can directly utilize the C/C++ API based on the RealGUI display engine to construct applications.
   - This approach offers the highest level of flexibility and system performance control, suitable for complex applications with high performance requirements.

2. Front-end low-code development

   - Develop front-end applications using JavaScript and XML.
   - It is recommended to use ``RVisualDesigner`` to quickly build beautiful UI interfaces through dragging and dropping components and visual configuration, enabling low-code development and significantly improving development efficiency.
   - The low-code development approach for the front end offers advantages in rapid development and iteration, making it particularly suitable for rapid prototyping and small-scale application development.

The HoneyGUI framework has excellent portability and can run on various chips and operating systems. This release provides a PC Windows version.

## Software Architecture


<div align="center">
  <img src="https://foruda.gitee.com/images/1753340174900062921/e20848cb_10737458.png" width="600" alt="HoneyGUI Software Architecture">
  <p><em>HoneyGUI Software Architecture</em></p>
</div>


## Installation on Windows

### Install Compiler

Download the MinGW-w64 toolchain, extract it to drive C, and add it to the system environment variable `Path`.

1. [MinGW-w64 Download](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/Personal%20Builds/mingw-builds/8.1.0/threads-posix/sjlj/x86_64-8.1.0-release-posix-sjlj-rt_v6-rev0.7z)
2. Extract and copy to directory: `C:\mingw64`
3. Add an environment variable: `C:\mingw64\bin`
   - Open the Start Menu and search for `Advanced system settings`.
   - Display `System Properties` and then go to the `Advanced` tab.
   - Click the `Environment Variables` button.
   - In the `User variables` section, find and select the `Path` variable, then click `Edit`.
   - Click `New` and add `C:\mingw64\bin`.
   - Click `OK` to close all dialogs.

### Install Python

Python 3.9.7 has been tested.

### Install Scons

Open a CMD window and execute the following command to install the Python scons library:

```shell
> pip install scons==4.4.0
```

### Install Kconfiglib

Install kconfiglib for configuration management:

```shell
> pip install kconfiglib
```

### Configure Application (Menuconfig)

Before compiling, you can use the `menuconfig` graphical configuration tool to select the sample application to run. This configuration is common to both **SCons** and **CMake** build systems.

#### Using Menuconfig for Configuration

Execute the following commands in the `win32_sim` directory:

```shell
> cd win32_sim
> menuconfig ../Kconfig.gui
```

After executing the command, the graphical configuration interface will open:

<div align="center">
  <img src="https://foruda.gitee.com/images/1764320806391041617/417b0f55_13406851.png" width="600" alt="Menuconfig Configuration Interface">
  <p><em>Menuconfig Configuration Interface</em></p>
</div>

#### Operation Instructions

1. Use **arrow keys** to move up and down to select items
2. Use **spacebar** or **Enter key** to deselect/select configuration items
3. Select `Select HoneyGUI Demo (Watch Turnkey Demo 410x502)` to enter application configuration
4. Select the sample application to run:

   - `Watch Turnkey Demo 410x502`: Watch application example (410x502 resolution)
   - `GUI Demo 800x480`: Dashboard application example (800x480 resolution)
   - Other available sample applications

<div align="center">
  <img src="https://foruda.gitee.com/images/1764321915931205772/1248fc4d_13406851.png" width="600" alt="Menuconfig Application Configuration Interface">
  <p><em>Menuconfig Application Configuration Interface</em></p>
</div>

5. Press **ESC** key to return to the previous menu
6. Select `< s >` to save configuration to `.config` file
7. Select `< q >` to exit the configuration interface

#### Configuration File Description

After configuration is complete, a `.config` file will be automatically generated in the `HoneyGUI\win32_sim` directory. The build system (SCons or CMake) will automatically read this configuration file and compile the corresponding application according to the configuration.

> **Note:** If you do not use menuconfig for configuration, the build system will use the default configuration or an existing `.config` file.

After installing the `MinGW-w64` toolchain and `scons` library, you can launch the application through multiple methods.

### Startup by CMD (Scons)

Open a CMD window in the `HoneyGUI` folder, then run the following commands to start the application.

```shell
> cd win32_sim
> scons
> cd ..
> .\win32_sim\gui.exe
```

The `scons` command performs the build process, then execute `gui.exe` to run the application.

<div align="center">
  <img src="https://foruda.gitee.com/images/1718704649306452668/282ac763_13408154.png" width="700" alt="Launch Application">
  <p><em>Launch Application</em></p>
</div>

### Startup by CMD (CMake)

#### Dependencies

- **CMake** (tested version 3.31.2): [https://cmake.org/download/](https://cmake.org/download/)

- **MinGW-w64**: As described above

#### Initialize Build Environment

Execute in the `HoneyGUI` folder:

```shell
> cd win32_sim
> mkdir build
> cd build
> cmake -G "MinGW Makefiles" ..
```

#### Configure Application

To change the application configuration, refer to the **Configure Application (Menuconfig)** section above, and execute the `menuconfig ../Kconfig.gui` command in the `win32_sim` directory.

#### Compile Project

Execute in the `HoneyGUI\win32_sim\build` folder:

```shell
> cmake -G "MinGW Makefiles" ..
> mingw32-make -j 32
```

During compilation, CMake will automatically:

- Read the configuration from the `.config` file
- Select the corresponding application source code based on the configuration
- Automatically process binary resource files (`.bin` files) required by the application
- Generate the executable file `gui.exe`

#### Run Program

Execute in the `HoneyGUI` folder:

```shell
> .\win32_sim\build\gui.exe
```

### Startup by VSCode

#### Install VSCode

- [Download VSCode](https://code.visualstudio.com/)
- [Install C/C++ Extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)

#### Open Project

- Click the `HoneyGUI.code-workspace` file

#### Run Project

After entering the VSCode interface, you can select the `Run and Debug` option, then click the `Run` button.

<div align="center">
  <img src="https://foruda.gitee.com/images/1699582639386992543/b2078d27_13671125.png" width="400" alt="VSCode Run Project">
  <p><em>VSCode Run Project</em></p>
</div>

## Display

### Watch Project

The watch face is displayed in the window, and you can interact with it by swiping and clicking.

<div align="center">
  <img src="https://docs.realmcu.com/HoneyGUI/image/gui_watch.gif" width="400" alt="Watch Project Example">
  <p><em>Watch Project Example</em></p>
</div>

### Dashboard Project

The dashboard is displayed in the window.

<div align="center">
  <img src="https://foruda.gitee.com/images/1753340204000227545/39228f04_10737458.png" width="400" alt="Dashboard Project Example">
  <p><em>Dashboard Project Example</em></p>
</div>
