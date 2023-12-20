
# Smart UI system overview

Smart UI system is an efficient embedded solution for display projects based on HoneyGUI.

HoneyGUI is a display software framework that includes RealUI and LVGL.

## RealUI workflow

The workflow of the smart UI system is mainly divided into four steps.

<center><img src="https://foruda.gitee.com/images/1703054193639447830/2052fd20_9325830.png",alt="GUI flow.png"/></center>

### System

System initialization mainly includes the initialization of the system clock, the initialization of peripherals and the initialization of other modules of the project, such as PSRAM, LCD, TP, and Bluetooth.

### GUI Server

First, the parts of the GUI port that have been filled in advance are initialized, including the operating system, display, input, and file system. The GUI server thread is then created and the GUI server runs continuously in GUI thread.

### GUI Application

A GUI application is a series of display interfaces consisting of multiple widgets. A GUI app is a series of display interfaces consisting of multiple widgets. In order to run a GUI APP, it needed to be started.

### GUI Server Task

GUI server is the running function of GUI task, and its specific running process is divided into six parts:

1. GUI APP EXIST: First, the currently running GUI APP needs to be obtained. When the GUI detects that there is a running GUI APP, it will proceed to the next step;
2. GET LCD DATA: Get real-time information about the screen;
3. GET TP DATA: Get real-time information about the touchpad and run the touch algorithm;
4. GET KB DATA: Get real-time information about the keyboard and run the keyboard algorithm;
5. OBJ DRAW: Drawing widgets in the APP, including functional operations and image processing.
6. UPDATE FB: Delivers the drawn results to the screen.

More detailed operation of the GUI APP can be found in the online documentation.

# Input sub-system

The UI system can receive input from other peripherals in the device, typical input devices are touchpads and buttons.

This chapter describes how to use input devices in the UI system and describes the processing of input information in detail.

## Touchpad

The touchpad is one of the most commonly used input devices, and most of the time, the touchpad is integrated into the display panel. The workflow for touch information is shown in the figure below.

<center><img src="https://foruda.gitee.com/images/1703054242639518699/13e78a92_9325830.png",alt="Touchpad Information Flow"/></center>

### Touchpad hardware and driver

Although different touchpad chips have different message data structures, the message always contains the touch status and the coordinates of the touch point. In order to transmit coordinate information, a data bus is needed, and I2C is the most commonly used data bus between touch chips and microprocessors.

In addition, different touch chips need to use different drivers according to their specifications, which needs to be ported.

### Get Touchpad Data

In the `port_touchpad_get_data` function, the touch information will be fetched in `drv_touch_read`, processed briefly, and fed into the touch algorithm handler as raw data.

<details> <summary>port_touchpad_get_data</summary>

```C
struct gui_touch_data *port_touchpad_get_data()
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

</details>

The data structure of the raw data is `gui_touch_data_t`.

### Touchpad Algorithm Processor

The code implementation of the touchpad algorithm processing is in the tp_algo_process function.

Gesture recognition is performed by judging changes in X-axis and Y-axis coordinate data, as well as touch time.

The input types obtained after the algorithm processing are as follows.

<details> <summary>GUI_InputType</summary>

```C
typedef enum
{
    TOUCH_INIT      = 0x100,
    TOUCH_HOLD_X,
    TOUCH_HOLD_Y,
    TOUCH_SHORT,
    TOUCH_LONG,
    TOUCH_ORIGIN_FROM_X,
    TOUCH_ORIGIN_FROM_Y,
    TOUCH_LEFT_SLIDE,
    TOUCH_RIGHT_SLIDE,
    TOUCH_UP_SLIDE,
    TOUCH_DOWN_SLIDE,
    TOUCH_SHORT_BUTTON,
    TOUCH_LONG_BUTTON,
    TOUCH_UP_SLIDE_TWO_PAGE,
    TOUCH_DOWN_SLIDE_TWO_PAGE,
    TOUCH_INVALIDE,

    KB_INIT      = 0x200,
    KB_SHORT,
    KB_LONG,
    KB_INVALIDE,
} GUI_InputType;
```

</details>

The algorithm processor fills in the touch_info_t structure, which is available to all widgets.

### Widget Response

Some widgets can respond to touchpad information, such as window widgets, button widgets, tab widgets, curtain widgets and progress bar widgets. Among them, windows and buttons mainly respond to click events, tab, curtain and progress bar mainly respond to swipe events. In addition, the display of tabs, curtains, and progress bars also depends on the touch real-time coordinates in the touch_info_t structure.

Most of the widgets that process touch information are located in the corresponding preparation function, such as win_prepare. Use `tp_get_info` to get touch information.

At the application level, different callback functions can be bound to different kinds of events in the following ways.
<details> <summary>example</summary>

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

</details>

In this example, a window named clock is created first, and when clicked, it executes the `show_clock` function. When prolonged, it executes the enter_homelist function.

## Keyboard

The workflow for keyboard information is shown in the figure below.

<center><img src="https://foruda.gitee.com/images/1703054218572846675/2cd18af7_9325830.png",alt="Keyboard Information Flow"/></center>

### Hardware and driver

The hardware design and driver of the keyboard are relatively simple, and this chapter will explain it through a single GPIO. For instructions on how to use GPIO, you can refer to the instructions in the SDK to use the generic API in rtl87x2g_gpio.c or use the wrapped API in drv_gpio.c. They can do the same thing.

### Get Keyboar Data

In the port_kb_get_data function, the touch information will be fetched. Users need to fill port_kb_get_data according to their functional requirements and fill the structure with keyboard input information.

### Keyboard Algorithm Processor

The code implementation of the keyboard algorithm processing is in the kb_algo_process function.

It can be determined whether the type of input is short press or long press by pressing for a long time.

The algorithm processor fills in the kb_info_t structure, which is available to all widgets.

### Response

There are two ways to respond to the keyboard, one is to respond to the processed key information in the widget such as window, and the other is to respond directly to the press action when the key is received.

The first way is as follow.

For the second type, please refer to the GPIO user manual.

# Display sub-system

The workflow of the display system is very complex, and there are different processes for different UI frameworks and different widgets.

## Display Workflow

As the most commonly used UI input source, the image is used here as an example to illustrate the complete workflow from the raw image to the screen, as shown in the figure below.

Because the hardware configuration of different types of IC is different, the RTL8772G chip platform is chosen, and RealUI is used as the UI system to explain the image display workflow.

</details></br><center>
<img src="https://foruda.gitee.com/images/1703054151955593791/7444de3c_9325830.png",alt="Image Display Work Flow"/>
</center></br>

### Flash File System

The original image is converted into a file in a special format and then downloaded into flash. Flash is configured with a pseudo-file system that provides image index information to the control layer.

After the simple migration of the file system is complete, the standard file system can be used.

Read the GUI online documentation for more information about image conversion tools.

### UI Widget

The image widget is the most basic UI widget used to display images. There are many widgets in the UI system that draw special images based on image widgets.

Here, the image widget loads the image data and reads the image information, which, combined with the UI design and the behavior of the widget layer, provides the image rendering requirements for the acceleration layer.

Such as image movement, image reduction and enlargement, image rotation and so on.

In addition, some of the hardware supports powerful GPU that can draw widgets with complex transformation effects, such as cube widget, color wheel widget, and so on.

### Acceleration

The function of the acceleration layer is to accelerate the UI image drawing process, which is divided into hardware acceleration and software acceleration.

In general, hardware acceleration is significantly better than software acceleration, but which one to use depends on the hardware environment in which the UI system is deployed. In addition, different hardware accelerators, also known as graphics processing units (GPU), also have different capabilities.

The accelerator receives the drawing task assigned by the UI widget and transfers the completed image to the display buffer.

### Buffer

In most embedded systems where RAM is limited, RealUI uses a chunked rendering mechanism that requires a display buffer.

The display buffer stores the image drawing results of the accelerator and the drawing results of other non-accelerating controls, and the data is transferred to the frame buffer through DMA.

Single frame draw mode can be used when the available RAM can accommodate a full frame, in which case a full frame buffer is used instead of a display buffer.

After configuring display controller, it will transfer the frame buffer data to the screen, at this time, the screen will display the UI interface.
