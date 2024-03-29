# Porting

The porting files are in the gui_port folder.
    There are four files that need to be modified, corresponding to the input device, display device, OS, and file system.
    At present, it has been transplanted in FreeRTOS, RT-Thread, and Windows, you can refer to it.
## Input device

- Refer to ``guidef.h`` and ``gui_port_indev.c``
- The input information is abstracted as touch screen contacts, and the input information structure is as follows

```C
typedef struct gui_touch_port_data
{
    uint8_t          event;                 /* The touch event of the data */
    uint8_t          track_id;              /* Track id of point */
    uint16_t         width;                 /* Point of width */
    uint16_t         x_coordinate;          /* Point of x coordinate */
    uint16_t         y_coordinate;          /* Point of y coordinate */
    uint32_t         timestamp;             /* The timestamp when the data was received */
    void            *data;
} gui_touch_port_data_t;
```

## Display device

- Refer to ``guidef.h`` and ``gui_port_dc.c``
- It is necessary to define the screen width and height, frame buffer address and mode, whether the resolution is scaled, and realize the refresh function. The structure is as follows.
- The framebuffer's size is ```fb_width*fb_height*bit_depth/8```.
- In ```DC_RAMLESS``` mode, two framebuffers are used, and the fb_height is section height.

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

## File system

- Refer to ``guidef.h`` and ``gui_port_filesystem.c``
- Need to define several posix-style interface operation files and folders, as follows.

```C
struct gui_fs
{
    int (*open)(const char *file, int flags, ...);
    int (*close)(int d);
    int (*read)(int fd, void *buf, size_t len);
    int (*write)(int fd, const void *buf, size_t len);
    int (*lseek)(int fd, int offset, int whence);
    /* directory api*/
    gui_fs_DIR *(*opendir)(const char *name);
    struct gui_fs_dirent *(*readdir)(gui_fs_DIR *d);
    int (*closedir)(gui_fs_DIR *d);
};
```

## OS

- Refer to ``guidef.h`` and ``gui_port_os.c``
- Need to define interfaces for threads, timers, message queues, and memory management, as follows

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

## Sleep management

In order to reduce power consumption and increase device usage time, the sleep(low-power) mode is supported.

- Refer to ``gui_app.h``

```C
typedef struct gui_app gui_app_t;
struct gui_app
{
    gui_obj_t screen;               //!< widget tree root
    const char *xml;                //!< widget tree design file
    uint32_t active_ms;             //!< screen shut dowm delay
    void *thread_id;                //!< thread handle(optional)
    void (* thread_entry)(void *this); //!< thread entry
    void (* ctor)(void *this);      //!< constructor
    void (* dtor)(void *this);      //!< destructor
    void (* ui_design)(gui_app_t *); //!< ui create entry
    bool lvgl;
};
```

``active_ms`` is the standby time of the gui app, which can be defined as different values in different apps.
Same as other types of electronic devices, when the screen continues to display an interface for longer than the standby time, it will enter sleep mode.
During sleep, the device can be awakened by touching the touchpad, pressing a key, or sending a message.
In chip manual, this low-power state where peripherals can be turned off is called deep low power state(DLPS). More information about DLPS can be found in the relevant guidance documentation in the SDK.
