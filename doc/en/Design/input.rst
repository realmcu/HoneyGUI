================
Input Subsystem
================
The :term:`UI` system can receive input from other peripherals in the device, typical input devices are touchpads and buttons.

This chapter describes how to use input devices in the UI system and describes the processing of input information in detail.

Touchpad
----------
The touchpad is one of the most commonly used input devices, and most of the time, the touchpad is integrated into the display panel. The workflow for touch information is shown in the figure below.

.. figure:: https://foruda.gitee.com/images/1703054242639518699/13e78a92_9325830.png
   :align: center
   :alt: Touchpad Information Flow

   Touchpad Information Flow

Touchpad Hardware and Driver
~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Although different touchpad chips have different message data structures, the message always contains the touch status and the coordinates of the touch point. In order to transmit coordinate information, a data bus is needed, and :term:`I2C` is the most commonly used data bus between touch chips and microprocessors.

In addition, different touch chips need to use different drivers according to their specifications, which needs to be ported.

Get Touchpad Data
~~~~~~~~~~~~~~~~~~
In the ``port_touchpad_get_data`` function, the touch information will be fetched in ``drv_touch_read``, processed briefly, and fed into the touch algorithm handler as raw data.


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

The data structure of the raw data is ``gui_touch_port_data_t``.

Touchpad Algorithm Processor
~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The code implementation of the touchpad algorithm processing is in the ``tp_algo_process`` function. Gesture recognition is performed by judging changes in X-axis and Y-axis coordinate data, as well as touch time. The input types obtained after the algorithm processing are as follows.


.. literalinclude:: ../../../realgui/core/def_input.h
   :language: c
   :start-after: /* T_GUI_INPUT_TYPE enum start*/
   :end-before: /* T_GUI_INPUT_TYPE enum end*/

The algorithm processor fills in the ``touch_info_t`` structure, which is available to all widgets.

Widget Response
~~~~~~~~~~~~~~~~
Since all widgets inherit from the base class ``gui_obj``, almost any widget can respond to touch events by binding a callback with ``gui_obj_add_event_cb``. A rich set of touch event types is available, such as click (``GUI_EVENT_TOUCH_CLICKED``), long press (``GUI_EVENT_TOUCH_LONG``), and slides in all directions. In addition, a widget can also adjust its display according to the real-time touch coordinates in the ``touch_info_t`` structure.

Internally, a widget obtains and processes touch information via ``tp_get_info`` in its preparation function (such as ``gui_win_prepare``).

At the application level, a callback function can be bound to a widget's event. For example, the following callback sets the incoming-call flag when it is triggered:

.. literalinclude:: ../../../example/application/screen_410_502/watchface_classic.c
   :language: c
   :start-after: /* touch event callback start */
   :end-before: /* touch event callback end */

After creating a widget, use ``gui_obj_add_event_cb`` to bind this callback to the widget's click event ``GUI_EVENT_TOUCH_CLICKED``:

.. literalinclude:: ../../../example/application/screen_410_502/watchface_classic.c
   :language: c
   :dedent:
   :start-after: /* touch img event bind start */
   :end-before: /* touch img event bind end */

In this example, after an image widget is created, ``gui_obj_add_event_cb`` binds its click event to the callback ``switch_call_incoming``, which is executed whenever the widget is clicked.

Keyboard
--------
The workflow for keyboard information is shown in the figure below.

.. figure:: https://foruda.gitee.com/images/1703054218572846675/2cd18af7_9325830.png
   :align: center

   Keyboard Information Flow

Hardware and Driver
~~~~~~~~~~~~~~~~~~~
The hardware design and driver program of the keyboard are relatively simple. Here, we will demonstrate this using a single :term:`GPIO`. For information on how to use GPIO, please refer to the instructions in the SDK. You can use the general :term:`API` in ``rtl87x2g_gpio.c`` or the encapsulated API in ``drv_gpio.c`` to accomplish the same tasks.

Get Keyboard Data
~~~~~~~~~~~~~~~~~
Keyboard input is registered through ``gui_kb_create``. Call ``gui_kb_create`` once for each physical key, passing the key name along with pointers to the key state, press timestamp, and release timestamp variables:

.. literalinclude:: ../../../win32_sim/port/realgui_port/gui_port_indev.c
   :language: c
   :dedent:
   :start-after: /* keyboard register start */
   :end-before: /* keyboard register end */

When porting, users need to update these state variables and timestamps whenever the key state changes (for example, in a GPIO interrupt); the system reads the key information through the registered pointers.

Keyboard Algorithm Processor
~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The code implementation of the keyboard algorithm processing is in the ``kb_algo_process`` function. When a key is released, it judges whether the input is a short press or a long press based on the press duration, and dispatches a ``GUI_EVENT_KB_SHORT_PRESSED`` or ``GUI_EVENT_KB_LONG_PRESSED`` event to the currently focused widget. The algorithm processor also fills in the ``kb_info_t`` structure, which is available to all widgets.

Response
~~~~~~~~~
A widget responds to keyboard events by binding a callback with ``gui_obj_add_event_cb``: a short press corresponds to ``GUI_EVENT_KB_SHORT_PRESSED`` and a long press to ``GUI_EVENT_KB_LONG_PRESSED``. For example, binding a callback to a widget's short-press event:

.. literalinclude:: ../../../example/application/screen_410_502/app_soccer.c
   :language: c
   :dedent:
   :start-after: /* keyboard event bind start */
   :end-before: /* keyboard event bind end */
