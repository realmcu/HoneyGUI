代码生成
========

本文介绍 HoneyDesigner 的双引擎代码生成流程、产物结构和保护区机制。

概述
-----

设计器把 HML 转换为目标引擎的 C 代码。目标引擎由 ``project.json`` 的 ``targetEngine`` 决定：

* ``honeygui``：生成 HoneyGUI API 调用代码，使用 SCons 构建。
* ``lvgl``：生成 LVGL API 调用代码，使用 CMake 构建。

代码生成流程
-------------

.. code-block:: text

   HML 文件
     -> HML 解析器
     -> 组件树 / 事件 / 定时器
     -> CodeGeneratorFactory
     -> HoneyGuiCCodeGenerator 或 LvglCCodeGenerator
     -> src/ 下的 C 代码

架构
-----

核心模块：

* ``CodeGeneratorFactory``：根据 ``targetEngine`` 选择生成器。
* ``HoneyGuiCCodeGenerator``：生成 HoneyGUI UI、回调、用户代码和 SConscript。
* ``LvglCCodeGenerator``：生成 LVGL UI、回调、入口文件和资源描述。
* ``EntryFileGenerator``：生成 HoneyGUI 项目入口 ``{ProjectName}Entry.c``。
* ``SConscriptGenerator``：生成 ``src/SConscript``。

生成命令
---------

* 工具栏 :guilabel:`生成代码`。
* 命令面板：``HoneyDesigner: 生成代码``。
* 快捷键：``Ctrl+G``。
* 编译仿真流程中也可以勾选“生成代码”。

生成时会扫描项目 ``ui/`` 目录下的所有 HML 文件。

HoneyGUI 产物
--------------

.. code-block:: text

   src/
   ├── {ProjectName}Entry.c      # 项目入口，含保护区
   ├── SConscript                # 构建脚本，每次生成
   ├── ui/
   │   ├── {name}_ui.h           # 每次覆盖
   │   └── {name}_ui.c           # 每次覆盖
   ├── callbacks/
   │   ├── {name}_callbacks.h    # 声明，重新生成
   │   └── {name}_callbacks.c    # 重新生成，保留保护区
   └── user/
       ├── {name}_user.h         # 只生成一次
       └── {name}_user.c         # 只生成一次

命名规则：

* ``{name}`` 是 HML 文件名去掉扩展名，例如 ``dashboard.hml`` -> ``dashboard_ui.c``。
* 所有 HML 的 UI 代码统一输出到 ``src/ui``，不再按原目录层级生成。
* 不同目录下的 HML 如果文件名相同，会生成同名 C 文件，因此建议项目内保持 HML basename 唯一。
* HML basename 应使用合法 C 标识符字符。

LVGL 产物
----------

.. code-block:: text

   src/lvgl/
   ├── {name}_lvgl_ui.c/h
   ├── {name}_lvgl_callbacks.c/h
   ├── lvgl_generated_ui.c/h     # 多页面入口
   ├── ui_resource.h             # external-bin 资源宏
   └── lv_img_dsc_list.c/h       # external-bin 图片描述

LVGL 资源有两种部署方式：

* ``c-array``：图片和字体编译为 C 数组，链接进固件。
* ``external-bin``：资源打包为 romfs，运行时从外部存储加载。

生成的代码结构
---------------

以 HoneyGUI 为例，一个 HML 文件会生成 UI 头文件、UI 实现文件和回调文件。LVGL 生成的文件结构类似，只是使用 LVGL API 和 ``{name}_lvgl_*`` 命名。

头文件 (.h)
~~~~~~~~~~~

.. code-block:: c

   #ifndef DASHBOARD_UI_H
   #define DASHBOARD_UI_H

   #include "gui_obj.h"
   #include "gui_view_instance.h"

   // 组件句柄声明
   extern gui_obj_t *lbl_title;
   extern gui_obj_t *btn_next;

   #endif

实现文件 (.c)
~~~~~~~~~~~~~

.. code-block:: c

   #include "dashboard_ui.h"
   #include "../callbacks/dashboard_callbacks.h"

   gui_obj_t *lbl_title = NULL;
   gui_obj_t *btn_next = NULL;

   static void view_home_switch_in(gui_view_t *view)
   {
       // 创建子组件并注册 switchView 事件
   }

   GUI_VIEW_INSTANCE("view_home", false, view_home_switch_in, view_home_switch_out, false);

回调文件 (_callbacks.h / _callbacks.c)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   void on_btn_next_click(void *obj, gui_event_t *e)
   {
       GUI_UNUSED(obj);
       GUI_UNUSED(e);
       // 事件回调主体，重新生成时覆盖
   }

   /* @protected start custom_functions */
   void on_next_click(void *obj, gui_event_t *e)
   {
       // 用户业务逻辑
   }
   /* @protected end custom_functions */

``{name}_user.c/h`` 只生成一次，入口文件和 ``SConscript`` 会随项目重新生成；保护区和更新规则见下一节。

代码保护区
-----------

生成器识别两种保护区标记：

.. code-block:: c

   /* @protected start custom_functions */
   /* 用户代码 */
   /* @protected end custom_functions */

.. code-block:: c

   /* USER CODE BEGIN custom_functions */
   /* 用户代码 */
   /* USER CODE END custom_functions */

各目录行为：

* ``src/ui``：每次生成覆盖，不要手动修改。
* ``src/callbacks``：文件重新生成，但保护区内容保留。
* ``src/user``：只生成一次，之后完全由用户维护。
* ``src/{ProjectName}Entry.c``：重新生成，但包含入口保护区。

.. warning::
   事件回调函数主体不是保护区，会随 HML 变化重新生成。业务逻辑请写在 ``custom_functions`` 中，并通过 ``callFunction`` 调用。

视图切换
---------

``hg_view`` 上的 ``switchView`` 不生成独立回调，而是在 ``GUI_VIEW_INSTANCE`` 的 ``switch_in`` 回调中注册：

.. code-block:: c

   static void view_main_switch_in(gui_view_t *view)
   {
       gui_view_switch_on_event(view, "view_settings",
                                SWITCH_OUT_TO_LEFT_USE_TRANSLATION,
                                SWITCH_IN_FROM_RIGHT_USE_TRANSLATION,
                                GUI_EVENT_TOUCH_MOVE_LEFT);
   }

   GUI_VIEW_INSTANCE("view_main", false, view_main_switch_in, view_main_switch_out, false);

注意 ``GUI_VIEW_INSTANCE`` 当前使用 5 个参数，最后一个参数用于快照开关。

多 HML 项目
------------

* 每个 HML 独立生成 ``{name}_ui.c``、``{name}_callbacks.c``、``{name}_user.c``。
* 多个 HML 生成一个项目入口文件和一份 ``SConscript``。
* ``switchView`` 可以跨 HML 文件跳转，目标按 view id 或 name 解析。
* 组件 ID 建议在项目内全局唯一，生成代码会作为 C 全局变量名使用。

注意事项
---------

1. HML 文件应放在项目 ``ui/`` 目录下。
2. HML basename 使用合法 C 标识符字符。
3. 不要修改保护区标记。
4. 修改 HML 后重新生成，保护区内的用户代码会保留。
5. 切换 ``targetEngine`` 后旧的生成目录不会自动清理，建议先清理再重新生成。

下一步
-------

* 学习 :doc:`resource_management` 管理项目资源
* 查看 :doc:`build_simulation` 了解如何构建和运行
* 参考 :doc:`deployment` 了解嵌入式部署方案
