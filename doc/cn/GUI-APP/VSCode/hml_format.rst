HML 文件格式
=============

HML（HoneyGUI Markup Language）是基于 XML 的界面描述语言，用于定义 HoneyGUI 或 LVGL 项目的界面结构、属性、事件和动画。

本文档主要用于以下场景：

* 需要手写或批量修改 HML 文件。
* 需要检查设计器生成的文件，或排查解析、校验问题。
* 需要确认组件标签、属性名、嵌套规则、资源路径或引擎支持。
* 需要编写脚本处理 HML，或审查 HML 的版本差异。

日常拖拽设计、事件配置和资源转换请分别参考 :doc:`设计器 <designer>`、:doc:`事件系统 <event_system>` 和 :doc:`资源管理 <resource_management>`。

概述
-----

HML 是设计器的底层存储格式，具有以下特点：

* 纯文本，适合 Git 等版本控制工具。
* 组件、事件、定时器和资源路径都以声明方式描述。
* 代码生成器根据 ``project.json`` 的 ``targetEngine`` 选择 HoneyGUI 或 LVGL 后端。

基本结构
---------

HML 文件的基本结构如下：

.. code-block:: xml

   <?xml version="1.0" encoding="UTF-8"?>
   <hml>
       <meta>
           <project name="MyApp" appId="com.example.myapp"
                    resolution="480x272" minSdk="1.0" pixelMode="RGB565" />
           <author name="Developer" email="dev@example.com" />
       </meta>
       <view>
           <!-- 页面和组件 -->
       </view>
   </hml>

根元素规则：

* ``<hml>`` 是必需根元素。
* ``<meta>`` 和 ``<view>`` 必须各出现一次，顺序为先 ``<meta>`` 后 ``<view>``。
* 文件编码为 UTF-8。
* 组件标签以 ``hg_`` 开头；自定义组件可使用 ``custom_`` 前缀。

元数据
~~~~~~~

``<project>`` 常用属性：

.. list-table::
   :header-rows: 1
   :widths: 25 20 55

   * - 属性
     - 类型
     - 说明
   * - ``name``
     - string
     - 项目名称
   * - ``appId``
     - string
     - 应用标识符
   * - ``resolution``
     - string
     - 分辨率，格式为 ``WxH``，如 ``480x272``
   * - ``minSdk``
     - string
     - 最低 SDK 版本
   * - ``pixelMode``
     - string
     - ``RGB565``、``RGB888``、``ARGB8888``

项目级实际配置以 ``project.json`` 为准，HML 中的 ``<meta>`` 主要用于文件自身描述。

视图与页面
~~~~~~~~~~~

``<view>`` 中包含一个或多个 ``hg_view`` 页面：

.. code-block:: xml

   <view>
       <hg_view id="view_home" x="0" y="0" width="480" height="272" entry="true">
           <hg_label id="lbl_title" x="20" y="20" width="200" height="40"
                     text="Home" fontFile="/NotoSansSC-Medium.ttf" fontSize="24" />
       </hg_view>
       <hg_view id="view_settings" x="0" y="0" width="480" height="272">
           <!-- 第二个页面 -->
       </hg_view>
   </view>

页面规则：

* 一个 HML 文件必须有且只有一个 ``hg_view`` 设置 ``entry="true"``。
* ``hg_view`` 不能嵌套在另一个 ``hg_view`` 中。
* ``hg_view`` 的 ``x`` / ``y`` 主要用于设计器画布排布，在设备上页面切换不依赖这两个坐标。
* 页面切换使用事件动作 ``switchView``。

通用属性
---------

所有组件支持以下基础属性：

.. list-table::
   :header-rows: 1
   :widths: 22 18 60

   * - 属性
     - 默认值
     - 说明
   * - ``id``
     - 自动生成
     - 组件标识符，建议全局唯一
   * - ``name``
     - 与 ``id`` 相同
     - 显示名称，``switchView`` 可按 name 查找页面
   * - ``x`` / ``y``
     - 0
     - 相对父容器的位置
   * - ``width`` / ``height``
     - 100 / 40
     - 宽高，单位像素。不要使用 ``w`` / ``h``
   * - ``visible``
     - true
     - 是否可见
   * - ``enabled``
     - true
     - 是否启用交互
   * - ``locked``
     - false
     - 设计器中是否锁定
   * - ``zIndex``
     - 0
     - 堆叠顺序，值越大越靠上
   * - ``showOverflow``
     - false
     - 设计器中是否显示溢出内容

约束与校验
-----------

组件嵌套规则
~~~~~~~~~~~~~

1. 只有容器组件可以包含子组件：``hg_view``、``hg_window``、``hg_list``、``hg_list_item``、``hg_menu_cellular``。
2. 非容器控件必须作为某个容器的子元素。
3. 非容器控件不能包含子组件。
4. ``hg_list`` 的子元素应为 ``hg_list_item``。
5. ``hg_list_item`` 可以包含任意非容器控件。
6. ``hg_view`` 不能包含 ``hg_view``。

校验规则
~~~~~~~~~

设计器打开或导入 HML 时会执行以下检查，未通过的项会产生错误或警告：

* XML 语法。
* ``<hml>``、``<meta>``、``<view>`` 结构。
* 组件 ID 唯一性和合法 C 标识符。
* 容器/非容器嵌套关系。
* ``hg_view`` 是否嵌套。
* ``entry="true"`` 是否唯一。
* 资源路径前缀。
* ``switchView`` 目标是否存在。
* 多语言 key 是否存在于 ``i18n/strings.json``。

组件 HML 示例
-------------

容器组件
~~~~~~~~~

hg_view
^^^^^^^^

.. code-block:: xml

   <hg_view id="view_main" x="0" y="0" width="480" height="272" entry="true">
       <!-- 子组件 -->
   </hg_view>

关键属性：``entry``、``backgroundColor``、``borderRadius``、``padding``、``overflow``、``residentMemory``、``animateStep``、``opacity``。

hg_window
^^^^^^^^^^

.. code-block:: xml

   <hg_window id="win_popup" x="100" y="80" width="280" height="160"
              showBackground="true" backgroundColor="#808080">
       <!-- 子组件 -->
   </hg_window>

关键属性：``showBackground``、``backgroundColor``、``enableBlur``、``blurDegree``。

hg_list / hg_list_item
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: xml

   <hg_list id="menu" x="0" y="50" width="480" height="200"
            itemWidth="480" itemHeight="50" direction="VERTICAL"
            noteNum="5" inertia="true" loop="false">
       <hg_list_item id="item_0" x="0" y="0" width="480" height="50">
           <hg_label id="lbl_0" x="20" y="10" width="200" height="30"
                     text="Item 0" fontSize="20" />
       </hg_list_item>
   </hg_list>

``hg_list`` 关键属性：``itemWidth``、``itemHeight``、``space``、``direction``、``style``、``noteNum``、``autoAlign``、``inertia``、``loop``、``createBar``、``useUserNoteDesign``。``hg_list_item`` 由 ``hg_list`` 管理，可包含非容器控件。

hg_menu_cellular
^^^^^^^^^^^^^^^^

.. code-block:: xml

   <hg_menu_cellular id="main_menu" x="0" y="0" width="480" height="502"
                     iconFolder="assets/feature" iconSize="64" />

关键属性：``iconFolder``、``iconSize``、``offsetX``、``offsetY``；仅 HoneyGUI。

基础控件
~~~~~~~~~

hg_button
^^^^^^^^^

.. code-block:: xml

   <hg_button id="btn_submit" x="20" y="200" width="120" height="48"
              text="Submit" imageOff="assets/btn_normal.png"
              imageOn="assets/btn_pressed.png" />

关键属性：``text``、``imageOn``、``imageOff``、``toggleMode``、``initialState``、``clickCallback``、``onCallback``、``offCallback``、``controlTarget``。

hg_label
^^^^^^^^

.. code-block:: xml

   <hg_label id="lbl_title" x="20" y="20" width="440" height="48"
             text="Welcome" i18nKey="home.title"
             fontFile="/NotoSansSC-Medium.ttf" fontSize="24"
             color="#FFFFFF" hAlign="CENTER" vAlign="MID" />

关键属性：``text``、``i18nKey``、``fontFile``、``fontSize``、``fontType``、``renderMode``、``color``、``hAlign``、``vAlign``、``letterSpacing``、``lineSpacing``、``wordWrap``、``wordBreak``、``enableScroll``。

hg_time_label / hg_timer_label
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: xml

   <hg_time_label id="clock" x="20" y="20" width="120" height="24"
                  timeFormat="HH:mm:ss" />
   <hg_timer_label id="timer" x="20" y="60" width="120" height="24"
                   timerType="stopwatch" timerFormat="HH:MM:SS"
                   timerAutoStart="true" />

关键属性：``timeFormat``、``timerType``、``timerFormat``、``timerInitialValue``、``timerAutoStart``。

hg_image
^^^^^^^^

.. code-block:: xml

   <hg_image id="img_logo" x="40" y="40" width="120" height="120"
             src="assets/logo.png" />

关键属性：``src``、``visible``。

输入控件（仅 LVGL）
~~~~~~~~~~~~~~~~~~~~

以下控件在 HoneyGUI 项目中不可用：

.. code-block:: xml

   <hg_input id="username" x="20" y="100" width="200" height="40"
             placeholder="Enter username" />
   <hg_checkbox id="remember" x="20" y="160" width="160" height="40"
                text="Remember me" value="false" />
   <hg_radio id="mode_a" x="20" y="210" width="160" height="40"
             text="Mode A" value="true" />
   <hg_switch id="enabled" x="20" y="260" width="50" height="28" value="false" />
   <hg_slider id="volume" x="20" y="300" width="240" height="40"
              min="0" max="100" value="50" />
   <hg_progressbar id="progress" x="20" y="350" width="200" height="20"
                   min="0" max="100" value="30" />

常用属性：``text``、``value``、``min``、``max``、``placeholder``、``orientation``。

图形控件
~~~~~~~~~

hg_progress
^^^^^^^^^^^

.. code-block:: xml

   <hg_progress id="progress" x="20" y="80" width="240" height="24"
                progressMode="linear" min="0" max="100" value="60" />

关键属性：``progressMode``、``min``、``max``、``value``、``activeColor``、``inactiveColor``、``segmentCount``、``segmentGap``、``arcStartAngle``、``arcSweepAngle``。

hg_arc / hg_circle / hg_rect
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: xml

   <hg_arc id="arc" x="40" y="40" width="96" height="96"
           radius="40" startAngle="0" endAngle="270" strokeWidth="8" />
   <hg_circle id="circle" x="160" y="40" width="80" height="80" fillColor="#007ACC" />
   <hg_rect id="rect" x="260" y="40" width="120" height="80" borderRadius="8" fillColor="#007ACC" />

关键属性：``radius``、``startAngle``、``endAngle``、``strokeWidth``、``color`` / ``fillColor``、``strokeColor``、``opacity``、``useGradient``、``gradientType``、``gradientDirection``。

``hg_circle`` 和 ``hg_rect`` 还支持发光属性（``glowEnabled``、``glowColor``、``glowOpacity``、``glowBlur``）、阴影属性（``shadowEnabled``、``shadowColor``、``shadowOpacity``、``shadowBlur``、``shadowSpread``、``shadowDirection``、``shadowDistance``）和按键效果属性（``buttonMode`` 等）。

hg_svg / hg_glass / hg_particle / hg_qbcode
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: xml

   <hg_svg id="svg_icon" x="20" y="20" width="64" height="64" src="assets/icon.svg" />
   <hg_glass id="glass" x="100" y="20" width="150" height="150"
             src="assets/glass.glass" distortion="10" region="50" />
   <hg_particle id="snow" x="260" y="20" width="200" height="200" particleEffect="snow" />
   <hg_qbcode id="qr" x="20" y="200" width="160" height="160"
              codeType="qrcode" codeContent="Hello" />

关键属性：``src``、``distortion``、``region``、``particleEffect``、``codeType``、``displayMode``、``encodeMode``、``codeContent``、``borderSize``。

多媒体控件
~~~~~~~~~~~

.. code-block:: xml

   <hg_video id="video_player" x="0" y="0" width="480" height="272"
             src="assets/demo.mp4" autoPlay="true" loop="false" />
   <hg_streaming id="camera" x="0" y="0" width="320" height="240"
                 codec="jpeg" transporter="camera_buffer" updateInterval="40" />
   <hg_gif id="gif_anim" x="40" y="40" width="150" height="150" src="assets/anim.gif" />
   <hg_3d id="model_cube" x="40" y="40" width="380" height="380"
          modelPath="assets/models/cube.bin" scale="5" />
   <hg_lottie id="lottie" x="200" y="40" width="150" height="150"
              src="assets/anim.json" autoplay="true" loop="true" />

关键属性：``src``、``autoPlay``、``loop``、``codec``、``transporter``、``updateInterval``、``dropMode``、``modelPath``、``drawType``、``scale``、``autoplay``。``hg_video``、``hg_streaming`` 和 ``hg_3d`` 仅 HoneyGUI 可用。

事件与定时器
-------------

组件通过 ``<events>`` 子元素声明事件，通过 ``timers`` 属性声明定时器和动画：

.. code-block:: xml

   <hg_button id="btn_next" x="20" y="200" width="120" height="48" text="Next">
       <events>
           <event type="onClick">
               <action type="switchView" target="view_page2" />
           </event>
       </events>
   </hg_button>

事件类型、动作参数、``TimerConfig`` 和 ``TimerAction`` 字段详见 :doc:`事件系统 <event_system>`。

完整示例
---------

.. code-block:: xml

   <?xml version="1.0" encoding="UTF-8"?>
   <hml>
       <meta>
           <project name="Demo" appId="com.example.demo"
                    resolution="480x272" pixelMode="RGB565" />
       </meta>
       <view>
           <hg_view id="view_home" x="0" y="0" width="480" height="272"
                    entry="true" backgroundColor="#101418">
               <hg_label id="lbl_title" x="24" y="24" width="432" height="48"
                         text="Hello HoneyGUI" i18nKey="home.title"
                         fontFile="/NotoSansSC-Medium.ttf" fontSize="28"
                         color="#FFFFFF" hAlign="LEFT" vAlign="MID" />
               <hg_image id="img_logo" x="24" y="96" width="96" height="96"
                         src="assets/logo.png" />
               <hg_button id="btn_next" x="24" y="216" width="160" height="40"
                          text="Next" imageOff="assets/btn.png">
                   <events>
                       <event type="onClick">
                           <action type="switchView" target="view_settings" />
                       </event>
                   </events>
               </hg_button>
           </hg_view>
           <hg_view id="view_settings" x="0" y="0" width="480" height="272">
               <hg_label id="lbl_settings" x="24" y="24" width="432" height="48"
                         text="Settings" fontFile="/NotoSansSC-Medium.ttf" fontSize="24" />
           </hg_view>
       </view>
   </hml>

下一步
-------

* 掌握 :doc:`事件系统 <event_system>`，配置事件、动作和定时器。
* 了解 :doc:`代码生成机制 <code_generation>`，查看 HML 如何转换为 C 代码。
* 阅读 :doc:`资源管理 <resource_management>`，配置图片、字体、模型等资源。
