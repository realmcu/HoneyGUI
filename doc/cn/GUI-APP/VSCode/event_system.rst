事件系统
========

HoneyGUI Design 的事件系统采用 **事件-动作 (Event-Action)** 模式,用户可以在设计器中为组件配置事件响应,无需编写代码即可实现常见交互逻辑。

核心概念
--------

事件 (Event)
~~~~~~~~~~~~

组件触发的交互行为。

动作 (Action)
~~~~~~~~~~~~~

事件触发后执行的操作,一个事件可以配置多个动作,按顺序执行。

消息 (Message)
~~~~~~~~~~~~~~

全局广播机制,组件可以发送消息,其他组件可以监听并响应。

UI 设计变更
-----------

属性面板标签页统一
~~~~~~~~~~~~~~~~~~

原有的"视图切换"功能已统一到 Event-Action 体系中:

.. code-block:: text

   ┌─────────────────────────────────────┐
   │ [属性] [事件]                       │  ← 所有组件统一使用
   ├─────────────────────────────────────┤

* **属性**: 组件基础属性 (位置、大小、样式等)
* **事件**: Event-Action 配置 (包含原"视图切换"功能)

事件类型 (Event Types)
-----------------------

通用事件 (所有组件支持)
~~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 20 20 60

   * - 事件名
     - 标识符
     - 说明
   * - 单击
     - ``onClick``
     - 点击后抬起触发
   * - 长按
     - ``onLongPress``
     - 按住超过阈值时间触发
   * - 按下
     - ``onTouchDown``
     - 手指按下时触发
   * - 抬起
     - ``onTouchUp``
     - 手指抬起时触发

滑动事件 (容器组件支持)
~~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 20 20 40 20

   * - 事件名
     - 标识符
     - 说明
     - 支持组件
   * - 左滑
     - ``onSwipeLeft``
     - 向左滑动触发
     - hg_view, hg_window
   * - 右滑
     - ``onSwipeRight``
     - 向右滑动触发
     - hg_view, hg_window
   * - 上滑
     - ``onSwipeUp``
     - 向上滑动触发
     - hg_view, hg_window
   * - 下滑
     - ``onSwipeDown``
     - 向下滑动触发
     - hg_view, hg_window

状态事件 (特定组件支持)
~~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 20 20 40 20

   * - 事件名
     - 标识符
     - 说明
     - 支持组件
   * - 值改变
     - ``onValueChange``
     - 值变化时触发
     - hg_slider, hg_progressbar, hg_input
   * - 选中改变
     - ``onCheckedChange``
     - 选中状态变化
     - hg_checkbox, hg_switch, hg_radio

生命周期事件 (容器组件支持)
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 20 20 40 20

   * - 事件名
     - 标识符
     - 说明
     - 支持组件
   * - 显示
     - ``onShow``
     - 组件显示时触发
     - hg_view, hg_window
   * - 隐藏
     - ``onHide``
     - 组件隐藏时触发
     - hg_view, hg_window

消息事件 (所有组件支持)
~~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 20 20 60

   * - 事件名
     - 标识符
     - 说明
   * - 消息监听
     - ``onMessage``
     - 收到指定消息时触发

组件支持的事件
--------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - 组件类型
     - 支持的事件
   * - hg_view
     - onClick, onLongPress, onTouchDown, onTouchUp, onSwipeLeft, onSwipeRight, onSwipeUp, onSwipeDown, onShow, onHide, onMessage
   * - hg_window
     - onClick, onLongPress, onTouchDown, onTouchUp, onSwipeLeft, onSwipeRight, onSwipeUp, onSwipeDown, onShow, onHide, onMessage
   * - hg_button
     - onClick, onLongPress, onTouchDown, onTouchUp, onMessage
   * - hg_image
     - onClick, onLongPress, onTouchDown, onTouchUp, onMessage
   * - hg_text
     - onClick, onLongPress, onMessage
   * - hg_switch
     - onClick, onCheckedChange, onMessage
   * - hg_checkbox
     - onClick, onCheckedChange, onMessage
   * - hg_radio
     - onClick, onCheckedChange, onMessage
   * - hg_slider
     - onClick, onValueChange, onMessage
   * - hg_progressbar
     - onClick, onValueChange, onMessage
   * - hg_input
     - onClick, onValueChange, onMessage
   * - hg_video
     - onClick, onLongPress, onMessage
   * - hg_3d
     - onClick, onLongPress, onMessage

动作类型 (Action Types)
------------------------

视图切换动作
~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 20 20 30 30

   * - 动作名
     - 标识符
     - 参数
     - 说明
   * - 跳转界面
     - ``switchView``
     - target, switchOutStyle, switchInStyle
     - 带动画的视图切换

**switchOutStyle 选项**:

* 平移: ``SWITCH_OUT_TO_LEFT/RIGHT/TOP/BOTTOM_USE_TRANSLATION``
* 立方体: ``SWITCH_OUT_TO_LEFT/RIGHT/TOP/BOTTOM_USE_CUBE``
* 其他: ``SWITCH_OUT_NONE_ANIMATION``, ``SWITCH_OUT_ANIMATION_FADE``

**switchInStyle 选项**:

* 平移: ``SWITCH_IN_FROM_LEFT/RIGHT/TOP/BOTTOM_USE_TRANSLATION``
* 立方体: ``SWITCH_IN_FROM_LEFT/RIGHT/TOP/BOTTOM_USE_CUBE``
* 其他: ``SWITCH_IN_NONE_ANIMATION``, ``SWITCH_IN_ANIMATION_FADE``

消息动作
~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 20 20 20 40

   * - 动作名
     - 标识符
     - 参数
     - 说明
   * - 发送消息
     - ``sendMessage``
     - message
     - 广播消息

自定义动作
~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 20 20 20 40

   * - 动作名
     - 标识符
     - 参数
     - 说明
   * - 调用函数
     - ``callFunction``
     - functionName
     - 调用用户自定义 C 函数

消息系统 (Message System)
--------------------------

概念
~~~~

消息系统提供组件间的解耦通信机制:

* **发送方**: 通过 ``sendMessage`` 动作广播消息
* **接收方**: 通过 ``onMessage`` 事件监听消息

使用场景
~~~~~~~~

1. **跨界面通信**: 界面 A 的操作影响界面 B 的显示
2. **全局状态同步**: 多个组件响应同一状态变化
3. **模块解耦**: 避免组件间直接引用

数据结构
--------

HML 中的事件配置
~~~~~~~~~~~~~~~~

**按钮点击跳转**:

.. code-block:: xml

   <hg_button id="btn1" x="10" y="10" w="100" h="40" text="Click">
     <events>
       <event type="onClick">
         <action type="switchView" target="screen2" />
       </event>
     </events>
   </hg_button>

**视图滑动切换 (原 view_switch 功能)**:

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

TypeScript 类型定义
~~~~~~~~~~~~~~~~~~~

.. code-block:: typescript

   // 事件类型
   type EventType =
     // 通用事件
     | 'onClick' | 'onLongPress' | 'onTouchDown' | 'onTouchUp'
     // 滑动事件
     | 'onSwipeLeft' | 'onSwipeRight' | 'onSwipeUp' | 'onSwipeDown'
     // 状态事件
     | 'onValueChange' | 'onCheckedChange'
     // 生命周期事件
     | 'onShow' | 'onHide'
     // 消息事件
     | 'onMessage';

   // 动作类型
   type ActionType =
     // 视图切换(带动画)
     | 'switchView'
     // 消息
     | 'sendMessage'
     // 自定义
     | 'callFunction';

   // 动作定义
   interface Action {
     type: ActionType;
     target?: string;           // 目标组件/界面ID
     message?: string;          // 消息名(sendMessage用)
     functionName?: string;     // 函数名(callFunction用)
     // switchView专用
     switchOutStyle?: string;   // 退出动画
     switchInStyle?: string;    // 进入动画
   }

   // 事件定义
   interface EventConfig {
     type: EventType;
     message?: string;          // 仅 onMessage 事件需要
     actions: Action[];
   }

C 代码生成实现
--------------

视图切换 (switchView)
~~~~~~~~~~~~~~~~~~~~~

无需生成独立的回调函数,直接在 ``GUI_VIEW_INSTANCE`` 宏的 ``switch_in`` 回调中使用 ``gui_view_switch_on_event`` 注册:

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

其他事件
~~~~~~~~

将生成回调函数并在组件创建时绑定:

.. code-block:: c

   // ui/main_ui.c
   gui_obj_add_event_cb(btn1, on_btn1_click, GUI_EVENT_CLICKED, NULL);

   // callbacks/main_callbacks.c
   /* @protected start on_btn1_click */
   void on_btn1_click(gui_obj_t *obj) {
       // 用户实现逻辑
   }
   /* @protected end on_btn1_click */

UI 设计
-------

属性面板 - 事件标签页
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: text

   ┌─────────────────────────────────────┐
   │ [属性] [事件]                       │
   ├─────────────────────────────────────┤
   │                                     │
   │ ▼ onSwipeLeft                [删除] │
   │ ┌─────────────────────────────────┐ │
   │ │ 1. switchView                   │ │
   │ │    目标视图: [screen2    ▼]     │ │
   │ │    退出动画: [向左平移   ▼]     │ │
   │ │    进入动画: [从右平移   ▼]     │ │
   │ │                         [×]     │ │
   │ └─────────────────────────────────┘ │
   │ [+ 添加动作]                        │
   │                                     │
   │ [+ 添加事件]                        │
   │                                     │
   └─────────────────────────────────────┘

实际使用示例
------------

示例 1: 按钮点击跳转
~~~~~~~~~~~~~~~~~~~~

**场景**: 点击"下一步"按钮跳转到第二页

**HML 配置**:

.. code-block:: xml

   <hg_button id="next_btn" x="190" y="200" w="100" h="40" text="下一步">
     <events>
       <event type="onClick">
         <action type="switchView" target="page2"
                 switchOutStyle="SWITCH_OUT_TO_LEFT_USE_TRANSLATION"
                 switchInStyle="SWITCH_IN_FROM_RIGHT_USE_TRANSLATION" />
       </event>
     </events>
   </hg_button>

**生成的 C 代码**:

.. code-block:: c

   // 在 callbacks 文件中生成回调
   void on_next_btn_click(gui_obj_t *obj) {
       gui_switch_app(gui_current_app(), gui_app_get_by_name("page2"));
   }

示例 2: 左滑切换界面
~~~~~~~~~~~~~~~~~~~~

**场景**: 在主界面左滑进入设置界面

**HML 配置**:

.. code-block:: xml

   <hg_view id="main_view" x="0" y="0" w="480" h="272">
     <events>
       <event type="onSwipeLeft">
         <action type="switchView" target="settings"
                 switchOutStyle="SWITCH_OUT_TO_LEFT_USE_TRANSLATION"
                 switchInStyle="SWITCH_IN_FROM_RIGHT_USE_TRANSLATION" />
       </event>
     </events>
   </hg_view>

**生成的 C 代码**:

.. code-block:: c

   static void main_view_switch_in(gui_view_t *view)
   {
       gui_view_switch_on_event(view, "settings",
                                SWITCH_OUT_TO_LEFT_USE_TRANSLATION,
                                SWITCH_IN_FROM_RIGHT_USE_TRANSLATION,
                                GUI_EVENT_TOUCH_MOVE_LEFT);
       // ...
   }

示例 3: 消息通信
~~~~~~~~~~~~~~~~

**场景**: 按钮发送消息,其他组件监听并响应

**发送方 HML**:

.. code-block:: xml

   <hg_button id="refresh_btn" x="10" y="10" w="100" h="40" text="刷新">
     <events>
       <event type="onClick">
         <action type="sendMessage" message="refresh_data" />
       </event>
     </events>
   </hg_button>

**接收方 HML**:

.. code-block:: xml

   <hg_text id="data_label" x="10" y="60" w="200" h="30" text="数据">
     <events>
       <event type="onMessage" message="refresh_data">
         <action type="callFunction" function="on_refresh_data" />
       </event>
     </events>
   </hg_text>

示例 4: 自定义函数调用
~~~~~~~~~~~~~~~~~~~~~~

**场景**: 按钮点击执行复杂业务逻辑

**HML 配置**:

.. code-block:: xml

   <hg_button id="save_btn" x="10" y="10" w="100" h="40" text="保存">
     <events>
       <event type="onClick">
         <action type="callFunction" function="save_settings" />
       </event>
     </events>
   </hg_button>

**用户实现**:

.. code-block:: c

   // callbacks/main_callbacks.c
   /* @protected start save_settings */
   void save_settings(gui_obj_t *obj) {
       // 读取各种设置
       bool wifi_enabled = get_wifi_switch_state();
       int brightness = get_brightness_value();

       // 保存到闪存
       flash_write_config(wifi_enabled, brightness);

       // 显示提示
       show_toast("设置已保存");
   }
   /* @protected end save_settings */

最佳实践
--------

事件命名规范
~~~~~~~~~~~~

* 使用清晰的函数名: ``on_save_click``, ``handle_login``
* 包含事件类型: ``on_button_click``, ``on_slider_change``
* 描述业务逻辑: ``save_user_settings``, ``refresh_data``

性能优化
~~~~~~~~

* 避免在频繁触发的事件中执行耗时操作
* 使用消息系统解耦复杂交互
* 合理使用防抖和节流

调试技巧
~~~~~~~~

* 在回调函数中添加日志输出
* 使用断点调试验证事件流程
* 检查生成的 C 代码确认事件绑定

下一步
------

* 学习 :doc:`code_generation` 了解代码生成机制
* 查看 :doc:`hml_format` 了解 HML 文件格式
* 参考 :doc:`build_simulation` 构建和测试应用
* 阅读 :doc:`deployment` 了解嵌入式部署方案
