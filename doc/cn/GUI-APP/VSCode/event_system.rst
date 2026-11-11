事件系统
========

HoneyDesigner 的事件系统采用 **事件-动作（Event-Action）** 模式。组件触发事件后，按顺序执行一个或多个动作，无需为常见交互手写 C 代码。

核心概念
---------

* **事件（Event）**：组件被点击、长按、滑动、按键、显示、隐藏或收到消息。
* **动作（Action）**：事件触发后执行的操作，例如切换页面、发送消息、调用 C 函数或控制定时器。
* **消息（Message）**：组件之间解耦通信的全局广播机制。

属性面板中的 **Events** 标签页用于配置事件和动作。**Properties** 标签页用于配置组件自身属性。

事件类型
---------

通用事件
~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 22 22 56

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
     - 手指抬起时触发；可设置 ``checkReleaseArea``

按键事件
~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 22 22 56

   * - 事件名
     - 标识符
     - 说明
   * - 按键短按
     - ``onKeyShortPress``
     - 需要设置 ``keyName``
   * - 按键长按
     - ``onKeyLongPress``
     - 需要设置 ``keyName``

``keyName`` 可选值：``Home``、``Back``、``Menu``、``Power``，也可以输入自定义名称。

滑动事件
~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 22 22 56

   * - 事件名
     - 标识符
     - 说明
   * - 左滑
     - ``onSwipeLeft``
     - 向左滑动触发
   * - 右滑
     - ``onSwipeRight``
     - 向右滑动触发
   * - 上滑
     - ``onSwipeUp``
     - 向上滑动触发
   * - 下滑
     - ``onSwipeDown``
     - 向下滑动触发
   * - 左边缘快滑
     - ``onSwipeLeftQuick``
     - 从边缘快速向左滑动
   * - 右边缘快滑
     - ``onSwipeRightQuick``
     - 从边缘快速向右滑动
   * - 上边缘快滑
     - ``onSwipeUpQuick``
     - 从边缘快速向上滑动
   * - 下边缘快滑
     - ``onSwipeDownQuick``
     - 从边缘快速向下滑动

状态、生命周期和消息事件
~~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 22 22 56

   * - 事件名
     - 标识符
     - 说明
   * - 值改变
     - ``onValueChange``
     - 数值变化时触发
   * - 选中改变
     - ``onCheckedChange``
     - 选中状态变化时触发
   * - 显示
     - ``onShow``
     - 仅 ``hg_view``
   * - 隐藏
     - ``onHide``
     - 仅 ``hg_view``
   * - 消息监听
     - ``onMessage``
     - 收到指定消息时触发，需要 ``message``

组件支持的事件
--------------

.. list-table::
   :header-rows: 1
   :widths: 28 72

   * - 组件
     - 支持的事件
   * - ``hg_view``
     - click、longPress、touchDown、touchUp、key、swipe、quick swipe、show、hide、message
   * - ``hg_window``
     - click、longPress、touchDown、touchUp、key、message
   * - ``hg_button``、``hg_image``
     - click、longPress、touchDown、touchUp、key、message
   * - ``hg_label``
     - click、longPress、key、message
   * - ``hg_switch``、``hg_checkbox``、``hg_radio``
     - click、checkedChange、key、message
   * - ``hg_slider``、``hg_progressbar``、``hg_input``
     - click、valueChange、key、message
   * - ``hg_list``
     - click、longPress、swipe、quick swipe、key、message
   * - 其他图形和多媒体控件
     - 默认支持 click、longPress、key、message；以属性面板实际显示为准

动作类型
---------

一个事件可以配置多个动作，按顺序执行。常用动作：

.. list-table::
   :header-rows: 1
   :widths: 22 30 48

   * - 动作
     - 关键参数
     - 说明
   * - ``switchView``
     - ``target``、``switchOutStyle``、``switchInStyle``
     - 切换页面；一个事件最多配置一个 ``switchView``
   * - ``sendMessage``
     - ``message``
     - 广播消息，供其他组件订阅
   * - ``callFunction``
     - ``functionName``
     - 调用 ``custom_functions`` 保护区中的 C 函数
   * - ``controlTimer``
     - ``timerTargets``
     - 启动、停止或重置其他组件的定时器

HML 中通过 ``<action>`` 声明，一个事件可以包含多个动作：

.. code-block:: xml

   <event type="onClick">
       <action type="switchView" target="view_page2" />
       <action type="callFunction" functionName="on_next_click" />
   </event>

``switchView`` 的切换样式、``controlTimer`` 的 JSON 参数等完整写法见 :doc:`HML 文件格式 <hml_format>`；回调函数实现见 :doc:`代码生成机制 <code_generation>`。

消息系统
---------

发送方用 ``sendMessage`` 广播消息，接收方用 ``onMessage`` 订阅。例如：

.. code-block:: xml

   <!-- 发送方 -->
   <hg_button id="btn_refresh" x="20" y="20" width="120" height="40" text="Refresh">
       <events>
           <event type="onClick">
               <action type="sendMessage" message="refresh_data" />
           </event>
       </events>
   </hg_button>

   <!-- 接收方 -->
   <hg_label id="lbl_data" x="20" y="80" width="240" height="40" text="Data">
       <events>
           <event type="onMessage" message="refresh_data">
               <action type="callFunction" functionName="on_refresh_data" />
           </event>
       </events>
   </hg_label>

生成代码使用 ``gui_msg_publish`` 广播、``gui_msg_subscribe`` 订阅；具体实现见 :doc:`代码生成机制 <code_generation>`。

生成代码
---------

事件回调由代码生成器写入 ``src/callbacks/``。函数主体会在重新生成时覆盖，业务逻辑应放在 ``custom_functions`` 保护区或 ``src/user``。

回调签名、``switchView`` 的生成方式和代码保护细节见 :doc:`代码生成机制 <code_generation>`。

使用示例
---------

示例 1：按钮点击切换页面
~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: xml

   <hg_button id="btn_next" x="20" y="200" width="120" height="48" text="Next">
       <events>
           <event type="onClick">
               <action type="switchView" target="view_page2" />
           </event>
       </events>
   </hg_button>

示例 2：页面左滑切换
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: xml

   <hg_view id="view_page1" x="0" y="0" width="480" height="272" entry="true">
       <events>
           <event type="onSwipeLeft">
               <action type="switchView" target="view_page2"
                       switchOutStyle="SWITCH_OUT_TO_LEFT_USE_TRANSLATION"
                       switchInStyle="SWITCH_IN_FROM_RIGHT_USE_TRANSLATION" />
           </event>
       </events>
   </hg_view>

示例 3：按键事件
~~~~~~~~~~~~~~~~~

.. code-block:: xml

   <hg_view id="view_main" x="0" y="0" width="480" height="272" entry="true">
       <events>
           <event type="onKeyShortPress" keyName="Back">
               <action type="callFunction" functionName="on_back_key" />
           </event>
       </events>
   </hg_view>

最佳实践
---------

* 使用清晰的函数名：``on_save_click``、``on_back_key``。
* 不要在频繁触发的事件中执行耗时操作。
* 跨页面通信优先使用消息系统。
* 使用 ``switchView`` 时确保目标页面存在，避免悬空跳转。
* 复杂逻辑放在 ``src/user`` 或 ``custom_functions`` 中，保持 HML 可读。

下一步
-------

* 学习 :doc:`code_generation` 了解代码生成机制
* 参考 :doc:`build_simulation` 构建和测试应用
* 阅读 :doc:`deployment` 了解嵌入式部署方案
