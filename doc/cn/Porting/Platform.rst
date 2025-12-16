.. _Platform_Porting_CN:

========
平台移植
========

移植文件位于 ``gui_port`` 文件夹中。需要修改六个文件，其文件名与功能如下。

.. list-table::
   :header-rows: 1

   * - 文件名
     - 功能
   * - ``gui_port_acc.c``
     - 加速
   * - ``gui_port_dc.c``
     - 显示设备
   * - ``gui_port_filesystem.c``
     - 文件系统
   * - ``gui_port_ftl.c``
     - 闪存转换层
   * - ``gui_port_indev.c``
     - 输入设备
   * - ``gui_port_os.c``
     - 操作系统

目前已在 FreeRTOS 、 RT-Thread 和 Windows 上进行了移植，可供参考。

加速
----

- 参考 ``guidef.h`` 和 ``gui_port_acc.c``。
- 需要根据平台型号，定义加速绘制接口，一般是 ``hw_acc_blit`` 或者 ``sw_acc_blit``。
- 结构体定义如下:

  .. code-block:: c

     typedef struct acc_engine
     {
         void (*blit)(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect);
     } acc_engine_t;

显示设备
--------

- 参考 ``guidef.h`` 和 ``gui_port_dc.c``。
- 需要定义屏幕的宽度和高度、帧缓冲区地址和模式、分辨率是否缩放等等，并实现刷新函数，结构体定义参考 ``guidef.h``。
- 一个典型的 ``gui_dispdev`` 结构体初始化声明如下：

  .. code-block:: c

     static struct gui_dispdev dc =
     {
         .bit_depth = DRV_PIXEL_BITS,
         .fb_width = DRV_LCD_WIDTH,
         .fb_height = FB_HEIGHT,
         .screen_width =  DRV_LCD_WIDTH,
         .screen_height = DRV_LCD_HEIGHT,
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

- 在 ``DC_SINGLE`` 模式下，帧缓冲区的大小为 ``screen_width * screen_height * bit_depth / 8``。
- 在 ``DC_RAMLESS`` 模式下，使用了两个部分帧缓冲区，大小为 ``fb_width*fb_height * bit_depth / 8``，此时的 ``fb_height`` 是分段高度。

支持接口类型
------------

以下表格列出了主流芯片支持的与 :term:`LCD` 相关的接口。如果您想了解更多信息，请点击特定芯片的名称。

.. list-table::
   :header-rows: 1
   :widths: 15 10 10 10 10 10

   * - 芯片
     - I8080
     - QSPI
     - RGB
     - MIPI
     - SPI
   * - RTL8762C_
     - Y
     - NA
     - NA
     - NA
     - Y
   * - RTL8762D_
     - Y
     - Y
     - NA
     - NA
     - Y
   * - RTL8763E_
     - Y
     - Y
     - NA
     - NA
     - Y
   * - RTL8772G_
     - Y
     - Y
     - Y
     - NA
     - Y
   * - RTL8773E_
     - Y
     - Y
     - Y
     - NA
     - Y

.. note::

     'Y' 表示驱动程序已包含在库中。
     'NA' 表示驱动程序尚未包含在库中。

已验证屏幕驱动
--------------

以下表格列出了主流芯片支持的与 LCD 相关的驱动 :term:`IC` 。如果您想了解更多信息，请点击特定芯片的名称。

.. list-table::
   :header-rows: 1
   :widths: 15 10 10 10 10 10 10 10 10 10 10 10 10 10

   * - 芯片
     - EK9716
     - ICNA3311
     - NT35510
     - NV3047
     - ST7701S
     - ST77903
     - ST7796
     - OTM8009A
     - SH8601A
     - SH8601Z
     - RM69330
     - ST7789
     - NV3041A
   * - RTL8762D_
     - NA
     - NA
     - NA
     - NA
     - NA
     - NA
     - Y
     - NA
     - NA
     - NA
     - Y
     - Y
     - Y
   * - RTL8763E_
     - NA
     - NA
     - Y
     - NA
     - NA
     - NA
     - NA
     - NA
     - NA
     - Y
     - NA
     - NA
     - NA
   * - RTL8772G_
     - Y
     - Y
     - Y
     - Y
     - Y
     - Y
     - Y
     - NA
     - NA
     - NA
     - NA
     - NA
     - NA
   * - RTL8773E_
     - NA
     - NA
     - NA
     - NA
     - NA
     - NA
     - NA
     - NA
     - Y
     - NA
     - NA
     - NA
     - NA

.. note::

     'Y' 表示驱动程序已包含在库中。
     'NA' 表示驱动程序尚未包含在库中。

文件系统
--------

- 参考 ``guidef.h`` 和 ``gui_port_filesystem.c``
- 需要定义几个类似 posix 风格的接口操作文件和文件夹。
- 不使用文件系统时可以填入空指针。
- 结构体定义如下：

  .. literalinclude:: ../../../realgui/core/def_fs.h
     :language: c
     :start-after: /* gui_fs struct define start */
     :end-before: /* gui_fs struct define end */

闪存转换层
----------

- 参考 ``guidef.h`` 和 ``gui_port_ftl.c``
- 需要定义闪存转换层的三个接口： ``read`` ， ``write`` ， ``erase``。
- 不使用闪存转换层时可以填入空指针。
- 结构体定义如下:

  .. literalinclude:: ../../../realgui/core/def_fs.h
     :language: c
     :start-after: /* gui_ftl struct define start */
     :end-before: /* gui_ftl struct define end */

输入设备
--------

- 参考 ``guidef.h`` 和 ``gui_port_indev.c``
- 输入设备包括触摸板、键盘和滚轮，输入信息的结构体如下：

  .. literalinclude:: ../../../realgui/core/def_input.h
     :language: c
     :start-after: /* gui_indev struct define start */
     :end-before: /* gui_indev struct define end */

- 如果需要某一种输入设备，需要在 ``gui_indev`` 中实现对应的数度获取函数，并填写所需的时间阈值。

触摸芯片
--------

以下表格列出了所有芯片支持的与触摸相关的 IC。如果您想了解更多信息，请点击特定芯片的名称。

.. list-table::
   :header-rows: 1
   :widths: 15 10 10 10 10 10 10 10

   * - 芯片
     - CST816S
     - CHSC6417
     - FT3169
     - GT911
     - ZT2717
     - CST816T
     - GT9147
   * - RTL8762D_
     - Y
     - NA
     - NA
     - NA
     - NA
     - NA
     - NA
   * - RTL8763E_
     - NA
     - NA
     - NA
     - NA
     - NA
     - Y
     - Y
   * - RTL8772G_
     - NA
     - NA
     - NA
     - Y
     - Y
     - NA
     - NA
   * - RTL8773E_
     - Y
     - NA
     - NA
     - Y
     - NA
     - NA
     - NA

.. note::

     'Y' 表示驱动程序已包含在库中。
     'NA' 表示驱动程序尚未包含在库中。

操作系统
--------

- 参考 ``guidef.h`` 和 ``gui_port_os.c``
- 需要定义线程、定时器、消息队列和内存管理的接口，结构体定义如下：

  .. literalinclude:: ../../../realgui/core/def_os.h
     :language: c
     :start-after: /* gui_os_api struct define start */
     :end-before: /* gui_os_api struct define end */

休眠管理
========

为了降低功耗和增加设备的使用时间，支持睡眠（低功耗）模式。

在芯片手册中，这种外设可以关闭的低功耗状态被称为深度低功耗状态（DLPS）。关于 DLPS 的更多信息，可以在 SDK 的相关指导文档中找到。

.. _RTL8762C: https://www.realmcu.com/en/Home/Product/93cc0582-3a3f-4ea8-82ea-76c6504e478a
.. _RTL8762D: https://www.realmcu.com/en/Home/Product/52feef61-22d0-483e-926f-06eb10e804ca
.. _RTL8763E: https://www.realmcu.com/en/Home/Product/eed7a243-66bf-4b5c-b811-a60d2d4e95cf
.. _RTL8772G: https://www.realmcu.com/en/Home/Product/c175760b-088e-43d9-86da-1fc9b3f07ec3
.. _RTL8773E: https://www.realmcu.com/en/Home/Product/eed7a243-66bf-4b5c-b811-a60d2d4e95cf