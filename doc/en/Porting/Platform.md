# Platform Porting

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

- Refer to `guidef.h` and `gui_port_acc.c`.
- Define the accelerated drawing interface depending on the platform model, generally `hw_acc_blit` or `sw_acc_blit`.
- The structure definition is as follows:

    ```C
    typedef struct acc_engine
    {
        void (*blit)(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect);
    } acc_engine_t;
    ```

## Display Device

- Refer to `guidef.h` and `gui_port_dc.c`.
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

- In `DC_SINGLE` mode, the framebuffer size is `screen_width * screen_height*bit_depth / 8`.
- In `DC_RAMLESS` mode, two partial framebuffers are used, with size `fb_width * fb_height * bit_depth / 8`, where `fb_height` is the segmented height.

### Interface

The following table lists the LCD-related interfaces supported by mainstream  chips.If you want to know more information, please click on the specific chip name.

| SOC                                                              | I8080 | QSPI | RGB | MIPI | SPI |
|:----------------------------------------------------------------:|:-----:|:----:|:---:|:----:|:---:|
| <span style="display:inline-block;width:70px">[RTL8762C]</span>  |   Y   |  NA  |  NA |  NA  |  Y  |
| [RTL8762D]                                                       |   Y   |  Y   |  NA |  NA  |  Y  |
| [RTL8763E]                                                       |   Y   |  Y   |  NA |  NA  |  Y  |
| [RTL8772G]                                                       |   Y   |  Y   |  Y  |  NA  |  Y  |
| [RTL8773E]                                                       |   Y   |  Y   |  Y  |  NA  |  Y  |

```{note}
'Y' means the driver already included in the library.
'NA' means the driver not yet included in the library.
```

### Driver IC

The following table lists the LCD-related driver IC supported by mainstream chips.If you want to know more information, please click on the specific chip name.

| SOC                                                             | EK9716 | ICNA3311 | NT35510 | NV3047 | ST7701S | ST77903 | ST7796 | OTM8009A | SH8601A | SH8601Z | RM69330 | ST7789 | NV3041A |
|:---------------------------------------------------------------:|:------:|:--------:|:-------:|:------:|:-------:|:-------:|:------:|:--------:|:-------:|:-------:|:-------:|:------:|:-------:|
| <span style="display:inline-block;width:70px">[RTL8762D]</span> |   NA   |    NA    |    NA   |   NA   |    NA   |    NA   |   Y    |    NA    |    NA   |    NA   |    Y    |   Y    |    Y    |
| [RTL8763E]                                                      |   NA   |    NA    |    Y    |   NA   |    NA   |    NA   |   NA   |    NA    |    NA   |    Y    |    NA   |   NA   |    NA   |
| [RTL8772G]                                                      |   Y    |    Y     |    Y    |   Y    |    Y    |    Y    |   Y    |    NA    |    NA   |    NA   |    NA   |   NA   |    NA   |
| [RTL8773E]                                                      |   NA   |    NA    |    NA   |   NA   |    NA   |    NA   |   NA   |    NA    |    Y    |    NA   |    NA   |   NA   |    NA   |

```{note}
'Y' means the driver already included in the library.
'NA' means the driver not yet included in the library.
```

## Filesystem

- Refer to `guidef.h` and `gui_port_filesystem.c`
- Define several posix-like interfaces to operate files and directories.
- If not using a filesystem, you can fill in null pointers.
- The structure definition is as follows:

    ```eval_rst
    .. literalinclude:: ../../../gui_engine/widget/guidef.h
        :language: c
        :start-after: /* gui_fs struct define start */
        :end-before: /* gui_fs struct define end */
    ```

## Flash Translation Layer

- Refer to `guidef.h` and `gui_port_ftl.c`
- Define three interfaces for the Flash Translation Layer: `read`, `write`, `erase`.
- If not using a Flash Translation Layer, you can fill in null pointers.
- The structure definition is as follows:

    ```eval_rst
    .. literalinclude:: ../../../gui_engine/widget/guidef.h
        :language: c
        :start-after: /* gui_ftl struct define start */
        :end-before: /* gui_ftl struct define end */
    ```

## Input Device

- Refer to `guidef.h` and `gui_port_indev.c`
- Input devices include touchpads, keyboards, and wheels. The structure for input information is as follows:

    ```eval_rst
    .. literalinclude:: ../../../gui_engine/widget/guidef.h
        :language: c
        :start-after: /* gui_indev struct define start */
        :end-before: /* gui_indev struct define end */
    ```

- If a specific input device is needed, the corresponding data acquisition function needs to be implemented in `gui_indev`, and the required time thresholds need to be filled in.

### Touch IC

The following table lists the Touch-related IC supported by all chips. If you want to know more information, please click on the specific chip name.

| SOC                                                              | CST816S | CHSC6417 | FT3169 | GT911 | ZT2717 | CST816T | GT9147 |
|:----------------------------------------------------------------:|:-------:|:--------:|:------:|:-----:|:------:|:-------:|:------:|
| <span style="display:inline-block;width:70px">[RTL8762D]</span>  |    Y    |    NA    |   NA   |   NA  |   NA   |    NA   |    NA  |
| [RTL8763E]                                                       |    NA   |    NA    |   NA   |   NA  |   NA   |     Y   |     Y  |
| [RTL8772G]                                                       |    NA   |    NA    |   NA   |    Y  |    Y   |    NA   |    NA  |
| [RTL8773E]                                                       |    Y    |    NA    |   NA   |    Y  |   NA   |    NA   |    NA  |

```{note}
'Y' means the driver already included in the library.
'NA' means the driver not yet included in the library.
```

## Operating System

- Refer to `guidef.h` and `gui_port_os.c`
- Define the interfaces for thread, timer, message queue, and memory management. The structure definition is as follows:

    ```eval_rst
    .. literalinclude:: ../../../gui_engine/widget/guidef.h
        :language: c
        :start-after: /* gui_os_api struct define start */
        :end-before: /* gui_os_api struct define end */
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

[RTL8762C]: https://www.realmcu.com/en/Home/Product/93cc0582-3a3f-4ea8-82ea-76c6504e478a
[RTL8762D]: https://www.realmcu.com/en/Home/Product/52feef61-22d0-483e-926f-06eb10e804ca
[RTL8763E]: https://www.realmcu.com/en/Home/Product/eed7a243-66bf-4b5c-b811-a60d2d4e95cf
[RTL8772G]: https://www.realmcu.com/en/Home/Product/c175760b-088e-43d9-86da-1fc9b3f07ec3
[RTL8773E]: https://www.realmcu.com/en/Home/Product/eed7a243-66bf-4b5c-b811-a60d2d4e95cf
