HML 文件格式
=============

HML (HoneyGUI Markup Language) 是一种基于 XML 的界面描述语言，用于定义 HoneyGUI 界面的结构和属性。

概述
-----

HML 文件是可视化设计器的底层存储格式，具有以下特点：

* **人类可读**：基于 XML 格式，便于阅读和编辑
* **版本控制友好**：纯文本格式，适合 Git 等版本控制工具
* **完整描述**：包含组件结构、属性、事件等所有信息
* **平台无关**：可以转换为不同目标平台的代码（C、Python 等）

基本结构
---------

HML 文件的基本结构如下：

.. code-block:: xml

   <?xml version="1.0" encoding="UTF-8"?>
   <hone:HoneyGUI xmlns:hone="http://www.honeygui.com">
       <HoneyGUI version="1.0">
           <!-- 界面组件定义 -->
       </HoneyGUI>
   </hone:HoneyGUI>

命名空间
~~~~~~~~

* **xmlns:hone**：HoneyGUI 命名空间声明
* **version**：HML 格式版本号

根元素
~~~~~~

* **hone:HoneyGUI**：文档根元素
* **HoneyGUI**：实际内容容器

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
* **w**：宽度（像素）
* **h**：高度（像素）

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

   <hg_view id="main_view" x="0" y="0" w="480" h="272">
       <!-- 子组件 -->
   </hg_view>

hg_window
"""""""""

窗口容器，支持多窗口管理：

.. code-block:: xml

   <hg_window id="popup" x="100" y="100" w="280" h="160">
       <!-- 窗口内容 -->
   </hg_window>

基础控件
~~~~~~~~

hg_button
"""""""""

按钮组件：

.. code-block:: xml

   <hg_button
       id="submit_btn"
       x="10" y="10" w="100" h="40"
       text="Submit"
       fontSize="16"
       textColor="#FFFFFF"
       backgroundColor="#007AFF"
       normalImage="assets/button_normal.bin"
       pressedImage="assets/button_pressed.bin"/>

hg_label
""""""""

文本标签：

.. code-block:: xml

   <hg_label
       id="title"
       x="0" y="0" w="480" h="50"
       text="Welcome to HoneyGUI"
       fontSize="24"
       fontColor="#333333"
       fontFamily="assets/font.bin"
       textAlign="center"/>

hg_image
""""""""

图片显示：

.. code-block:: xml

   <hg_image
       id="logo"
       x="50" y="50" w="100" h="100"
       src="assets/logo.bin"
       mode="fit"
       rotation="0"/>

hg_switch
"""""""""

开关按钮：

.. code-block:: xml

   <hg_switch
       id="wifi_switch"
       x="300" y="20" w="60" h="30"
       checked="true"
       onColor="#34C759"
       offColor="#8E8E93"/>

输入控件
~~~~~~~~

hg_input
""""""""

文本输入框：

.. code-block:: xml

   <hg_input
       id="username"
       x="20" y="100" w="200" h="40"
       placeholder="Enter username"
       maxLength="20"
       fontSize="14"/>

hg_checkbox
"""""""""""

复选框：

.. code-block:: xml

   <hg_checkbox
       id="agree"
       x="20" y="150" w="24" h="24"
       checked="false"
       label="I agree to the terms"/>

hg_radio
""""""""

单选按钮：

.. code-block:: xml

   <hg_radio
       id="option1"
       x="20" y="180" w="24" h="24"
       groupName="options"
       checked="true"
       label="Option 1"/>

高级控件
~~~~~~~~

hg_progressbar
""""""""""""""

进度条：

.. code-block:: xml

   <hg_progressbar
       id="loading"
       x="50" y="200" w="380" h="10"
       min="0" max="100" value="75"
       color="#007AFF"
       backgroundColor="#E5E5EA"/>

hg_slider
"""""""""

滑动条：

.. code-block:: xml

   <hg_slider
       id="volume"
       x="50" y="220" w="380" h="30"
       min="0" max="100" value="50"
       thumbColor="#007AFF"
       trackColor="#E5E5EA"/>

hg_canvas
"""""""""

自定义绘图画布：

.. code-block:: xml

   <hg_canvas
       id="drawing"
       x="0" y="0" w="480" h="272"
       backgroundColor="#FFFFFF"/>

hg_list
"""""""

列表容器：

.. code-block:: xml

   <hg_list
       id="menu"
       x="0" y="50" w="480" h="200"
       itemHeight="50"
       scrollable="true">
       <!-- 列表项 -->
   </hg_list>

多媒体组件
~~~~~~~~~~

hg_video
""""""""

视频播放器：

.. code-block:: xml

   <hg_video
       id="player"
       x="0" y="0" w="480" h="272"
       src="assets/video.mp4"
       autoplay="false"
       loop="false"/>

hg_3d
"""""

3D 模型显示：

.. code-block:: xml

   <hg_3d
       id="model"
       x="50" y="50" w="380" h="380"
       modelSrc="assets/model.bin"
       rotationX="0" rotationY="0" rotationZ="0"/>

事件配置
---------

事件使用子元素定义，遵循 Event-Action 模式。

基本语法
~~~~~~~~

.. code-block:: xml

   <hg_button id="btn1" x="10" y="10" w="100" h="40" text="Next">
       <events>
           <event type="onClick">
               <action type="switchView" target="page2" animation="slideLeft"/>
           </event>
       </events>
   </hg_button>

事件类型
~~~~~~~~

* **onClick**：点击事件
* **onLongPress**：长按事件（默认 1000ms）
* **onSwipe**：滑动事件（up、down、left、right）
* **onChange**：值改变事件（用于输入框、滑动条等）
* **onFocus**：获得焦点
* **onBlur**：失去焦点

动作类型
~~~~~~~~

switchView
""""""""""

切换到另一个视图：

.. code-block:: xml

   <action type="switchView" target="main_menu" animation="fade"/>

动画选项：

* ``fade``：淡入淡出
* ``slideLeft``：从右向左滑动
* ``slideRight``：从左向右滑动
* ``slideUp``：从下向上滑动
* ``slideDown``：从上向下滑动
* ``none``：无动画

sendMessage
"""""""""""

发送自定义消息：

.. code-block:: xml

   <action type="sendMessage" message="user_logout"/>

callFunction
""""""""""""

调用用户定义的 C 函数：

.. code-block:: xml

   <action type="callFunction" function="on_button_click"/>

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
   <hone:HoneyGUI xmlns:hone="http://www.honeygui.com">
       <HoneyGUI version="1.0">
           <hg_view id="login_view" x="0" y="0" w="480" h="272">
               <!-- 标题 -->
               <hg_label
                   id="title"
                   x="0" y="20" w="480" h="40"
                   text="User Login"
                   fontSize="24"
                   fontColor="#333333"
                   textAlign="center"/>

               <!-- 用户名输入 -->
               <hg_label
                   id="username_label"
                   x="40" y="80" w="100" h="30"
                   text="Username:"
                   fontSize="16"/>
               <hg_input
                   id="username_input"
                   x="150" y="80" w="280" h="40"
                   placeholder="Enter username"
                   maxLength="20"/>

               <!-- 密码输入 -->
               <hg_label
                   id="password_label"
                   x="40" y="130" w="100" h="30"
                   text="Password:"
                   fontSize="16"/>
               <hg_input
                   id="password_input"
                   x="150" y="130" w="280" h="40"
                   placeholder="Enter password"
                   type="password"
                   maxLength="20"/>

               <!-- 记住密码 -->
               <hg_checkbox
                   id="remember"
                   x="150" y="180" w="24" h="24"
                   label="Remember me"/>

               <!-- 登录按钮 -->
               <hg_button
                   id="login_btn"
                   x="150" y="220" w="180" h="40"
                   text="Login"
                   backgroundColor="#007AFF"
                   textColor="#FFFFFF">
                   <events>
                       <event type="onClick">
                           <action type="callFunction" function="handle_login"/>
                       </event>
                   </events>
               </hg_button>
           </hg_view>
       </HoneyGUI>
   </hone:HoneyGUI>

多视图导航
~~~~~~~~~~

.. code-block:: xml

   <?xml version="1.0" encoding="UTF-8"?>
   <hone:HoneyGUI xmlns:hone="http://www.honeygui.com">
       <HoneyGUI version="1.0">
           <hg_view id="home_view" x="0" y="0" w="480" h="272">
               <hg_label
                   id="home_title"
                   x="0" y="20" w="480" h="40"
                   text="Home Page"
                   fontSize="24"
                   textAlign="center"/>

               <hg_button
                   id="to_settings"
                   x="190" y="100" w="100" h="40"
                   text="Settings">
                   <events>
                       <event type="onClick">
                           <action type="switchView" target="settings_view" animation="slideLeft"/>
                       </event>
                   </events>
               </hg_button>

               <hg_button
                   id="to_about"
                   x="190" y="160" w="100" h="40"
                   text="About">
                   <events>
                       <event type="onClick">
                           <action type="switchView" target="about_view" animation="fade"/>
                       </event>
                   </events>
               </hg_button>
           </hg_view>
       </HoneyGUI>
   </hone:HoneyGUI>

手动编辑 HML
--------------

虽然推荐使用可视化设计器，但您也可以直接编辑 HML 文件：

1. 在 VSCode 中右键点击 ``.hml`` 文件
2. 选择 **用文本编辑器打开**
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

* 使用小写字母和下划线：``main_view``, ``login_button``
* 避免使用数字开头：``view1`` 改为 ``view_1``
* 使用有意义的名称：``submit_btn`` 而不是 ``btn1``

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
