C-APP应用程序
=================
- 在本章中，我们将探讨GUI框架内C-APP的创建与管理。C-APP实质上是用户可以开发以制作互动和视觉吸引力用户界面的应用程序。每个C-APP都可以打开、关闭、切换，并在切换时应用动态过渡效果。

- C-APP中显示的内容是使用嵌套控件树结构组织的。该结构包括诸如窗口、可滚动页面和可切屏的容器控件，以及诸如文本、图像和画布之类的内容显示控件。

- 除了默认功能和效果外，C-APP内的控件提供了高度的自定义性。用户可以为控件设置自定义帧动画，并绑定事件以执行他们定义的操作。这种灵活性使得能够根据特定需求和要求创建高度动态和互动的用户界面。

定义一个 C-APP
--------------

- 使用特定名称通过 ``GUI_APP_DEFINE_NAME_ANIMATION`` API定义应用程序句柄。

.. literalinclude:: ../../../../realgui/example/screen_454_454/gui_menu/apps_in_menu.c
   :language: c
   :start-after: /*Define a app with name APP_STOPWATCH*/
   :end-before: /*Define a app with name APP_STOPWATCH end*/

- 还有其他方式可以定义应用程序，例如：

   - ``GUI_APP_DEFINE`` 
 
   - ``GUI_APP_DEFINE_NAME`` 

   - ``GUI_APP_DEFINE_NAME_ANIMATION_FUNC_CUSTOM``

   - ``struct gui_app``

- 使用 ``GUI_APP_ENTRY`` API定义应用程序的UI设计入口函数。

- 当应用程序启动时，UI设计入口函数将被执行一次。

.. literalinclude:: ../../../../realgui/example/screen_454_454/gui_menu/apps_in_menu.c
   :language: c
   :start-after: /*define the app's ui design*/
   :end-before: /*define the app's ui design end*/
   


创建一个C-APP的控件树
-----------------------
- 这是一个时钟应用程序，作为本节的示例。

- 在下图中，您可以看到应用程序界面有秒表和倒计时器选项。

- 点击这些选项可以在它们之间切换。

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1727059552792713966/15519dc3_10088396.gif" width= "400" /></div>
   <br>

下图显示了精简过的控件树结构：

- SCREEN:APP_STOPWATCH: 秒表应用程序的主容器。

   - WINDOW:LEFT_BUTTON: 包含左按钮的窗口。

      - CANVAS_RECT:LEFT_BUTTON: 左按钮的背景画布。

      - TEXTBOX:LEFT_BUTTON: 左按钮的文本标签。

   - WINDOW:RIGHT_BUTTON: 包含右按钮的窗口。

      - CANVAS_RECT:RIGHT_BUTTON: 右按钮的背景画布。

      - TEXTBOX:RIGHT_BUTTON: 右按钮的文本标签。

   - MULTI_LEVEL:0_0: 多级容器。

      - MULTI_LEVEL:1_0: 多级容器中的一个子容器，供秒表使用。

      - MULTI_LEVEL:1_1: 多级容器中的另一个子容器，供倒计时器使用。

.. image:: https://foruda.gitee.com/images/1727332782414751516/5754a436_10088396.png
   :align: center

C-APP操作
---------

以下是上述操作如何具体应用于秒表应用程序：

- ``GUI_APP_SHUTDOWN(APP_STOPWATCH)`` ：此命令将关闭秒表应用程序。如果应用程序正在运行计时器，它将停止计时器并关闭界面。关闭时任何相关资源将被释放。

- ``GUI_APP_STARTUP(APP_STOPWATCH)`` ：此命令将初始化并启动秒表应用程序。用户界面将会显示，应用程序将准备好开始记录时间。

- ``GUI_APP_SWAP(APP_STOPWATCH, APP_MAP)`` ：这将在当前运行的秒表应用程序和地图应用程序之间切换。

C-APP 转场动画
----------------------------------


C-APP 提供了一个强大的功能集来管理应用程序之间的转场动画。它主要提供三个功能：“内置动画”、“自定义动画”和“图层管理”。这些功能旨在通过提供流畅且视觉上令人愉悦的过渡来增强用户体验。

- 内置动画

   开发者可以使用 ``GUI_APP_DEFINE_NAME_ANIMATION`` API，轻松实现应用程序过渡的内置动画。此 API 允许您指定应用程序打开或关闭时发生的过渡动画。第二个参数用于定义应用程序启动时的动画，而第三个参数用于指定关闭应用程序的动画，如 ``GUI_APP_ANIMATION_1`` 。这个简单的 API 简化了在应用程序中集成过渡效果的过程。
   
- 自定义动画

   对于更复杂或独特的动画需求，C-APP 支持通过 ``GUI_APP_DEFINE_NAME_ANIMATION_FUNC_CUSTOM`` API 来实现自定义动画。此功能允许开发者为应用打开和关闭过渡设置自定义动画回调函数。第二个参数是打开动画的回调函数，第三个参数是关闭动画的回调函数。这些回调函数通过 ``GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE`` API 定义。这个 API 提供一个 ``gui_animate_t`` 实例作为参数，其中包含成员可以提供有关动画进度和状态的信息，从而允许进行精细的控制和定制。

- 图层管理

   C-APP 还包括用于管理应用程序图层的 API，这对于视觉层次结构和用户体验来说至关重要。 ``gui_app_layer_top`` 和 ``gui_app_layer_bottom`` API 允许开发者定义当前活动应用程序与即将打开的应用程序之间的图层关系。此功能确保窗口的正确顺序，并有助于维护应用程序界面的预期焦点和组织。

示例
^^^^^

- 内置动画

   * 定义一个 C-APP

      * 启动动画：从屏幕中心放大 (``GUI_APP_ANIMATION_1``)

      * 关闭动画：缩小到屏幕中心 (``GUI_APP_ANIMATION_5``)

   * 切换到该 C-APP

      * 从应用程序 APP_WATCH 到 APP_STOPWATCH


.. literalinclude:: ../../../../realgui/example/screen_454_454/gui_menu/apps_in_menu.c
   :language: c
   :start-after: /*Define a app with name APP_STOPWATCH*/
   :end-before: /*Define a app with name APP_STOPWATCH end*/


.. literalinclude:: ../../../../realgui/example/screen_454_454/gui_menu/menu_in_tab.c
   :language: c
   :start-after: /*swap to APP_STOPWATCH*/
   :end-before: /*swap to APP_STOPWATCH end*/


.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1736837380134649772/48ffca47_10088396.gif" width= "400" /></div>
   <br>



- 自定义动画

   * 定义一个 C-APP

      * 启动动画：从屏幕底部弹出 (``heart_rate_startup``)

      * 关闭动画：滑动消失 (``heart_rate_shutdown``)

   * 切换到该 C-APP

      * 从应用程序 APP_WATCH 到 APP_HEART_RATE




.. literalinclude:: ../../../../realgui/example/screen_454_454/gui_menu/apps_in_menu.c
   :language: c
   :start-after: /*Define a app with name APP_HEART_RATE*/
   :end-before: /*Define a app with name APP_HEART_RATE end*/
   

.. literalinclude:: ../../../../realgui/example/screen_454_454/gui_menu/menu_in_tab.c
   :language: c
   :start-after: /*swap to APP_HEART_RATE*/
   :end-before: /*swap to APP_HEART_RATE end*/

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1736905537073287973/7ded32a2_10088396.gif" width= "400" /></div>
   <br>







API
---

.. doxygenfile:: gui_app.h



