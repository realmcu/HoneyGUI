# Porting

The porting files are located in the `gui_port` folder.
Six files need to be modified, with their filenames and functions as follows.
| Filename                   | Function             |
|---                         |---                   |
| gui_port_acc.c             | Acceleration         |
| gui_port_dc.c              | Display Device       |
| gui_port_filesystem.c      | Filesystem           |
| gui_port_ftl.c             | Flash Translation Layer |
| gui_port_indev.c           | Input Device         |
| gui_port_os.c              | Operating System     |

Currently, porting has been done on FreeRTOS, RT-Thread, and Windows for reference.

## Acceleration

- Refer to `guidef.h` and `gui_port_acc.c`
- Define the accelerated drawing interface depending on the platform model, generally `hw_acc_blit` or `sw_acc_blit`.
- The structure definition is as follows:

    ```C
    typedef struct acc_engine
    {
        void (*blit)(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect);
    } acc_engine_t;
    ```

## Display Device

- Refer to `guidef.h` and `gui_port_dc.c`
- Define the screen width and height, framebuffer address and mode, whether the resolution is scaled, and implement the refresh function. Refer to `guidef.h` for the structure definition.
- A typical `gui_dispdev` structure initialization declaration is as follows:

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

- In `DC_SINGLE` mode, the framebuffer size is `fb_width * fb_height * bit_depth / 8`.
- In `DC_RAMLESS` mode, two partial framebuffers are used, with size `fb_width * fb_height * bit_depth / 8`, where `fb_height` is the segmented height.

### Interface

```eval_rst

The following table lists the LCD-related interfaces supported by mainstream  chips.If you want to know more information, please click on the specific chip name.

===============  =======  ========  =======  =======  =======
SOC              I8080    QSPI      RGB      MIPI     SPI      
---------------  -------  --------  -------  -------  ------- 
`RTL8762C`_      Y        NA        NA       NA       Y         
`RTL8762D`_      Y        Y         NA       NA       Y         
`RTL8763E`_      Y        Y         NA       NA       Y         
`RTL8772G`_      Y        Y         Y        NA       Y        
`RTL8773E`_      Y        Y         Y        NA       Y        
`RTL8772F`_      Y        Y         Y        Y        Y         
===============  =======  ========  =======  =======  =======

'Y' means the driver already included in the library.
'NA' means the driver not yet included in the library. 

.. _RTL8762C: https://www.realmcu.com/en/Home/Product/93cc0582-3a3f-4ea8-82ea-76c6504e478a
.. _RTL8762D: https://www.realmcu.com/en/Home/Product/52feef61-22d0-483e-926f-06eb10e804ca
.. _RTL8763E: https://www.realmcu.com/en/Home/Product/eed7a243-66bf-4b5c-b811-a60d2d4e95cf
.. _RTL8772G: https://www.realmcu.com/en/Home/Product/c175760b-088e-43d9-86da-1fc9b3f07ec3
.. _RTL8773E: https://www.realmcu.com/en/Home/Product/eed7a243-66bf-4b5c-b811-a60d2d4e95cf
.. _RTL8772F: https://www.realmcu.com/en/Home/Product/c175760b-088e-43d9-86da-1fc9b3f07ec3
```

### Driver IC

```eval_rst

The following table lists the LCD-related driver IC supported by mainstream chips.If you want to know more information, please click on the specific chip name.

===============  =========  ==========  =========  =========  =========  ==========  ==========  ==========  ========  ========  ========  ========  ========
SOC              EK9716     ICNA3311    NT35510    NV3047     ST7701S    ST77903     ST7796      OTM8009A    SH8601A   SH8601Z   RM69330   ST7789    NV3041A
---------------  ---------  ----------  ---------  ---------  ---------  ----------  ----------  ----------  --------  --------  --------  --------  --------
`RTL8762D`_      NA         NA          NA         NA         NA         NA          Y           NA          NA        NA        Y         Y         Y
`RTL8763E`_      NA         NA          Y          NA         NA         NA          NA          NA          NA        Y         NA        NA        NA           
`RTL8772G`_      Y          Y           Y          Y          Y          Y           Y           NA          NA        NA        NA        NA        NA
`RTL8773E`_      NA         NA          NA         NA         NA         NA          NA          NA          Y         NA        NA        NA        NA
`RTL8772F`_      NA         Y           Y          NA         Y          NA          NA          Y           Y         Y         NA        NA        NA
===============  =========  ==========  =========  =========  =========  ==========  ==========  ==========  ========  ========  ========  ========  ========

'Y' means the driver already included in the library.
'NA' means the driver not yet included in the library. 

.. _RTL8762C: https://www.realmcu.com/en/Home/Product/93cc0582-3a3f-4ea8-82ea-76c6504e478a
.. _RTL8762D: https://www.realmcu.com/en/Home/Product/52feef61-22d0-483e-926f-06eb10e804ca
.. _RTL8763E: https://www.realmcu.com/en/Home/Product/eed7a243-66bf-4b5c-b811-a60d2d4e95cf
.. _RTL8772G: https://www.realmcu.com/en/Home/Product/c175760b-088e-43d9-86da-1fc9b3f07ec3
.. _RTL8773E: https://www.realmcu.com/en/Home/Product/eed7a243-66bf-4b5c-b811-a60d2d4e95cf
.. _RTL8772F: https://www.realmcu.com/en/Home/Product/c175760b-088e-43d9-86da-1fc9b3f07ec3
```

## Filesystem

- Refer to `guidef.h` and `gui_port_filesystem.c`
- Define several posix-like interfaces to operate files and directories.
- If not using a filesystem, you can fill in null pointers.
- The structure definition is as follows:

    ```C
    struct gui_fs
    {
        int (*open)(const char *file, int flags, ...);
        int (*close)(int d);
        int (*read)(int fd, void *buf, size_t len);
        int (*write)(int fd, const void *buf, size_t len);
        int (*lseek)(int fd, int offset, int whence);
        /* directory api */
        gui_fs_dir *(*opendir)(const char *name);
        struct gui_fs_dirent *(*readdir)(gui_fs_dir *d);
        int (*closedir)(gui_fs_dir *d);
        int (*ioctl)(int fildes, int cmd, ...);
    };
    ```

## Flash Translation Layer

- Refer to `guidef.h` and `gui_port_ftl.c`
- Define three interfaces for the Flash Translation Layer: `read`, `write`, `erase`.
- If not using a Flash Translation Layer, you can fill in null pointers.
- The structure definition is as follows:

    ```C
    static struct gui_ftl ftl_port =
    {
        .read      = (int (*)(uint32_t addr, uint8_t *buf, uint32_t len))port_ftl_read,
        .write     = (int (*)(uint32_t addr, const uint8_t *buf, uint32_t len))port_ftl_write,
        .erase     = (int (*)(uint32_t addr, uint32_t len))port_ftl_erase,
    };
    ```

## Input Device

- Refer to `guidef.h` and `gui_port_indev.c`
- Input devices include touchpads, keyboards, and wheels. The structure for input information is as follows:

    ```C
    struct gui_indev
    {
        uint16_t tp_width;
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

- If a specific input device is needed, the corresponding data acquisition function needs to be implemented in `gui_indev`, and the required time thresholds need to be filled in.

### Touch IC

```eval_rst

The following table lists the Touch-related IC supported by all chips. If you want to know more information, please click on the specific chip name.

==================  =========  ==========  ========  =======  ========  ========  =========                  
SOC                 CST816S    CHSC6417    FT3169    GT911    ZT2717    CST816T    GT9147                 
------------------  ---------  ----------  --------  -------  --------  --------  ---------         
`RTL8762D`_         Y          NA          NA        NA       NA        NA         NA
`RTL8763E`_         NA         NA          NA        NA       NA        Y          Y
`RTL8772G`_         NA         NA          NA        Y        Y         NA         NA
`RTL8773E`_         Y          NA          NA        Y        NA        NA         NA
`RTL8772F`_         Y          Y           Y         Y        NA        NA         NA 
==================  =========  ==========  ========  =======  ========  ========  =========

'Y' means the driver already included in the library.
'NA' means the driver not yet included in the library.              


.. _RTL8762D: https://www.realmcu.com/en/Home/Product/52feef61-22d0-483e-926f-06eb10e804ca
.. _RTL8763E: https://www.realmcu.com/en/Home/Product/eed7a243-66bf-4b5c-b811-a60d2d4e95cf
.. _RTL8772G: https://www.realmcu.com/en/Home/Product/c175760b-088e-43d9-86da-1fc9b3f07ec3
.. _RTL8773E: https://www.realmcu.com/en/Home/Product/eed7a243-66bf-4b5c-b811-a60d2d4e95cf
.. _RTL8772F: https://www.realmcu.com/en/Home/Product/c175760b-088e-43d9-86da-1fc9b3f07ec3
```

## Operating System

- Refer to `guidef.h` and `gui_port_os.c`
- Define the interfaces for thread, timer, message queue, and memory management. The structure definition is as follows:

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

## Sleep Management

To reduce power consumption and increase the device's usage time, sleep (low power) mode is supported.

- Refer to `gui_app.h`

    ```C
    typedef struct gui_app gui_app_t;
    struct gui_app
    {
        gui_obj_t screen;               //!< Root node of the control tree
        const char *xml;                //!< Control tree design file
        uint32_t active_ms;             //!< Screen off delay
        void *thread_id;                //!< Thread handle (optional)
        void (* thread_entry)(void *this); //!< Thread entry function
        void (* ctor)(void *this);      //!< Constructor
        void (* dtor)(void *this);      //!< Destructor
        void (* ui_design)(gui_app_t *); //!< UI creation entry function
        bool lvgl;
        bool arm2d;
        bool close;
        bool next;
        bool close_sync;
    };
    ```

`active_ms` is the standby time of the GUI application, which can be defined as different values in different applications.
Like other types of electronic devices, when the screen continuously displays an interface for the standby time, the device will enter sleep mode.
In sleep mode, the device can be awakened by touching the touchpad, pressing a key, or sending a message.
In the chip manual, this low power state where peripherals can be turned off is called Deep Low Power State (DLPS). More information about DLPS can be found in the relevant SDK documentation.
