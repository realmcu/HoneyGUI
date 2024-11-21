# 使用 LVGL 设计应用程序

## LVGL 简介
- <a href="https://lvgl.io/">LVGL 官网</a>
- [LVGL 在线文档](https://docs.lvgl.io/master/intro/index.html) 
- [LVGL 简介](https://docs.lvgl.io/8.3/intro/index.html) 

LVGL（Light and Versatile Graphics Library）是最受欢迎的免费开源嵌入式图形库，可为任何 MCU、MPU 和显示类型创建漂亮的用户界面，提供了一切你需要创建嵌入式 GUI 所需的功能，包括易于使用的图形元素、美观的视觉效果和低内存占用。

LVGL 在其官网上展示了 Demo 效果以体现 LVGL 的 UI 构建能力。在线文档是 LVGL 的主要开发资料，其中详细介绍了 LVGL 的设计和运行逻辑、各个控件的使用方法、丰富的示例程序以及移植方法等。无论是新手还是有经验的开发者，都可以基于在线在线文档快速上手并深入理解LVGL的功能和特性。

- <a href="https://lvgl.io/demos">LVGL Demo</a>
- [LVGL Example](https://docs.lvgl.io/8.3/examples.html)


<!-- 点击直接查看 demo 和 example 效果 -->


## HoneyGUI 模拟器
<!-- PC 模拟器运行 lvgl demo（脱离EVB开发） -->

模拟器是开发 UI 时使用的一个强大的工具，用于在计算机上模拟嵌入式设备的UI界面。它可以模拟真实硬件平台的行为和外观，提供给开发人员一个便捷的环境来快速创建、调试和测试UI设计。

模拟器的主要作用是实时展示和交互测试设计的UI界面，从而减少在实际硬件上进行反复测试的时间和成本。通过使用模拟器，开发人员可以快速迭代设计，实时查看效果，并进行调试和验证。这大大加快了UI的开发速度和质量，并提高了工作效率。

使用模拟器有以下优点：
- 实时预览：模拟器可以即时显示UI界面的效果，使开发人员能够快速看到设计的外观和功能效果，方便进行调整和修改。
- 跨平台支持：模拟器可以在计算机上运行，开发人员不需要依赖具体的硬件平台。
- 节省时间和资源：使用模拟器可以避免在实际硬件上频繁烧录和测试 UI，减少了额外的时间和成本开销。
- 调试和测试：模拟器提供了丰富的调试和测试功能，可以检查 UI 元素的交互、事件处理和布局效果，有助于解决问题和优化性能。


### 在 HoneyGUI 模拟器中运行 LVGL
HoneyGUI 模拟器基于 scons 工具 和 MinGW-w64 工具链，在 VScode 中运行和进行调试，具体的环境配置和启动运行请参考 [入门指南-安装](../../get-started/index.md) 章节。

完成 HoneyGUI 模拟器的环境安装后，启动运行将看到模拟器默认的 HoneyGUI 工程。修改模拟器配置文件以运行 LVGL 的工程，在路径 `your HoneyGUI dir/win32_sim/` 下的 `menu_config.h` 文件为模拟器的配置文件，在 `HoneyGUI Demo Select` 下注释掉所有的 Demo，在 `HoneyGUI Enable LVGL` 下使能 `CONFIG_REALTEK_BUILD_LVGL_GUI`。在 VScode 中再次启动运行，构建编译通过后即可看到 LVGL 默认的 Demo 工程运行。

<div style="text-align: center"><img width= "800" src="https://foruda.gitee.com/images/1729750006337280334/445a33a3_9218678.png" alt="lv_menu_config.png"></div><br/>


1. 当需要修改屏幕尺寸时，修改文件 `your HoneyGUI dir/realgui/example/demo/` 下的 `SConscript`
文件，修改其中的屏幕宽度 `DRV_LCD_WIDTH` 和 屏幕高度`DRV_LCD_HIGHT`，均为像素单位。
<div style="text-align: center"><img width= "400" src="https://foruda.gitee.com/images/1727161740835693997/89fd9c57_9218678.png" alt="lv_menu_config.png"></div><br/>

<!-- - 运行流程，结构 -->


### HoneyGUI LVGL 
以下为 HoneyGUI 中与 LVGL 相关的目录及文件：

```
HoneyGUI Dir
|-- Arm2D
|-- cmake
|-- doc
|-- realgui
|  |-- 3rd
|  |-- app
|  |-- core
|  |-- dc
|  |-- engine
|  :
|  |__ example
|     |-- BAK
|     |-- demo
|     |  |__ app_ui_lvgl.c               // 模拟器 LVGL UI 入口
|     :
|     :
|     |__ screen_lvgl   
|        |-- assets                       // LVGL 用户图片和字库 C 文件 
|        |  |__ lvgl_example_assets.c     // assets example 
|        |-- root                         // LVGL 文件系统根目录
|        |-- _bin_mkromfs.py
|        |-- mkromfs_0x4600000.bat        // User Data 打包脚本
|        |-- resource.h                   // 打包的文件资源地址映射
|        |__ root(0x4600000).bin          // 打包的 User Data
|-- keil_sim
|-- lib
|-- lvgl                                  // LVGL 
|  |-- demos                              // LVGL demo 源文件
|  |  |-- benchmark
|  |  |-- keypad_encoder
|  |  |-- music
|  |  |-- stress
|  |  |__ widgets
|  |-- docs
|  |-- env_support
|  |-- examples                           // LVGL example 源文件
|  |  |-- anim
|  |  |-- arduino
|  |  |-- assets
|  |  |-- event
|  |  |-- get_started
|  |  |-- layouts
|  |  |-- libs
|  |  |-- others
|  |  |-- porting                         // LVGL porting 模板
|  |  |-- scroll
|  |  |-- styles
|  |  |__ widgets                         // LVGL example 控件源文件，包含各控件 example
|  |-- rlottie
|  |-- scripts
|  |-- src
|  |  :
|  |  |-- widgets 
|  |  |__ font                            // LVGL 内置字库
|  |__ tests
:
:
|__ win32_sim
    |-- gui_port                          // 模拟器 HoneyGUI porting
    |__ lvgl_port                         // 模拟器 LVGL porting 
      |-- lv_conf.h                       // 模拟器 LVGL 配置定义
      |-- lv_port_disp.c                  
      |-- lv_port_disp.h
      |-- lv_port_fs.c
      |-- lv_port_fs.h
      |-- lv_port_indev.c
      |__ lv_port_indev.h                      
```

1. HoneyGUI 中 LVGL 源文件在目录 `your HoneyGUI dir/lvgl` 下：
    - demos：存放 LVGL 一些综合的内置示例，部分示例可以在 [LVGL Demo](https://lvgl.io/demos) 中体验。
    - docs：存放 LVGL 的开发文档，可在 LVGL 的文档站点在线阅读：[LVGL 在线文档](https://docs.lvgl.io/master/intro/index.html)。
    - env_support：一些环境或者平台的支持。
    - examples：存放 LVGL 的内置示例，可在 [LVGL Example](https://docs.lvgl.io/8.3/examples.html) 中体验。
    - scripts：存放一些处理脚本，在使用 LVGL 时基本不会用到。
    - src：存放 LVGL 实际的源码，使用 LVGL 进行开发时，都是使用这里面的代码文件。
    - tests：存放一些 CI 测试文件，在使用LVGL时不会用到。

2. HoneyGUI 模拟器运行 LVGL 时，LVGL UI 将从目录 `your HoneyGUI dir/realgui/example/demo` 下的 `app_ui_lvgl.c` 开始运行

3. 使用 HoneyGUI 模拟器运行 LVGL 时，调用的 LVGL 文件系统接口所指向的根目录为 `your HoneyGUI dir/realgui/example/screen_lvgl/root/`



## 实机移植
- 文档说明： [LVGL Porting](https://docs.lvgl.io/8.3/porting/index.html)

LVGL 提供了广泛的移植支持，使开发者可以将其轻松地集成到各种嵌入式系统和平台中。它支持各种显示设备的驱动、触摸屏、输入设备和自定义 GPU 等。开发者可以根据项目的需求进行移植配置，例如更换显示设备时调整显示参数，替换输入设备时适配输入接口等。本文以显示设备、输入设备和文件系统为例，介绍移植过程和方法，更多细节请参考 [LVGL Porting](https://docs.lvgl.io/8.3/porting/index.html)。



__注意：__ 以下示例不包含硬件设备驱动的具体实现，仅示例如何将驱动对接到 LVGL 的接口。开发者在实现硬件设备驱动时，可在与示例驱动一致的 api 框架下来完成驱动功能，以对接到 HoneyGUI driver 层接口，往上则可复用示例工程的 porting 接口。

### 显示
- 文档说明： [LVGL Porting Display](https://docs.lvgl.io/8.3/porting/display.html), [LVGL Overview Display](https://docs.lvgl.io/8.3/overview/display.html)

在开发者完成显示设备的驱动功能调试后，设备能够与显示设备正常通信并显示色彩，将驱动与 LVGL 的显示接口进行对接以展现 LVGL 的 UI 界面。

LVGL 的显示接口在文件 `lv_port_disp.c` 中实现，显示参数在初始化函数 `void lv_port_disp_init(void)` 中进行配置，如屏幕尺寸和 frame buffer 配置准备等，显示刷新函数为 `void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)`。


详尽的显示设备移植方法和注意事项请参阅文档 [LVGL Porting Display](https://docs.lvgl.io/8.3/porting/display.html)，以下代码段示例了 porting 不带有 RAM 的 display IC：

- 使用不带有 RAM 的 display IC 时，需要为其分配整屏尺寸的 frame buffer，因此在 PSRAM 上分配了两个整屏尺寸的 frame buffer 用于显示。显示的参数宏定义已定义在文件 `lv_conf.h` 中。
- 若使用的 display IC 带有 RAM，则 frame buffer 的大小不必为整屏尺寸。由于刷屏方式的不同，需要配置 `lv_port_disp.c` 中的 `LVGL_USE_EDPI` 为不启用(0)，以切换 `disp_flush` 函数适配刷屏。


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


### 输入设备
- 文档说明： [LVGL Porting Input devices](https://docs.lvgl.io/8.3/porting/indev.html)

在开发者完成输入设备的驱动功能调试后，设备能够与输入设备正常通信，将驱动与 LVGL 的输入接口进行对接以与 LVGL 的 UI 界面进行交互。

LVGL 的输入接口在文件 `lv_port_indev.c` 中实现，输入设备参数在初始化函数 `void lv_port_indev_init(void)` 中进行配置，如选择设备类型等，输入数据获取函数配置在函数指针 `indev_drv.read_cb()`，取决于输入设备类型，均在 `lv_port_indev.c` 中对接。

详尽的输入设备移植方法和注意事项请参阅文档 [LVGL Porting Input devices](https://docs.lvgl.io/8.3/porting/indev.html)，以下代码段示例了 porting 触屏 IC：

- 在初始化函数 `void lv_port_indev_init(void)` 中选择注册对应类型的输入设备，如触屏设备则选择 `Touchpad`
- LVGL 将通过函数指针 `indev_drv.read_cb` 获取输入的数据，开发者需要在其指向的函数中提供输入数据，如触屏设备则为函数 `void touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)`。触屏输入设备仅需提供触点的坐标及触摸状态即可。

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




### 文件系统
- 文档说明： [LVGL Overview File system](https://docs.lvgl.io/8.3/overview/file-system.html)

使用文件系统来管理存储介质使数据更加有条理和易于维护，可以提高外部存储设备的兼容性和跨平台性，通过文件系统接口，开发者可以方便地操作文件数据，更加灵活和高效。开发者对接文件系统到 LVGL 的文件系统接口，使资源数据与工程代码得以分开存储，缩短编译时间，提高开发效率，也增强了 UI 设计的灵活性。

LVGL 的文件系统接口在文件 `lv_port_fs.c` 中实现，文件系统在初始化函数 `void lv_port_fs_init(void)` 中进行配置，包括文件系统的初始化、挂载盘符等，开发者需要将文件系统各功能的接口对接到对应的 LVGL fs porting 函数中，保证输入输出数据格式与接口定义的相一致。


详尽的文件系统移植方法和注意事项请参阅文档 [LVGL Overview File system](https://docs.lvgl.io/8.3/overview/file-system.html)，以下示例了 `romfs` 的 porting:

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


#### romfs 文件系统镜像

HoneyGUI 提供 `romfs` 文件系统镜像的打包支持：
1. 工作路径为`your HoneyGUI dir/realgui/example/screen_lvgl/`，执行打包过程需要有 python 环境支持，工程用到的外部文件资源需要打包为文件系统镜像最终作为 `User Data` 下载。

2. 打开工作路径，将需要打包的文件放置于 `root/` 文件夹下，双击脚本 `mkromfs_0x4600000.bat` 生成文件系统镜像 `root(0x4600000).bin` 和资源映射地址 `resource.h`。文件的默认 base address 为 `0x4600000`，`resource.h` 中记录了打包文件的映射地址，由于 `romfs` 支持物理地址直接访问，开发者可通过映射地址直接访问资源文件。

3. 请使用 MP Tool 的 `User Data` 功能下载烧录文件系统镜像到 flash，烧录地址需与 base address 保持一致。若需要修改 base address, 修改脚本 `mkromfs_0x4600000.bat` 中的 `addr` 参数即可，如下示例为修改 base address 从 `0x4600000` 改为 `0x4000000`。

```shell
# before - base address: 0x4600000, image: root(0x4600000).bin
python _bin_mkromfs.py --binary --addr 0x4600000 root root(0x4600000).bin


# after  - base address: 0x4000000, image: root(0x4000000).bin
python _bin_mkromfs.py --binary --addr 0x4000000 root root(0x4000000).bin
```


## LVGL 基准测试
<!-- - benchmark 介绍
- 参考指标，意义
- 不同平台上的 benchmark 数据参考
-（介绍、演示、分数统计）(性能对比：对比方式/测项，待定) -->


LVGL 的 Benchmark 是一个性能测试工具，用于评估 LVGL 库在各种硬件和软件环境下的图形显示性能。通过运行 Benchmark，用户可以获取帧率、渲染速度和内存使用情况等数据，从而帮助优化显示配置和调试性能问题。Benchmark 包括多种测试场景，如图形绘制、动画和文本渲染，每个场景模拟实际应用中的常见操作。用户可以通过这些测试来比较不同配置和平台的性能表现，从而做出针对性的优化调整。
LVGL 基准测试的官方文档位于 `your HoneyGUI dir/lvgl/demos/benchmark/README.md`。

### LVGL 基准测试结果

|   芯片   | 处理器主频 | 加速器 | 显示面积 |   缓冲区配置  |                结果               |
|:--------:|:----------:|:------:|:--------:|:-------------:|:---------------------------------:|
| RTL8762E |    40MHz   |   SW   |  240*280 | Double buffing | Weighted FPS:15; Opa. speed: 100% |
| RTL8762E |    40MHz   |   SW   |  80*160  | Double buffing |  Weighted FPS:34; Opa. speed: 95% |
| RTL8762D |    90MHz   |   SW   |  240*280 | Double buffing | Weighted FPS:161; Opa. speed: 77% |
| RTL8762D |    90MHz   |   SW   |  80*160  | Double buffing | Weighted FPS:337; Opa. speed: 95% |
| RTL8772G |   125MHz   | PPE1.0 |  480*480 |  Two buffer  | Weighted FPS:20; Opa. speed: 100% |
| RTL8772G |   125MHz   | PPE1.0 |  240*280 | Double buffing | Weighted FPS:721; Opa. speed: 77% |
| RTL8773E |   100MHz   | PPE2.0 |  390*450 | Double buffing | Weighted FPS:159; Opa. speed: 86% |




##  从 Demo 入门开发
- <a href="https://lvgl.io/demos">LVGL Demo</a>
- [LVGL Example](https://docs.lvgl.io/8.3/examples.html)

<!-- 控件API，按照demo (简单修改+引导到示例+文档)

- 需要掌握的基本概念
- 如何从 demo 和 example 开始开发 -->

建议开发者开发前先行阅读理解 [LVGL Overview](https://docs.lvgl.io/8.3/overview/index.html) 和 [LVGL Widgets - Base object](https://docs.lvgl.io/8.3/widgets/obj.html) 部分以了解 LVGL 的设计概念和设计逻辑。

LVGL 提供了丰富的 Demo 和 example 来帮助开发者了解熟悉各个控件和特性的使用。
- [LVGL Demo](https://lvgl.io/demos) 中展示了综合性比较强的 Demo ，其源码保存在目录 `your HoneyGUI dir/lvgl/src/demo` 下，开发者可直接调用对应的 `lv_demo_xxx()` 函数来熟悉了解。
- 在线文档 [LVGL Example](https://docs.lvgl.io/8.3/examples.html) 中展示了各个 example 的运行效果，其源码保存在目录 `your HoneyGUI dir/lvgl/src/example` 下，开发者可直接调用对应的 `lv_example_xxx()` 函数来熟悉控件和理解特性。


## 资源转换器
<!-- （Img + font，介绍和演示） -->
LVGL 的图片和字库需要借助工具转换为 LVGL 可以识别的格式，才能在 UI 中使用。LVGL 支持转换为 C 数组格式和 bin 二进制文件的资源，其中 C 数组格式的资源将会参与编译过程，每当程序逻辑发生变化时，都会参与编译，资源大小计入 APP image（OTA 时需要更大空间），bin 二进制文件格式的资源不参与编译，单独存储，需要文件系统等来支持访问。在路径 `your HoneyGUI dir/realgui/example/screen_lvgl/assets/` 下已提供 example `lvgl_example_assets.c` 示例如何为控件配置不同格式的资源。

### 图片转换器

#### LVGL 在线转换器
- 在线转换工具： [LVGL Image Converter](https://lvgl.io/tools/imageconverter)
- 文档说明： [LVGL Overview Images](https://docs.lvgl.io/8.3/overview/image.html)

使用步骤请参考 [LVGL Overview Images - Online Converter](https://docs.lvgl.io/8.3/overview/image.html#online-converter)：

1. 选择 LVGL 版本
2. 选取图片文件
3. 选择输出文件的颜色格式
颜色格式的说明请参考 [LVGL Overview Images - color format](https://docs.lvgl.io/8.3/overview/image.html#color-formats)
4. 选择输出图片的类型 (C array/binary file)
5. 点击转换获取输出文件

在文档 [LVGL Overview Images](https://docs.lvgl.io/8.3/overview/image.html) 中详细介绍了如何在 LVGL 中使用图片资源和图片转换工具，并提供了简单的使用范例。以 C array 生成的图片资源置于 `your HoneyGUI dir/realgui/example/screen_lvgl/assets/` 下即可被自动构建到工程中。 

值得一提的是，使用 bin 文件的图片资源时，bin 文件中数据的格式为 `4 Byte header + data`, 其中 `header` 中包含有 `Color format`, `width` 和 `height`，此时利用 `header` 信息来计算出 `data_size` 即可构建一个完整的 `lv_img_dsc_t ` 来描述图片。

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



#### HoneyGUI 图像转换工具
- 转换工具下载链接： [HoneyGUI Image Convert Tool](https://docs.realmcu.com/HoneyGUI/cn/latest/tool/index.html)
- 文档说明：[HoneyGUI Image Convert Tool - Doc](https://docs.realmcu.com/HoneyGUI/cn/latest/tool/Resource/image.html)

当需要进一步压缩图片资源占用空间时，HoneyGUI 图像转换工具支持对图片进行压缩转换，IC 支持软硬件解码。HoneyGUI 图像转换工具采用 RLE(Run-length Encoding) 压缩，该压缩算法是一种简单的无损算法，通过编码连续重复的像素值和重复次数来减少存储空间，计算复杂度低且压缩率较高，非常适合用于压缩 GUI 资源。

##### 压缩图片
用户可利用 HoneyGUI 图像转换工具将图片资源转换为 RLE 压缩的二进制文件格式，具体使用步骤请参考 [HoneyGUI Image Converter - Doc](https://docs.realmcu.com/HoneyGUI/cn/latest/tool/Resource/image.html)：

1. 选择需要压缩的图片文件（支持 PNG、JPEG 等格式）
2. 配置图片的转换参数：启用 `Compress`，`Compress Mode` 选择 `RLE`， 启用 `Color Head`，`Color Space` 按需选择
3. 点击转换生成压缩的二进制文件


##### 导入 LVGL
HoneyGUI 图像转换工具生成的二进制文件可导入 LVGL 使用：
1. **若作为文件导入**

   **注意**: 修改文件扩展名为 `.rle` ,即可放入文件系统使用`(SDK/subsys/gui/realgui/example/screen_lvgl/root)`

   ```c
   //file: lvgl_example_assets.c
   void load_img_rle_file(void)
   {
       lv_obj_t *icon = lv_img_create(lv_scr_act());
       lv_img_set_src(icon, "F:/logo_lvgl.rle");
       lv_obj_set_pos(icon, 0, 0);
   }
   ```
   **备注**：使用 RLE 解码器 + ROMFS 时，解码器将会直接从文件系统即 FLASH 上获取图片，不做额外缓存,需要做缓存处理的情况请参考 [文件系统](#文件系统)；
2. **若作为 C 数组格式导入**

 - a. 打开LVGL图片转换工具并上传要转换的压缩文件
 - b. 在`Color format`选项中，务必选择 **CF_RAW**
 - c. 将转换后的图片文件导出为C文件格式，例如 `logo_lvgl_rle.c`

    **注意1：转换结果文件的存放路径:** 将转换后的C文件存放在以下参考路径：
    `SDK/subsys/gui/realgui/example/screen_lvgl/assets`
    
    **注意2：修改图像描述符中的色彩格式 cf:** 导出的C文件，例如`logo_lvgl_rle.c`，需要对其中的图像描述符进行修改，保证 `cf` 设置为 `LV_IMG_CF_RAW`：
    ```c
    //file:logo_lvgl_rle.c
    const lv_img_dsc_t logo_lvgl_rle = {
      .header.cf = LV_IMG_CF_RAW,
      .header.always_zero = 0,
      .header.reserved = 0,
      .header.w = 0,
      .header.h = 0,
      .data_size = 1889,
      .data = logo_lvgl_rle_map,
    };
    ```
 - d. 在项目中包含生成的C文件
   创建图像对象
   ```c
   //file:lvgl_example_assets.c
   void load_img_rle_c_file(void)
   {
       LV_IMG_DECLARE(logo_lvgl_rle);
       lv_obj_t *icon = lv_img_create(lv_scr_act());
       lv_img_set_src(icon, &logo_lvgl_rle);
       lv_obj_set_pos(icon, 0, 0);
   }
   ```

3. 若作为文件导入,以文件地址的方式访问图片资源
 - a. 构建LVGL image header，例如：
   ```c
   //file:lvgl_example_assets.c
   #include "resource.h" 
   
   const lv_img_dsc_t lvgl_test_img_rle = {
     .header.cf = LV_IMG_CF_RAW, 
     .header.always_zero = 0,
     .header.reserved = 0,
     .header.w = 0,
     .header.h = 0,
     .data_size = 0,
     .data = LOGO_LVGL_RLE,
   };
   ```
   **注意：图像描述符中的色彩格式设置为 cf = LV_IMG_CF_RAW**

  - b. 图片资源访问，控件创建：
   ```c
   //file: lvgl_example_assets.c
   void load_img_rle_dataAddr_file(void)
   {
       lv_obj_t *icon = lv_img_create(lv_scr_act());
       lv_img_set_src(icon, &lvgl_test_img_rle);
       lv_obj_set_pos(icon, 0, 0);
   }
   ```
##### LVGL 启用 RLE 解码器

为了在 LVGL 中解码 RLE 压缩的图片资源，需要配置启用 RLE 解码器，并为其分配缓存空间。

1. 启用 RLE 解码器：在配置文件 `lv_conf.h` 中找到 `LV_USE_RTK_IDU` 宏定义，并将其设置为启用（1）
2. 分配解码缓存：在 `lv_conf.h` 文件中配置以下参数：
    - `LV_PSRAM_START`：缓存的起始地址
    - `LV_PSRAM_SIZE`：缓存空间大小，确保此大小足够容纳所使用的最大整张图片的解码数据

```c
// file: lv_conf.h

/*RTK_IDU decoder library*/
#define LV_USE_RTK_IDU 1

#ifdef LV_USE_RTK_IDU
#define LV_MEM_PSRAM_ADR    0x08000000
#define LV_PSRAM_SIZE       (MY_DISP_HOR_RES * MY_DISP_VER_RES * 4)
#define LV_PSRAM_START      (LV_MEM_PSRAM_ADR + 2 * MY_DISP_HOR_RES * MY_DISP_VER_RES * LV_COLOR_DEPTH / 8)
#ifndef LV_MEM_ADR
#define LV_MEM_ADR LV_PSRAM_START
#endif
#endif

```


备注：
- 使用 RLE 解码器 + ROMFS 时，解码器将会直接从文件系统即 FLASH 上获取图片，不做额外缓存；




### 字库转换器
- 在线转换工具：[LVGL Font Converter](https://lvgl.io/tools/fontconverter)
- 文档说明：[LVGL Overview Fonts](https://docs.lvgl.io/8.3/overview/font.html)

使用步骤请参考[LVGL Overview Font - Add a new font](https://docs.lvgl.io/8.3/overview/font.html#add-a-new-font)：

1. 设定输出字库的名字
2. 设定字体的高度 height，像素单位
3. 设定字体的 bpp(bit-per-piel)
表示采用多少个 bit 来描述一个像素，当数值越大时，字符的抗锯齿效果越好，边缘越平滑，字库占用空间越大
4. 选择输出字库的类型 (C array/bin file)
5. 选择字体文件 (TTF/WOFF)
6. 设定需要转换的字符 Unicode 范围，也可直接列出需要转换的字符


在文档 [LVGL Overview Fonts](https://docs.lvgl.io/8.3/overview/font.html) 中详细介绍了如何在 LVGL 中使用字库资源和字库转换工具，并提供了简单的使用范例。在 example 中 `lv_example_label_3()` 示例了如何为 label 控件配置指定的字库。以 C array 生成的字库资源置于 `your HoneyGUI dir/realgui/example/screen_lvgl/assets/` 下即可被自动构建到工程中。 

在 LVGL 中提供了内置的字库，以数组的形式保存在目录 `your HoneyGUI dir/lvgl/src/font/` 下，每份字库所包含的字符均注明在文件开头。内置字库中包含有一份汉字字库 `lv_font_simsun_16_cjk.c` cjk 16 号字库，但为单一字号，字符数有限。


## 开发资源支持

### 在线文档
- <a href="https://docs.lvgl.io/master/intro/index.html">LVGL 文档</a>

LVGL 的[在线文档](https://docs.lvgl.io/master/intro/index.html) 提供了全面的技术文档和教程，帮助开发者更好地了解和使用 LVGL 图形库。该文档包含以下内容：
- 概述和特性：文档介绍了 LVGL 的基本概念和特性，包括图形对象、屏幕管理、事件处理、主题样式等。用户可以通过阅读文档了解 LVGL 的核心功能和优势。
- 应用开发指南：文档提供了详细的应用开发指南，包括如何初始化和配置 LVGL 、如何创建和管理图形对象、如何处理用户输入和事件、如何添加主题和样式等。这些指南可以帮助用户快速上手使用LVGL并开发自己的应用程序。
- API 文档：文档详细列举了 LVGL 的 API 接口和函数，以及它们的参数和用法。用户可以根据需要查阅API文档来了解具体的函数和接口的功能和用法，以便进行更高级的自定义和扩展。
- 示例代码：文档中提供了众多的示例代码，涵盖了常见的应用场景和功能。用户可以借鉴这些示例代码，加快开发速度，并快速实现特定功能的需求。

使用 LVGL 的在线文档可以帮助用户更好地理解和掌握 LVGL 的使用方法和技巧，提高开发效率。用户可以通过逐步学习文档中的内容，从简单的界面构建到复杂的应用开发，逐步掌握 LVGL 的各种功能和特性。同时，文档还提供了示例和代码片段，方便用户更快地开发出具有丰富界面和功能的应用程序。

用户可以通过在网页浏览器中打开 LVGL 的在线文档，并浏览各个章节和内容，根据自己的需要查找和学习相关的知识。另外，用户还可以通过搜索功能来快速查找文档中的具体信息。总之，LVGL 的在线文档是用户理解和使用 LVGL 图形库的重要资源，可以提供全面而详细的指导，帮助用户快速上手和开发出更好的应用程序。


基于文档开发能够完成大部分的 UI 效果，值得注意的是，文档内容并不一定齐全，当文档内容存在疏漏时，最终还是以代码为准。


### Github 仓库
- <a href="https://github.com/lvgl/lvgl">Github LVGL</a>

LVGL 的 GitHub 仓库是开发者使用和贡献 LVGL 的重要平台：
- 获取最新版本：LVGL 的 GitHub 仓库可以获得最新的 LVGL 版本和更新。开发者可以及时获取最新的功能更新、修复和改进，保持应用程序与 LVGL 的同步。
- 参与社区和贡献代码：通过 GitHub 仓库，开发者可以积极参与 LVGL 社区的讨论和交流，了解其他开发者的问题和解决方案。同时，开发者也可以贡献自己的代码和改进，让 LVGL 更加完善和强大。
- 提交问题和 bug 报告：GitHub 仓库提供了问题和bug报告的平台，开发者可以提交他们在使用 LVGL 过程中遇到的问题和 bug。这有助于 LVGL 开发团队及时发现和解决问题，提高 LVGL 的稳定性和可靠性。
- 学习示例和文档：GitHub 仓库中还包含示例代码和文档，帮助开发者更好地理解和学习 LVGL 的使用。开发者可以通过浏览仓库中的示例代码和文档，学习 LVGL 的各个功能和特性，提高开发技能。


### 设计器
<!-- - GUI Guider: [NXP GUI Guider](https://www.nxp.com/design/design-center/software/development-software/gui-guider:GUI-GUIDER) -->
- GUI Guider: 免费
- Squareline: [Squareline Studio](https://squareline.io/)，付费

<!-- - 什么是设计器（演示，放图）
辅助开发，是否需要付费，移植，限制 -->

LVGL的设计器（LVGL Designer）是一个为 LVGL 图形库设计和开发界面的可视化工具。它提供了一个直观且用户友好的界面，使开发者能够快速创建和编辑 LVGL 的 GUI 界面。

LVGL Designer 具有以下特点和功能：

- 可视化界面设计：设计器提供了直观的可视化界面，开发者可以使用鼠标和简单的拖放操作来创建和编辑GUI界面。它允许添加和调整各种图形对象、标签、按钮、文本框、图像等元素，并设置它们的大小、位置、样式等属性。

- 实时预览和调试：设计器支持实时预览，开发者可以随时查看他们所设计的界面的外观和行为。这有助于开发者快速调整和优化界面，确保其满足预期效果。

- 事件和交互管理：设计器使开发者能够方便地添加和管理事件和交互行为。开发者可以为图形对象添加点击、滚动、拖动等事件，并通过简单的配置设置它们的响应行为。

- 主题和样式定制：设计器支持主题和样式的定制，开发者可以轻松地选择和应用不同的主题和样式，使界面更具个性化和美观。

- 导出代码：设计器允许将设计的界面导出为 LVGL 代码，并提供所需的初始化和配置。这样，开发者可以直接将导出的代码用于 LVGL 应用程序的开发，省去手动编写代码的步骤。

使用 LVGL 的设计器可以极大地加速 GUI 界面的设计和开发过程，尤其适用于非专业的 UI 设计师或开发者。通过简单的拖放和配置操作，开发者可以快速创建出具有吸引力和交互性的界面，提高开发效率和用户体验。同时，设计器还提供一个便捷的方法来导出设计的界面为可用的 LVGL 代码，使开发者能够直接将其集成到他们的应用程序中。



### 论坛 
- <a href="https://forum.lvgl.io/">LVGL Forum</a>
<!-- 开放提问，LVGL 开发者会回复。 -->

LVGL 的官方论坛是一个开发者社区，致力于讨论和分享有关 LVGL 图形库的话题和资源。它提供了一个平台，供开发者之间交流、寻求帮助和分享他们的经验和项目。

LVGL 论坛的一些特点和功能包括：
- 提问和回答：开发者可以在论坛上提出他们在使用 LVGL 时遇到的问题，并获得其他开发者的帮助和回答。这使得论坛成为一个宝贵的知识库，提供了解决问题的经验和技巧。
- 教程和示例：论坛上有许多有用的教程和示例代码，展示了如何使用 LVGL 的不同功能和特性。这些资源对于新手开发者学习和掌握 LVGL 非常有帮助。
- 开发者贡献和项目展示：论坛上的开发者可以分享他们的项目和定制的 LVGL 界面，以及其他开发者可以共享、讨论和参考的贡献。
- 更新和发布通告：LVGL 的开发团队在论坛上发布关于新版本发布和更新的通告和说明。这使得开发者可以及时了解最新功能和改进。
- 社区互动：论坛提供了一个社区互动的平台，开发者可以互相交流、分享和建立联系，加强 LVGL 开发社区的合作和发展。

LVGL 论坛对于使用 LVGL 的开发者来说，是获取支持、解决问题、学习和分享经验的重要资源。


### 博客
- <a href="https://blog.lvgl.io/">LVGL Blog</a>

LVGL 的官方博客是一个定期更新的平台，提供关于 LVGL 图形库的最新信息、教程、案例研究和开发者见解。LVGL 的开发团队和社区成员经常在博客上发布有关 LVGL 的各种内容，这些内容可以使开发者更好地了解和使用 LVGL。

LVGL 的博客包含以下内容：
- 更新和新功能介绍：博客上会发布关于 LVGL 最新版本的更新和改进的文章，这些文章介绍了新功能、修复了的问题和性能提升，使开发者可以了解和利用最新的 LVGL 特性。
- 教程和使用指南：博客会提供有关 LVGL 的实用教程和使用指南，涵盖从入门到高级的各种主题。这些教程通常包括示例代码和详细说明，帮助开发者掌握 LVGL 的使用和最佳实践。
- 案例研究和项目展示：博客上会分享一些使用 LVGL 实现的案例研究和项目展示。这些文章介绍了如何使用 LVGL 构建实际应用程序和界面，让开发者从实践中获取灵感和经验。
- 技术深入解析和开发者见解：博客还会涵盖对LVGL的深入分析和开发者的见解。这些文章可能探讨 LVGL 的内部工作原理、性能优化技巧、优秀设计实践等方面的主题，给开发者提供更深入的了解和思考。

LVGL 的博客是一个重要的资源，对于 LVGL 的开发者来说是了解和掌握 LVGL 的宝贵来源。通过阅读博客，开发者可以获取到关于 LVGL 最新动态、学习材料和技术见解，帮助他们更好地使用 LVGL 构建出优秀的图形界面。


## 常见问题
- <a href="https://docs.lvgl.io/8.3/intro/index.html#faq">LVGL FAQ</a>

<!-- （引导+在线文档新增FAQ） -->

### HoneyGUI vs LVGL 绘制图片帧率

#### GRAM屏幕（280x456）RAM 分块绘制
背景：RGB565，非压缩图片，测试单张图片的显示绘制性能（HoneyGUI 矩形填充数据暂无，LVGL 暂时未适配 PPE 硬件加速处理图像缩放）。

| 测试类型         | HoneyGUI 帧率(FPS) SW | HoneyGUI 帧率(FPS) PPE | LVGL 帧率(FPS) SW  | LVGL 帧率(FPS) PPE |
|------------------|------------------------|------------------------|--------------------|--------------------|
| 绘制图片         | 73（280x456）          | 74（280x456）          | 70（280x456）      | 73（280x456）      |
| 填充矩形         | 1（280x456）           | 1（280x456）           | 74（280x456）      | 74（280x456）      |
| 图像旋转45°      | 7（280x456）           | 7（280x456）           | 4（280x456）       | 4（280x456）       |
| 图像放大 1.5 倍  | 3（280x456）           | 31（280x456）          | 3（280x456）       | 25（280x456）      |
| 图像缩小 0.5 倍  | 9（280x456）           | 73（280x456）          | 12（280x456）      | 25（280x456）      |

RAM分块绘制测试数据：

| Section | HoneyGUI 帧率(FPS) | LVGL 帧率(FPS) |
|---------|--------------------|----------------|
| 10      | 70                 | 45             |
| 20      | 73                 | 73             |
| 30      | 74                 | 73             |


#### 分析

1. **绘制图片**:
    - **HoneyGUI** 在 SW 和 PPE 模式下的帧率非常接近，分别为 73 和 74 FPS。
    - **LVGL** 也表现良好，在 SW 和 PPE 模式下的帧率为 70 和 73 FPS。
    - 两者在绘制图片时的性能非常接近，可以满足流畅的图像绘制需求。

2. **填充矩形**:
    - **HoneyGUI** 数据暂时未给出。
    - **LVGL** 在 SW 和 PPE 模式下的帧率均为 74 FPS，表现非常稳定。

3. **图像旋转**:
    - **HoneyGUI** 在 SW 和 PPE 模式下的帧率均为 7 FPS，性能稳定。
    - **LVGL** 在 SW 模式下的帧率为 4 FPS，在 PPE 模式下的数据缺失。
    - 在这项测试中，HoneyGUI 明显优于 LVGL。

4. **图像放大 1.5 倍**:
    - **HoneyGUI** 在 SW 模式下的帧率为 3 FPS，而在 PPE 模式下的帧率显著提升至 31 FPS。
    - **LVGL** 在 SW 模式下的帧率为 3 FPS，PPE 模式下暂时未适配。
    - HoneyGUI 在硬件加速（PPE 模式）下表现出色。

5. **图像缩小 0.5 倍**:
    - **HoneyGUI** 在 SW 模式下的帧率为 9 FPS，在 PPE 模式下的帧率提升至 73 FPS。
    - **LVGL** 在 SW 模式下的帧率为 12 FPS，在 PPE 模式下的数据缺失。
    - HoneyGUI 在硬件加速（PPE 模式）下同样表现优异。

#### PSRAM整帧buffer绘制（800x480）

背景：rgb565，图片尺寸 315x316，非压缩图片，RGB 屏幕，测试单张图片的显示绘制性能。

| 测试类型            | HoneyGUI SW (FPS) | HoneyGUI PPE (FPS) | LVGL SW (FPS) | LVGL PPE (FPS) |
|---------------------|-------------------|--------------------|---------------|----------------|
| 绘制图片            | 80 (316x315)      | 82 (316x315)       | 17 (316x315)  | 25 (316x315)   |
| 填充矩形            | -                 | -                  | 25 (316x315)  | 26 (316x315)   |
| 图像旋转            | 3 (316x315)       | 3 (316x315)        | 6 (316x315)   | 4 (316x315)    |
| 中心图像放大1.5倍   | 2 (316x315)       | 23 (316x315)       | 3 (316x315)   | 13 (316x315)    |
| 中心图像缩小0.5倍   | 10 (316x315)      | 82 (316x315)       | 13 (316x315)  | 50 (316x315)    |

#### 分析
对于 RGB 屏幕需要额外的 psram 作为缓存 buffer，LVGL完全使用psram作为图像缓存buffer，相比于 HoneyGUI 采用 ram 与 sram 结合的方式，LVGL 各方面性能表现较差；

#### 结论

- **适用场景**: 需要推动大尺寸的屏幕（例如 800x480），并且整帧绘制的情况，推荐旋转 HoneyGUI，对于需要频繁刷新脏块的项目，推荐使用 LVGL；分块绘制，在ram资源紧张的情况下，推荐使用 HoneyGUI，section 推荐参数 10。
- **旋转，放大缩小**：LVGL 在图像旋转方面由于采用 2x2 的矩阵，在二维图渲染方面，相比于HoneyGUI的 3x3 矩阵，运算方面数据量更少，因此表现更快，而对于显示 2.5D，仿三维效果时，HoneyGUI将表现更好。
- 在实际项目中，可以根据具体的帧率需求、系统资源情况以及其他功能需求，选择合适的显示框架。如果可行，进行具体的性能测试和评估是最为理想的做法。

通过以上分析，可以为项目选择显示框架时提供参考，帮助决策人员根据实际需要做出最佳选择。

### HoneyGUI vs LVGL RAM消耗

#### GRAM 屏幕 (280x456) 动态 RAM 消耗

| 测试类型       | HoneyGUI（Bytes） | LVGL 控件消耗（Bytes） | 
| -------------- | :-----------: | :-----------------: |
| 绘制图片       | 156           | 176                 | 
| 填充矩形       | -             | 200                 | 
| 图像旋转       | 156           | 208                 | 
| 图像放大 1.5 倍 | 156           | 208                 |
| 图像缩小 0.5 倍 | 156           | 176                 |

#### GRAM 屏幕 (280x456) 静态 RAM 消耗

| 测试类型       | HoneyGUI（Bytes） | LVGL 控件消耗（Bytes） |
| -------------- | :-----------: | :-----------------: |
| 绘制图片       | 41892（40KB）  | 55300（54KB）       |
| 填充矩形       | -             | 55300（54KB）       |
| 图像旋转       | 41892（40KB）  | 55300（54KB）       |
| 图像放大 1.5 倍 | 41892（40KB）  | 55300（54KB）       |
| 图像缩小 0.5 倍 | 41892（40KB）  | 55300（54KB）       |
















