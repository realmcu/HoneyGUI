HML 文件格式
=============

HML (HoneyGUI Markup Language) 是一种基于 XML 的界面描述语言，用于定义 HoneyGUI 界面的结构和属性。

概述
-----

HML 文件是可视化设计器的底层存储格式，具有以下特点：

* **人类可读**：基于 XML 格式，便于阅读和编辑
* **版本控制友好**：纯文本格式，适合 Git 等版本控制工具
* **完整描述**：包含组件结构、属性、事件等所有信息
* **平台无关**：可以转换为不同目标平台的代码 (C、Python 等)

基本结构
---------

HML 文件的基本结构如下：

.. code-block:: xml

   <?xml version="1.0" encoding="UTF-8"?>
   <hml>
       <meta>
           <project name="MyApp" resolution="480X272" />
       </meta>
       <view>
           <!-- 界面组件定义 -->
       </view>
   </hml>

根元素
~~~~~~

* **hml**：文档根元素
* **meta**：元数据，包含项目名称、分辨率等配置
* **view**：界面内容容器，包含所有组件定义

组件定义
---------

基本语法
~~~~~~~~

每个组件使用一个 XML 元素表示：

.. code-block:: xml

   <hg_button id="btn1" x="10" y="20" w="100" h="40" text="Click Me"/>

组件可以嵌套：

.. code-block:: xml

   <hg_view id="container" x="0" y="0" w="480" h="272">
       <hg_label id="title" x="10" y="10" w="200" h="30" text="Hello"/>
       <hg_button id="btn1" x="10" y="50" w="100" h="40" text="OK"/>
   </hg_view>

必需属性
~~~~~~~~

所有组件都必须包含以下属性：

* **id**：组件唯一标识符

  * 规则：字母、数字、下划线，不能以数字开头
  * 示例：``main_view``, ``login_button``, ``user_avatar``

位置和尺寸属性
~~~~~~~~~~~~~~~

* **x**：水平位置（像素）
* **y**：垂直位置（像素）
* **width**：宽度（像素）
* **height**：高度（像素）

.. note::
   位置是相对于父组件的左上角。

组件类型
---------

容器组件
~~~~~~~~

hg_view
"""""""

视图容器，用于组织布局：

.. code-block:: xml

   <hg_view id="main_view" x="0" y="0" width="480" height="272" entry="true">
       <!-- 子组件 -->
   </hg_view>

属性：

* ``entry``：是否为入口视图（每个项目至少一个）
* ``backgroundColor``：背景色（仅设计器显示用）

hg_window
"""""""""

窗口容器，支持多窗口管理：

.. code-block:: xml

   <hg_window id="popup" x="100" y="100" width="280" height="160">
       <!-- 窗口内容 -->
   </hg_window>

hg_canvas
"""""""""

自定义绘图画布：

.. code-block:: xml

   <hg_canvas
       id="drawing"
       x="0" y="0" width="480" height="272"/>

hg_list
"""""""

列表容器：

.. code-block:: xml

   <hg_list
       id="menu"
       x="0" y="50" width="480" height="200"
       itemWidth="480" itemHeight="50"
       direction="VERTICAL"
       noteNum="5"
       inertia="true"
       loop="false"/>

hg_menu_cellular
""""""""""""""""

蜂窝菜单容器：

.. code-block:: xml

   <hg_menu_cellular
       id="main_menu"
       x="0" y="0" width="480" height="502"
       iconFolder="assets/feature"
       iconSize="64"/>

基础控件
~~~~~~~~

hg_button
"""""""""

按钮组件：

.. code-block:: xml

   <hg_button
       id="submit_btn"
       x="10" y="10" width="100" height="40"
       text="Submit"
       color="#FFFFFF"
       imageOn="assets/button_on.png"
       imageOff="assets/button_off.png"/>

hg_label
""""""""

文本标签：

.. code-block:: xml

   <hg_label
       id="title"
       x="0" y="0" width="480" height="50"
       text="Welcome to HoneyGUI"
       fontSize="24"
       color="#333333"
       fontFile="assets/font/Inter24pt-Medium.ttf"
       hAlign="CENTER"
       vAlign="MID"/>

hg_image
""""""""

图片显示：

.. code-block:: xml

   <hg_image
       id="logo"
       x="50" y="50" width="100" height="100"
       src="assets/logo.png"/>

输入控件
~~~~~~~~

hg_input
""""""""

文本输入框：

.. code-block:: xml

   <hg_input
       id="username"
       x="20" y="100" width="200" height="40"
       placeholder="Enter username"/>

hg_checkbox 和 hg_radio
""""""""""""""""""""""""

复选框和单选按钮目前在 SDK 中尚未实现 (TODO)，暂不支持使用。

多媒体组件
~~~~~~~~~~

hg_video
""""""""

视频播放器：

.. code-block:: xml

   <hg_video
       id="player"
       x="0" y="0" width="480" height="272"
       src="assets/video.mp4"
       autoplay="false"
       loop="false"/>

hg_gif
""""""

GIF 动画：

.. code-block:: xml

   <hg_gif
       id="anim"
       x="50" y="50" width="150" height="150"
       src="assets/anim.gif"/>

hg_3d
"""""

3D 模型显示：

.. code-block:: xml

   <hg_3d
       id="model"
       x="50" y="50" width="380" height="380"
       modelPath="assets/models/cube.bin"/>

事件配置
---------

事件使用子元素定义，遵循 Event-Action 模式。

基本语法
~~~~~~~~

.. code-block:: xml

   <hg_button id="btn1" x="10" y="10" width="100" height="40" text="Next">
       <events>
           <event type="onClick">
               <action type="switchView" target="page2"
                       switchOutStyle="SWITCH_OUT_TO_LEFT_USE_TRANSLATION"
                       switchInStyle="SWITCH_IN_FROM_RIGHT_USE_TRANSLATION"/>
           </event>
       </events>
   </hg_button>

事件类型
~~~~~~~~

* **onClick**：点击事件
* **onLongPress**：长按事件
* **onTouchDown**：按下事件
* **onTouchUp**：抬起事件
* **onSwipeLeft / onSwipeRight / onSwipeUp / onSwipeDown**：滑动事件（容器组件）
* **onMessage**：消息监听事件

动作类型
~~~~~~~~

switchView
""""""""""

切换到另一个视图：

.. code-block:: xml

   <action type="switchView" target="main_menu"
           switchOutStyle="SWITCH_OUT_TO_LEFT_USE_TRANSLATION"
           switchInStyle="SWITCH_IN_FROM_RIGHT_USE_TRANSLATION"/>

switchOutStyle / switchInStyle 选项：

* 平移： ``SWITCH_OUT_TO_LEFT/RIGHT/TOP/BOTTOM_USE_TRANSLATION``
* 立方体： ``SWITCH_OUT_TO_LEFT/RIGHT/TOP/BOTTOM_USE_CUBE``
* 其他： ``SWITCH_OUT_NONE_ANIMATION``、 ``SWITCH_OUT_ANIMATION_FADE``

sendMessage
"""""""""""

发送自定义消息：

.. code-block:: xml

   <action type="sendMessage" message="user_logout"/>

callFunction
""""""""""""

调用用户定义的 C 函数：

.. code-block:: xml

   <action type="callFunction" functionName="on_button_click"/>

.. note::
   用户需要在生成的回调文件中实现此函数。

多个事件
~~~~~~~~

一个组件可以响应多个事件：

.. code-block:: xml

   <hg_button id="btn1" x="10" y="10" w="100" h="40" text="Action">
       <events>
           <event type="onClick">
               <action type="callFunction" function="handle_click"/>
           </event>
           <event type="onLongPress">
               <action type="callFunction" function="handle_long_press"/>
           </event>
       </events>
   </hg_button>

完整示例
---------

简单登录界面
~~~~~~~~~~~~

.. code-block:: xml

   <?xml version="1.0" encoding="UTF-8"?>
   <hml>
       <meta>
           <project name="LoginApp" resolution="480X272" />
       </meta>
       <view>
           <hg_view id="login_view" x="0" y="0" width="480" height="272" entry="true">
               <!-- 标题 -->
               <hg_label
                   id="title"
                   x="0" y="20" width="480" height="40"
                   text="User Login"
                   fontSize="24"
                   color="#333333"
                   hAlign="CENTER"
                   vAlign="MID"/>

               <!-- 用户名输入 -->
               <hg_label
                   id="username_label"
                   x="40" y="80" width="100" height="30"
                   text="Username:"
                   fontSize="16"/>
               <hg_input
                   id="username_input"
                   x="150" y="80" width="280" height="40"
                   placeholder="Enter username"/>

               <!-- 密码输入 -->
               <hg_label
                   id="password_label"
                   x="40" y="130" width="100" height="30"
                   text="Password:"
                   fontSize="16"/>
               <hg_input
                   id="password_input"
                   x="150" y="130" width="280" height="40"
                   placeholder="Enter password"/>

               <!-- 登录按钮 -->
               <hg_button
                   id="login_btn"
                   x="150" y="220" width="180" height="40"
                   text="Login">
                   <events>
                       <event type="onClick">
                           <action type="callFunction" functionName="handle_login"/>
                       </event>
                   </events>
               </hg_button>
           </hg_view>
       </view>
   </hml>

多视图导航
~~~~~~~~~~

.. code-block:: xml

   <?xml version="1.0" encoding="UTF-8"?>
   <hml>
       <meta>
           <project name="NavApp" resolution="480X272" />
       </meta>
       <view>
           <hg_view id="home_view" x="0" y="0" width="480" height="272" entry="true">
               <hg_label
                   id="home_title"
                   x="0" y="20" width="480" height="40"
                   text="Home Page"
                   fontSize="24"
                   hAlign="CENTER"
                   vAlign="MID"/>

               <hg_button
                   id="to_settings"
                   x="190" y="100" width="100" height="40"
                   text="Settings">
                   <events>
                       <event type="onClick">
                           <action type="switchView" target="settings_view"
                                   switchOutStyle="SWITCH_OUT_TO_LEFT_USE_TRANSLATION"
                                   switchInStyle="SWITCH_IN_FROM_RIGHT_USE_TRANSLATION"/>
                       </event>
                   </events>
               </hg_button>

               <hg_button
                   id="to_about"
                   x="190" y="160" width="100" height="40"
                   text="About">
                   <events>
                       <event type="onClick">
                           <action type="switchView" target="about_view"
                                   switchOutStyle="SWITCH_OUT_ANIMATION_FADE"
                                   switchInStyle="SWITCH_IN_ANIMATION_FADE"/>
                       </event>
                   </events>
               </hg_button>
           </hg_view>
       </view>
   </hml>

手动编辑 HML
--------------

虽然推荐使用可视化设计器，但您也可以直接编辑 HML 文件：

1. 在 VSCode 中右键点击 ``.hml`` 文件
2. 选择 :guilabel:`用文本编辑器打开`
3. 直接修改 XML 内容
4. 保存后，可以重新用设计器打开查看效果

.. warning::
   手动编辑时请确保 XML 格式正确，否则可能导致设计器无法打开。

验证 HML
~~~~~~~~~

设计器会在打开时自动验证 HML 格式：

* 检查 XML 语法
* 验证必需属性
* 检查组件 ID 唯一性
* 验证嵌套关系

如果验证失败，会显示详细错误信息。

最佳实践
---------

命名规范
~~~~~~~~

* 使用小写字母和下划线： ``main_view``, ``login_button``
* 避免使用数字开头： ``1view`` 改为 ``view_1``
* 使用有意义的名称： ``submit_btn`` 而不是 ``btn1``

组织结构
~~~~~~~~

* 使用容器组织相关组件
* 保持适当的嵌套层级（不超过 5 层）
* 相似组件使用一致的命名模式

注释
~~~~

HML 支持 XML 注释：

.. code-block:: xml

   <!-- 这是一个注释 -->
   <hg_view id="main">
       <!-- 头部区域 -->
       <hg_view id="header">
           <!-- ... -->
       </hg_view>

       <!-- 内容区域 -->
       <hg_view id="content">
           <!-- ... -->
       </hg_view>
   </hg_view>

版本控制
~~~~~~~~

* HML 是纯文本格式，适合 Git 管理
* 定期提交更改
* 写清楚的提交信息
* 使用分支进行实验性设计

下一步
-------

* 学习 :doc:`代码生成机制 <code_generation>` 了解如何转换为 C 代码
* 掌握 :doc:`事件系统 <event_system>` 实现复杂交互
* 探索 :doc:`资源管理 <resource_management>` 管理图片等资源
