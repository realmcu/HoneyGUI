# **欢迎来到 HoneyGUI !**

[EN](README.md)   |  [中文](README_zh.md)

详细内容可参考 [HoneyGUI 在线文档](https://docs.realmcu.com/gui/cn/latest/index.html)。

## 示例
<table>
  <tr>
    <td align="center"><img src="https://foruda.gitee.com/images/1699931734086261442/f0690691_10641540.png"width = "200" height = "250"/><br><sub><b>动态表盘</b></sub>
    <td align="center"><img src="https://foruda.gitee.com/images/1723621216978994553/76b62adc_10737458.gif"width = "200" height = "250"/><br><sub><b>水果忍者</b></sub>
    <td align="center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/Calculator/calculator.gif"width = "200" height = "250"/><br><sub><b>计算器</b></sub>
  </tr>
  <tr>
    <td align="center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/watchface.gif" width = "200" height = "250"/><br><sub><b>表盘</b></sub>
    <td align="center"><img src="https://foruda.gitee.com/images/1734073855265837536/f8a3808e_13408154.gif"width = "200" height = "250"/><br><sub><b>计时器</b></sub>
    <td align="center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/musicPlayer.gif"width = "200" height = "250"/><br><sub><b>音乐播放器</b></sub>
  </tr>
  <tr>
    <td align="center"><img src="https://foruda.gitee.com/images/1725848489900237181/97911fc4_13408154.gif"width = "200" height = "250"/><br><sub><b>矢量图形</b></sub>
    <td align="center"><img src="https://foruda.gitee.com/images/1734070660330786955/61e4ff9d_13408154.gif"width = "200" height = "250"/><br><sub><b>3D 蝴蝶</b></sub>
    <td align="center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/soccer.gif"width = "200" height = "250"/><br><sub><b>2.5D 足球</b></sub>
    
  </tr>
  
</table>


## 介绍

HoneyGUI 是由 Realtek 自主研发的图形显示框架，它是一款专为资源受限的微控制器和嵌入式系统设计的开源嵌入式图形用户界面（GUI）库。HoneyGUI 具备轻量级、功能丰富和高度可定制性的特点，广泛应用于消费电子、家电、医疗设备和智能手表等领域。

HoneyGUI 提供基于 `PC` 的仿真环境，使开发者能够快速进行应用开发和调试，而无需依赖嵌入式硬件平台。同时，HoneyGUI 还可以与 Realtek 自研的前端设计工具 `RVD` 配合使用，实现可视化编程。

HoneyGUI 目前主要支持以下两种高效的 APP 开发方式：

1. C/C++ API 开发

   - 开发者可以直接调用基于 RealGUI 显示引擎的 C/C++ API 来构建应用程序。
   - 这种方式提供了最高的灵活性和对系统性能的极致控制，适合开发复杂的、对性能要求高的应用。

2. 前端低代码开发

   - 采用 JavaScript 和 XML 进行前端应用的开发。
   - 推荐使用 ``RVisualDesigner``，通过拖拽组件和可视化配置的方式，快速构建美观的 UI 界面，实现低代码开发，显著提升开发效率。
   - 前端低代码开发方式提供了快速开发和迭代的优势，特别适合于快速原型设计和小规模应用开发。

HoneyGUI 框架具有很强的可移植性，可以在多种芯片和 OS 上运行。此次提供了 PC Windows 版本。

## 软件架构


<div align="center">
  <img src="https://foruda.gitee.com/images/1753340174900062921/e20848cb_10737458.png" width="600" alt="HoneyGUI 软件架构">
  <p><em>HoneyGUI 软件架构</em></p>
</div>


## 安装在 Windows 操作系统上

### 安装编译器
下载 MinGW-w64 工具链，解压到 C 盘，并将其添加到系统环境变量 ``Path`` 中。

1.  [Mingw64 下载](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/Personal%20Builds/mingw-builds/8.1.0/threads-posix/sjlj/x86_64-8.1.0-release-posix-sjlj-rt_v6-rev0.7z)
2.  解压并复制到目录：`C:/mingw64`
3.  添加一个环境变量：`C:/mingw64/bin`
    - 打开开始菜单，搜索 ``高级系统设置``。
    - 显示 ``系统属性``，然后转到 ``高级`` 选项卡。
    - 点击 ``环境变量`` 按钮。
    - 在 ``用户变量`` 部分，找到并选择 ``Path`` 变量，然后点击 ``编辑``。
    - 点击 ``新建`` 并添加 ``C:\mingw64\bin``。
    - 点击 ``确定`` 关闭所有对话框。


### 安装 Python

测试过 Python 3.9.7 版本。

### 安装scons
打开一个 CMD 窗口，并执行以下命令来安装 Python 的 scons 库：
```shell
> pip install scons==4.4.0
```

### 安装 kconfiglib
安装 kconfiglib 用于配置管理：

```shell
> pip install kconfiglib
```

### 配置应用程序（Menuconfig）

在编译之前，可以使用 `menuconfig` 图形化配置工具来选择要运行的示例应用程序。此配置对 **SCons** 和 **CMake** 两种构建系统都通用。

#### 使用 Menuconfig 进行配置

在 `win32_sim` 目录下执行以下命令：

```shell
> cd win32_sim
> menuconfig ../Kconfig.gui
```

执行命令后，将打开图形化配置界面：

<div align="center">
  <img src="https://foruda.gitee.com/images/1764320806391041617/417b0f55_13406851.png" width="600" alt="Menuconfig 配置界面">
  <p><em>Menuconfig 配置界面</em></p>
</div>

#### 操作说明

1. 使用 **方向键** 上下移动选择项目
2. 使用 **空格键** 或 **回车键** 取消/选中配置项
3. 选择 `Select HoneyGUI Demo (Watch Turnkey Demo 410x502)` 进入应用程序配置
4. 选择要运行的示例应用程序：

   - `Watch Turnkey Demo 410x502`：手表应用示例（410x502 分辨率）
   - `GUI Demo 800x480`：仪表盘应用示例（800x480 分辨率）
   - 其他可用的示例应用程序

<div align="center">
  <img src="https://foruda.gitee.com/images/1764321915931205772/1248fc4d_13406851.png" width="600" alt="Menuconfig 应用程序配置界面">
  <p><em>Menuconfig 应用程序配置界面</em></p>
</div>

5. 按 **ESC** 键返回上一级菜单
6. 选择 `< s >` 保存配置到 `.config` 文件
7. 选择 `< q >` 退出配置界面

#### 配置文件说明

配置完成后，会在 `HoneyGUI\win32_sim` 目录下自动生成 `.config` 文件。构建系统（SCons 或 CMake）会自动读取此配置文件，并根据配置编译相应的应用程序。

> **注意：** 如果不使用 menuconfig 进行配置，构建系统将使用默认配置或已存在的 `.config` 文件。

安装 `mingw-w64` 工具链和 `scons` 库后，可以通过两种方式启动应用程序：通过 CMD 启动或通过 VSCode 启动。

### 通过 CMD 启动（Scons）
在 `HoneyGUI` 文件夹中打开一个 CMD 窗口，然后运行以下命令启动应用程序。

```shell
> cd win32_sim
> scons
> cd ..
> .\win32_sim\gui.exe
```

`scons` 命令执行构建过程，然后执行 `gui.exe` 来运行应用程序。


<div align="center">
  <img src="https://foruda.gitee.com/images/1718704649306452668/282ac763_13408154.png" width="600" alt="启动应用程序">
  <p><em>启动应用程序</em></p>
</div>

### 通过 CMD 启动（CMake）

#### 依赖软件

- **CMake**（测试版本为 3.31.2）：[https://cmake.org/download/](https://cmake.org/download/)

- **MinGW-w64**：如前所述

#### 初始化构建环境

在 `HoneyGUI` 文件夹中执行：

```shell
> cd win32_sim
> mkdir build
> cd build
> cmake -G "MinGW Makefiles" ..
```

#### 配置应用程序

如需更改应用程序配置，请参考前面的 **配置应用程序（Menuconfig）** 部分，在 `win32_sim` 目录下执行 `menuconfig ../Kconfig.gui` 命令。

#### 编译项目

在 `HoneyGUI\win32_sim\build` 文件夹中执行：

```shell
> cmake -G "MinGW Makefiles" ..
> mingw32-make -j 32
```

编译过程中，CMake 会自动：

- 读取 `.config` 文件中的配置
- 根据配置选择相应的应用程序源代码
- 自动处理应用程序所需的二进制资源文件（`.bin` 文件）
- 生成可执行文件 `gui.exe`

#### 运行程序

在 `HoneyGUI` 文件夹中执行：

```shell
> .\win32_sim\build\gui.exe
```


### 通过 VSCode 启动

#### 安装 VSCode
   - [```下载 VSCode```](https://code.visualstudio.com/)
   - [```安装 C/C++ 插件```](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)

#### 打开项目
   - 单击 ```HoneyGUI.code-workspace``` 文件

#### 运行项目
进入 VSCode 界面后，可以选择 `Run and Debug` 选项，然后点击 `Run` 按钮。

<div align="center">
  <img src="https://foruda.gitee.com/images/1699582639386992543/b2078d27_13671125.png" width="300" alt="VSCode 运行项目">
  <p><em>VSCode 运行项目</em></p>
</div>


## 显示
### 手表工程
窗口中显示出表盘，您可以通过滑动和点击与其进行交互。

<div align="center">
  <img src="https://docs.realmcu.com/HoneyGUI/image/gui_watch.gif" width="400" alt="手表工程示例">
  <p><em>手表工程示例</em></p>
</div>

### 仪表盘工程
窗口中显示仪表盘。

<div align="center">
  <img src="https://foruda.gitee.com/images/1753340204000227545/39228f04_10737458.png" width="400" alt="仪表盘工程示例">
  <p><em>仪表盘工程示例</em></p>
</div>
