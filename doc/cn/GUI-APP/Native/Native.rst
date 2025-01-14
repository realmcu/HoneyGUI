C-APP应用程序
=================
- 在本章中，我们将探讨GUI框架内C-APP的创建与管理。C-APP实质上是用户可以开发以制作互动和视觉吸引力用户界面的应用程序。每个C-APP都可以打开、关闭、切换，并在切换时应用动态过渡效果。

- C-APP中显示的内容是使用嵌套控件树结构组织的。该结构包括诸如窗口、可滚动页面和可切屏的容器控件，以及诸如文本、图像和画布之类的内容显示控件。

- 除了默认功能和效果外，C-APP内的控件提供了高度的自定义性。用户可以为控件设置自定义帧动画，并绑定事件以执行他们定义的操作。这种灵活性使得能够根据特定需求和要求创建高度动态和互动的用户界面。

定义一个 C-APP
--------------

- 使用特定名称通过 ``GUI_APP_DEFINE_NAME`` API定义应用程序句柄。

.. literalinclude:: ../../../../realgui/example/screen_454_454/gui_menu/apps_in_menu.c
   :language: c
   :start-after: /*Define a app with name APP_STOPWATCH*/
   :end-before: /*Define a app with name APP_STOPWATCH end*/

- 使用 ``GUI_APP_ENTRY`` API定义应用程序的UI设计入口函数。
- 当应用程序被创建时，UI设计入口函数将被执行一次。




.. literalinclude:: ../../../../realgui/example/screen_454_454/gui_menu/apps_in_menu.c
   :language: c
   :start-after: /*define the app's ui design*/
   :end-before: /*define the app's ui design end*/
- 还有其他方式可以定义应用程序，例如  ``GUI_APP_DEFINEE`` 和 ``struct gui_app``。

创建一个C-APP的控件树
--------------------
- 这是一个时钟应用程序，作为本节的示例。在下图中，您可以看到应用程序界面有秒表和倒计时器选项。点击这些选项可以在它们之间切换。

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1727059552792713966/15519dc3_10088396.gif" width= "400" /></div>
   <br>

下图显示了精简过的控件树结构：

- **SCREEN:APP_STOPWATCH**: 秒表应用程序的主容器。
   - **WINDOW:LEFT_BUTTON**: 包含左按钮的窗口。
      - **CANVAS_RECT:LEFT_BUTTON**: 左按钮的背景画布。
      - **TEXTBOX:LEFT_BUTTON**: 左按钮的文本标签。
   - **WINDOW:RIGHT_BUTTON**: 包含右按钮的窗口。
      - **CANVAS_RECT:RIGHT_BUTTON**: 右按钮的背景画布。
      - **TEXTBOX:RIGHT_BUTTON**: 右按钮的文本标签。
   - **MULTI_LEVEL:0_0**: 多级容器。
      - **MULTI_LEVEL:1_0**: 多级容器中的一个子容器，供秒表使用。
      - **MULTI_LEVEL:1_1**: 多级容器中的另一个子容器，供倒计时器使用。

.. image:: https://foruda.gitee.com/images/1727332782414751516/5754a436_10088396.png
   :align: center

C-APP操作
---------
以下是上述操作如何具体应用于秒表应用程序：

- ``GUI_APP_SHUTDOWN(APP_STOPWATCH)`` ：此命令将关闭秒表应用程序。如果应用程序正在运行计时器，它将停止计时器并关闭界面。关闭时任何相关资源将被释放。
- ``GUI_APP_STARTUP(APP_STOPWATCH)`` ：此命令将初始化并启动秒表应用程序。用户界面将会显示，应用程序将准备好开始记录时间。
- ``GUI_APP_SWAP(APP_STOPWATCH, APP_MAP)`` ：这将在当前运行的秒表应用程序和地图应用程序之间切换。


API
---

.. doxygenfile:: gui_app.h



