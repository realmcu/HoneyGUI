# **欢迎来到RTKIOT GUI wiki!**

## 源代码项目下载

- 在GitHub上下载： <https://github.com/realmcu/HoneyGUI>
- 在Gitee上下载： <https://gitee.com/realmcu/HoneyGUI>

## 介绍

HoneyGUI 是由 Realtek 自主研发的图形显示框架，它是一款专为资源受限的微控制器和嵌入式系统设计的开源嵌入式图形用户界面（GUI）库。HoneyGUI 具备轻量级、功能丰富和高度可定制性的特点，广泛应用于消费电子、家电、医疗设备和智能手表等领域。

作为一套综合显示框架，HoneyGUI 不仅包含 Realtek 自主研发的显示引擎开发工具，还支持直接调用如 LVGL、ARM2D 等外部 API 进行应用程序的开发。此外，HoneyGUI 提供基于 PC 的仿真环境，使开发者能够快速进行应用开发和调试，而无需依赖嵌入式硬件平台。同时，HoneyGUI 还可以与 Realtek 自研的前端设计工具 RVD 配合使用，实现可视化编程。

以下是几种常见的 APP 开发方式：

- 基于RealGUI 显示引擎，调用C/C++ API 开发应用程序。
- 也可以直接调用 [LVGL](https://lvgl.io/)的API 开发应用程序。
- 也可以直接调用 [ARM-2D](https://github.com/ARM-software/Arm-2D)的API 开发应用程序。
- 前端开发方式，包含JavaScript和XML。推荐使用```RVisualDesigner``` 作为PC设计器进行低代码开发。

GUI框架具有很强的可移植性，可以在多种芯片和操作系统上运行。此次提供了PC Windows版本。

## 软件架构


<div style="text-align: center"><img width= "600" src ="https://foruda.gitee.com/images/1720700131929539160/ec0dbbb9_1860080.png"/></div><br/>


## 安装

### 安装编译器
下载MinGW-w64工具链，解压到C盘，并将其添加到系统环境变量 `Path`中。
1.  [MinGW-w64 下载](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/Personal%20Builds/mingw-builds/8.1.0/threads-posix/sjlj/x86_64-8.1.0-release-posix-sjlj-rt_v6-rev0.7z)
2.  解压并复制到目录： `C:/mingw64`
3.  添加一个环境变量： `C:/mingw64/bin`

### 安装scons
打开一个CMD窗口，并执行以下命令来安装Python的scons库。
```
> pip install scons==4.4.0
```

安装 `MinGW-w64` 工具链和 `scons` 库后，可以通过两种方式启动应用程序：通过CMD启动或通过GUI启动。

### 通过CMD启动
在 `HoneyGUI` 文件夹中打开一个CMD窗口，然后运行以下命令启动应用程序。

```shell
> cd win32_sim
> scons
> cd ..
> .\win32_sim\gui.exe
```
`scons` 命令执行构建过程，然后执行 `gui.exe` 来运行应用程序。


<div style="text-align: center"><img width= "600"  src ="https://foruda.gitee.com/images/1718704649306452668/282ac763_13408154.png"/></div><br/>


### 通过VSCode启动

#### 安装VSCode
   - [下载VSCode](https://code.visualstudio.com/)
   - [安装C/C++插件](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)

#### 打开项目
   - 单击 `HoneyGUI.code-workspace` 文件

#### 运行项目
进入VSCode界面后，可以选择 `Run and Debug` 选项，然后点击 `Run button`。


<div style="text-align: center"><img width="300" src ="https://foruda.gitee.com/images/1699582639386992543/b2078d27_13671125.png"/></div><br/>


### 显示表盘
您可以通过滑动和长按与其进行交互。


<div style="text-align: center"><img width="300" src ="https://foruda.gitee.com/images/1698286583110259632/b48ad0af_10088396.png"/></div><br/>