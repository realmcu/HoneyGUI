# Design Spec

## Smart UI system overview

Smart UI system is an efficient embedded solution for display projects based on HoneyGUI.

HoneyGUI is a display software framework that includes RealUI and LVGL.

### RealUI workflow

The workflow of the smart UI system is mainly divided into four steps.

<center><img src="https://foruda.gitee.com/images/1703054193639447830/2052fd20_9325830.png",alt="GUI flow.png"/></center>

#### System

System initialization mainly includes the initialization of the system clock, the initialization of peripherals and the initialization of other modules of the project, such as PSRAM, LCD, TP, and Bluetooth.

#### GUI Server

First, the parts of the GUI port that have been filled in advance are initialized, including the operating system, display, input, and file system. The GUI server thread is then created and the GUI server runs continuously in GUI thread.

#### GUI Application

A GUI application is a series of display interfaces consisting of multiple widgets. A GUI app is a series of display interfaces consisting of multiple widgets. In order to run a GUI APP, it needed to be started.

#### GUI Server Task

GUI server is the running function of GUI task, and its specific running process is divided into six parts:

1. GUI APP EXIST: First, the currently running GUI APP needs to be obtained. When the GUI detects that there is a running GUI APP, it will proceed to the next step;
2. GET LCD DATA: Get real-time information about the screen;
3. GET TP DATA: Get real-time information about the touchpad and run the touch algorithm;
4. GET KB DATA: Get real-time information about the keyboard and run the keyboard algorithm;
5. OBJ DRAW: Drawing widgets in the APP, including functional operations and image processing.
6. UPDATE FB: Delivers the drawn results to the screen.

More detailed operation of the GUI APP can be found in the online documentation.

## Input sub-system

The UI system can receive input from other peripherals in the device, typical input devices are touchpads and buttons.

This chapter describes how to use input devices in the UI system and describes the processing of input information in detail.

### Touchpad

The touchpad is one of the most commonly used input devices, and most of the time, the touchpad is integrated into the display panel. The workflow for touch information is shown in the figure below.

<center><img src="https://foruda.gitee.com/images/1703054242639518699/13e78a92_9325830.png",alt="Touchpad Information Flow"/></center>

#### Touchpad hardware and driver

Although different touchpad chips have different message data structures, the message always contains the touch status and the coordinates of the touch point. In order to transmit coordinate information, a data bus is needed, and I2C is the most commonly used data bus between touch chips and microprocessors.

In addition, different touch chips need to use different drivers according to their specifications, which needs to be ported.

#### Get Touchpad Data

In the `port_touchpad_get_data` function, the touch information will be fetched in `drv_touch_read`, processed briefly, and fed into the touch algorithm handler as raw data.

<details> <summary>port_touchpad_get_data</summary>

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

</details>

The data structure of the raw data is `gui_touch_port_data_t`.

#### Touchpad Algorithm Processor

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

#### Widget Response

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

### Keyboard

The workflow for keyboard information is shown in the figure below.

<center><img src="https://foruda.gitee.com/images/1703054218572846675/2cd18af7_9325830.png",alt="Keyboard Information Flow"/></center>

#### Hardware and driver

The hardware design and driver of the keyboard are relatively simple, and this chapter will explain it through a single GPIO. For instructions on how to use GPIO, you can refer to the instructions in the SDK to use the generic API in rtl87x2g_gpio.c or use the wrapped API in drv_gpio.c. They can do the same thing.

#### Get Keyboar Data

In the port_kb_get_data function, the touch information will be fetched. Users need to fill port_kb_get_data according to their functional requirements and fill the structure with keyboard input information.

#### Keyboard Algorithm Processor

The code implementation of the keyboard algorithm processing is in the kb_algo_process function.

It can be determined whether the type of input is short press or long press by pressing for a long time.

The algorithm processor fills in the kb_info_t structure, which is available to all widgets.

#### Response

There are two ways to respond to the keyboard, one is to respond to the processed key information in the widget such as window, and the other is to respond directly to the press action when the key is received.

The first way is as follow.

For the second type, please refer to the GPIO user manual.

## Display sub-system

The workflow of the display system is very complex, and there are different processes for different UI frameworks and different widgets.

### Display Workflow

As the most commonly used UI input source, the image is used here as an example to illustrate the complete workflow from the raw image to the screen, as shown in the figure below.

Because the hardware configuration of different types of IC is different, the RTL8772G chip platform is chosen, and RealUI is used as the UI system to explain the image display workflow.

</details></br><center>
<img src="https://foruda.gitee.com/images/1703054151955593791/7444de3c_9325830.png",alt="Image Display Work Flow"/>
</center></br>

#### Flash File System

The original image is converted into a file in a special format and then downloaded into flash. Flash is configured with a pseudo-file system that provides image index information to the control layer.

After the simple migration of the file system is complete, the standard file system can be used.

Read the GUI online documentation for more information about image conversion tools.

#### UI Widget

The image widget is the most basic UI widget used to display images. There are many widgets in the UI system that draw special images based on image widgets.

Here, the image widget loads the image data and reads the image information, which, combined with the UI design and the behavior of the widget layer, provides the image rendering requirements for the acceleration layer.

Such as image movement, image reduction and enlargement, image rotation and so on.

In addition, some of the hardware supports powerful GPU that can draw widgets with complex transformation effects, such as cube widget, color wheel widget, and so on.

#### Acceleration

The function of the acceleration layer is to accelerate the UI image drawing process, which is divided into hardware acceleration and software acceleration.

In general, hardware acceleration is significantly better than software acceleration, but which one to use depends on the hardware environment in which the UI system is deployed. In addition, different hardware accelerators, also known as graphics processing units (GPU), also have different capabilities.

The accelerator receives the drawing task assigned by the UI widget and transfers the completed image to the display buffer.

#### Buffer

In most embedded systems where RAM is limited, RealUI uses a chunked rendering mechanism that requires a display buffer.

The display buffer stores the image drawing results of the accelerator and the drawing results of other non-accelerating controls, and the data is transferred to the frame buffer through DMA.

Single frame draw mode can be used when the available RAM can accommodate a full frame, in which case a full frame buffer is used instead of a display buffer.

After configuring display controller, it will transfer the frame buffer data to the screen, at this time, the screen will display the UI interface.


## Software Accelerate

### Overall Flow Chart
The flowchart depicts the image resource processing flow accelerated by software. When processing images, different processing methods are selected based on the compression status and type of image:

- **Bypass**: Write the source image data directly to the corresponding position in the frame buffer. Bypass mode is incapable of handling the transparency of images. It applies a global opacity value to the entire image, thereby affecting the overall transparency. When it comes to creating transparency effects, bypass mode is more space-efficient compared to source_over mode.
- **Filter**: The filtering technique effectively sifts out pixel data with a value of zero from the originating image data, which essentially means that black pixels are precluded from being inscribed into the frame buffer. This mechanism induces much swifter refresh dynamics. Pixels of any color other than black undergo the standard processing method and are duly recorded into the frame buffer.
- **Source_over**: A blending method that combines image color data and frame buffer pixel color data to calculate the final color based on the opacity_value value `Sa`, and writes it to the corresponding location in the frame buffer. The formula is `((255 - Sa) * D + Sa * S) / 255)`, where `Sa` is the opacity_value of the original image, `D` is the frame buffer pixel data, and `S` is the source image pixel data.

<div style="text-align: center"><img src ="https://foruda.gitee.com/images/1710412187386597657/3b668eac_13671125.png"/></div><br/>

- The `img_type` can be obtained from the `head` of the image, where the structure of the image head is as follows.
```
struct gui_rgb_data_head
{
    unsigned char scan : 1;
    unsigned char rsvd : 4;
    unsigned char compress : 3;
    char type;
    short w;
    short h;
    char version;
    char rsvd2;
};
```
- The value of `img_type` is depicted in the enum below. If the value is `IMDC_COMPRESS`, it indicates that the image is compressed and enters the `rle `processing flow; otherwise, it enters the `no rle `processing flow.
```
typedef enum
{
    RGB565 = 0,
    ARGB8565 = 1,
    RGB888 = 3,
    RGBA8888 = 4,
    BINARY = 5,
    ALPHAMASK = 9,
    IMDC_COMPRESS = 10,
    BMP = 11,
    JPEG = 12,
    PNG = 13,
    GIF = 14,
    RTKARGB8565 = 15,
} GUI_FormatType;
```

- Execute the corresponding `blit` process based on different `blend_mode`.
```
typedef enum
{
    IMG_BYPASS_MODE = 0,
    IMG_FILTER_BLACK,
    IMG_SRC_OVER_MODE, //S * Sa + (1 - Sa) * D
    IMG_COVER_MODE,
    IMG_RECT,
} BLEND_MODE_TYPE;
```

- When the image is compressed, it is necessary to obtain the compression header from the address of the compressed data. The `algorithm_type` parameter of this header contains the actual image type. The types of compressed images are described in the `imdc_src_type` struct, which includes three types: `IMDC_SRC_RGB565`, `IMDC_SRC_RGB888`, and `IMDC_SRC_ARGB8888`.
```
typedef struct imdc_file_header
{
    struct
    {
        uint8_t algorithm: 2;
        uint8_t feature_1: 2;
        uint8_t feature_2: 2;
        uint8_t pixel_bytes: 2;
    } algorithm_type;
    uint8_t reserved[3];
    uint32_t raw_pic_width;
    uint32_t raw_pic_height;
} imdc_file_header_t;
```
```
typedef enum
{
    IMDC_SRC_RGB565 = 0x04, // 4,
    IMDC_SRC_RGB888 = 0x44, // 68,
    IMDC_SRC_ARGB8888 = 0x84, // 132,

} imdc_src_type;
```

### Overview No RLE Bypass Mode

The following flow describes the `bypass mode` process for `No RLE` compressed image. Select a processing method based on the image matrix and the pixel byte of the display device, and write it to the frame buffer.

<div style="text-align: center"><img src ="https://foruda.gitee.com/images/1710327745470292085/b2ffb40d_13671125.png"/></div><br/>

- If the matrix is an identity matrix, a blit process without matrix operations is performed; otherwise, a blit process with matrix operations is carried out.
- The `dc_bytes_per_pixel` is pixel bytes of  display device, calculated as `dc->bit_depth >> 3`, where `bit_depth` is the bit depth of the display device. Taking a display device with a bit depth of 24 as an example, its pixel bytes are 3.

#### No RLE Bypass Mode
The following flowchart describes the process of writing `uncompressed images` to a frame buffer in `bypass mode`. Taking the target device image type as RGB565 as an example.

<div style="text-align: center"><img src ="https://foruda.gitee.com/images/1710318765186128026/16894745_13671125.png"/></div><br/>

1. Perform different processing steps based on the `img_type`.
2. Based on the `opacity_value`, execute the corresponding operation to write image pixels into the framebuffer.
    - If the `opacity_value` is `0`, the image is not displayed and the process is break.
    - If the `opacity_value` is `255`, convert the source image pixels to RGB565 format and write them to the frame buffer.
    - If the `opacity_value` is between `0 and 255`, perform an alpha blending operation to blend the source image pixels with the corresponding frame buffer pixels. The blending formula is `((255 - Sa) * D + Sa * S) / 255)`. Write the blended result to the frame buffer.

#### No RLE Bypass Matrix
The following flowchart describes the process of writing `uncompressed images` to a frame buffer using `blend mode with matrix operations`. Taking the target device image type as RGB565 as an example.

<div style="text-align: center"><img src ="https://foruda.gitee.com/images/1710318778978528410/26fe6091_13671125.png"/></div><br/>

1. Perform different processing steps based on the `img_type`.
2. Perform matrix calculation to map the target area write-in points to image pixels, and obtain the pixel value of the image pixels.
3. Based on the `opacity_value`, execute the corresponding operation to write image pixels into the framebuffer.
    - If the `opacity_value` is `0`, the image is not displayed and the process is break.
    - If the `opacity_value` is `255`, convert the source image pixels to RGB565 format and write them to the frame buffer.
    - If the `opacity_value` is between `0 and 255`, perform an alpha blending operation to blend the source image pixels with the corresponding frame buffer pixels. The blending formula is `((255 - Sa) * D + Sa * S) / 255)`. Write the blended result to the frame buffer.

### Overview No RLE Filter
The following flow describes the `filter mode` process for `No RLE` compressed image. Select a processing method based on the image matrix and the pixel byte of the display device, and write it to the frame buffer.

<div style="text-align: center"><img src ="https://foruda.gitee.com/images/1710327755412119622/87231a49_13671125.png"/></div><br/>

#### No RLE Filter
The following flowchart describes the process of writing `uncompressed images` to a frame buffer using `filter mode`. Taking the target device image type as RGB565 as an example.

<div style="text-align: center"><img src ="https://foruda.gitee.com/images/1710318784445569859/c4bf1075_13671125.png"/></div><br/>

1. Perform different processing steps based on the `img_type`.
2. If the pixel value is 0, skip the processing; otherwise, perform the subsequent writing operation.
3. Based on the `opacity_value`, execute the corresponding operation to write image pixels into the framebuffer.
    - If the `opacity_value` is `0`, the image is not displayed and the process is break.
    - If the `opacity_value` is `255`, convert the source image pixels to RGB565 format and write them to the frame buffer.
    - If the `opacity_value` is between `0 and 255`, perform an alpha blending operation to blend the source image pixels with the corresponding frame buffer pixels. The blending formula is `((255 - Sa) * D + Sa * S) / 255)`. Write the blended result to the frame buffer.

#### No RLE Filter Matrix
The following flowchart describes the process of writing `uncompressed images` to a frame buffer using `filter mode with matrix operations`. Taking the target device image type as RGB565 as an example.

<div style="text-align: center"><img src ="https://foruda.gitee.com/images/1710318788829157429/3ca92cb9_13671125.png"/></div><br/>

1. Perform different processing steps based on the `img_type`.
2. Perform matrix calculation to map the target area write-in points to image pixels, and obtain the pixel value of the image pixels.
3. If the pixel value is 0, skip the processing; otherwise, perform the subsequent writing operation.
4. Based on the `opacity_value`, execute the corresponding operation to write image pixels into the framebuffer.
    - If the `opacity_value` is `0`, the image is not displayed and the process is break.
    - If the `opacity_value` is `255`, convert the source image pixels to RGB565 format and write them to the frame buffer.
    - If the `opacity_value` is between `0 and 255`, perform an alpha blending operation to blend the source image pixels with the corresponding frame buffer pixels. The blending formula is `((255 - Sa) * D + Sa * S) / 255)`. Write the blended result to the frame buffer.

### Overview No RLE Source_over
The following flow describes the `source_over mode` process for `No RLE` compressed image. Select a processing method based on the image matrix and the pixel byte of the display device, and write it to the frame buffer.

<div style="text-align: center"><img src ="https://foruda.gitee.com/images/1710327760371625897/982f575e_13671125.png"/></div><br/>

#### No RLE Alpha No Matrix
The following flowchart describes the process of writing `uncompressed images` to a frame buffer using `source_over mode`. Taking the target device image type as RGB565 and the source image type as RGB565 as an example.

<div style="text-align: center"><img width="400" src ="https://foruda.gitee.com/images/1710318794490000646/1da4e0a2_13671125.png"/></div><br/>

Based on the `opacity_value`, execute the corresponding operation to write image pixels into the framebuffer.
    - If the `opacity_value` is `0`, the image is not displayed and the process is break.
    - If the `opacity_value` is `255`, convert the source image pixels to RGB565 format and write them to the frame buffer.
    - If the `opacity_value` is between `0 and 255`, perform `do_blending_acc_2_rgb565_opacity` to blend the source image pixels with the corresponding frame buffer pixels. Write the blended result to the frame buffer.

#### No RLE Alpha Matrix
The following flowchart describes the process of writing `uncompressed images` to a frame buffer using `source_over mode with matrix operations`. Taking the target device image type as RGB565 and the source image type as RGB565 as an example.

<div style="text-align: center"><img width="400" src ="https://foruda.gitee.com/images/1710318799964556869/22c5cdfb_13671125.png"/></div><br/>

1. Perform matrix calculation to map the target area write-in points to image pixels, and obtain the pixel value of the image pixels.
2. Based on the `opacity_value`, execute the corresponding operation to write image pixels into the framebuffer.
    - If the `opacity_value` is `0`, the image is not displayed and the process is break.
    - If the `opacity_value` is `255`, convert the source image pixels to RGB565 format and write them to the frame buffer.
    - If the `opacity_value` is between `0 and 255`, perform `do_blending_acc_2_rgb565_opacity` to blend the source image pixels with the corresponding frame buffer pixels. Write the blended result to the frame buffer.

### Overview RLE Bypass Mode
The following flow describes the `bypass mode` process for `RLE` compressed image. Select a processing method based on the image matrix and the pixel byte of the display device, and write it to the frame buffer.

<div style="text-align: center"><img src ="https://foruda.gitee.com/images/1710327763804701147/539b16fd_13671125.png"/></div><br/>

#### RLE Bypass No Matrix
The following flowchart describes the process of writing `compressed images` to a frame buffer in `bypass mode`. Taking the target device image type as RGB565 as an example.

<div style="text-align: center"><img src ="https://foruda.gitee.com/images/1710318858967603728/74fc9285_13671125.png"/></div><br/>

1. Perform different processing steps based on the `img_type` from the head of compression data.
2. Decompress the compressed image data.
3. Based on the `opacity_value`, execute the corresponding operation to write image pixels into the framebuffer.
    - If the `opacity_value` is `0`, the image is not displayed and the process is break.
    - If the `opacity_value` is `255`, convert the source image pixels to RGB565 format and write them to the frame buffer.
    - If the `opacity_value` is between `0 and 255`, perform an alpha blending operation to blend the source image pixels with the corresponding frame buffer pixels. The blending formula is `((255 - Sa) * D + Sa * S) / 255)`. Write the blended result to the frame buffer.

#### RLE Bypass Matrix
The following flowchart describes the process of writing `compressed images` to a frame buffer in `bypass mode with matrix operations`. Taking the target device image type as RGB565 as an example.

<div style="text-align: center"><img src ="https://foruda.gitee.com/images/1710318868326988985/8e089811_13671125.png"/></div><br/>

1. Perform different processing steps based on the `img_type` from the head of compression data.
2. Decompress the compressed image data.
3. Perform matrix calculation to map the target area write-in points to image pixels, and obtain the pixel value of the image pixels.
4. Based on the `opacity_value`, execute the corresponding operation to write image pixels into the framebuffer.
    - If the `opacity_value` is `0`, the image is not displayed and the process is break.
    - If the `opacity_value` is `255`, convert the source image pixels to RGB565 format and write them to the frame buffer.
    - If the `opacity_value` is between `0 and 255`, perform an alpha blending operation to blend the source image pixels with the corresponding frame buffer pixels. The blending formula is `((255 - Sa) * D + Sa * S) / 255)`. Write the blended result to the frame buffer.

### Overview RLE Filter
The following flow describes the `filter mode` process for `RLE` compressed image. Select a processing method based on the image matrix and the pixel byte of the display device, and write it to the frame buffer.

<div style="text-align: center"><img src ="https://foruda.gitee.com/images/1710327772848839544/88fe64aa_13671125.png"/></div><br/>

#### RLE Filter
The following flowchart describes the process of writing `compressed images` to a frame buffer in `filter mode`. Taking the target device image type as RGB565 as an example.

<div style="text-align: center"><img src ="https://foruda.gitee.com/images/1710318872306891932/9145c589_13671125.png"/></div><br/>

1. Perform different processing steps based on the `img_type` from the head of compression data.
2. Decompress the compressed image data.
3. If the pixel value is 0, skip the processing; otherwise, perform the subsequent writing operation.
4. Based on the `opacity_value`, execute the corresponding operation to write image pixels into the framebuffer.
    - If the `opacity_value` is `0`, the image is not displayed and the process is break.
    - If the `opacity_value` is `255`, convert the source image pixels to RGB565 format and write them to the frame buffer.
    - If the `opacity_value` is between `0 and 255`, perform an alpha blending operation to blend the source image pixels with the corresponding frame buffer pixels. The blending formula is `((255 - Sa) * D + Sa * S) / 255)`. Write the blended result to the frame buffer.

#### RLE Filter Matrix
The following flowchart describes the process of writing `compressed images` to a frame buffer in `filter mode with matrix operations`. Taking the target device image type as RGB565 as an example.

<div style="text-align: center"><img src ="https://foruda.gitee.com/images/1710318876464804105/64471b6b_13671125.png"/></div><br/>

1. Perform different processing steps based on the `img_type` from the head of compression data.
2. Decompress the compressed image data.
3. Perform matrix calculation to map the target area write-in points to image pixels, and obtain the pixel value of the image pixels.
4. If the pixel value is 0, skip the processing; otherwise, perform the subsequent writing operation.
5. Based on the `opacity_value`, execute the corresponding operation to write image pixels into the framebuffer.
    - If the `opacity_value` is `0`, the image is not displayed and the process is break.
    - If the `opacity_value` is `255`, convert the source image pixels to RGB565 format and write them to the frame buffer.
    - If the `opacity_value` is between `0 and 255`, perform an alpha blending operation to blend the source image pixels with the corresponding frame buffer pixels. The blending formula is `((255 - Sa) * D + Sa * S) / 255)`. Write the blended result to the frame buffer.

### Overview RLE Source_over
The following flow describes the `source_over mode` process for `RLE` compressed image. Select a processing method based on the image matrix and the pixel byte of the display device, and write it to the frame buffer.

<div style="text-align: center"><img src ="https://foruda.gitee.com/images/1710327781855932915/3ce7e711_13671125.png"/></div><br/>

#### RLE Source_over No Matrix
The following flowchart describes the process of writing `compressed images` to a frame buffer in `source_over mode`. Taking the target device image type as RGB565 as an example.

<div style="text-align: center"><img src ="https://foruda.gitee.com/images/1710318880764265303/dee05307_13671125.png"/></div><br/>

1. Perform different processing steps based on the `img_type` from the head of compression data.
2. Decompress the compressed image data.
3. Based on the `opacity_value`, execute the corresponding operation to write image pixels into the framebuffer.
    - If the `opacity_value` is `0`, the image is not displayed and the process is break.
    - If the `opacity_value` is `255`: When the source image is in RGB565 format, directly write it to the frame buffer. Otherwise, perform the corresponding `do blend` operation and write the blend result to the frame buffer.
    - If the `opacity_value` is between `0 and 255`, perform the appropriate `do_blending` operation to blend the source image pixels with the corresponding frame buffer pixels. Write the blended result to the frame buffer.

#### RLE Source_over Matrix
The following flowchart describes the process of writing `compressed images` to a frame buffer in `source_over mode with matrix operations`. Taking the target device image type as RGB565 as an example.

<div style="text-align: center"><img src ="https://foruda.gitee.com/images/1710318886248980211/a668df5f_13671125.png"/></div><br/>

1. Perform different processing steps based on the `img_type` from the head of compression data.
2. Decompress the compressed image data.
3. Perform matrix calculation to map the target area write-in points to image pixels, and obtain the pixel value of the image pixels.
4. Based on the `opacity_value`, execute the corresponding operation to write image pixels into the framebuffer.
    - If the `opacity_value` is `0`, the image is not displayed and the process is break.
    - If the `opacity value` level is `255`: When the source image is in RGB565 format, directly write it to the frame buffer. Otherwise, perform the corresponding `do blend` operation and write the blend result to the frame buffer.
    - If the `opacity_value` is between `0 and 255`, perform the appropriate `do_blending` operation to blend the source image pixels with the corresponding frame buffer pixels. Write the blended result to the frame buffer.






```{eval-rst}
.. note::
   In compressed source_over matrix mode output rle_rgb888 and rle_rgba8888 equivalent to output as rle_rgb565.
```

## Support Input type and Output type

|  |  |  |  |
| --- | --- | --- | --- |
|  |  |  |  |
|  |  |  |  |
|  |  |  |  |
|  |  |  |  |
|  |  |  |  |
|  |  |  |  |
|  |  |  |  |





