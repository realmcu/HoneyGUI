# Use LVGL Design An Application


## Overview
- [LVGL Website](https://lvgl.io/) 
- [LVGL Document](https://docs.lvgl.io/master/intro/index.html) 
- [LVGL Intro](https://docs.lvgl.io/8.3/intro/index.html) 


LVGL (Light and Versatile Graphics Library) is the most popular free and open-source embedded graphics library to create beautiful UIs for any MCU, MPU and display type. LVGL  provides everything you need to create an embedded GUI with easy-to-use graphical elements, beautiful visual effects and a low memory footprint.

LVGL showcases Demo effects on its official website to demonstrate the UI building capabilities of LVGL. The online documentation serves as the primary development resource for LVGL, providing detailed information on the design and operational logic of LVGL, instructions on using various widgets, a wide range of example programs, and guidelines for porting LVGL. Whether you are a beginner or an experienced developer, you can quickly get started and gain a deep understanding of LVGL's functionality and features based on the online documentation.

- [LVGL Demo](https://lvgl.io/demos)
- [LVGL Example](https://docs.lvgl.io/8.3/examples.html)




## HoneyGUI Simulator

A simulator is a powerful tool used for developing UI that simulates the UI interface of embedded devices on a computer. It can mimic the behavior and appearance of a real hardware platform, providing developers with a convenient environment to quickly create, debug, and test UI designs.

The primary purpose of a simulator is to display and interactively test the designed UI interface in real-time, thereby reducing the time and cost of repetitive testing on actual hardware. By using a simulator, developers can iterate designs quickly, view the effects in real-time, and perform debugging and validation. This greatly speeds up UI development and improves workflow efficiency.

Using a simulator has the following advantages:
- Real-time preview: The simulator can show the immediate effects of the UI interface, allowing developers to quickly see the appearance and functionality of their design, facilitating adjustments and modifications.
- Cross-platform support: Simulators can run on computers, eliminating the need for specific hardware platforms.
- Time and resource-saving: Simulators help avoid frequent flashing and testing of UI on actual hardware, reducing additional time and cost overhead.
- Debugging and testing: Simulators provide rich debugging and testing capabilities to inspect the interaction, event handling, and layout effects of UI elements, aiding problem-solving and performance optimization.


### Run LVGL in HoneyGUI Simulator
HoneyGUI Simulator is based on the scons tool and MinGW-w64 toolchain. It can be run and debugged in VScode. For specific environment setup and running instructions, please refer to the "Getting Started - Installation" section.

After completing the environment setup for the HoneyGUI Simulator, when you start running it, you will see the default HoneyGUI project in the simulator. To modify the simulator configuration file to run an LVGL project, go to the path `your HoneyGUI dir/win32_sim/` and open the file `menu_config.h`, which is the configuration file for the simulator. Under the section "HoneyGUI Demo Select", comment out all the demos. Under the section "HoneyGUI Enable LVGL", enable `CONFIG_REALTEK_BUILD_LVGL_GUI`. Then, start running it again in VScode. After the build is successful, you will see the default LVGL demo project running in the simulator.

<div style="text-align: center"><img width= "600" src="https://foruda.gitee.com/images/1727156557578163348/cb760047_9218678.png" alt="lv_menu_config.png"></div><br/>


1. If you need to modify the screen size, open the file `SConscript` under the directory `your HoneyGUI dir/gui_engine/example/demo/`, and modify the values of `DRV_LCD_WIDTH` and `DRV_LCD_HEIGHT` to the desired pixel values.

<div style="text-align: center"><img width= "600" src="https://foruda.gitee.com/images/1727161740835693997/89fd9c57_9218678.png" alt="lv_menu_config.png"></div><br/>




### HoneyGUI LVGL 
The directories and files related to LVGL in HoneyGUI are as follows:

```
HoneyGUI Dir
|-- Arm2D
|-- cmake
|-- doc
|-- gui_engine
|  |-- 3rd
|  |-- app
|  |-- core
|  |-- dc
|  |-- engine
|  :
|  |__ example
|     |-- BAK
|     |-- demo
|     |  |__ app_ui_lvgl.c               // simulator LVGL UI entrance
|     :
|     :
|     |__ screen_lvgl                     
|        |-- root                         // LVGL file system root floder
|        |-- _bin_mkromfs.py
|        |-- mkromfs_0x4600000.bat        // User Data pack script
|        |-- resource.h                   // resource files packed address
|        |__ root(0x4600000).bin          // packed User Data
|-- keil_sim
|-- lib
|-- lvgl                                  // LVGL 
|  |-- demos                              // LVGL demo source files
|  |  |-- benchmark
|  |  |-- keypad_encoder
|  |  |-- music
|  |  |-- stress
|  |  |__ widgets
|  |-- docs
|  |-- env_support
|  |-- examples                           // LVGL example source files
|  |  |-- anim
|  |  |-- arduino
|  |  |-- assets
|  |  |-- event
|  |  |-- get_started
|  |  |-- layouts
|  |  |-- libs
|  |  |-- others
|  |  |-- porting                         // LVGL porting template
|  |  |-- scroll
|  |  |-- styles
|  |  |__ widgets                         // LVGL example widges 
|  |-- rlottie
|  |-- scripts
|  |-- src
|  |  :
|  |  |-- widgets 
|  |  |__ font                            // LVGL internal font
|  |__ tests
:
:
|__ win32_sim
    |-- gui_port                          // HoneyGUI  Simulator porting
    |__ lvgl_port                         // LVGL Simulator porting 
      |-- lv_conf.h                       // LVGL Simulator configuration
      |-- lv_port_disp.c                  
      |-- lv_port_disp.h
      |-- lv_port_fs.c
      |-- lv_port_fs.h
      |-- lv_port_indev.c
      |__ lv_port_indev.h                      
```
1. In HoneyGUI, the LVGL source files are located in the directory `your HoneyGUI dir/lvgl`:
   - demos: Contains various comprehensive built-in examples of LVGL. Some examples can be experienced on [LVGL Demo](https://lvgl.io/demos).
   - docs: Contains the development documentation for LVGL. It can be read online on the LVGL documentation site: [LVGL Online Documentation](https://docs.lvgl.io/master/intro/index.html).
   - env_support: Provides support for various environments or platforms.
   - examples: Stores the built-in examples of LVGL. They can be experienced on [LVGL Example](https://docs.lvgl.io/8.3/examples.html).
   - scripts: Contains some processing scripts that are not typically used when using LVGL.
   - src: Stores the actual source code of LVGL. When developing with LVGL, the code files from this directory are used.
   - tests: Contains some CI testing files that are not used when using LVGL.

2. When running LVGL with the HoneyGUI simulator, the LVGL UI will start running from the file `app_ui_lvgl.c` under the directory `your HoneyGUI dir/gui_engine/example/demo`.

3. When running LVGL with the HoneyGUI simulator, the root directory pointed to by the LVGL file system interface is `your HoneyGUI dir/gui_engine/example/screen_lvgl/root/`.

## Porting
- Documentation: [LVGL Porting](https://docs.lvgl.io/8.3/porting/index.html)

LVGL provides extensive porting support, allowing developers to easily integrate it into various embedded systems and platforms. It supports drivers for various display devices, touchscreens, input devices, and custom GPUs. Developers can configure the porting according to the requirements of their projects, such as adjusting the display parameters when changing display devices, or adapting the input interface when replacing input devices. This article focuses on the porting process and methods for display devices, input devices, and file systems. For more details, please refer to [LVGL Porting](https://docs.lvgl.io/8.3/porting/index.html).

__Note:__ The following examples do not include the specific implementation of hardware device drivers. They only illustrate how to integrate drivers with the LVGL interface. When implementing hardware device drivers, developers can complete the driver functionality under a consistent API framework with the example driver, in order to interface with the HoneyGUI driver layer. The porting interfaces of the example projects can be reused in higher layers.

### Display
- Documentation: [LVGL Porting Display](https://docs.lvgl.io/8.3/porting/display.html), [LVGL Overview Display](https://docs.lvgl.io/8.3/overview/display.html)

Once the developers have completed the debugging of the display device driver, and the device can communicate properly with the display device and show colors, it can be integrated with the display interface of LVGL to showcase LVGL's UI interface.

The display interface of LVGL is implemented in the file `lv_port_disp.c`. Display parameters are configured in the initialization function `void lv_port_disp_init(void)`, such as screen size and frame buffer configuration. The display refresh function is defined as `void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)`.

For detailed display device porting methods and considerations, please refer to the documentation [LVGL Porting Display](https://docs.lvgl.io/8.3/porting/display.html). The following code snippet demonstrates porting a display IC without RAM:

- When using a display IC without RAM, a frame buffer that covers the entire screen size needs to be allocated. Therefore, two frame buffers with a size equal to the screen size are allocated on the PSRAM for display. The macro definitions for display parameters are defined in the file `lv_conf.h`.
- If the display IC used has RAM, the size of the frame buffer does not need to be the same as the screen size. Due to different screen update methods, the `LVGL_USE_EDPI` in `lv_port_disp.c` needs to be configured as not enabled (0) to switch the `disp_flush` function for screen update adaptation.

```c
// flush func select: 1 (IC without RAM), 0 (IC with RAM) 
#define LVGL_USE_EDPI       1

// frame buffer config 
#define LV_PORT_BUF1        (uint32_t)0x08000000   // address in PSRAM
#define LV_PORT_BUF2        (uint32_t)(0x08000000 + MY_DISP_HOR_RES * MY_DISP_VER_RES * LV_COLOR_DEPTH / 8)

void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    /*-----------------------------------
    * Register the display in LVGL
    *----------------------------------*/

    static lv_disp_drv_t disp_drv;                  /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = MY_DISP_HOR_RES;
    disp_drv.ver_res = MY_DISP_VER_RES;

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_flush;

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/

    /**
     * LVGL requires a buffer where it internally draws the widgets.
     * Later this buffer will passed to your display driver's `flush_cb` to copy its content to your display.
     * The buffer has to be greater than 1 display row
     *
     * There are 3 buffering configurations:
     * 1. Create ONE buffer:
     *      LVGL will draw the display's content here and writes it to your display
     *
     * 2. Create TWO buffer:
     *      LVGL will draw the display's content to a buffer and writes it your display.
     *      You should use DMA to write the buffer's content to the display.
     *      It will enable LVGL to draw the next part of the screen to the other buffer while
     *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
     *
     * 3. Double buffering
     *      Set 2 screens sized buffers and set disp_drv.full_refresh = 1.
     *      This way LVGL will always provide the whole rendered screen in `flush_cb`
     *      and you only need to change the frame buffer's address.
     */
    static lv_disp_draw_buf_t draw_buf_dsc_3;
    lv_color_t *buf_3_1 = (lv_color_t *)LV_PORT_BUF1;           /*A screen sized buffer*/
    lv_color_t *buf_3_2 = (lv_color_t *)LV_PORT_BUF2;           /*Another screen sized buffer*/
    lv_disp_draw_buf_init(&draw_buf_dsc_3, buf_3_1, buf_3_2,
                          MY_DISP_VER_RES * MY_DISP_HOR_RES);   /*Initialize the display buffer*/

    /*Set a display buffer*/
    disp_drv.draw_buf = &draw_buf_dsc_3;

    /*Required for Example 3)*/
    disp_drv.full_refresh = 1;

    /* Fill a memory array with a color if you have GPU.
     * Note that, in lv_conf.h you can enable GPUs that has built-in support in LVGL.
     * But if you have a different GPU you can use with this callback.*/
    //disp_drv.gpu_fill_cb = gpu_fill;

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);
}


```


### Input Device
- Documentation: [LVGL Porting Input devices](https://docs.lvgl.io/8.3/porting/indev.html)

Once the developers have completed the debugging of the input device driver, and the device can communicate properly with the input device, it can be integrated with the input interface of LVGL to interact with LVGL's UI interface.

The input interface of LVGL is implemented in the file `lv_port_indev.c`. Input device parameters are configured in the initialization function `void lv_port_indev_init(void)`, including selecting the device type, etc. The input data acquisition function is configured in the function pointer `indev_drv.read_cb()`, which depends on the type of input device and is integrated in `lv_port_indev.c`.

For detailed input device porting methods and considerations, please refer to the documentation [LVGL Porting Input devices](https://docs.lvgl.io/8.3/porting/indev.html). The following code snippet demonstrates porting a touch IC:

- In the initialization function `void lv_port_indev_init(void)`, select and register the corresponding type of input device. For example, for a touchpad device, select `Touchpad`.
- LVGL will retrieve the input data through the function pointer `indev_drv.read_cb()`. Developers need to provide the input data in the function it points to. For a touch screen device, it would be the function `void touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)`. For a touch screen input device, you only need to provide the coordinates of the touch point and the touch state.

```c

void lv_port_indev_init(void)
{
    /**
     * Here you will find example implementation of input devices supported by LittelvGL:
     *  - Touchpad
     *  - Mouse (with cursor support)
     *  - Keypad (supports GUI usage only with key)
     *  - Encoder (supports GUI usage only with: left, right, push)
     *  - Button (external buttons to press points on the screen)
     *
     *  The `..._read()` function are only examples.
     *  You should shape them according to your hardware
     */

    static lv_indev_drv_t indev_drv;

    /*------------------
     * Touchpad
     * -----------------*/

    /*Initialize your touchpad if you have*/
    touchpad_init();

    /*Register a touchpad input device*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read;
    indev_touchpad = lv_indev_drv_register(&indev_drv);
}


/*------------------
 * Touchpad
 * -----------------*/

static uint16_t touch_x = 0;
static uint16_t touch_y = 0;
static bool touch_pressing = 0;


/*Initialize your touchpad*/
static void touchpad_init(void)
{
    /*Your code comes here*/
}

/*Will be called by the library to read the touchpad*/
static void touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    static lv_coord_t last_x = 0;
    static lv_coord_t last_y = 0;

    /* rt touch read port */
    if (drv_touch_read(&touch_x, &touch_y, &touch_pressing) == false)
    {
        return;
    }

    /*Save the pressed coordinates and the state*/
    if (touchpad_is_pressed())
    {
        touchpad_get_xy(&last_x, &last_y);
        data->state = LV_INDEV_STATE_PR;
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }

    /*Set the last pressed coordinates*/
    data->point.x = last_x;
    data->point.y = last_y;
}
/*Return true is the touchpad is pressed*/
// static lv_coord_t touch_x;
// static lv_coord_t touch_y;
static bool touchpad_is_pressed(void)
{
    /*Your code comes here*/
    return touch_pressing;

}

/*Get the x and y coordinates if the touchpad is pressed*/
static void touchpad_get_xy(lv_coord_t *x, lv_coord_t *y)
{
    /*Your code comes here*/
    (*x) = touch_x;
    (*y) = touch_y;
}
```




### File System
- Documentation: [LVGL Overview File system](https://docs.lvgl.io/8.3/overview/file-system.html)

Using a file system to manage storage media makes data more organized and easier to maintain. It can improve compatibility and cross-platform support for external storage devices. Through the file system interface, developers can easily manipulate file data, making it more flexible and efficient. Integrating the file system with LVGL allows resource data to be stored separately from project code, reducing compilation time, improving development efficiency, and enhancing the flexibility of UI design.

The file system interface of LVGL is implemented in the file `lv_port_fs.c`. The file system is configured in the initialization function `void lv_port_fs_init(void)`, which includes initializing the file system and mounting drive letters. Developers need to integrate the interfaces of various file system functions into the corresponding LVGL fs porting functions, ensuring that the input and output data formats are consistent with the interface definitions.

For detailed file system porting methods and considerations, please refer to the documentation [LVGL Overview File system](https://docs.lvgl.io/8.3/overview/file-system.html). The following example demonstrates the porting of `romfs`.

```c
#include "romfs.h"

/**********************
 *      MACROS
 **********************/
#define ROMFS_ADDR 0x04600000
/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_fs_init(void)
{
    /*----------------------------------------------------
     * Initialize your storage device and File System
     * -------------------------------------------------*/
    fs_init();

    /*---------------------------------------------------
     * Register the file system interface in LVGL
     *--------------------------------------------------*/

    /*Add a simple drive to open images*/
    static lv_fs_drv_t fs_drv;
    lv_fs_drv_init(&fs_drv);

    /*Set up fields...*/
    fs_drv.letter = 'F';
    fs_drv.open_cb = fs_open;
    fs_drv.close_cb = fs_close;
    fs_drv.read_cb = fs_read;
    fs_drv.write_cb = fs_write;
    fs_drv.seek_cb = fs_seek;
    fs_drv.tell_cb = fs_tell;

    fs_drv.dir_close_cb = fs_dir_close;
    fs_drv.dir_open_cb = fs_dir_open;
    fs_drv.dir_read_cb = fs_dir_read;

    lv_fs_drv_register(&fs_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your Storage device and File system.*/
static void fs_init(void)
{
    /*E.g. for FatFS initialize the SD card and FatFS itself*/

    /*You code here*/
    romfs_mount((void *)ROMFS_ADDR);
}

/**
 * Open a file
 * @param drv       pointer to a driver where this function belongs
 * @param path      path to the file beginning with the driver letter (e.g. S:/folder/file.txt)
 * @param mode      read: FS_MODE_RD, write: FS_MODE_WR, both: FS_MODE_RD | FS_MODE_WR
 * @return          a file descriptor or NULL on error
 */
static void *fs_open(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    void *f = NULL;

    if (mode == LV_FS_MODE_WR)
    {
        /*Open a file for write*/
        f = NULL;         /*Add your code here*/
    }
    else if (mode == LV_FS_MODE_RD)
    {
        /*Open a file for read*/
        const char *filePath = path;
        f = (void *)open(filePath, O_RDONLY);        /*Add your code here*/
    }
    else if (mode == (LV_FS_MODE_WR | LV_FS_MODE_RD))
    {
        /*Open a file for read and write*/
        f = NULL;         /*Add your code here*/
    }

    return f;
}

/**
 * Close an opened file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable. (opened with fs_open)
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_close(lv_fs_drv_t *drv, void *file_p)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /*Add your code here*/
    res = close((int)file_p);
    return res;
}

/**
 * Read data from an opened file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable.
 * @param buf       pointer to a memory block where to store the read data
 * @param btr       number of Bytes To Read
 * @param br        the real number of read bytes (Byte Read)
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_read(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br)
{
    lv_fs_res_t res = LV_FS_RES_OK;

    /*Add your code here*/
    *br = read((int)file_p, buf, btr);
    return res;
}

```


#### File System Image

HoneyGUI provides support for packaging `romfs` file system images:

1. The working directory is `your HoneyGUI dir/gui_engine/example/screen_lvgl/`. The packaging process requires Python environment support. The external file resources used in the project need to be packaged as a file system image and downloaded as `User Data`.

2. Open the working directory and place the files to be packaged in the `root/` folder. Double-click the `mkromfs_0x4600000.bat` script to generate the file system image `root(0x4600000).bin` and the resource mapping address `resource.h`. The default base address of the files is `0x4600000`. `resource.h` records the mapping address of the packaged files. Since `romfs` supports direct access using physical addresses, developers can access the resource files directly through the mapping address.

3. Use the `User Data` feature of the MP Tool to download and burn the file system image to flash. The burn address should match the base address. If you need to modify the base address, you can modify the `addr` parameter in the `mkromfs_0x4600000.bat` script. For example, the following example changes the base address from `0x4600000` to `0x4000000`.

```shell
# before - base address: 0x4600000, image: root(0x4600000).bin
python _bin_mkromfs.py --binary --addr 0x4600000 root root(0x4600000).bin


# after  - base address: 0x4000000, image: root(0x4000000).bin
python _bin_mkromfs.py --binary --addr 0x4000000 root root(0x4000000).bin
```


## Benchmark

LVGL Benchmark is a performance testing tool designed to evaluate the graphical display performance of the LVGL library in various hardware and software environments. By running the Benchmark, users can obtain data on frame rate, rendering speed, and memory usage, helping to optimize display configurations and debug performance issues. The Benchmark includes various test scenarios such as graphical drawing, animations, and text rendering, each simulating common operations in real applications. Users can use these tests to compare the performance of different configurations and platforms, enabling targeted optimization adjustments.
The official documentation for the LVGL benchmark test is located at `\lvgl\demos\benchmark\README.md`.

### Benchmark for Reference

|    SOC   | CPU CLK | Accelerator | Display Size | Buffering Configurations |               Result              |
|:--------:|:----------:|:------:|:--------:|:-------------:|:---------------------------------:|
| RTL8762E |    40MHz   |   SW   |  240*280 | Double buffing | Weighted FPS:15; Opa. speed: 100% |
| RTL8762E |    40MHz   |   SW   |  80*160  | Double buffing |  Weighted FPS:34; Opa. speed: 95% |
| RTL8762D |    90MHz   |   SW   |  240*280 | Double buffing | Weighted FPS:161; Opa. speed: 77% |
| RTL8762D |    90MHz   |   SW   |  80*160  | Double buffing | Weighted FPS:337; Opa. speed: 95% |
| RTL8772G |   125MHz   | PPE1.0 |  480*480 |  Two buffer  | Weighted FPS:20; Opa. speed: 100% |
| RTL8772G |   125MHz   | PPE1.0 |  240*280 | Double buffing | Weighted FPS:721; Opa. speed: 77% |
| RTL8773E |   100MHz   | PPE2.0 |  390*450 | Double buffing | Weighted FPS:159; Opa. speed: 86% |




##  Start with Demo
- [LVGL Demo](https://lvgl.io/demos)
- [LVGL Example](https://docs.lvgl.io/8.3/examples.html)

It is recommended for developers to read and understand the [LVGL Overview](https://docs.lvgl.io/8.3/overview/index.html) and [LVGL Widgets - Base object](https://docs.lvgl.io/8.3/widgets/obj.html) sections before starting development. This will help them grasp the design concepts and logic of LVGL.

LVGL provides a rich set of demos and examples to help developers understand and familiarize themselves with the usage of various controls and features.
- The [LVGL Demo](https://lvgl.io/demos) showcases comprehensive demos with their source code stored in the directory `your HoneyGUI dir/lvgl/src/demo`. Developers can directly invoke the corresponding `lv_demo_xxx()` function to explore and understand them.
- The online documentation [LVGL Example](https://docs.lvgl.io/8.3/examples.html) demonstrates the running effects of various examples, with their source code stored in the directory `your HoneyGUI dir/lvgl/src/example`. Developers can directly call the corresponding `lv_example_xxx()` function to familiarize themselves with controls and understand their features.

## Resource Online Converter
To use images and fonts in LVGL, they need to be converted to formats that LVGL can recognize using specific tools. LVGL supports converting resources to C array format and bin binary file format. 

In the C array format, the resources will be included in the compilation process. They will be compiled every time the program logic changes, and the size of the resources will be included in the APP image. 

In the bin binary file format, the resources are not included in the compilation. They are stored separately and require a file system or other means to access them.
### Image Converter
- Online conversion tool: [LVGL Image Converter](https://lvgl.io/tools/imageconverter)
- Documentation: [LVGL Overview Images](https://docs.lvgl.io/8.3/overview/image.html)

Please refer to the following steps for usage in [LVGL Overview Images - Online Converter](https://docs.lvgl.io/8.3/overview/image.html#online-converter):

1. Select the LVGL version.
2. Choose the image file.
3. Select the color format for the output file. For color format details, please refer to [LVGL Overview Images - color format](https://docs.lvgl.io/8.3/overview/image.html#color-formats).
4. Choose the type of output image (C array/bin file).
5. Click "Convert" to obtain the output file.

The [LVGL Overview Images](https://docs.lvgl.io/8.3/overview/image.html) document provides detailed instructions on how to use image resources and the image conversion tool in LVGL, along with simple usage examples.

It's worth mentioning that when using the bin file as an image resource, the data in the bin file follows the format of `4 Byte header + data`. The `header` contains information such as `Color format`, `width`, and `height`. To construct a complete `lv_img_dsc_t` to describe the image, you can calculate the `data_size` using the information from the `header`.

```c
typedef struct {

    uint32_t cf : 5;          /*Color format: See `lv_img_color_format_t`*/
    uint32_t always_zero : 3; /*It the upper bits of the first byte. Always zero to look like a
                                 non-printable character*/

    uint32_t reserved : 2; /*Reserved to be used later*/

    uint32_t w : 11; /*Width of the image map*/
    uint32_t h : 11; /*Height of the image map*/
} lv_img_header_t;

/** Image header it is compatible with
 * the result from image converter utility*/
typedef struct {
    lv_img_header_t header; /**< A header describing the basics of the image*/
    uint32_t data_size;     /**< Size of the image in bytes*/
    const uint8_t * data;   /**< Pointer to the data of the image*/
} lv_img_dsc_t;
```


### Font Converter
- Online conversion tool: [LVGL Font Converter](https://lvgl.io/tools/fontconverter)
- Documentation: [LVGL Overview Fonts](https://docs.lvgl.io/8.3/overview/font.html)

Please refer to the following steps for usage in [LVGL Overview Font - Add a new font](https://docs.lvgl.io/8.3/overview/font.html#add-a-new-font):

1. Set the name of the output font.
2. Set the height of the font in pixels.
3. Set the bpp (bits per pixel) of the font. It represents how many bits are used to describe each pixel. Higher values result in better anti-aliasing and smoother edges, but larger font file size.
4. Choose the type of output font (C array/bin file).
5. Select the font file (TTF/WOFF).
6. Set the Unicode range of characters to convert, or directly list the characters that need to be converted.

The [LVGL Overview Fonts](https://docs.lvgl.io/8.3/overview/font.html) document provides detailed instructions on how to use font resources and the font conversion tool in LVGL, along with simple usage examples. In the example, `lv_example_label_3()` demonstrates how to configure a specific font for a label control.

LVGL provides built-in fonts, which are saved as arrays in the directory `your HoneyGUI dir/lvgl/src/font/`. Each font file specifies the included characters at the beginning of the file. The built-in fonts include a Chinese font, `lv_font_simsun_16_cjk.c`, which is a CJK (Chinese, Japanese, and Korean) 16px font, but it is a single font size with a limited character set.

## Development Resources

### Online Doucument
- [LVGL Online Documentation](https://docs.lvgl.io/master/intro/index.html) 

The LVGL [online documentation](https://docs.lvgl.io/master/intro/index.html) provides comprehensive technical documentation and tutorials to help developers better understand and use the LVGL graphics library. The documentation includes the following:

- Overview and features: The documentation introduces the basic concepts and features of LVGL, including graphical objects, screen management, event handling, theme styles, and more. Users can read the documentation to understand the core functionality and advantages of LVGL.

- Application development guide: The documentation provides detailed application development guides, including how to initialize and configure LVGL, how to create and manage graphical objects, how to handle user input and events, and how to add themes and styles. These guides can help users quickly get started with LVGL and develop their own applications.

- API documentation: The documentation provides a comprehensive list of LVGL's API interfaces and functions, along with their parameters and usage. Users can consult the API documentation to understand the specific functions and usage of individual functions and interfaces, enabling more advanced customization and extension.

- Example code: The documentation provides numerous example codes covering common application scenarios and functionalities. Users can leverage these example codes to accelerate development and quickly implement specific functionality requirements.

Using the LVGL online documentation can help users better understand and master the usage and techniques of LVGL, improving development efficiency. Users can gradually learn the contents of the documentation, starting from simple interface construction to complex application development, gradually mastering the various features and capabilities of LVGL. Additionally, the documentation provides examples and code snippets, making it easier for users to develop applications with rich interfaces and functionality.

Users can access the LVGL online documentation through a web browser and browse through the chapters and contents to find and learn relevant knowledge according to their needs. Additionally, the documentation provides a search function to quickly find specific information within the documentation. In summary, the LVGL online documentation is an important resource for users to understand and use the LVGL graphics library. It provides comprehensive and detailed guidance to help users quickly get started and develop better applications.

It is worth noting that while developing based on the documentation can complete most of the UI effects, the documentation may not be exhaustive. When there are omissions in the documentation, the code should be considered the most reliable source.

### Github Repo
- [Github LVGL](https://github.com/lvgl/lvgl)

The LVGL GitHub repository is an important platform for developers to use and contribute to LVGL:

- Getting the latest version: The LVGL GitHub repository provides access to the latest LVGL versions and updates. Developers can stay up-to-date with the latest feature updates, bug fixes, and improvements, keeping their applications in sync with LVGL.

- Engaging in the community and contributing code: Through the GitHub repository, developers can actively participate in LVGL community discussions and exchanges, learning about other developers' issues and solutions. At the same time, developers can contribute their own code and improvements, making LVGL more robust and powerful.

- Submitting issues and bug reports: The GitHub repository offers a platform for issue and bug reporting, allowing developers to submit problems and bugs encountered during their use of LVGL. This helps the LVGL development team promptly discover and resolve issues, improving the stability and reliability of LVGL.

- Learning from examples and documentation: The GitHub repository also includes example code and documentation to help developers better understand and learn how to use LVGL. By browsing the repository's example code and documentation, developers can learn about the various features and capabilities of LVGL, enhancing their development skills.


### Designer
<!-- - GUI Guider: [NXP GUI Guider](https://www.nxp.com/design/design-center/software/development-software/gui-guider:GUI-GUIDER) -->
- GUI Guider: Free
- Squareline: [Squareline Studio](https://squareline.io/), Paid


The LVGL Designer is a visual tool for designing and developing interfaces for the LVGL graphics library. It provides an intuitive and user-friendly interface that allows developers to quickly create and edit GUI interfaces using LVGL.

The LVGL Designer has the following features and functionalities:

- Visual interface design: The designer provides an intuitive visual interface where developers can create and edit GUI interfaces using mouse and simple drag-and-drop operations. It allows adding and adjusting various graphical objects, labels, buttons, text boxes, images, and more, while setting their size, position, style, and other attributes.

- Real-time preview and debugging: The designer supports real-time preview, allowing developers to see the appearance and behavior of the designed interface at any time. This helps developers quickly adjust and optimize the interface to achieve the desired effect.

- Event and interaction management: The designer enables developers to conveniently add and manage events and interaction behaviors. Developers can add click, scroll, drag, and other events to graphical objects and configure their response behaviors through simple configurations.

- Theme and style customization: The designer supports customization of themes and styles, allowing developers to easily select and apply different themes and styles to make the interface more personalized and visually appealing.

- Code export: The designer allows exporting the designed interface as LVGL code, providing the necessary initialization and configuration. This enables developers to directly use the exported code for LVGL application development, eliminating the need for manual code writing.

Using the LVGL Designer greatly accelerates the design and development process of GUI interfaces, especially for non-professional UI designers or developers. With simple drag-and-drop and configuration operations, developers can quickly create attractive and interactive interfaces, improving development efficiency and user experience. Additionally, the designer provides a convenient way to export the designed interface as usable LVGL code, allowing developers to easily integrate it into their applications.


### Forum
- [LVGL Forum](https://forum.lvgl.io/)

The official LVGL forum is a developer community dedicated to discussing and sharing topics and resources related to the LVGL graphics library. It provides a platform for developers to exchange ideas, seek help, and share their experiences and projects.

Some features and functionalities of the LVGL forum include:
- Questions and answers: Developers can ask questions about their LVGL usage on the forum and receive help and answers from other developers. This makes the forum a valuable knowledge base, providing experience and tips for problem-solving.
- Tutorials and examples: The forum contains many useful tutorials and example code, demonstrating how to use different features and functionalities of LVGL. These resources are helpful for novice developers to learn and master LVGL.
- Developer contributions and project showcases: Developers on the forum can share their projects and customized LVGL interfaces, as well as contributions that other developers can share, discuss, and reference.
- Updates and release announcements: The LVGL development team provides announcements and explanations about new version releases and updates on the forum. This allows developers to stay informed about the latest features and improvements.
- Community interaction: The forum provides a platform for community interaction, where developers can communicate, share, and establish connections, enhancing collaboration and development within the LVGL community.

The LVGL forum is an important resource for developers using LVGL to receive support, solve problems, learn, and share experiences.



### Blog
- [LVGL Blog](https://blog.lvgl.io/)

The official LVGL blog is a regularly updated platform that provides the latest information, tutorials, case studies, and developer insights about the LVGL graphics library. The LVGL development team and community members frequently publish various content related to LVGL on the blog, helping developers better understand and use LVGL.

The LVGL blog covers the following content:
- Updates and new feature introductions: The blog publishes articles on the latest version of LVGL, highlighting new features, bug fixes, and performance improvements. This allows developers to stay up-to-date and leverage the latest LVGL capabilities.
- Tutorials and guides: The blog provides practical tutorials and guides on LVGL, covering various topics ranging from beginner to advanced. These tutorials often include example code and detailed explanations, helping developers master the usage of LVGL and best practices.
- Case studies and project showcases: The blog shares case studies and project showcases implemented with LVGL. These articles demonstrate how to use LVGL to build real-world applications and interfaces, providing developers with inspiration and experience from practical implementations.
- Technical deep dives and developer insights: The blog also covers in-depth analyses of LVGL and insights from developers. These articles may explore topics such as the internal workings of LVGL, performance optimization techniques, and excellent design practices, providing developers with a deeper understanding and food for thought.

The LVGL blog is an important resource for developers to understand and master LVGL. By reading the blog, developers can gain insights on the latest LVGL developments, learning materials, and technical insights, helping them utilize LVGL to build excellent graphical interfaces.

## FAQ
- [LVGL FAQ](https://docs.lvgl.io/8.3/intro/index.html#faq)


