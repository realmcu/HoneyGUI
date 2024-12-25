==========
输入子系统
==========
UI系统可以接收设备中其他外设的输入，典型的输入设备有触摸板和按钮。

本章节详细介绍了如何在UI系统中使用输入设备，并详细描述了输入信息的处理过程。

触摸板
------
触摸板是最常用的输入设备之一，大部分情况下，触摸板会集成在显示面板中。触摸信息的工作流程如下图所示。

.. figure:: https://foruda.gitee.com/images/1703054242639518699/13e78a92_9325830.png
   :align: center

   触摸板工作流程

触摸板硬件和驱动程序
~~~~~~~~~~~~~~~~~~
尽管不同的触摸板芯片具有不同的消息数据结构，但消息始终包含触摸状态和触摸点的坐标。为了传输坐标信息，需要一个数据总线，而 :term:`I2C` 是触摸芯片和微处理器之间最常用的数据总线。

此外，根据规格要求，不同的触摸芯片需要使用不同的驱动程序，需要进行移植。

获取触摸板数据
~~~~~~~~~~~~~~
在 ``port_touchpad_get_data`` 函数中，触摸信息将在 ``drv_touch_read`` 中获取，经过简单处理后，作为原始数据供触摸算法处理程序使用。

.. code-block:: c

   struct gui_touch_port_data *port_touchpad_get_data()
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
~~~~~~~~~~~~~~
触摸板算法处理的代码实现在 ``tp_algo_process`` 函数中。通过判断X轴和Y轴坐标数据的变化以及触摸时间，进行手势识别。经过算法处理后得到的输入类型如下：


.. literalinclude:: ../../../realgui/widget/guidef.h
   :language: c
   :start-after: /* T_GUI_INPUT_TYPE enum start*/
   :end-before: /* T_GUI_INPUT_TYPE enum end*/


算法处理器将填充 ``touch_info_t`` 结构体，该结构体对所有控件可用。

控件响应
~~~~~~~~
一些控件可以对触摸板信息做出响应，例如窗口控件、按钮控件、选项卡控件、窗帘控件和进度条控件。其中，窗口和按钮主要响应点击事件，选项卡、窗帘和进度条主要响应滑动事件。此外，选项卡、窗帘和进度条的显示也取决于 ``touch_info_t`` 结构体中的实时触摸坐标。

大多数控件在相应的准备函数中处理触摸信息，比如 ``win_prepare``。使用 ``tp_get_info`` 函数获取触摸信息。

在应用程序层面，可以根据不同类型的事件绑定不同的回调函数，示例如下：

.. code-block:: c

    gui_img_t *hour;
    gui_img_t *minute;
    gui_img_t *second;
    void show_clock(void *obj, gui_event_t e)
    {
        if (GET_BASE(hour) == false)
        {
            gui_obj_show(hour, false);
            gui_obj_show(minute, false);
            gui_obj_show(second, false);
            gui_img_set_attribute((gui_img_t *)home_bg, "home_bg", home[1], 0, 0);
        }
        else
        {
            gui_obj_show(hour, true);
            gui_obj_show(minute, true);
            gui_obj_show(second, true);
            gui_img_set_attribute((gui_img_t *)home_bg, "home_bg", home[0], 0, 0);
        }
    }
    void enter_homelist(void *obj, gui_event_t e)
    {
        gui_log("enter_tablist \n");
        gui_app_switch(gui_current_app(), get_app_homelist());
    }
    void design_tab_home(void *parent)
    {
        hour = gui_img_create_from_mem(parent, "hour", TIME_HOUR_BIN, 160, 192, 0, 0);
        minute = gui_img_create_from_mem(parent, "minute", TIME_MUNITE_BIN, 160, 192, 0, 0);
        second = gui_img_create_from_mem(parent, "second", TIME_SECOND_BIN, 160, 192, 0, 0);
        gui_win_t *clock = gui_win_create(parent, "clock", 0, 84, 320, 300);
        gui_obj_add_event_cb(clock, (gui_event_cb_t)show_clock, GUI_EVENT_TOUCH_CLICKED, NULL);
        gui_obj_add_event_cb(clock, (gui_event_cb_t)enter_homelist, GUI_EVENT_TOUCH_LONG, NULL);
    }


在这个例子中，首先创建了一个名为clock的窗口，在点击时执行 ``show_clock`` 函数，在长按时执行 ``enter_homelist`` 函数。

键盘
----
键盘信息的工作流程如下图所示：

.. figure:: https://foruda.gitee.com/images/1703054218572846675/2cd18af7_9325830.png
   :align: center

   键盘工作流程

硬件和驱动程序
~~~~~~~~~~~~~
键盘的硬件设计和驱动程序比较简单，本章将通过一个单独的 :term:`GPIO` 来说明。有关如何使用GPIO，请参考SDK中的说明。您可以使用 ``rtl87x2g_gpio.c`` 中的通用 :term:`API`  或 ``drv_gpio.c`` 中的封装API来完成相同的操作。

获取键盘数据
~~~~~~~~~~~~
在 ``port_kb_get_data`` 函数中，可以获取到键盘信息。用户根据自己的功能需求填写 ``port_kb_get_data``，并将结构体填充为键盘输入信息。

键盘算法处理器
~~~~~~~~~~~~~~
键盘算法的代码实现在 ``kb_algo_process`` 函数中。通过按压时间的长短来确定输入的类型是短按还是长按。算法处理器将填充 ``kb_info_t`` 结构体，该结构体对所有控件都可用。

响应
~~~~~
对键盘的响应有两种方式，一种是在控件（如窗口）中响应经过处理的按键信息，另一种是在接收到按键时直接响应按下动作。

第一种方式示例如下所示。

.. code-block:: c

    static void win_prepare(gui_obj_t *obj)  
    {  
        gui_dispdev_t *dc = gui_get_dc();  
        touch_info_t *tp = tp_get_info();  
        kb_info_t *kb = kb_get_info();  
        if (kb->pressed == true)  
        {  
            gui_obj_event_set(obj, GUI_EVENT_KB_DOWN_PRESSED);  
        }  
    ......
    }  


对于第二种方式，请参考 GPIO 用户手册。
