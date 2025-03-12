.. _Library_Porting_CN:

==============
HoneyGUI移植
==============

HoneyGUI 是一个轻量级的嵌入式 GUI 系统，专为 Realtek 系列芯片优化设计。本文档将指导您如何在不同的 Realtek 芯片平台上编译 HoneyGUI 库，包括 Armclang 和 Armcc 两种编译器环境的配置和使用方法。

注意事项
--------

环境配置
~~~~~~~~

- 请确保已正确安装 Keil MDK 开发环境。
- 确认 CMake（3.15或以上版本）已安装并添加到系统环境变量。
- 编译前请验证所有依赖库已正确安装。

编译器选择
~~~~~~~~~

- Keil AC5 工程：必须使用 armcc 编译器构建库文件。
- Keil AC6 工程：必须使用 armclang 编译器构建库文件。
- 确保编译器版本与工程设置完全匹配。

路径配置
~~~~~~~

- Armcc 编译器路径检查：
    - 默认位置：``C:/Keil_v5/ARM/ARMCC/bin``。
    - 如安装位置不同，需在 CMake 中更新路径设置。
- Armclang 编译器路径检查：
    - 默认位置：``C:/Keil_v5/ARM/ArmCompilerforEmbedded6.22/bin``。
    - 如安装位置不同，需在 CMake 中更新路径设置。

常见问题处理
~~~~~~~~~~~

- 编译错误时，首先检查：
    - 芯片型号是否正确指定。
    - 编译器路径是否正确配置。
    - 编译器版本是否与工程匹配。
- 确保所有必要的环境变量已正确设置。

编译环境要求
------------
- CMake 3.15 或以上版本。
- Keil MDK 5 或以上版本。
- Windows 操作系统。

Armcc 编译
----------
支持芯片:
  - RTL8773E (默认)
  - RTL8763E
  - RTL8762G
  - RTL8763D
  

编译步骤:

1. 在工程路径 armcc 目录下打开 cmd 窗口，生成构建文件，运行命令 :kbd:`cmake -G "MinGW Makefiles" -DSOC=RTL8763D -B "./temp"`：

.. code-block:: console
    :emphasize-lines: 1

    E:\HoneyGUI\lib\armcc>cmake -G "MinGW Makefiles" -DSOC=RTL8763D -B "./temp"
    soc = RTL8763D
    -- The C compiler identification is ARMCC 5.6.960
    -- The CXX compiler identification is ARMCC 5.6.960
    -- Detecting C compiler ABI info
    -- Detecting C compiler ABI info - done
    ...
    -- Configuring done (2.7s)
    -- Generating done (0.9s)
    -- Build files have been written to: E:/HoneyGUI/lib/armcc/temp

.. note::
    若不指定芯片型号，默认为 RTL8773E。

2. 进入 temp 目录编译项目，运行命令 :kbd:`cd temp` :kbd:`cmake --build .`：

.. code-block:: console
    :emphasize-lines: 1,2

    E:\HoneyGUI\lib\armcc>cd temp
    E:\HoneyGUI\lib\armcc\temp>cmake --build .
    [  1%] Building C object CMakeFiles/gui.dir/E_/HoneyGUI/realgui/3rd/cJSON/cJSON.o
    [  2%] Building C object CMakeFiles/gui.dir/E_/HoneyGUI/realgui/3rd/ezXML/ezxml.o
    [  3%] Building C object CMakeFiles/gui.dir/E_/HoneyGUI/realgui/3rd/nanovg/base/nanovg.o
    ...
    [100%] Linking C static library gui.lib
    [100%] Built target gui

3. 安装资源，运行命令 :kbd:`cmake --build . --target install`：

.. code-block:: console
    :emphasize-lines: 1

    E:\HoneyGUI\lib\armcc\temp>cmake --build . --target install
    [100%] Built target gui
    Install the project...
    -- Install configuration: ""
    -- Installing: E:/HoneyGUI/lib/armcc/install/lib/gui.lib
    ...

4. 编译生成的资源文件位置：
    - 头文件： ``<your HoneyGUI dir>/lib/armcc/install/include``。
    - 库文件： ``<your HoneyGUI dir>/lib/armcc/install/lib/gui.lib``。

Armclang 编译
------------
支持芯片:
  - RTL8762G (默认)
  - RTL8762D
  - RTL8773E
  - RTL8773G

编译步骤:

1. 在工程路径 armclang 目录下打开 cmd 窗口，生成构建文件，运行命令 :kbd:`cmake -G "MinGW Makefiles" -DSOC=RTL8762G -B "./temp"`：

.. code-block:: console
    :emphasize-lines: 1

    E:\HoneyGUI\lib\armclang>cmake -G "MinGW Makefiles" -DSOC=RTL8762G -B "./temp"
    soc = RTL8762G
    -- The C compiler identification is ARMClang
    -- The CXX compiler identification is ARMClang
    ...
    -- Configuring done
    -- Generating done
    -- Build files have been written to: E:/HoneyGUI/lib/armclang/temp

.. note::
    若不指定芯片型号，默认为 RTL8762G。

2. 进入 temp 目录编译项目，运行命令 :kbd:`cd temp` :kbd:`cmake --build .`：

.. code-block:: console
    :emphasize-lines: 1,2

    E:\HoneyGUI\lib\armclang>cd temp
    E:\HoneyGUI\lib\armclang\temp>cmake --build .
    [  0%] Building C object CMakeFiles/gui.dir/...
    ...
    [100%] Built target gui

3. 安装资源，运行命令 :kbd:`cmake --build . --target install`：

.. code-block:: console
    :emphasize-lines: 1

    E:\HoneyGUI\lib\armclang\temp>cmake --build . --target install
    [100%] Built target gui
    Install the project...
    -- Installing: E:/HoneyGUI/lib/armclang/install/lib/gui.lib
    ...

4. 编译生成的资源文件位置：
    - 头文件： ``<your HoneyGUI dir>/lib/armclang/install/include``。
    - 库文件： ``<your HoneyGUI dir>/lib/armclang/install/lib/gui.lib``。

工程移植示例
------------

本示例以RTL8773GWP仪表盘工程为例。

1. 将编译生成的资源文件复制到工程目录：

   - 复制头文件（.h）到工程的资源目录。
   - 复制库文件（gui.lib）到工程的资源目录。
   
2. 工程配置：

   - 在 Keil MDK 中添加头文件路径。
   - 在工程设置中链接 gui.lib 库。

.. figure:: https://foruda.gitee.com/images/1741674292411402494/07c72a64_13406851.jpeg
   :align: center
   :width: 800px

   链接头文件路径到工程目录

.. figure:: https://foruda.gitee.com/images/1741657914099825115/216c4d69_13406851.jpeg
   :align: center
   :width: 800px

   链接库文件到工程目录   