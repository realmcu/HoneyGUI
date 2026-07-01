==========
输入子系统
==========
:term:`UI` 系统可以接收设备中其他外设的输入，典型的输入设备有触摸板和按钮。

本章节详细介绍了如何在 UI 系统中使用输入设备，并详细描述了输入信息的处理过程。

触摸板
------
触摸板是最常用的输入设备之一，大部分情况下，触摸板会集成在显示面板中。触摸信息的工作流程如下图所示。

.. figure:: https://foruda.gitee.com/images/1703054242639518699/13e78a92_9325830.png
   :align: center

   触摸板工作流程

触摸板硬件和驱动程序
~~~~~~~~~~~~~~~~~~~~
尽管不同的触摸板芯片具有不同的消息数据结构，但消息始终包含触摸状态和触摸点的坐标。为了传输坐标信息，需要一个数据总线，而 :term:`I2C` 是触摸芯片和微处理器之间最常用的数据总线。

此外，根据规格要求，不同的触摸芯片需要使用不同的驱动程序，需要进行移植。

获取触摸板数据
~~~~~~~~~~~~~~
在 ``port_touchpad_get_data`` 函数中，触摸信息将在 ``drv_touch_read`` 中获取，经过简单处理后，作为原始数据供触摸算法处理程序使用。

.. code-block:: c

   gui_touch_port_data_t *port_touchpad_get_data()
   {
       uint16_t x = 0;
       uint16_t y = 0;
       bool pressing = 0;
       if (drv_touch_read(&x, &y, &pressing) == false)
       {
           return NULL;
       }
       if (pressing == true)
       {
           raw_data.event = 2;
       }
       else
       {
           raw_data.event = 1;
       }
       raw_data.timestamp_ms = os_sys_tick_get();
       raw_data.width = 0;
       raw_data.x_coordinate = x;
       raw_data.y_coordinate = y;
       //gui_log("event = %d, x = %d, y = %d, \n", raw_data.event, raw_data.x_coordinate, raw_data.y_coordinate);
       return &raw_data;
   }

原始数据的数据结构是 ``gui_touch_port_data_t``。

触摸板算法处理器
~~~~~~~~~~~~~~~~
触摸板算法处理的代码实现在 ``tp_algo_process`` 函数中。通过判断 X 轴和 Y 轴坐标数据的变化以及触摸时间，进行手势识别。经过算法处理后得到的输入类型如下：


.. literalinclude:: ../../../realgui/core/def_input.h
   :language: c
   :start-after: /* T_GUI_INPUT_TYPE enum start*/
   :end-before: /* T_GUI_INPUT_TYPE enum end*/


算法处理器将填充 ``touch_info_t`` 结构体，该结构体对所有控件可用。

控件响应
~~~~~~~~
由于所有控件都继承自基类 ``gui_obj`` ，几乎所有控件都可以通过 ``gui_obj_add_event_cb`` 绑定回调函数来响应触摸事件。触摸事件类型丰富，例如点击 ``GUI_EVENT_TOUCH_CLICKED`` 、长按 ``GUI_EVENT_TOUCH_LONG`` ，以及上下左右各方向的滑动等。此外，控件也可以根据 ``touch_info_t`` 结构体中的实时触摸坐标来调整自身的显示。

控件内部会在其准备函数（如 ``gui_win_prepare`` ）中通过 ``tp_get_info`` 函数获取触摸信息并进行处理。

在应用程序层面，可以为控件绑定事件回调函数。例如，下面的回调函数会在被触发时置位来电标志：

.. literalinclude:: ../../../example/application/screen_410_502/watchface_classic.c
   :language: c
   :start-after: /* touch event callback start */
   :end-before: /* touch event callback end */

创建控件后，通过 ``gui_obj_add_event_cb`` 将该回调绑定到控件的点击事件 ``GUI_EVENT_TOUCH_CLICKED`` 上：

.. literalinclude:: ../../../example/application/screen_410_502/watchface_classic.c
   :language: c
   :dedent:
   :start-after: /* touch img event bind start */
   :end-before: /* touch img event bind end */

在这个例子中，创建了一个图片控件后，通过 ``gui_obj_add_event_cb`` 将其点击事件绑定到回调函数 ``switch_call_incoming`` ，当该控件被点击时便会执行此回调函数。

键盘
----
键盘信息的工作流程如下图所示：

.. figure:: https://foruda.gitee.com/images/1703054218572846675/2cd18af7_9325830.png
   :align: center

   键盘工作流程

硬件和驱动程序
~~~~~~~~~~~~~~~
键盘的硬件设计和驱动程序比较简单，本章将通过一个单独的 :term:`GPIO` 来说明。有关如何使用 GPIO，请参考 SDK 中的说明。您可以使用 ``rtl87x2g_gpio.c`` 中的通用 :term:`API`  或 ``drv_gpio.c`` 中的封装 API 来完成相同的操作。

获取键盘数据
~~~~~~~~~~~~
键盘输入通过 ``gui_kb_create`` 注册。每个物理按键调用一次 ``gui_kb_create`` ，传入按键名称，以及按键状态、按下时间戳和释放时间戳的变量指针：

.. literalinclude:: ../../../win32_sim/port/realgui_port/gui_port_indev.c
   :language: c
   :dedent:
   :start-after: /* keyboard register start */
   :end-before: /* keyboard register end */

移植时，用户需要在按键状态发生变化时（例如在 GPIO 中断中）更新这些状态变量和时间戳，系统会通过注册的指针读取按键信息。

键盘算法处理器
~~~~~~~~~~~~~~
键盘算法的代码实现在 ``kb_algo_process`` 函数中。当按键释放时，通过按压时长来判断输入类型是短按还是长按，并向当前焦点控件派发 ``GUI_EVENT_KB_SHORT_PRESSED`` 或 ``GUI_EVENT_KB_LONG_PRESSED`` 事件。算法处理器还会填充 ``kb_info_t`` 结构体，该结构体对所有控件都可用。

响应
~~~~~
控件通过 ``gui_obj_add_event_cb`` 绑定回调函数来响应键盘事件，短按对应 ``GUI_EVENT_KB_SHORT_PRESSED`` 事件，长按对应 ``GUI_EVENT_KB_LONG_PRESSED`` 事件。例如，为控件绑定短按事件的回调：

.. literalinclude:: ../../../example/application/screen_410_502/app_soccer.c
   :language: c
   :dedent:
   :start-after: /* keyboard event bind start */
   :end-before: /* keyboard event bind end */

