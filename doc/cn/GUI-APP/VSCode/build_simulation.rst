构建与模拟
==========

本文档介绍如何构建和运行 HoneyGUI Design 项目,包括开发环境配置、构建流程和模拟测试。

概述
----

HoneyGUI Design 支持在 PC 上模拟运行,无需嵌入式硬件即可快速开发和测试 GUI 应用。项目使用 SCons 构建系统,支持 Windows、Linux 和 WSL 环境。

开发环境要求
------------

Windows
~~~~~~~

必需软件:

* **MinGW-w64**: C/C++ 编译器

  * 推荐版本: 8.1.0 或更高
  * 安装路径: ``C:\mingw64`` (默认)
  * 下载: https://sourceforge.net/projects/mingw-w64/

* **Python**: 3.9.7 或更高版本

  * 下载: https://www.python.org/downloads/

* **SCons**: 构建工具

  .. code-block:: bash

     pip install scons==4.4.0

* **kconfiglib**: 配置工具

  .. code-block:: bash

     pip install kconfiglib

* **VSCode**: 代码编辑器

  * 下载: https://code.visualstudio.com/
  * 安装 HoneyGUI Design 插件

可选软件:

* **CMake**: 3.31.2 或更高 (如果使用 CMake 构建)

  .. code-block:: bash

     pip install cmake

Linux/WSL
~~~~~~~~~

必需软件:

.. code-block:: bash

   # Ubuntu/Debian
   sudo apt-get update
   sudo apt-get install build-essential python3 python3-pip libsdl2-dev

   # 安装 Python 工具
   pip3 install scons==4.4.0 kconfiglib

环境变量配置
~~~~~~~~~~~~

**Windows**:

确保 MinGW-w64 的 ``bin`` 目录在 PATH 中:

.. code-block:: bash

   # 检查编译器
   gcc --version
   g++ --version

   # 如果找不到,添加到 PATH
   set PATH=C:\mingw64\bin;%PATH%

**Linux/WSL**:

.. code-block:: bash

   # 检查编译器
   gcc --version

   # 检查 SDL2
   pkg-config --libs sdl2

项目结构
--------

HoneyGUI Design 项目的标准结构:

.. code-block:: text

   my_project/
   ├── project.json          # 项目配置文件
   ├── ui/                   # HML 设计文件
   │   ├── main/
   │   │   └── main.hml
   │   └── settings/
   │       └── settings.hml
   ├── assets/               # 资源文件
   │   ├── images/          # 图片资源
   │   ├── fonts/           # 字体资源
   │   └── models/          # 3D 模型
   ├── src/                  # 生成的 C 代码
   │   ├── {ProjectName}Entry.c
   │   ├── SConscript
   │   ├── ui/              # UI 代码
   │   ├── callbacks/       # 回调代码
   │   └── user/            # 用户代码
   └── win32_sim/           # 模拟器目录 (构建后)

项目配置文件
------------

``project.json`` 定义项目的基本信息:

.. code-block:: json

   {
     "name": "MyWatchApp",
     "version": "1.0.0",
     "description": "My smart watch application",
     "resolution": "410x502",
     "author": "Your Name",
     "license": "MIT"
   }

支持的分辨率:

* ``240x240`` - 小型圆屏手表
* ``410x502`` - 标准智能手表
* ``466x466`` - 大屏手表
* ``480x272`` - 横屏设备
* ``800x480`` - 平板/车机

构建流程
--------

使用 VSCode 插件构建
~~~~~~~~~~~~~~~~~~~~

1. **打开项目**: 在 VSCode 中打开 HoneyGUI Design 项目文件夹

2. **生成代码**:

   * 点击工具栏的"生成代码"按钮
   * 或使用命令面板: ``Ctrl+Shift+P`` → ``HoneyGUI: Generate Code``

3. **配置应用**:

   * 使用命令: ``HoneyGUI: Configure Application``
   * 选择要运行的应用

4. **构建项目**:

   * 使用命令: ``HoneyGUI: Build Project``
   * 或在终端中执行:

     .. code-block:: bash

        cd win32_sim
        scons

5. **运行模拟器**:

   * 使用命令: ``HoneyGUI: Run Simulator``
   * 或手动执行:

     .. code-block:: bash

        # Windows
        ./win32_sim/gui.exe

        # Linux/WSL
        ./win32_sim/gui

使用命令行构建
~~~~~~~~~~~~~~

**完整构建流程**:

.. code-block:: bash

   # 1. 进入模拟器目录
   cd win32_sim

   # 2. 配置应用 (可选)
   menuconfig ../Kconfig.gui

   # 3. 构建项目
   scons

   # 4. 运行
   cd ..
   ./win32_sim/gui.exe  # Windows
   ./win32_sim/gui      # Linux/WSL

**清理构建**:

.. code-block:: bash

   cd win32_sim
   scons -c

**调试模式**:

.. code-block:: bash

   # 跳过严格检查,方便开发调试
   set DEBUG_MODE=1        # Windows
   export DEBUG_MODE=1     # Linux/WSL
   scons

构建配置
--------

应用配置 (Menuconfig)
~~~~~~~~~~~~~~~~~~~~~

在构建前配置要运行的应用:

.. code-block:: bash

   cd win32_sim
   menuconfig ../Kconfig.gui

配置界面允许选择:

* 预设应用 (手表演示、GUI 演示等)
* 屏幕分辨率
* 功能开关

配置保存在 ``.config`` 文件中,SCons 和 CMake 都会读取。

编译器标志
~~~~~~~~~~

**默认编译器标志** (严格模式):

* ``-Wall``: 启用所有警告
* ``-Werror``: 将警告视为错误
* ``-O2``: 优化级别 2

**调试模式标志** (``DEBUG_MODE=1``):

* ``-g``: 生成调试信息
* ``-O0``: 不优化,方便调试
* 不使用 ``-Werror``

使用 CMake 构建 (可选)
---------------------

HoneyGUI 也支持 CMake 构建系统:

.. code-block:: bash

   cd win32_sim
   mkdir build
   cd build

   # Windows (MinGW)
   cmake -G "MinGW Makefiles" ..
   mingw32-make -j 32

   # Linux
   cmake ..
   make -j 8

   # 运行
   cd ../..
   ./win32_sim/build/gui.exe  # Windows
   ./win32_sim/build/gui      # Linux

运行模拟器
----------

启动模拟器
~~~~~~~~~~

.. code-block:: bash

   # Windows
   ./win32_sim/gui.exe

   # Linux/WSL
   ./win32_sim/gui

模拟器窗口将显示您设计的 GUI 界面。

交互方式
~~~~~~~~

* **鼠标左键**: 模拟触摸点击
* **鼠标拖动**: 模拟滑动手势
* **鼠标长按**: 模拟长按操作
* **键盘 ESC**: 退出模拟器
* **窗口关闭**: 退出模拟器

调试技巧
~~~~~~~~

1. **查看日志**: 模拟器会输出调试信息到控制台

2. **使用 GDB 调试** (Linux):

   .. code-block:: bash

      gdb ./win32_sim/gui
      (gdb) run
      (gdb) break main
      (gdb) continue

3. **Visual Studio Code 调试** (Windows):

   配置 ``launch.json``:

   .. code-block:: json

      {
        "version": "0.2.0",
        "configurations": [
          {
            "name": "Debug HoneyGUI",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/win32_sim/gui.exe",
            "cwd": "${workspaceFolder}",
            "MIMode": "gdb",
            "miDebuggerPath": "C:/mingw64/bin/gdb.exe"
          }
        ]
      }

常见问题
--------

构建错误
~~~~~~~~

**错误: 找不到 gcc**

.. code-block:: text

   'gcc' is not recognized as an internal or external command

解决方案:

* 确认已安装 MinGW-w64
* 检查 PATH 环境变量
* 重新启动终端/VSCode

**错误: SDL2 库缺失**

.. code-block:: text

   fatal error: SDL2/SDL.h: No such file or directory

解决方案 (Linux):

.. code-block:: bash

   sudo apt-get install libsdl2-dev

**错误: Python 模块缺失**

.. code-block:: text

   ModuleNotFoundError: No module named 'SCons'

解决方案:

.. code-block:: bash

   pip install scons==4.4.0 kconfiglib

运行错误
~~~~~~~~

**错误: 无法启动模拟器**

.. code-block:: text

   Failed to initialize SDL

解决方案:

* Windows: 确认系统支持 OpenGL
* Linux: 检查 SDL2 库安装
* WSL: 需要配置 X Server (如 VcXsrv)

**错误: 资源文件找不到**

.. code-block:: text

   Error loading image: assets/logo.bin

解决方案:

* 确认资源文件已转换为 .bin 格式
* 检查文件路径是否正确
* 重新构建项目

性能优化
--------

构建速度优化
~~~~~~~~~~~~

1. **并行编译**:

   .. code-block:: bash

      # SCons
      scons -j 8  # 使用 8 个线程

      # CMake
      make -j 8

2. **增量构建**: 只修改部分文件后,构建系统自动只重新编译改动的文件

3. **使用 ccache** (Linux):

   .. code-block:: bash

      sudo apt-get install ccache
      export CC="ccache gcc"
      export CXX="ccache g++"

运行性能优化
~~~~~~~~~~~~

1. **资源优化**: 使用合适的图片格式和分辨率

2. **代码优化**: 避免在频繁调用的回调中执行耗时操作

3. **编译优化**: 发布版本使用 ``-O2`` 或 ``-O3`` 优化级别

持续集成
--------

在 CI/CD 流水线中构建 HoneyGUI 项目:

.. code-block:: yaml

   # GitHub Actions 示例
   name: Build HoneyGUI
   on: [push, pull_request]

   jobs:
     build:
       runs-on: ubuntu-latest
       steps:
         - uses: actions/checkout@v2

         - name: Install dependencies
           run: |
             sudo apt-get update
             sudo apt-get install -y build-essential libsdl2-dev python3-pip
             pip3 install scons==4.4.0 kconfiglib

         - name: Build project
           run: |
             cd win32_sim
             scons

         - name: Archive artifacts
           uses: actions/upload-artifact@v2
           with:
             name: gui-binary
             path: win32_sim/gui

下一步
------

* 学习 :doc:`code_generation` 了解代码生成流程
* 查看 :doc:`event_system` 实现交互逻辑
* 参考 :doc:`resource_management` 管理项目资源
* 阅读 :doc:`deployment` 了解嵌入式部署方案
