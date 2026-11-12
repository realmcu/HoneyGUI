编译与仿真
==========

本章介绍如何在 :term:`PC` 上编译和运行 HoneyDesigner 项目，以及 HoneyGUI 与 LVGL 两种引擎的构建差异。

概述
-----

插件在项目根目录生成 ``build/`` 构建目录，并自动完成资源转换、代码生成、编译和仿真器启动。HoneyGUI 使用 SCons，LVGL 使用 CMake。

开发环境要求
------------

通用软件：

* **VSCode**: 代码编辑器

  * 推荐版本： 1.80.0 或更高
  * 下载： https://code.visualstudio.com/
  * 安装 HoneyDesigner 插件

* **Python**: 3.9.7 或更高版本

  * 下载： https://www.python.org/downloads/

* **SCons**: 构建工具

  .. code-block:: bash

     pip install scons==4.4.0

通用可选软件：

* **CMake**: 3.31.2 或更高（LVGL 项目需要）

  * 下载： https://cmake.org/download/

* **FFmpeg**: 视频转换需要

  * 下载： https://ffmpeg.org/download.html

* **Git**: 从模板创建项目时需要

  * 下载： https://git-scm.com/downloads

Windows
~~~~~~~

必需软件：

* **MinGW-w64**: C/C++ 编译器

  * 推荐版本： 8.1.0 或更高
  * 安装路径： ``C:\mingw64`` （默认）
  * 下载： https://sourceforge.net/projects/mingw-w64/

* **SDL2**: 已随仓库提供，无需单独安装

Linux/WSL
~~~~~~~~~~

.. code-block:: bash

   # Ubuntu/Debian
   sudo apt-get update
   sudo apt-get install build-essential python3 python3-pip libsdl2-dev
   pip3 install scons==4.4.0

Linux/WSL 使用系统 ``gcc``/``g++``，SDL2 由 ``libsdl2-dev`` 提供。WSL 需要 WSLg 或 X Server 才能显示仿真窗口。

环境变量配置
~~~~~~~~~~~~~

**Windows**

构建脚本 ``menu_config.py`` 通过 ``PATH`` 查找 ``gcc``，找不到时会直接报错，因此 ``C:\mingw64\bin`` 必须在 ``PATH`` 中。可先在当前 CMD 窗口临时添加：

.. code-block:: bat

   set PATH=C:\mingw64\bin;%PATH%

要永久生效，请在“系统属性 → 环境变量”中编辑用户或系统 ``PATH``，然后重新打开终端和 VSCode。检查配置：

.. code-block:: bash

   gcc --version
   g++ --version
   mingw32-make --version

**Linux/WSL**

系统 GCC 由 ``build-essential`` 安装，通常无需额外配置。检查：

.. code-block:: bash

   gcc --version
   pkg-config --libs sdl2

如果 SDL2 不在系统默认路径，CMake 可通过 ``SDL2_INCLUDE_DIR`` 和 ``SDL2_LIB_DIR`` 指定头文件和库目录。

**FFmpeg**

需要视频转换时，将 FFmpeg 的 ``bin`` 目录加入 ``PATH``；否则视频资源会被跳过。

环境检查
~~~~~~~~~

1. 打开 HoneyDesigner 侧边栏的 **环境** 视图。
2. 查看 Python、SCons、GCC、SDL2、FFmpeg 状态。
3. 点击刷新按钮或执行 ``刷新环境检查``。
4. 使用视图中的安装入口安装缺失项。

项目结构
---------

.. code-block:: text

   my_project/
   ├── project.json
   ├── ui/
   │   └── MyProjectMain.hml
   ├── src/
   │   ├── MyProjectEntry.c
   │   ├── SConscript
   │   ├── ui/
   │   ├── callbacks/
   │   ├── user/
   │   └── lvgl/                # LVGL 项目
   ├── assets/
   └── build/                   # 插件生成的构建目录

``win32_sim/`` 不是用户项目目录。插件会把一套固定的 PC 仿真工程骨架复制到 ``build/``，包括 SCons/CMake 构建脚本、``main.c`` 入口、``port/`` 平台适配层（显示、输入、音频、文件系统和 OS）、链接脚本，以及 Windows 下使用的 SDL2 静态库。用户只需要维护 ``ui/``、``src/``、``assets/`` 和 ``project.json``；``build/`` 是生成目录，可以删除后重新生成。

项目配置
---------

``project.json`` 示例：

.. code-block:: json

   {
     "$schema": "HoneyGUI",
     "type": "Designer",
     "name": "MyWatchApp",
     "appId": "com.example.mywatch",
     "version": "1.0.0",
     "resolution": "410x502",
     "cornerRadius": 20,
     "targetEngine": "honeygui",
     "pixelMode": "RGB565",
     "minSdk": "1.0",
     "mainHmlFile": "ui/MyWatchAppMain.hml",
     "romfsBaseAddr": "0x704D1000",
     "aiAssets": true,
     "uart": {
       "port": "COM3",
       "baudRate": 115200,
       "chipType": "RTL87X3E"
     }
   }

常用字段：

* ``resolution``：屏幕分辨率，格式为 ``WxH``。
* ``cornerRadius``：``0`` 矩形，``-1`` 圆形，正数为圆角半径。
* ``targetEngine``：``honeygui`` 或 ``lvgl``。
* ``pixelMode``：``RGB565``、``RGB888``、``ARGB8888``。
* ``romfsBaseAddr``：romfs 在 Flash 中的基地址。
* ``uart``：UART 下载配置。

构建流程
---------

构建项目
~~~~~~~~~

**使用插件构建：**

1. 打开项目中的 HML 文件。
2. 点击工具栏的 :guilabel:`仿真`，或在命令面板执行 ``HoneyDesigner: 编译仿真``。
3. 插件默认依次执行资源转换、代码生成、编译和仿真器启动。
4. 如需跳过某个阶段，点击 :guilabel:`仿真` 按钮右侧的下拉箭头，在 **仿真流程** 中取消勾选对应项。

插件相关命令：

* ``HoneyDesigner: Debug Simulation``：调试模式启动。
* ``HoneyDesigner: Run Simulation Without Code Generation``：跳过代码生成。
* ``HoneyDesigner: Stop Simulation``：停止仿真。
* 工具栏 :guilabel:`清理` 或 **快捷操作** 视图的 :guilabel:`清理`：清理构建产物。

**手动构建：**

插件推荐使用工具栏或命令完成构建。需要手动排查时，可以先执行代码生成，然后进入 ``build/`` 运行 SCons：

.. code-block:: bash

   cd build
   scons

LVGL 项目使用 CMake：

.. code-block:: bash

   cd lvgl-pc
   cmake -S . -B build
   cmake --build build -j4

构建产物
~~~~~~~~~

无论使用插件还是手动构建，主要产物如下：

HoneyGUI：

* Windows：``build/gui.exe``
* Linux/WSL：``build/gui``
* 资源：``build/assets/``
* romfs：``build/app_romfs_<baseAddr>.bin``

LVGL：

* 工程：``lvgl-pc/``
* 可执行文件：``lvgl-pc/build/lvgl_pc.exe`` 或 ``lvgl-pc/build/lvgl_pc``

调试
-----

* 使用 ``HoneyDesigner: Debug Simulation`` 启动调试会话。
* Windows 下需要 GDB，通常随 MinGW 一起安装。
* 也可以在 ``launch.json`` 中配置 ``cppdbg``，程序路径指向 ``build/gui.exe``。

示例：

.. code-block:: json

   {
     "version": "0.2.0",
     "configurations": [
       {
         "name": "Debug HoneyGUI",
         "type": "cppdbg",
         "request": "launch",
         "program": "${workspaceFolder}/build/gui.exe",
         "cwd": "${workspaceFolder}",
         "MIMode": "gdb",
         "miDebuggerPath": "C:/mingw64/bin/gdb.exe"
       }
     ]
   }

运行仿真器
-----------

启动仿真器
~~~~~~~~~~~

使用插件启动：

1. 点击工具栏的 :guilabel:`仿真`，或执行 ``HoneyDesigner: 编译仿真``。
2. 如果代码已经生成，可执行 ``HoneyDesigner: Run Simulation Without Code Generation`` 跳过代码生成直接启动。

也可以手动运行：

.. code-block:: bash

   # Windows
   ./build/gui.exe

   # Linux/WSL
   ./build/gui

LVGL 项目的可执行文件位于 ``lvgl-pc/build/lvgl_pc.exe`` 或 ``lvgl-pc/build/lvgl_pc``。

交互方式
~~~~~~~~~

* **鼠标左键**：模拟触摸点击。
* **鼠标拖动**：模拟滑动手势。
* **鼠标长按**：模拟长按。
* **Esc**：退出仿真器。
* **关闭窗口**：退出仿真器。

常见问题
---------

**找不到 gcc**

.. code-block:: text

   'gcc' is not recognized as an internal or external command

解决方案：

* 安装 MinGW-w64 或 GCC。
* 把编译器 ``bin`` 目录加入 ``PATH``。
* 重启终端和 VSCode。

**找不到 SDL2**

.. code-block:: text

   fatal error: SDL2/SDL.h: No such file or directory

解决方案：

.. code-block:: bash

   sudo apt-get install libsdl2-dev

**SCons 模块缺失**

.. code-block:: bash

   pip install scons==4.4.0

**无法启动仿真器**

* Windows：确认显卡/OpenGL 环境正常。
* Linux：确认 SDL2 已安装。
* WSL：需要配置 X Server 或使用 WSLg。

**资源文件找不到**

* 先执行资源转换。
* 检查 HML 中的资源路径是否以 ``assets/`` 或字体 ``/`` 开头。
* 重新执行完整构建。

性能优化
---------

* 使用增量构建，只修改少量文件时不需要清理全部产物。
* 图片和视频使用合适的分辨率、格式和压缩方式。
* 避免在频繁触发的回调中执行耗时操作。
* 发布版本使用优化级别构建。
* LVGL 项目可以调整 CMake 构建并行度。

下一步
-------

* 阅读 :doc:`deployment` 了解嵌入式部署方案
