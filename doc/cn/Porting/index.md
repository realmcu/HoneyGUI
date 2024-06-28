# 移植

移植文件位于gui_port文件夹中。
需要修改四个文件，分别对应输入设备、显示设备、操作系统和文件系统。
目前已在FreeRTOS、RT-Thread和Windows上进行了移植，可供参考。
## 输入设备

- 参考 ``guidef.h`` 和 ``gui_port_indev.c``
- 输入信息被抽象为触摸屏接触，输入信息的结构体如下：

```C
typedef struct gui_touch_port_data
{
    uint8_t          event;                 /* 数据的触摸事件 */
    uint8_t          track_id;              /* 点的跟踪id */
    uint16_t         width;                 /* 点的宽度 */
    uint16_t         x_coordinate;          /* 点的x坐标 */
    uint16_t         y_coordinate;          /* 点的y坐标 */
    uint32_t         timestamp;             /* 收到数据时的时间戳 */
    void            *data;
} gui_touch_port_data_t;
```

## 显示设备

- 参考 ``guidef.h`` 和 ``gui_port_dc.c``
- 需要定义屏幕的宽度和高度、帧缓冲区地址和模式、分辨率是否缩放，并实现刷新函数。结构如下。
- 帧缓冲区的大小为 ```fb_width*fb_height*bit_depth/8```.
- 在 ```DC_RAMLESS``` 模式下，使用了两个帧缓冲区，fb_height 是分段高度。

```C
static struct gui_dispdev dc =
{
    .bit_depth = DRV_PIXEL_BITS,
    .fb_width = DRV_LCD_WIDTH,
    .fb_height = FB_HEIGHT,
    .screen_width =  DRV_LCD_WIDTH,
    .screen_height = DRV_LCD_HIGHT,
    .driver_ic_fps = 60,
    .driver_ic_hfp = 10,
    .driver_ic_hbp = 10,
    .driver_ic_active_width = 320,
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
    .gpu_type = GPU_SW,
};
```

## 文件系统

- 参考 ``guidef.h`` 和 ``gui_port_filesystem.c``
- 需要定义几个类似 posix 风格的接口操作文件和文件夹，如下：

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
};
```

## 操作系统

- 参考 ``guidef.h`` 和 ``gui_port_os.c``
- 需要定义线程、定时器、消息队列和内存管理的接口，如下：

```C
struct gui_os_api
{
    char *name;
    void *(*thread_create)(const char *name, void (*entry)(void *param), void *param,
                           uint32_t stack_size, uint8_t priority);
    bool (*thread_delete)(void *handle);
    bool (*thread_mdelay)(uint32_t ms);
    char *(*thread_get_name)(void);
    void *(*timer_create)(char *name, void (*timeout)(void *param), void *param, uint32_t time,
                          bool reload);
    bool (*timer_start)(void *handle);
    bool (*timer_stop)(void *handle);
    bool (*timer_delete)(void *handle);
    void *(*mq_create)(const char *name, uint32_t msg_size, uint32_t max_msgs);
    bool (*mq_send)(void *handle, void *buffer, uint32_t size, uint32_t timeout);
    bool (*mq_send_urgent)(void *handle, void *buffer, uint32_t size, uint32_t timeout);
    bool (*mq_recv)(void *handle, void *buffer, uint32_t size, uint32_t timeout);

    void *(*f_malloc)(uint32_t);
    void (*f_free)(void *rmem);
    void (*assert_handler)(const char *ex_string, const char *func, uint32_t line);
    uint32_t (*tick_get)(void);
    //void (*log)(const char *fmt, ...);

};
```

## 休眠管理

为了降低功耗和增加设备的使用时间，支持睡眠（低功耗）模式。

- 参考 ``gui_app.h``

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
};
```

``active_ms`` 是 gui 应用程序的待机时间，可以在不同的应用程序中定义为不同的值。
与其他类型的电子设备一样，当屏幕持续显示一个界面的时间超过待机时间时，设备将进入睡眠模式。
在睡眠状态下，通过触摸触摸板、按键或发送消息可以唤醒设备。
在芯片手册中，这种外设可以关闭的低功耗状态被称为深度低功耗状态（DLPS）。关于DLPS的更多信息，可以在SDK的相关指导文档中找到。
