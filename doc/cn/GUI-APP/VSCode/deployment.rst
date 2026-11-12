嵌入式部署
==========

本章介绍如何把 HoneyDesigner 项目集成到固件，以及 HoneyGUI 和 LVGL 两种目标的部署方式。

概述
-----

当前插件把 HML 设计转换为 **C 代码**，并通过资源转换和 romfs 打包生成固件可用的资源。插件不提供“运行时 HML(XML) 解析”或“HML 二进制描述文件”导出入口。

.. list-table::
   :header-rows: 1
   :widths: 20 30 50

   * - 目标引擎
     - 主要产物
     - 部署方式
   * - HoneyGUI
     - C 代码、Entry.c、SConscript、romfs
     - 集成到固件工程，或通过 UART 下载 romfs
   * - LVGL
     - C 代码、资源描述、romfs
     - c-array 或 external-bin

HoneyGUI 部署
--------------

生成产物
~~~~~~~~~

.. code-block:: text

   src/
   ├── MyProjectEntry.c
   ├── SConscript
   ├── ui/
   ├── callbacks/
   └── user/

   build/
   ├── assets/
   ├── app_romfs_<baseAddr>.bin
   ├── app_romfs.c
   └── ui_resource.h

关键文件：

* ``src/{ProjectName}Entry.c``：项目入口，包含受保护的入口代码区。
* ``src/ui``：UI 创建代码，每次生成覆盖。
* ``src/callbacks``：事件回调代码，保留保护区。
* ``src/user``：用户代码，只生成一次。
* ``build/app_romfs_<baseAddr>.bin``：可下载到 Flash 的资源镜像。

集成到固件
~~~~~~~~~~~

1. 在插件中生成代码。
2. 把 ``src/ui``、``src/callbacks``、``src/user`` 和 ``src/{ProjectName}Entry.c`` 加入固件工程。
3. 把 ``src/SConscript`` 或其中的源文件列表并入固件构建系统。
4. 按固件工程的方式加入源文件和头文件路径。
5. 确保 ``{ProjectName}Entry.c`` 参与编译；其中的 ``GUI_INIT_APP_EXPORT(app_init)`` 会注册启动初始化。

.. note::
   固件工程可能使用 SCons、Keil、IAR 或其他构建系统。生成的 ``src/SConscript`` 可直接用于 SCons 工程；其他工程请按现有方式加入上述源文件和头文件路径。

资源打包
~~~~~~~~~

HoneyGUI 项目构建时会：

1. 扫描 HML 引用的图片、字体、视频、模型等资源。
2. 把源资源转换为目标格式。
3. 生成 ``build/assets/``。
4. 打包为 ``build/app_romfs_<baseAddr>.bin``。

``<baseAddr>`` 来自 ``project.json`` 的 ``romfsBaseAddr``，例如 ``0x704D1000``。

打包流程和输出文件的详细说明见 :ref:`romfs 打包 <romfs-packaging-cn>`。

UART 下载
~~~~~~~~~~

1. 先执行一次完整编译，确保 ``build/app_romfs_<baseAddr>.bin`` 存在。
2. 点击工具栏的 :guilabel:`下载`，或使用 **快捷操作** 视图的 :guilabel:`下载`。
3. 选择串口、波特率、芯片类型和烧录地址。
4. 下载完成后重启开发板。

``project.json`` 中可以保存 UART 配置：

.. code-block:: json

   {
     "romfsBaseAddr": "0x704D1000",
     "uart": {
       "port": "COM3",
       "baudRate": 115200,
       "chipType": "RTL87X3E"
     }
   }

支持的芯片类型以插件当前配置为准，常见值包括 ``RTL87X3E``、``RTL87X3EP``、``RTL87X3D``、``RTL87X3G``。

LVGL 部署
----------

生成产物
~~~~~~~~~

.. code-block:: text

   src/lvgl/
   ├── {name}_lvgl_ui.c/h
   ├── {name}_lvgl_callbacks.c/h
   ├── lvgl_generated_ui.c/h
   ├── ui_resource.h
   └── lv_img_dsc_list.c/h

资源部署模式
~~~~~~~~~~~~~

在资源面板中为图片或文件夹选择部署方式：

* ``c-array``：资源编译为 C 数组，链接进固件。适合资源较小、希望减少外部文件的场景。
* ``external-bin``：资源打包为 romfs，通过 ``ui_resource.h`` 和 ``lv_img_dsc_list.c/h`` 在运行时访问。适合资源较多、希望单独更新资源的场景。

集成步骤：

1. 在 ``project.json`` 中设置 ``"targetEngine": "lvgl"``。
2. 生成代码。
3. 把 ``src/lvgl`` 加入固件工程。
4. 如果使用 ``external-bin``，把生成的 romfs 和资源描述文件一起加入固件。
5. 在固件启动时调用 ``lvgl_generated_ui_create()``。

资源路径
---------

* 图片：``assets/...``
* 字体：``/...``（相对 ``assets/``）
* 3D 模型：相对 ``assets/``
* 不要在 HML 中写本机绝对路径。

常见问题
---------

**能否导出 HML XML 或 HML BIN 在设备上解析？**

当前插件不提供这两个导出入口。插件生成的是 C 代码和资源镜像。

**更新 UI 是否需要重新编译固件？**

使用 C 代码方案时需要重新编译固件。资源镜像可以单独通过 UART 更新，但 UI 结构代码仍在固件中。

**如何切换 HoneyGUI 和 LVGL？**

修改 ``project.json`` 的 ``targetEngine``，清理旧生成目录后重新生成代码。

**OTA 更新安全吗？**

需要在固件侧实现签名校验和版本校验。插件不负责设备端 OTA 安全策略。

**为什么下载时提示 romfs 不存在？**

先执行完整编译，确认 ``build/app_romfs_<baseAddr>.bin`` 已生成，并检查 ``romfsBaseAddr`` 是否一致。

开发建议
---------

* 开发阶段使用 PC 仿真快速迭代。
* 发布前确认所有 ``callFunction`` 回调已在 ``custom_functions`` 或 ``src/user`` 中实现。
* 资源较大时优先使用压缩格式和合适的分辨率。
* 固件集成后保留生成的入口文件和保护区，避免覆盖用户代码。
