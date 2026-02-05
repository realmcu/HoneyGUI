嵌入式部署
==========

本文档描述 HoneyGUI Design 项目到嵌入式平台的部署方案,包括三种不同的部署方式及其适用场景。

概述
----

HoneyGUI Design 设计器支持多种部署方式,可以根据项目需求选择最合适的方案:

.. list-table::
   :header-rows: 1
   :widths: 15 20 15 20 30

   * - 方案
     - 导出格式
     - 固件改动
     - 资源占用
     - 适用场景
   * - 方案一
     - C 代码
     - 每次都要
     - 最小
     - 资源受限、UI 固定
   * - 方案二
     - HML (XML)
     - 不需要
     - 中等 (~10KB 解析器)
     - 需要 OTA 更新 UI
   * - 方案三
     - HML (BIN)
     - 不需要
     - 小 (~2KB 解析器)
     - 需要 OTA + 资源受限

方案一: 生成 C 代码
-------------------

描述
~~~~

设计器将 HML 设计转换为 C 代码,直接编译到固件中。这是最常用的方式。

导出产物
~~~~~~~~

.. code-block:: text

   export/c/
   ├── ui/
   │   ├── main_ui.h           # UI 声明
   │   └── main_ui.c           # UI 实现
   ├── callbacks/
   │   ├── main_callbacks.h    # 回调声明
   │   └── main_callbacks.c    # 回调实现(保护区)
   └── assets/
       └── *.bin               # 二进制资源

生成代码示例
~~~~~~~~~~~~

.. code-block:: c

   // main_ui.c
   #include "main_ui.h"

   gui_obj_t *button1 = NULL;
   gui_obj_t *label1 = NULL;

   void main_ui_create(gui_obj_t *parent) {
       gui_obj_t *view = gui_view_create(parent, "main_view", 0, 0, 480, 272);

       button1 = gui_button_create(view, "button1", 10, 10, 100, 40);
       gui_button_set_text(button1, "Click");
       gui_obj_set_event_cb(button1, GUI_EVENT_CLICK, on_button1_click);

       label1 = gui_text_create(view, "label1", 10, 60, 200, 30);
       gui_text_set_text(label1, "Hello");
   }

优点
~~~~

* ROM/RAM 占用最小
* 执行速度最快
* 调试方便 (可单步跟踪)
* 事件处理灵活 (纯 C 代码)

缺点
~~~~

* 改 UI 必须重新编译固件
* 无法 OTA 更新界面

适用场景
~~~~~~~~

* 资源受限的 MCU (Flash < 256KB)
* UI 设计定型后不再改动
* 对启动速度要求高

部署步骤
~~~~~~~~

1. **生成代码**:

   在 VSCode 中点击"生成代码"按钮,生成 C 代码文件。

2. **集成到固件项目**:

   将 ``src/`` 目录下的文件复制到固件项目中:

   .. code-block:: bash

      # 假设固件项目在 /path/to/firmware
      cp -r src/ui /path/to/firmware/app/gui/
      cp -r src/callbacks /path/to/firmware/app/gui/
      cp -r src/user /path/to/firmware/app/gui/
      cp src/*Entry.c /path/to/firmware/app/

3. **添加资源文件**:

   .. code-block:: bash

      cp -r assets/*.bin /path/to/firmware/resources/

4. **修改构建脚本**:

   在固件的 Makefile 或 CMakeLists.txt 中添加源文件:

   .. code-block:: cmake

      # CMakeLists.txt 示例
      file(GLOB GUI_SOURCES
           "app/gui/ui/*.c"
           "app/gui/callbacks/*.c"
           "app/gui/user/*.c"
           "app/*Entry.c")

      target_sources(firmware PRIVATE ${GUI_SOURCES})

5. **编译固件**:

   .. code-block:: bash

      cd /path/to/firmware
      mkdir build && cd build
      cmake ..
      make

6. **烧录固件**:

   使用对应平台的烧录工具将固件烧录到设备。

方案二: 生成 HML (XML)
-----------------------

描述
~~~~

设计器导出 HML 文件 (XML 格式),嵌入式设备运行时解析并创建 UI。

导出产物
~~~~~~~~

.. code-block:: text

   export/hml/
   ├── ui/
   │   └── main.hml            # HML 文件
   ├── assets/
   │   ├── images/             # 图片资源
   │   └── fonts/              # 字体资源
   └── manifest.json           # 资源清单

HML 文件示例
~~~~~~~~~~~~

.. code-block:: xml

   <?xml version="1.0" encoding="UTF-8"?>
   <hg_screen id="main_screen" w="480" h="272">
     <hg_view id="main_view" x="0" y="0" w="480" h="272">
       <hg_button id="button1" x="10" y="10" w="100" h="40"
                  text="Click"
                  onClick="on_button1_click" />
       <hg_text id="label1" x="10" y="60" w="200" h="30"
                text="Hello"
                fontSize="16"
                color="#FFFFFF" />
     </hg_view>
   </hg_screen>

嵌入式侧实现
~~~~~~~~~~~~

需要集成 XML 解析器 (推荐 ezxml,约 10KB):

.. code-block:: c

   #include "ezxml.h"
   #include "gui_hml_loader.h"

   // 加载 HML 文件
   gui_obj_t *ui = gui_hml_load("/flash/ui/main.hml");

   // 回调函数注册表 (固件中预编译)
   const callback_entry_t g_callbacks[] = {
       {"on_button1_click", on_button1_click},
       {"on_button1_long_press", on_button1_long_press},
       {NULL, NULL}
   };

事件绑定机制
~~~~~~~~~~~~

HML 中声明事件绑定:

.. code-block:: xml

   <hg_button id="btn1" onClick="on_btn1_click" />

固件中实现回调:

.. code-block:: c

   void on_btn1_click(gui_obj_t *obj) {
       // 复杂业务逻辑
       ble_send_data(...);
       update_status(...);
   }

优点
~~~~

* UI 布局可 OTA 更新
* 可读性好,便于调试
* 支持数据绑定 (``text="{data.title}"``)

缺点
~~~~

* 需要 XML 解析器 (~10KB ROM)
* 解析速度较慢
* 运行时内存占用较大

适用场景
~~~~~~~~

* 需要 OTA 更新 UI
* Flash 充足 (> 256KB)
* 开发调试阶段

推荐 XML 解析器
~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 20 20 60

   * - 解析器
     - ROM
     - 特点
   * - ezxml
     - ~10KB
     - DOM 模式,API 简洁
   * - yxml
     - ~3KB
     - SAX 流式,零动态内存
   * - mxml
     - ~30KB
     - 功能完整

方案三: 生成 HML (BIN)
-----------------------

描述
~~~~

设计器将 HML 编译为二进制格式,嵌入式设备直接加载,无需 XML 解析。

导出产物
~~~~~~~~

.. code-block:: text

   export/bin/
   ├── ui/
   │   └── main.bin            # 二进制 UI 描述
   ├── assets/
   │   └── *.bin               # 二进制资源
   └── manifest.bin            # 二进制资源清单

二进制格式设计
~~~~~~~~~~~~~~

.. code-block:: c

   // 文件头
   struct hml_bin_header {
       uint32_t magic;            // 0x484D4C42 "HMLB"
       uint16_t version;          // 格式版本
       uint16_t component_count;  // 组件数量
       uint32_t string_table_offset;  // 字符串表偏移
       uint32_t data_offset;      // 数据区偏移
   };

   // 组件描述
   struct hml_bin_component {
       uint8_t  type;             // 组件类型 (HG_BUTTON=1, HG_TEXT=2...)
       uint8_t  parent_index;     // 父组件索引
       uint16_t id_offset;        // ID 字符串偏移
       int16_t  x, y, w, h;       // 位置和尺寸
       uint16_t props_count;      // 属性数量
       uint32_t props_offset;     // 属性数据偏移
       uint16_t events_count;     // 事件数量
       uint32_t events_offset;    // 事件数据偏移
   };

   // 属性描述
   struct hml_bin_prop {
       uint16_t key_offset;       // 属性名偏移
       uint16_t value_offset;     // 属性值偏移
       uint8_t  value_type;       // 值类型 (STRING=0, INT=1, FLOAT=2...)
   };

   // 事件描述
   struct hml_bin_event {
       uint8_t  event_type;       // 事件类型 (CLICK=1, LONG_PRESS=2...)
       uint16_t handler_offset;   // 处理函数名偏移
   };

嵌入式侧实现
~~~~~~~~~~~~

二进制加载器 (约 2KB):

.. code-block:: c

   #include "gui_hml_bin_loader.h"

   // 直接内存映射加载
   gui_obj_t *ui = gui_hml_bin_load("/flash/ui/main.bin");

   // 或从内存加载
   extern const uint8_t main_ui_bin[];
   gui_obj_t *ui = gui_hml_bin_load_mem(main_ui_bin);

优点
~~~~

* 解析器极小 (~2KB ROM)
* 加载速度快 (可直接内存映射)
* UI 可 OTA 更新
* 文件体积比 XML 小

缺点
~~~~

* 不可读,调试困难
* 需要设计器支持编译
* 格式升级需要兼容处理

适用场景
~~~~~~~~

* 资源受限但需要 OTA
* 对启动速度有要求
* 产品发布阶段

事件处理机制 (通用)
-------------------

三种方案都采用 **函数名绑定** 机制:

设计器中配置事件
~~~~~~~~~~~~~~~~

在属性面板中为组件配置事件处理函数名。

导出时生成回调骨架
~~~~~~~~~~~~~~~~~~

.. code-block:: c

   // main_callbacks.c (首次生成,后续保护)

   /* @protected start on_button1_click */
   void on_button1_click(gui_obj_t *obj) {
       // TODO: 实现点击逻辑
   }
   /* @protected end on_button1_click */

   /* @protected start on_button1_long_press */
   void on_button1_long_press(gui_obj_t *obj) {
       // TODO: 实现长按逻辑
   }
   /* @protected end on_button1_long_press */

固件中注册回调表
~~~~~~~~~~~~~~~~

.. code-block:: c

   // callback_registry.c
   #include "main_callbacks.h"

   const callback_entry_t g_callbacks[] = {
       {"on_button1_click", on_button1_click},
       {"on_button1_long_press", on_button1_long_press},
       // ... 其他回调
       {NULL, NULL}
   };

   // 查找回调函数
   void *find_callback(const char *name) {
       for (int i = 0; g_callbacks[i].name; i++) {
           if (strcmp(g_callbacks[i].name, name) == 0) {
               return g_callbacks[i].func;
           }
       }
       return NULL;
   }

数据绑定 (方案二、三支持)
-------------------------

HML 中使用数据绑定
~~~~~~~~~~~~~~~~~~

.. code-block:: xml

   <hg_text id="title" text="{data.title}" />
   <hg_text id="battery" text="{data.battery}%" />
   <hg_image id="icon" src="{data.icon_path}" />

嵌入式侧绑定数据
~~~~~~~~~~~~~~~~

.. code-block:: c

   // 定义数据结构
   typedef struct {
       char title[32];
       int battery;
       char icon_path[64];
   } ui_data_t;

   // 绑定数据
   ui_data_t data = {"消息通知", 85, "/assets/msg.bin"};
   gui_hml_bind_data(ui, &data);

方案选择指南
------------

.. code-block:: text

                   ┌─────────────────┐
                   │ 需要 OTA 更新 UI?│
                   └────────┬────────┘
                            │
             ┌──────────────┴──────────────┐
             │ 否                          │ 是
             ▼                             ▼
     ┌───────────────┐           ┌─────────────────┐
     │ 方案一: C 代码 │           │ Flash 充足?      │
     └───────────────┘           └────────┬────────┘
                                          │
                           ┌──────────────┴──────────────┐
                           │ 是                          │ 否
                           ▼                             ▼
                   ┌───────────────┐           ┌───────────────┐
                   │ 方案二: HML    │           │ 方案三: BIN    │
                   │ (调试方便)     │           │ (体积小)       │
                   └───────────────┘           └───────────────┘

性能和资源对比
--------------

.. list-table::
   :header-rows: 1
   :widths: 20 20 20 20 20

   * - 指标
     - 方案一 (C)
     - 方案二 (XML)
     - 方案三 (BIN)
     - 说明
   * - ROM 占用
     - 最小
     - +10KB
     - +2KB
     - 解析器大小
   * - 启动速度
     - 最快
     - 慢
     - 快
     - 需要解析时间
   * - OTA 支持
     - ✗
     - ✓
     - ✓
     - 是否可远程更新
   * - 调试难度
     - 低
     - 低
     - 高
     - 可读性
   * - 文件大小
     - N/A
     - 大
     - 中
     - UI 描述文件

开发建议
--------

开发阶段
~~~~~~~~

* 使用 **方案一 (C 代码)** 在 PC 模拟器上快速迭代
* 使用 VSCode 插件的构建和模拟功能
* 利用保护区保留自定义逻辑

测试阶段
~~~~~~~~

* 根据项目需求选择最终方案
* 如需 OTA,切换到 **方案二 (XML)** 进行测试
* 验证资源占用和性能指标

发布阶段
~~~~~~~~

* 资源充足: 使用 **方案二 (XML)** 保持灵活性
* 资源受限: 使用 **方案三 (BIN)** 或 **方案一 (C)**
* 确保所有回调函数实现完整

常见问题
--------

Q: 如何在三种方案间切换?
~~~~~~~~~~~~~~~~~~~~~~~~~

A: 设计器生成代码时可以选择导出格式。已有项目可以重新导出为不同格式。

Q: 方案二和方案三可以混用吗?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A: 可以。例如开发时使用 XML,发布时编译为 BIN。两者数据结构兼容。

Q: OTA 更新 UI 安全吗?
~~~~~~~~~~~~~~~~~~~~~~

A: 需要在固件中实现签名验证机制,确保 UI 文件来源可信。

Q: 性能差异有多大?
~~~~~~~~~~~~~~~~~~

A: 方案一最快 (直接执行),方案三次之 (简单解析),方案二最慢 (XML 解析)。实际差异取决于 UI 复杂度,通常在几十毫秒级别。

下一步
------

* 查看 :doc:`code_generation` 了解代码生成细节
* 学习 :doc:`build_simulation` 在 PC 上测试
* 参考 :doc:`resource_management` 优化资源使用
* 阅读 HoneyGUI SDK 移植文档了解平台适配
