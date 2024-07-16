# 移植

移植文件位于gui_port文件夹中。
需要修改六个文件，其文件名与功能如下。
|文件名|功能|
|---|---|
|gui_port_acc.c|加速|
|gui_port_dc.c|显示设备|
|gui_port_filesystem.c|文件系统|
|gui_port_ftl.c|闪存转换层|
|gui_port_indev.c|输入设备|
|gui_port_os.c|操作系统|

目前已在FreeRTOS、RT-Thread和Windows上进行了移植，可供参考。

## 加速

- 参考 `guidef.h` 和 `gui_port_acc.c`
- 需要根据平台型号，定义加速绘制接口，一般是`hw_acc_blit`或者`sw_acc_blit`。
- 结构体定义如下:

```C
typedef struct acc_engine
{
    void (*blit)(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect);
} acc_engine_t;
```

## 显示设备

- 参考 `guidef.h` 和 `gui_port_dc.c`
- 需要定义屏幕的宽度和高度、帧缓冲区地址和模式、分辨率是否缩放等等，并实现刷新函数，结构体定义参考 `guidef.h`。
- 一个典型的`gui_dispdev`结构体初始化声明如下：

```C
static struct gui_dispdev dc =
{
    .bit_depth = DRV_PIXEL_BITS,
    .fb_width = DRV_LCD_WIDTH,
    .fb_height = FB_HEIGHT,
    .screen_width =  DRV_LCD_WIDTH,
    .screen_height = DRV_LCD_HIGHT,
    .dc.disp_buf_1 = disp_write_buff1_port,
    .dc.disp_buf_2 = disp_write_buff2_port,
    .driver_ic_fps = 60,
    .driver_ic_hfp = 10,
    .driver_ic_hbp = 10,
    .driver_ic_active_width = DRV_LCD_WIDTH,
    .type = DC_RAMLESS,
    .adaption = false,
    .section = {0, 0, 0, 0},
    .section_count = 0,
    .lcd_update = port_gui_lcd_update,
    .flash_seq_trans_disable = flash_boost_disable,
    .flash_seq_trans_enable = flash_boost_enable,
    .reset_lcd_timer = reset_vendor_counter,
    .get_lcd_us = read_vendor_counter_no_display,
    .lcd_te_wait = port_lcd_te_wait,
    .dc.scale_x = 1,
    .dc.scale_y = 1,
};
```

- 在 `DC_SINGLE` 模式下，帧缓冲区的大小为 `fb_width*fb_height*bit_depth/8`。
- 在 `DC_RAMLESS` 模式下，使用了两个部分帧缓冲区，大小为`fb_width*fb_height*bit_depth/8`，此时的`fb_height`是分段高度。

## 文件系统

- 参考 `guidef.h` 和 `gui_port_filesystem.c`
- 需要定义几个类似 posix 风格的接口操作文件和文件夹。
- 不使用文件系统时可以填入空指针。
- 结构体定义如下：

```C
struct gui_fs
{
    int (*open)(const char *file, int flags, ...);
    int (*close)(int d);
    int (*read)(int fd, void *buf, size_t len);
    int (*write)(int fd, const void *buf, size_t len);
    int (*lseek)(int fd, int offset, int whence);
    /* directory api*/
    gui_fs_dir *(*opendir)(const char *name);
    struct gui_fs_dirent *(*readdir)(gui_fs_dir *d);
    int (*closedir)(gui_fs_dir *d);
    int (*ioctl)(int fildes, int cmd, ...);
};
```

## 闪存转换层

- 参考 `guidef.h` 和 `gui_port_ftl.c`
- 需要定义闪存转换层的三个接口：`read`，`write`，`erase`。
- 不使用闪存转换层时可以填入空指针。
- 结构体定义如下:

```C
static struct gui_ftl ftl_port =
{
    .read      = (int (*)(uint32_t addr, uint8_t *buf, uint32_t len))port_ftl_read,
    .write     = (int (*)(uint32_t addr, const uint8_t *buf, uint32_t len))port_ftl_write,
    .erase     = (int (*)(uint32_t addr, uint32_t len))port_ftl_erase,
};
```

## 输入设备

- 参考 `guidef.h` 和 `gui_port_indev.c`
- 输入设备包括触摸板、键盘和滚轮，输入信息的结构体如下：

```C
struct gui_indev
{
    uint16_t tp_witdh;
    uint16_t tp_height;
    uint32_t touch_timeout_ms;
    uint16_t long_button_time_ms;
    uint16_t short_button_time_ms;
    uint16_t kb_long_button_time_ms;
    uint16_t kb_short_button_time_ms;
    uint16_t quick_slide_time_ms;

    void (*ext_button_indicate)(void (*callback)(void));

    gui_touch_port_data_t *(*tp_get_data)(void);

    gui_kb_port_data_t *(*kb_get_port_data)(void);

    gui_wheel_port_data_t *(*wheel_get_port_data)(void);
};
```

- 如果需要某一种输入设备，需要在`gui_indev`中实现对应的数度获取函数，并填写所需的时间阈值。

## 操作系统

- 参考 `guidef.h` 和 `gui_port_os.c`
- 需要定义线程、定时器、消息队列和内存管理的接口，结构体定义如下：

```C
struct gui_os_api
{
    char *name;
    void *(*thread_create)(const char *name, void (*entry)(void *param), void *param,
                           uint32_t stack_size, uint8_t priority);
    bool (*thread_delete)(void *handle);
    bool (*thread_suspend)(void *handle);
    bool (*thread_resume)(void *handle);
    bool (*thread_mdelay)(uint32_t ms);
    uint32_t (*thread_ms_get)(void);
    uint32_t (*thread_us_get)(void);
    bool (*mq_create)(void *handle, const char *name, uint32_t msg_size, uint32_t max_msgs);
    bool (*mq_send)(void *handle, void *buffer, uint32_t size, uint32_t timeout);
    bool (*mq_send_urgent)(void *handle, void *buffer, uint32_t size, uint32_t timeout);
    bool (*mq_recv)(void *handle, void *buffer, uint32_t size, uint32_t timeout);

    void *(*f_malloc)(uint32_t);
    void *(*f_realloc)(void *ptr, uint32_t);
    void (*f_free)(void *rmem);

    void (*gui_sleep_cb)(void);

    void *mem_addr;
    uint32_t mem_size;

    uint32_t mem_threshold_size;
    void *lower_mem_addr;
    uint32_t lower_mem_size;

    log_func_t log;
    void (*gui_tick_hook)(void);
};
```

## 休眠管理

为了降低功耗和增加设备的使用时间，支持睡眠（低功耗）模式。

- 参考 `gui_app.h`

```C
typedef struct gui_app gui_app_t;
struct gui_app
{
    gui_obj_t screen;               //!< 控件树的根节点
    const char *xml;                //!< 控件树的设计文件
    uint32_t active_ms;             //!< 屏幕关闭延时
    void *thread_id;                //!< 线程句柄（可选）
    void (* thread_entry)(void *this); //!< 线程入口函数
    void (* ctor)(void *this);      //!< 构造函数
    void (* dtor)(void *this);      //!< 析构函数
    void (* ui_design)(gui_app_t *); //!< UI创建入口函数
    bool lvgl;
    bool arm2d;
    bool close;
    bool next;
    bool close_sync;
};
```

`active_ms` 是 gui 应用程序的待机时间，可以在不同的应用程序中定义为不同的值。
与其他类型的电子设备一样，当屏幕持续显示一个界面的时间超过待机时间时，设备将进入睡眠模式。
在睡眠状态下，通过触摸触摸板、按键或发送消息可以唤醒设备。
在芯片手册中，这种外设可以关闭的低功耗状态被称为深度低功耗状态（DLPS）。关于DLPS的更多信息，可以在SDK的相关指导文档中找到。
