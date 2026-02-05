代码生成
========

本文档描述 HoneyGUI Design 的代码生成功能,包括架构设计、使用方法和实现细节。

概述
----

从 HML 设计器生成调用 HoneyGUI API 的 C 代码,实现可视化设计到嵌入式 GUI 代码的转换。

代码生成流程
------------

.. code-block:: text

   HML文件 → 解析器 → 组件树 → C代码生成器 → HoneyGUI API调用代码

架构设计
--------

核心组件
~~~~~~~~

.. code-block:: text

   CodeGenerator (服务层)
     ├─ 扫描项目中的所有 HML 文件
     ├─ 生成项目入口文件 (EntryFileGenerator)
     ├─ 生成构建脚本 (SConscriptGenerator)
     └─ 为每个 HML 生成代码 (HoneyGuiCCodeGenerator)

   EntryFileGenerator (独立工具)
     └─ 生成 {ProjectName}Entry.c

   SConscriptGenerator (独立工具)
     └─ 生成 src/SConscript

   HoneyGuiCCodeGenerator (核心生成器)
     ├─ 生成 {hmlFileName}_ui.h
     ├─ 生成 {hmlFileName}_ui.c
     ├─ 生成 {hmlFileName}_callbacks.h
     ├─ 生成 {hmlFileName}_callbacks.c (含保护区)
     ├─ 生成 {hmlFileName}_user.h
     └─ 生成 {hmlFileName}_user.c

文件命名和目录规则
------------------

命名规则
~~~~~~~~

* 使用 HML 文件名作为生成文件的基础名称
* 例如: ``main.hml`` → ``main.h``, ``main.c``, ``main_callbacks.h``, ``main_callbacks.c``

目录映射
~~~~~~~~

* HML 文件在 ``ui/xxx/`` 目录 → C 代码生成到 ``src/`` 目录的对应子目录
* 每个 HML 文件生成到独立的子目录
* 例如: ``ui/main/main.hml`` → ``src/ui/main_ui.c``, ``src/callbacks/main_callbacks.c``, ``src/user/main_user.c``
* 自动创建不存在的子目录

生成文件结构
~~~~~~~~~~~~

.. code-block:: text

   project/
   ├── ui/
   │   ├── main/
   │   │   └── main.hml
   │   └── settings/
   │       └── settings.hml
   └── src/
       ├── {ProjectName}Entry.c    # 项目入口文件
       ├── SConscript              # 构建脚本
       ├── ui/                     # UI代码(每次覆盖)
       │   ├── main_ui.h
       │   ├── main_ui.c
       │   ├── settings_ui.h
       │   └── settings_ui.c
       ├── callbacks/              # 回调代码(保护区)
       │   ├── main_callbacks.h
       │   ├── main_callbacks.c
       │   ├── settings_callbacks.h
       │   └── settings_callbacks.c
       └── user/                   # 用户代码(只生成一次)
           ├── main_user.h
           ├── main_user.c
           ├── settings_user.h
           └── settings_user.c

使用方法
--------

生成代码
~~~~~~~~

1. 在设计器中打开任意 HML 文件
2. 点击工具栏的"生成代码"按钮
3. 自动扫描项目中所有 HML 文件并生成代码
4. 显示生成进度和结果

**生成内容**:

* 所有设计稿的 C 代码文件
* 项目入口文件 ``{ProjectName}Entry.c``
* 构建脚本 ``SConscript``

**快捷方式**:

* 命令面板: ``Ctrl+Shift+P`` → ``HoneyGUI: Generate Code``
* 工具栏按钮: 点击"生成代码"

代码保护区
----------

保护区语法
~~~~~~~~~~

.. code-block:: c

   // HONEYGUI PROTECTED START [unique_id]
   // 这里的代码在重新生成时会被保留
   // HONEYGUI PROTECTED END [unique_id]

示例
~~~~

**首次生成**:

.. code-block:: c

   // HONEYGUI PROTECTED START [on_start_click]
   void on_start_click(gui_obj_t *obj) {
       // TODO: 实现事件处理逻辑
   }
   // HONEYGUI PROTECTED END [on_start_click]

**用户修改后**:

.. code-block:: c

   // HONEYGUI PROTECTED START [on_start_click]
   void on_start_click(gui_obj_t *obj) {
       printf("Starting application...\n");
       init_app_state();
       load_user_settings();
   }
   // HONEYGUI PROTECTED END [on_start_click]

**重新生成后**: 保护区内的代码被保留,其他代码更新。

生成的代码结构
--------------

头文件 (.h)
~~~~~~~~~~~

.. code-block:: c

   #ifndef MAIN_H
   #define MAIN_H

   #include "gui_api.h"

   // 组件声明
   extern gui_obj_t *mainView;
   extern gui_obj_t *titleLabel;
   extern gui_obj_t *startButton;

   // 初始化函数
   void main_init(void);

   // 更新函数
   void main_update(void);

   #endif

实现文件 (.c)
~~~~~~~~~~~~~

.. code-block:: c

   #include "main.h"
   #include "main_callbacks.h"

   // 组件定义
   gui_obj_t *mainView = NULL;
   gui_obj_t *titleLabel = NULL;
   gui_obj_t *startButton = NULL;

   // 视图切换回调(switch_in)
   static void main_view_switch_in(gui_view_t *view)
   {
       // 注册事件
       gui_view_switch_on_event(view, "screen2", SWITCH_OUT_TO_LEFT_USE_TRANSLATION,
                                SWITCH_IN_FROM_RIGHT_USE_TRANSLATION, GUI_EVENT_TOUCH_MOVE_LEFT);

       // 创建子组件
       titleLabel = gui_text_create(view, "titleLabel", 10, 10, 460, 40);
       // ...
   }

   GUI_VIEW_INSTANCE("mainView", false, main_view_switch_in, main_view_switch_out);

回调文件 (_callbacks.h / _callbacks.c)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   // main_callbacks.c
   #include "main_callbacks.h"

   /* @protected start on_start_click */
   void on_start_click(gui_obj_t *obj) {
       // 用户实现逻辑
   }
   /* @protected end on_start_click */

注意事项
--------

1. **目录结构**: HML 文件必须在 ``ui/`` 目录下的子目录中
2. **文件命名**: HML 文件名应使用有效的 C 标识符
3. **保护区**: 不要手动修改保护区标记
4. **重新生成**: 修改 HML 后重新生成,用户代码会被保留
5. **多文件**: 每个 HML 独立生成,互不影响

视图切换实现
------------

对于 ``hg_view`` 组件的视图切换 (switchView),代码生成器不再生成独立的回调函数,而是利用 HoneyGUI SDK 的 ``GUI_VIEW_INSTANCE`` 宏和 ``gui_view_switch_on_event`` 函数在 ``switch_in`` 回调中直接注册。这大大简化了代码结构,并减少了不必要的保护区。

示例
~~~~

HML 文件中配置的视图切换事件:

.. code-block:: xml

   <hg_view id="main_view" x="0" y="0" w="480" h="272">
       <events>
           <event type="onSwipeLeft">
               <action type="switchView" target="screen2"
                       switchOutStyle="SWITCH_OUT_TO_LEFT_USE_TRANSLATION"
                       switchInStyle="SWITCH_IN_FROM_RIGHT_USE_TRANSLATION" />
           </event>
       </events>
   </hg_view>

生成的 C 代码:

.. code-block:: c

   static void main_view_switch_in(gui_view_t *view)
   {
       // 注册左滑跳转事件
       gui_view_switch_on_event(view, "screen2",
                                SWITCH_OUT_TO_LEFT_USE_TRANSLATION,
                                SWITCH_IN_FROM_RIGHT_USE_TRANSLATION,
                                GUI_EVENT_TOUCH_MOVE_LEFT);

       // 创建子组件...
   }

   GUI_VIEW_INSTANCE("main_view", false, main_view_switch_in, main_view_switch_out);

构建生成的代码
--------------

生成代码后,可以使用 SCons 构建系统编译项目:

.. code-block:: bash

   cd win32_sim
   scons

详细的构建和模拟说明请参考 :doc:`build_simulation`。

下一步
------

* 了解 :doc:`event_system` 学习如何处理组件事件
* 学习 :doc:`resource_management` 管理项目资源
* 查看 :doc:`build_simulation` 了解如何构建和运行
* 参考 :doc:`deployment` 了解嵌入式部署方案
