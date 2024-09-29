# Input Subsystem

The UI system can receive input from other peripherals in the device, typical input devices are touchpads and buttons.

This chapter describes how to use input devices in the UI system and describes the processing of input information in detail.

## Touchpad

The touchpad is one of the most commonly used input devices, and most of the time, the touchpad is integrated into the display panel. The workflow for touch information is shown in the figure below.

<center><img src="https://foruda.gitee.com/images/1703054242639518699/13e78a92_9325830.png",alt="Touchpad Information Flow"/></center>

### Touchpad Hardware and Driver

Although different touchpad chips have different message data structures, the message always contains the touch status and the coordinates of the touch point. In order to transmit coordinate information, a data bus is needed, and [I2C](/Glossary.rst#term-I2C) is the most commonly used data bus between touch chips and microprocessors.

In addition, different touch chips need to use different drivers according to their specifications, which needs to be ported.

### Get Touchpad Data

In the `port_touchpad_get_data` function, the touch information will be fetched in `drv_touch_read`, processed briefly, and fed into the touch algorithm handler as raw data.


```C
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
```

The data structure of the raw data is `gui_touch_port_data_t`.

### Touchpad Algorithm Processor

The code implementation of the touchpad algorithm processing is in the `tp_algo_process` function. Gesture recognition is performed by judging changes in X-axis and Y-axis coordinate data, as well as touch time. The input types obtained after the algorithm processing are as follows.


```eval_rst
.. literalinclude:: ../../../gui_engine/widget/guidef.h
   :language: c
   :start-after: /* T_GUI_INPUT_TYPE enum start*/
   :end-before: /* T_GUI_INPUT_TYPE enum end*/
```

The algorithm processor fills in the `touch_info_t` structure, which is available to all widgets.

### Widget Response

Some widgets can respond to touchpad information, such as window widgets, button widgets, tab widgets, curtain widgets and progress bar widgets. Among them, windows and buttons mainly respond to click events, tab, curtain and progress bar mainly respond to swipe events. In addition, the display of tabs, curtains, and progress bars also depends on the touch real-time coordinates in the `touch_info_t` structure.

Most of the widgets that process touch information are located in the corresponding preparation function, such as `win_prepare`. Use `tp_get_info` to get touch information.

At the application level, different callback functions can be bound to different kinds of events in the following ways.

```C
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
    gui_switch_app(gui_current_app(), get_app_homelist());
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
```

In this example, a window named clock is created first, and when clicked, it executes the `show_clock` function. When prolonged, it executes the `enter_homelist` function.

## Keyboard

The workflow for keyboard information is shown in the figure below.

<center><img src="https://foruda.gitee.com/images/1703054218572846675/2cd18af7_9325830.png",alt="Keyboard Information Flow"/></center>

### Hardware and Driver

The hardware design and driver program of the keyboard are relatively simple. Here, we will demonstrate this using a single [GPIO](/Glossary.rst#term-GPIO). For information on how to use GPIO, please refer to the instructions in the SDK. You can use the general [API](/Glossary.rst#term-API) in `rtl87x2g_gpio.c` or the encapsulated API in `drv_gpio.c` to accomplish the same tasks.

### Get Keyboard Data

In the `port_kb_get_data` function, the touch information will be fetched. Users need to fill `port_kb_get_data` according to their functional requirements and fill the structure with keyboard input information.

### Keyboard Algorithm Processor

The code implementation of the keyboard algorithm processing is in the `kb_algo_process` function. It can be determined whether the type of input is short press or long press by pressing for a long time. The algorithm processor fills in the `kb_info_t` structure, which is available to all widgets.

### Response

There are two ways to respond to the keyboard, one is to respond to the processed key information in the widget such as window, and the other is to respond directly to the press action when the key is received.

The first way is as follow.

```c
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
```

For the second type, please refer to the GPIO user manual.
