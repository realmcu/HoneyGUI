.. _Platform_Porting_EN:

=================
Platform Porting
=================

The porting files are located in the ``gui_port`` folder.
Six files need to be modified, with their filenames and functions as follows.

.. list-table::
   :header-rows: 1

   * - Filename
     - Description
   * - ``gui_port_acc.c``
     - Acceleration
   * - ``gui_port_dc.c``
     - Display Device
   * - ``gui_port_filesystem.c``
     - Filesystem
   * - ``gui_port_ftl.c``
     - Flash Translation Layer
   * - ``gui_port_indev.c``
     - Input Device
   * - ``gui_port_os.c``
     - Operating System

Currently, porting has been done on FreeRTOS, RT-Thread, and Windows for reference.

Acceleration
------------

- Refer to ``guidef.h`` and ``gui_port_acc.c``.
- Define the accelerated drawing interface depending on the platform model, generally ``hw_acc_blit`` or ``sw_acc_blit``.
- The structure definition is as follows:

  .. literalinclude:: ../../../realgui/core/def_engine.h
     :language: c
     :start-after: /* acc_engine_t struct define start */
     :end-before: /* acc_engine_t struct define end */

Display Device
--------------

- Refer to ``guidef.h`` and ``gui_port_dc.c``.
- Define the screen width and height, framebuffer address and mode, whether the resolution is scaled, and implement the refresh function.
- The structure definition is as follows:

  .. literalinclude:: ../../../realgui/core/def_fb.h
     :language: c
     :start-after: /* gui_dispdev struct define start */
     :end-before: /* gui_dispdev struct define end */

- A typical ``gui_dispdev`` structure initialization example:

  .. literalinclude:: ../../../win32_sim/port/realgui_port/gui_port_dc.c
     :language: c
     :start-after: /* gui_dispdev example define start */
     :end-before: /* gui_dispdev example define end */

- In ``DC_SINGLE`` mode, the framebuffer size is ``screen_width * screen_height * bit_depth / 8``.
- In ``DC_RAMLESS`` mode, two partial framebuffers are used, with size ``fb_width * fb_height * bit_depth / 8``, where ``fb_height`` is the segmented height.

Supported Interface Types
~~~~~~~~~~~~~~~~~~~~~~~~~

The following table lists the :term:`LCD`-related interfaces supported by mainstream chips. If you want to know more information, please click on the specific chip name.

+-------------------+------+------+------+------+------+
| SOC               | 8080 | QSPI | RGB  | MIPI | SPI  |
+===================+======+======+======+======+======+
| RTL8762C_         |  Y   |  NA  |  NA  |  NA  |  Y   |
+-------------------+------+------+------+------+------+
| RTL8762D_         |  Y   |  Y   |  NA  |  NA  |  Y   |
+-------------------+------+------+------+------+------+
| RTL8763E_         |  Y   |  Y   |  NA  |  NA  |  Y   |
+-------------------+------+------+------+------+------+
| RTL8772G_         |  Y   |  Y   |  Y   |  NA  |  Y   |
+-------------------+------+------+------+------+------+
| RTL8773E_         |  Y   |  Y   |  Y   |  NA  |  Y   |
+-------------------+------+------+------+------+------+

.. note::

    'Y' means the driver is already included in the library.
    'NA' means the driver is not yet included in the library.

Verified Screen Drivers
~~~~~~~~~~~~~~~~~~~~~~~

The following table lists the LCD-related driver :term:`IC` supported by mainstream chips. If you want to know more information, please click on the specific chip name.

+-------------------+------+--------+-------+-------+--------+--------+-------+---------+--------+--------+--------+-------+--------+
| SOC               |EK9716|ICNA3311|NT35510|NV3047 |ST7701S |ST77903 |ST7796 |OTM8009A |SH8601A |SH8601Z |RM69330 |ST7789 |NV3041A |
+===================+======+========+=======+=======+========+========+=======+=========+========+========+========+=======+========+
| RTL8762D_         |  NA  |    NA  |   NA  |  NA   |   NA   |   NA   |   Y   |   NA    |   NA   |   NA   |   Y    |   Y   |   Y    |
+-------------------+------+--------+-------+-------+--------+--------+-------+---------+--------+--------+--------+-------+--------+
| RTL8763E_         |  NA  |    NA  |   Y   |  NA   |   NA   |   NA   |   NA  |   NA    |   NA   |   Y    |   NA   |   NA  |   NA   |
+-------------------+------+--------+-------+-------+--------+--------+-------+---------+--------+--------+--------+-------+--------+
| RTL8772G_         |  Y   |    Y   |   Y   |   Y   |   Y    |   Y    |   Y   |   NA    |   NA   |   NA   |   NA   |   NA  |   NA   |
+-------------------+------+--------+-------+-------+--------+--------+-------+---------+--------+--------+--------+-------+--------+
| RTL8773E_         |  NA  |    NA  |   NA  |  NA   |   NA   |   NA   |   NA  |   NA    |   Y    |   NA   |   NA   |   NA  |   NA   |
+-------------------+------+--------+-------+-------+--------+--------+-------+---------+--------+--------+--------+-------+--------+

.. note::

    'Y' means the driver is already included in the library.
    'NA' means the driver is not yet included in the library.

Filesystem
----------

- Refer to ``guidef.h`` and ``gui_port_filesystem.c``
- Define several posix-like interfaces to operate files and directories.
- If not using a filesystem, you can fill in null pointers.
- The structure definition is as follows:

  .. literalinclude:: ../../../realgui/core/def_fs.h
     :language: c
     :start-after: /* gui_fs struct define start */
     :end-before: /* gui_fs struct define end */

Flash Translation Layer
-----------------------

- Refer to ``guidef.h`` and ``gui_port_ftl.c``
- Define three interfaces for the Flash Translation Layer: ``read``, ``write``, ``erase``.
- If not using a Flash Translation Layer, you can fill in null pointers.
- The structure definition is as follows:

  .. literalinclude:: ../../../realgui/core/def_fs.h
     :language: c
     :start-after: /* gui_ftl struct define start */
     :end-before: /* gui_ftl struct define end */

Input Device
------------

- Refer to ``guidef.h`` and ``gui_port_indev.c``
- Input devices include touchpads, keyboards, and wheels. The structure for input information is as follows:

  .. literalinclude:: ../../../realgui/core/def_input.h
     :language: c
     :start-after: /* gui_indev struct define start */
     :end-before: /* gui_indev struct define end */

- If a specific input device is needed, the corresponding data acquisition function needs to be implemented in ``gui_indev``, and the required time thresholds need to be filled in.

Touch Chips
~~~~~~~~~~~

The following table lists the touch-related IC supported by all chips. If you want to know more information, please click on the specific chip name.

+-------------------+--------+---------+--------+-------+--------+--------+--------+
| SOC               | CST816S| CHSC6417| FT3169 | GT911 | ZT2717 | CST816T| GT9147 |
+===================+========+=========+========+=======+========+========+========+
| RTL8762D_         |  Y     |   NA    |   NA   |  NA   |   NA   |   NA   |   NA   |
+-------------------+--------+---------+--------+-------+--------+--------+--------+
| RTL8763E_         |  NA    |   NA    |   NA   |  NA   |   NA   |   Y    |   Y    |
+-------------------+--------+---------+--------+-------+--------+--------+--------+
| RTL8772G_         |  NA    |   NA    |   NA   |  Y    |   Y    |   NA   |   NA   |
+-------------------+--------+---------+--------+-------+--------+--------+--------+
| RTL8773E_         |  Y     |   NA    |   NA   |  Y    |   NA   |   NA   |   NA   |
+-------------------+--------+---------+--------+-------+--------+--------+--------+

.. note::

    'Y' means the driver is already included in the library.
    'NA' means the driver is not yet included in the library.

Operating System
----------------

- Refer to ``guidef.h`` and ``gui_port_os.c``
- Define the interfaces for thread, timer, message queue, and memory management. The structure definition is as follows:

  .. literalinclude:: ../../../realgui/core/def_os.h
     :language: c
     :start-after: /* gui_os_api struct define start */
     :end-before: /* gui_os_api struct define end */

Sleep Management
----------------

To reduce power consumption and increase the device's usage time, sleep (low power) mode is supported.

In the chip manual, this low power state where peripherals can be turned off is called Deep Low Power State (DLPS). More information about DLPS can be found in the relevant SDK documentation.

.. _RTL8762C: https://www.realmcu.com/en/Home/Product/93cc0582-3a3f-4ea8-82ea-76c6504e478a
.. _RTL8762D: https://www.realmcu.com/en/Home/Product/52feef61-22d0-483e-926f-06eb10e804ca
.. _RTL8763E: https://www.realmcu.com/en/Home/Product/eed7a243-66bf-4b5c-b811-a60d2d4e95cf
.. _RTL8772G: https://www.realmcu.com/en/Home/Product/c175760b-088e-43d9-86da-1fc9b3f07ec3
.. _RTL8773E: https://www.realmcu.com/en/Home/Product/eed7a243-66bf-4b5c-b811-a60d2d4e95cf
