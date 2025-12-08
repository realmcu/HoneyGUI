==========================
使用 LVGL V9 设计
==========================

LVGL 简介
==========================
- `LVGL 官网 <https://lvgl.io/>`_
- `LVGL 在线文档 <https://docs.lvgl.io/9.4/index.html>`_
- `LVGL 简介 <https://docs.lvgl.io/9.4/introduction/index.html>`_

LVGL (Light and Versatile Graphics Library) 是一个免费且开源的 C 语言图形库，提供了一切所需的工具，以创建嵌入式 :term:`GUI` ，具有易于使用的图形元素、美观的视觉效果和低内存占用。

LVGL 提供了“ GUI 引擎”，处理应用程序与最终用户的所有交互，不仅包括绘制显示内容、接受用户输入并将其转化为应用程序可捕获的事件，还提供了 30 多个内置小部件、100 多个样式属性、滚动、动画、先进的软件渲染、从 MCU 到 OpenGL 的内置 GPU 支持等等。这些功能的结合使您能够高效地开发用户界面，而无需重新发明轮子。

LVGL 在其官网上展示了 Demo 效果以体现 LVGL 的 :term:`UI` 构建能力。在线文档是 LVGL 的主要开发资料，其中详细介绍了 LVGL 的设计和运行逻辑、各个控件的使用方法、丰富的示例程序以及移植方法等。无论是新手还是有经验的开发者，都可以基于在线在线文档快速上手并深入理解 LVGL 的功能和特性。

- `LVGL Demo <https://lvgl.io/demos>`_
- `LVGL Example <https://docs.lvgl.io/9.4/examples.html>`_

模拟器
==========================

模拟器是开发 UI 时使用的一个强大的工具，用于在计算机上模拟嵌入式设备的 UI 界面。它可以模拟真实硬件平台的行为和外观，提供给开发人员一个便捷的环境来快速创建、调试和测试 UI 设计。

模拟器的主要作用是实时展示和交互测试设计的 UI 界面，从而减少在实际硬件上进行反复测试的时间和成本。通过使用模拟器，开发人员可以快速迭代设计，实时查看效果，并进行调试和验证。这大大加快了 UI 的开发速度和质量，并提高了工作效率。

使用模拟器有以下优点：

- 实时预览：模拟器可以即时显示 UI 界面的效果，使开发人员能够快速看到设计的外观和功能效果，方便进行调整和修改。

- 跨平台支持：模拟器可以在计算机上运行，开发人员不需要依赖具体的硬件平台。

- 节省时间和资源：使用模拟器可以避免在实际硬件上频繁烧录和测试 UI ，减少了额外的时间和成本开销。

- 调试和测试：模拟器提供了丰富的调试和测试功能，可以检查 UI 元素的交互、事件处理和布局效果，有助于解决问题和优化性能。

.. _在模拟器中运行 LVGL:

在模拟器中运行 LVGL
-----------------------------
LVGL 模拟器基于 scons 工具 和 MinGW-w64 工具链，在 VScode 中运行和进行调试，具体的环境配置和启动运行请参考 :ref:`入门指南`  章节。

完成模拟器的环境安装后，启动运行将看到模拟器默认的 LVGL 工程。修改模拟器配置文件以运行不同的 LVGL 的工程，在路径 :file:`your lvgl dir\\rtk\\win32_sim\\` 下的 :file:`menu_config.h` 文件为模拟器的配置文件，可以通过修改 ``LVGL_DEMO_APP`` 的数值来选择模拟器运行的示例工程。在 VScode 中再次启动运行，构建编译通过后即可看到模拟器运行选定的示例工程。

.. figure:: https://foruda.gitee.com/images/1753343343333452747/20abc72e_9325830.png
   :align: center
   :width: 408

   模拟器配置文件

当需要修改屏幕尺寸时，修改文件 :file:`your lvgl dir\\rtk\\demos\\xxxxxx\\` 下的 :file:`SConscript`
文件，修改其中的屏幕宽度 ``DRV_LCD_WIDTH`` 和 屏幕高度 ``DRV_LCD_HEIGHT``，均为像素单位。

.. figure:: https://foruda.gitee.com/images/1753343552380283109/09329a68_9325830.png
   :align: center
   :width: 408

   模拟器屏幕尺寸配置文件

LVGL 结构
----------------

以下为 LVGL 相关的目录及文件：

::

    LVGL Dir
    |-- demos
    |-- docs
    |-- env_support
    |-- examples
    |-- libs
    |-- rtk
    |  |--demos
    |  |  |-- single_demo
    |  |  |__ screen_410_502_lvgl
    |  |     |--fonts
    |  |     |--images
    |  |     :
    |  |     :
    |  |     |__root_image_lvgl
    |  |        |-- root                         // 文件系统根目录
    |  |        |-- _bin_mkromfs.py
    |  |        |-- mkromfs_0x704D1400.bat       // User Data 打包脚本
    |  |        |-- root(0x704D1400).bin         // 打包的 User Data
    |  |        :
    |  |        |__ ui_resource.h                // 打包的文件资源地址映射
    |  |--tool
    |  |__win32_sim
    |     :
    |     |__ port
    |        |__ lvgl_port                       // 模拟器 LVGL porting
    |           |-- lv_conf.h                    // 模拟器 LVGL 配置定义
    |           |-- lv_port_disp.c
    |           |-- lv_port_disp.h
    |           |-- lv_port_fs.c
    |           |-- lv_port_fs.h
    |           |-- lv_port_indev.c
    |           |__ lv_port_indev.h
    |-- scripts
    |-- src
    |-- tests
    :
    :
    |__ zephyr


1. Realtek SDK 中 LVGL 的文件结构与 LVGLV9.3 正式版本相同

- demos ：存放 LVGL 一些综合的内置示例，部分示例可以在 `LVGL Demo <https://lvgl.io/demos>`_ 中体验。

- docs ：存放 LVGL 的开发文档，可在 LVGL 的文档站点在线阅读：`LVGL Document <https://docs.lvgl.io/master/index.html>`_ 。

- env_support ：一些环境或者平台的支持。

- examples ：存放 LVGL 的内置示例，可在 `LVGL Example <https://docs.lvgl.io/9.4/examples.html>`_ 中体验。

- libs ：存放 LVGL 使用到的的库文件。

- rtk ：存放 Realtek SDK 中 LVGL 相关的文件，包括模拟器相关文件以及示例工程。

- scripts ：存放一些 LVGL 的处理脚本。

- src ：存放 LVGL 实际的源码，使用 LVGL 进行开发时，都是使用这里面的代码文件。RTK 对 LVGL 源码有微量的改动，改动内容全部记录在 :file:`your lvgl dir\\rtk\\` 下的 :file:`LVGL_SourceCode_Changelog.md` 文件中。

- tests ：存放 LVGL 测试文件。

- zephyr ：存放 Zephyr 相关的配置文件。

2. 模拟器运行 LVGL 时， LVGL 将从目录 :file:`your lvgl dir\\rtk\\demos\\xxxxxx\\` 下的 :file:`app_sim_port.c` 开始运行。

3. 模拟器运行 LVGL 时，调用的 LVGL 文件系统接口所指向的根目录为 :file:`your lvgl dir\\rtk\\demos\\xxxxxx\\root\\`。

实机移植
==========================

LVGL 提供了广泛的移植支持，使开发者可以将其轻松地集成到各种嵌入式系统和平台中。它支持各种显示设备的驱动、触摸屏、输入设备和自定义 :term:`GPU` 等。开发者可以根据项目的需求进行移植配置，例如更换显示设备时调整显示参数，替换输入设备时适配输入接口等。本文以显示设备、输入设备和文件系统为例，介绍移植过程和方法，更多细节请参考 `LVGL Integration and Drivers <https://docs.lvgl.io/9.3/details/integration/index.html>`_ 以及 `LVGL Main Modules <https://docs.lvgl.io/9.3/details/main-modules/index.html>`_ 。


.. note::
    以下示例不包含硬件设备驱动的具体实现，仅示例如何将驱动对接到 LVGL 的接口。开发者在实现硬件设备驱动时，可在与示例驱动一致的 :term:`API` 框架下来完成驱动功能，以对接到 driver 层接口，往上则可复用示例工程的 porting 接口。


显示
-----------------------------
- 文档说明： `LVGL Display (lv_display) <https://docs.lvgl.io/9.4/main-modules/display/index.html>`_ 以及 `LVGL Setting Up Your Display(s) <https://docs.lvgl.io/9.4/main-modules/display/setup.html>`_

在开发者完成显示设备的驱动功能调试后，设备能够与显示设备正常通信并显示色彩。本小节介绍如何将驱动与 LVGL 的显示接口进行对接以展现 LVGL 的 UI 界面。

LVGL 的显示接口在文件 :file:`lv_port_disp.c` 中实现，显示参数在初始化函数 ``void lv_port_disp_init(void)`` 中进行配置，如屏幕尺寸和 frame buffer 配置准备等，显示刷新函数为 ``static void disp_flush(lv_display_t *disp_drv, const lv_area_t *area, uint8_t *px_map)``。

LVGL 支持三种显示缓冲区的渲染方式，每种方式适用于不同的内存与性能需求：

- LV_DISPLAY_RENDER_MODE_PARTIAL （分块渲染）

  - 利用较小的缓冲区将屏幕分为若干小部分逐步渲染。
  - 推荐缓冲区大小至少为屏幕的 1/10。
  - 优点：节省 :term:`RAM` ，适合内存有限的场景。

- LV_DISPLAY_RENDER_MODE_DIRECT （直写渲染）

  - 缓冲区尺寸与屏幕相同， LVGL 直接渲染到对应的缓冲区位置。
  - 缓冲区始终包含完整的画面，仅更新发生变化的区域。
  - 如有两个缓冲区，内容会自动同步，刷新时仅需切换缓冲区地址。

- LV_DISPLAY_RENDER_MODE_FULL （全屏重绘）

  - 每次刷新都会重绘整个屏幕，即使只更改了一个像素。
  - 使用双缓冲时，刷新同样只需切换缓冲区地址。
  - 优点：实现简单，适合对刷新速度或兼容性有特殊要求的场合。


根据实际开发需求，根据不同的内存资源以及渲染方式， SDK 中的文件 :file:`lv_port_disp.c` 已配置好了四种推荐的渲染方案供参考，配置 ``ACTIVE_DISPLAY_SCHEME`` 以切换模式：

- ``SCHEME_RAM_PARTIAL`` 区域绘制方案

  - 使用分块渲染方案
  - 配置两块 RAM 区域缓存
  - 适用于没有 :term:`PSRAM` 的 MCU
  - 该方案仅支持带有 RAM 的 display :term:`IC`

- ``SCHEME_RAM_PSRAM_PARTIAL`` 带有整屏缓存的区域绘制方案

  - 使用分块渲染方案
  - 配置两块 RAM 区域缓存以及一块 PSRAM 整屏缓存
  - 该方案仅支持带有 PSRAM 的 MCU
  - 对 display IC 无特殊要求

- ``SCHEME_PSRAM_DIRECT`` 整屏直写方案

  - 使用直写渲染方案
  - 配置两块 PSRAM 区域缓存
  - 该方案仅支持带有 PSRAM 的 MCU
  - 对 display IC 无特殊要求

- ``SCHEME_PSRAM_FULL`` 整屏重绘方案

  - 使用全屏重绘方案
  - 配置两块 PSRAM 区域缓存
  - 该方案仅支持带有 PSRAM 的 MCU
  - 对 display IC 无特殊要求


输入设备
-----------------------------
- 文档说明： `LVGL Input Device (lv_indev) <https://docs.lvgl.io/9.4/main-modules/indev/index.html>`_

在开发者完成输入设备的驱动功能调试后，设备能够与输入设备正常通信。本小节介绍如何将驱动与 LVGL 的输入接口进行对接以与 LVGL 的 UI 界面进行交互。

LVGL 的输入接口在文件 :file:`lv_port_indev.c` 中实现，输入设备参数在初始化函数 ``void lv_port_indev_init(void)`` 中进行配置，如选择设备类型，配置数据读取回调函数，以及指针绑定等。

详尽的输入设备移植方法和注意事项请参阅文档 `LVGL Input Device (lv_indev) <https://docs.lvgl.io/9.4/main-modules/indev/index.html>`_。

- 在初始化函数 ``void lv_port_indev_init(void)`` 中选择注册对应类型的输入设备，如触屏设备则选择 **Touchpad** ， 并选择对应的接口进行实现。

- LVGL 会定期通过回调函数指针 ``indev_drv.read_cb`` 获取当前输入设备的数据，如触屏设备则为函数 ``static void touchpad_read(lv_indev_t *indev_drv, lv_indev_data_t *data)``,该函数不需要修改。

- 开发者需要填充 ``static bool touchpad_is_pressed(void)`` 函数以及 ``static void touchpad_get_xy(int32_t *x, int32_t *y)`` 函数，从触摸板获取当前的状态信息，包括触点的坐标及触摸状态。

.. code-block:: c
   :emphasize-lines: 4,7,8,9,22,52,59,60

    void lv_port_indev_init(void)
    {
      /*Initialize your touchpad if you have*/
      touchpad_init();

      /*Register a touchpad input device*/
      indev_touchpad = lv_indev_create();
      lv_indev_set_type(indev_touchpad, LV_INDEV_TYPE_POINTER);
      lv_indev_set_read_cb(indev_touchpad, touchpad_read);
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
      touch_driver_init();
      /*Your code comes here*/
   }

   /*Will be called by the library to read the touchpad*/
   static void touchpad_read(lv_indev_t *indev_drv, lv_indev_data_t *data)
   {
      static int32_t last_x = 0;
      static int32_t last_y = 0;

      /*Save the pressed coordinates and the state*/
      if (touchpad_is_pressed())
      {
         touchpad_get_xy(&last_x, &last_y);
         data->state = LV_INDEV_STATE_PRESSED;
      }
      else
      {
         data->state = LV_INDEV_STATE_RELEASED;
      }

      /*Set the last pressed coordinates*/
      data->point.x = last_x;
      data->point.y = last_y;
   }

   /*Return true is the touchpad is pressed*/
   static bool touchpad_is_pressed(void)
   {
      /*Your code comes here*/
      return get_raw_touch_data().is_press;
   }

   /*Get the x and y coordinates if the touchpad is pressed*/
   static void touchpad_get_xy(int32_t *x, int32_t *y)
   {
      /*Your code comes here*/
      (*x) = get_raw_touch_data().x;
      (*y) = get_raw_touch_data().y;
   }


文件系统
-----------------------------

- 文档说明： `LVGL File System (lv_fs_drv) <https://docs.lvgl.io/9.4/main-modules/fs.html>`_

LVGL Benchmark 测试
==========================

LVGL 的 Benchmark 是一个性能测试工具，用于评估 LVGL 库在各种硬件和软件环境下的图形显示性能。通过运行 Benchmark ，用户可以获取帧率、渲染速度和内存使用情况等数据，从而帮助优化显示配置和调试性能问题。 Benchmark 包括多种测试场景，如图形绘制、动画和文本渲染，每个场景模拟实际应用中的常见操作。用户可以通过这些测试来比较不同配置和平台的性能表现，从而做出针对性的优化调整。

LVGL 基准测试的官方文档位于 :file:`your lvgl dir\\lvgl\\demos\\README.md`。

参考 Benchmark
-----------------------------

平台背景， 8773G 平台， 200MHz 主频， QSPI 410*502 屏幕， 96KB RAM 搭配 3MB PSRAM ，双 buffer 模式，推屏方式 direct 。

编译环境： armclang6.22 版本，优化方式 -Ofast 开启 LTO ； gcc 12.2 版本，优化方式 -Ofast。

使用1000除以渲染时间（ms），可以得到当前场景下的极限帧率，单位为帧率/秒。

.. csv-table:: Benchmark 不同加速方式测试结果
   :header: 场景, SW 渲染时间(ms), SW+MVE+ARM2D 渲染时间(ms), SW+MVE+ARM2D+PPE 渲染时间(ms)
   :widths: 60 50 50 50
   :align: center

   Empty screen, 4, 4, 4
   Moving wallpaper, 16, 16, 11
   Single rectangle, 0, 0, 0
   Multiple rectangles, 4, 4, 2
   Multiple RGB images, 5, 5, 2
   Multiple ARGB images, 10, 9, 2
   Rotated ARGB images, 11, 12, 0
   Multiple labels, 8, 9, 9
   Screen sized text, 32, 31, 30
   Multiple arcs, 6, 6, 5
   Containers, 6, 6, 6
   Containers with overlay, 27, 21, 24
   Containers with opa, 9, 11, 7
   Containers with opa_layer, 15, 11, 10
   Containers with scrolling, 23, 20, 21
   Widget demo, 31, 29, 30
   All screen, 12, 11, 10


.. csv-table:: Benchmark 不同编译环境测试结果
  :header: 场景, 加速方式, ARMCLANG, GCC
  :widths: 60 50 50 50
  :align: center

   Empty screen, SW+MVE+PPE, 5, 5
   Moving wallpaper, SW+MVE+PPE, 16, 16
   Single rectangle, SW+MVE+PPE, 0, 0
   Multiple rectangles, SW+MVE+PPE, 2, 4
   Multiple RGB images, SW+MVE+PPE, 2, 5
   Multiple ARGB images, SW+MVE+PPE, 3, 3
   Rotated ARGB images, SW+MVE+PPE, 1, 0
   Multiple labels, SW+MVE+PPE, 11, 13
   Screen sized text, SW+MVE+PPE, 38, 37
   Multiple arcs, SW+MVE+PPE, 6, 8
   Containers, SW+MVE+PPE, 6, 6
   Containers with overlay, SW+MVE+PPE, 24, 25
   Containers with opa, SW+MVE+PPE, 6, 6
   Containers with opa_layer, SW+MVE+PPE, 11, 10
   Containers with scrolling, SW+MVE+PPE, 21, 22
   Widget demo, SW+MVE+PPE, 30, 34
   All screen, SW+MVE+PPE, 11, 12

.. csv-table:: 不同平台渲染加速
  :header: 芯片型号, 处理器主频, 硬件加速器, 填充, 边框, 阴影, 字符, 标签, 图像, 图层, 线条, 圆弧, 三角形, 矩形蒙版, 矢量, 3D
  :align: center

  RTL8772G, 125MHz, PPE1.0, HW, SW, SW, SW, SW, HW, HW, SW, SW, SW, SW, SW, SW
  RTL8773E, 100MHz, PPE2.0, HW, SW, SW, SW, SW, HW, HW, SW, SW, SW, SW, SW, SW
  RTL8773G, 200MHz, PPE2.2, HW, HW, HW, SW, HW, HW, HW, SW, SW, SW, SW, SW, SW


.. note::
  1. 涉及 LVGL Mask 的效果均需要 SW 处理
  2. RTL8772G 支持 Helium 硬件加速器


从 Demo 入门开发
==========================

LVGL 原生 Demo
-----------------

- `LVGL Demo <https://lvgl.io/demos>`_
- `LVGL Example <https://docs.lvgl.io/9.4/examples.html>`_

建议开发者开发前先行阅读理解 `LVGL Introduction <https://docs.lvgl.io/9.4/introduction/index.html>`_ 和 `LVGL Widget Basics <https://docs.lvgl.io/9.4/common-widget-features/index.html>`_ 部分以了解 LVGL 的设计概念和设计逻辑。

LVGL 提供了丰富的 demo 和 example 来帮助开发者了解熟悉各个控件和特性的使用。

- `LVGL Demo <https://lvgl.io/demos>`_ 中展示了综合性比较强的 Demo ，其源码保存在目录 :file:`your lvgl dir\\lvgl\\src\\demo` 下，开发者可直接调用对应的 ``lv_demo_xxx()`` 函数来熟悉了解。

- 在线文档 `LVGL Example <https://docs.lvgl.io/9.4/examples.html>`_ 中展示了各个 example 的运行效果，其源码保存在目录 :file:`your lvgl dir/example` 下，开发者可直接调用对应的 ``lv_example_xxx()`` 函数来熟悉控件和理解特性。

RTK 扩展 Demo
-----------------

RTK 基于 LVGL 已有框架，将部分常用的 UI 组件设计封装为了标准的 LVGL 控件或者控件扩展模块，开发者可以直接调用这些控件与模块来快速搭建 UI 界面。

为了方便开发者快速了解 RTK 的扩展控件， RTK 对扩展功能制作了独立 Demo ， 并尽可能使 Demo 足够精简并且便于理解。

RTK 扩展 Demo 入口函数位于 :file:`your lvgl dir\\rtk\\demos\\single_demo\\ui_init.c` , 其中包含了部分 LVGL 原生 Demo 以及 RTK 扩展 Demo ， 用户可以选择其中任意一个 Demo 执行并查看效果。

所有扩展 Demo 均可使用模拟器运行，开发者可直接在模拟器中查看 Demo 的运行效果，也可以进行修改并快速查看修改后的效果差异。

RTK 扩展 Demo 源码保存在目录 :file:`your lvgl dir\\rtk\\demos\\single_demo\\xxx\\rtk_demo_xxx.c` 下，开发者可直接调用对应的 ``rtk_demo_xxx()`` 函数来熟悉了解。


卡片示例
~~~~~~~~~~~~

用户可以使用 ``lv_card_view_create`` 函数创建 card view 容器控件，其子控件只能是 card 控件，该函数可以配置显示风格，子控件 card 的高度、间隔和总数量以及设计函数，通过用户自定义的设计函数完成对 card 控件中 UI 内容的设计，支持动态创建即 card 控件在进入显示区域时才会被创建并调用设计函数，退出显示区域就会被清理。

用户可以使用 ``lv_card_view_set_number`` 函数设置 card view 控件的子控件 card 的数量。

用户可以使用 ``lv_card_view_set_offset`` 函数设置 card view 控件的偏移量。

.. raw:: html

   <br>
   <div style="text-align: center">
   <img src="https://foruda.gitee.com/images/1757557061445981954/fe093ccb_9325830.gif" width="502" />
   </div>
   <br>


蜂窝列表示例
~~~~~~~~~~~~~~~~

用户可以使用 ``lv_cellular_create_with_icon`` 函数创建 cellular 容器控件，需要提供图像描述指针数组以及回调函数指针数组。

用户可以使用 ``lv_cellular_set_offset`` 函数设置 cellular 控件的偏移量。

.. raw:: html

   <br>
   <div style="text-align: center">
   <img src="https://foruda.gitee.com/images/1757557520907101802/95926e8f_9325830.gif" width="502" />
   </div>
   <br>


平铺视图转场示例
~~~~~~~~~~~~~~~~~~~

.. raw:: html

   <br>
   <div style="text-align: center">
   <img src="https://foruda.gitee.com/images/1757557892430171667/0183d5bd_9325830.gif" width="502" />
   </div>
   <br>

平铺视图（ tileview ）是一个容器小部件，其子元素为瓦片（ tile ），可以以网格形式排列。用户可以通过滑动在瓦片之间导航。如果平铺视图是屏幕大小，用户界面类似智能手表上的界面。

RTK 扩展 Demo 中包含了一个平铺视图转场示例，该示例创建了一组平铺试图，并对每个瓦片增加了独立的转场效果，例如缩放、淡入淡出等。

与原生 LVGL 平铺视图不同，本 Demo 中的平铺视图转场过程支持跟手拖拽滑动，更加符合用户的操作习惯。

如果想要使用平铺转场效果，开发者需要对每个瓦片设置转场效果，并对平铺试图增加转场事件回调。

转场效果以及回调函数在软件包中已经封装完毕，可以直接使用，源码保存在 :file:`lv_custom_tileview_slide.c` 中。

平铺视图转场示例的完整代码请参考 :file:`rtk_demo_tileview_slide.c` 中的 ``rtk_demo_tileview_slide()`` 函数，此处提供一个精简版本与上文介绍对应理解：

.. code-block:: c
   :emphasize-lines: 14,15,16

   tileview_slide_t slide_info;
   SLIDE_EFFECT center_effect = CLASSIC;
   SLIDE_EFFECT right_effect = SCALE;
   ......

   void rtk_demo_tileview_slide_snapshot(void)
   {
      lv_obj_t *tv = lv_tileview_create(lv_screen_active());
      ......
      lv_obj_t *tile_center = lv_tileview_add_tile(tv, 1, 1, LV_DIR_VER);
      lv_obj_t *tile_right = lv_tileview_add_tile(tv, 2, 1, LV_DIR_VER);
      ......

      lv_obj_set_user_data(tile_center, &center_effect);
      lv_obj_set_user_data(tile_right, &right_effect);
      lv_obj_add_event_cb(tv, tileview_custom_cb, LV_EVENT_ALL, &slide_info);
   }


带有 2.5D 转场特效和快照缓存机制的平铺试图转场示例
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. raw:: html

   <br>
   <div style="text-align: center">
   <img src="https://foruda.gitee.com/images/1757559644406993741/db4f4ff2_9325830.gif" width="502" />
   </div>
   <br>

本示例与上一个实例创建了一组相同的平铺试图，并额外增加了 2.5D 转场特效与快照缓存机制。

2.5D 转场特效目前支持盒、旋转、立方体三种效果，开发者可以参考 :file:`lv_custom_tileview_slide.c` 的已有特效代码继续扩充。

如果想要使用 2.5D 转场效果，需要对每个瓦片设置转场效果，并使能 LVGL 中的配置项 ``LV_DRAW_TRANSFORM_USE_MATRIX`` 。

启用快照机制，需要开发者在每个瓦片下创建一个控件，用来管理快照缓存图像和实际的页面内容，并且新增两个快照事件，用来响应转场过程中快照的创建和销毁。

开发者可以参考 :file:`rtk_demo_tileview_slide.c` 中的 ``rtk_demo_tileview_slide_snapshot()`` 函数，进行二次开发，此处提供一个精简版本与上文介绍对应理解：

.. code-block:: c

   tileview_slide_t slide_info;
   ......
   SLIDE_EFFECT right2_effect = BOX;
   SLIDE_EFFECT right3_effect = CUBE_ROTATION;
   SLIDE_EFFECT right4_effect = ROTATION;

   static tile_info_t tile_cfg[] =
   {
      ......
      , {3, 1, LV_DIR_HOR,   LV_PALETTE_AMBER,  "Right2", &right2_effect}
      , {4, 1, LV_DIR_HOR,   LV_PALETTE_ORANGE, "Right3", &right3_effect}
      , {5, 1, LV_DIR_LEFT,  LV_PALETTE_BROWN,  "Right4", &right4_effect}
   };

   static lv_obj_t *snapshot_tile_base_create(lv_obj_t *tile, uint32_t event_create,
                                             uint32_t event_delete)
   {
      // Mandatory: Create the src_tile object and snapshot
      lv_obj_t *src_tile = lv_obj_create(tile);
      lv_obj_remove_style_all(src_tile);
      lv_obj_set_size(src_tile, LV_PCT(100), LV_PCT(100));
      create_snapshot_obj_with_enent(tile, tile, event_create, event_delete);
      return src_tile;
   }

   static void create_snapshot_tile_content(lv_obj_t *tile, lv_palette_t palette, const char *text,
                                          uint32_t event_create, uint32_t event_delete)
   {
      lv_obj_t *src_tile = snapshot_tile_base_create(tile, event_create, event_delete);
      ......
   }

   static inline void bind_tile_effect(lv_obj_t *tile, SLIDE_EFFECT *effect)
   {
      // Mandatory: Always associate effect data with tile user data.
      lv_obj_set_user_data(tile, effect);
   }

   void rtk_demo_tileview_slide_snapshot(void)
   {
      event_snapshot_creat  = lv_event_register_id();
      event_snapshot_delete = lv_event_register_id();

      slide_info.scrolling = false;
      slide_info.snapshot = true;
      slide_info.create_snapshot = event_snapshot_creat;
      slide_info.delete_snapshot = event_snapshot_delete;

      lv_obj_t *tv = lv_tileview_create(lv_screen_active());
      ......

      for (size_t i = 0; i < sizeof(tile_cfg) / sizeof(tile_cfg[0]); i++)
      {
         lv_obj_t *tile = lv_tileview_add_tile(tv, tile_cfg[i].col, tile_cfg[i].row, tile_cfg[i].dir);
         bind_tile_effect(tile, tile_cfg[i].effect);
         create_snapshot_tile_content(tile, tile_cfg[i].palette, tile_cfg[i].text, event_snapshot_creat,
                                       event_snapshot_delete);
      }

      lv_tileview_set_tile_by_index(tv, 1, 1, LV_ANIM_OFF);
      lv_obj_add_event_cb(tv, tileview_custom_cb, LV_EVENT_ALL, &slide_info);
   }


.. note::
  1. 2.5D 转场特效需要支持矩阵绘制的 LVGL 版本以及支持矩阵绘制的硬件平台
  2. 使用 2.5D 转场特效，需要同时启用平铺试图的快照缓存机制
  3. 快照缓存机制可以单独启用，用于优化转场过程中的性能
  4. 快照缓存机制需要较大的内存空间

3D 模型示例
~~~~~~~~~~~~~~~~

LVGL 中集成了 Realtek 自研的 Lite3D 引擎，并封装为控件 ``lv_lite3d``，用户可以该控件加载和渲染 3D 模型。关于 Lite3D 引擎的详细介绍请参考 :ref:`3D 模型 (3D Model)` 。

使用步骤：

  1. 准备 3D 模型的描述文件。
  2. 调用 Lite3D 库中的 ``l3_create_model()`` 函数创建 3D 模型。
  3. 使用 ``l3_set_global_transform()`` 函数对 3D 模型进行全局变换，包括初始化世界坐标系和相机坐标系。
  4. 使用 ``l3_set_face_transform()`` 函数对 3D 模型的不同面进行局部变换。（可选）
  5. 调用 ``lv_lite3d_create()`` 函数创建 ``lv_lite3d`` 控件，并将 3D 模型与该控件进行绑定。
  6. 使用 ``lv_lite3d_set_click_cb()`` 函数为 ``lv_lite3d`` 控件设置点击事件回调函数。（可选）

以下提供了一个圆盘的 3D 模型示例，示例代码保存在 :file:`rtk_demo_lite3d_disc.c` 中：

.. code-block:: c

   void rtk_demo_lite3d_disc(void)
   {
   #if LV_DRAW_TRANSFORM_USE_MATRIX != 1
      LV_LOG_WARN("It's recommended to enable LV_DRAW_TRANSFORM_USE_MATRIX for 3D");
   #endif
      lv_obj_t *screen = lv_scr_act();
      lv_obj_set_style_bg_color(screen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);

      l3_model_t *disc_3d = l3_create_model((void *)_acdesc_disc, L3_DRAW_FRONT_ONLY, 15, 0,
                                             DISC_MODEL_WIDTH, DISC_MODEL_HEIGHT);
      l3_set_global_transform(disc_3d, (l3_global_transform_cb)disc_global_cb);
      l3_set_face_transform(disc_3d, (l3_face_transform_cb)disc_face_cb);
      lv_obj_t *lite3d_disc = lv_lite3d_create(screen, disc_3d);

      l3_model_t *disc_cube = l3_create_model((void *)_acdesc_disc_cube, L3_DRAW_FRONT_AND_SORT, 15, 0,
                                             DISC_MODEL_WIDTH,
                                             DISC_MODEL_HEIGHT);
      l3_set_global_transform(disc_cube, (l3_global_transform_cb)disc_global_cb);
      l3_set_face_transform(disc_cube, (l3_face_transform_cb)disc_cube_face_cb);
      lv_obj_t *lite3d_disc_cube = lv_lite3d_create(screen, disc_cube);

      lv_lite3d_set_click_cb(lite3d_disc, disc_click_cb);
      lv_lite3d_set_click_cb(lite3d_disc_cube, disc_click_cb);
      lv_timer_t *timer = lv_timer_create(update_disc_animation, 16, lite3d_disc);
   }

.. raw:: html

   <br>
   <div style="text-align: center">
   <img src="https://docs.realmcu.com/HoneyGUI/image/Lite3D/lvgl_disc.gif" width="502" />
   </div>
   <br>

.. note::
   需要使能 LVGL 中的配置项 ``LV_DRAW_TRANSFORM_USE_MATRIX``。

.. _资源转换器:

资源转换器
==========================
.. <!-- （Img + font，介绍和演示） -->

LVGL 的图片和字库需要借助工具转换为 LVGL 可以识别的格式，才能在 UI 中使用。LVGL 支持转换为 C 数组格式和 bin 文件的资源：

1. C 数组格式
  资源（图片/字库）会被转换为 C 语言数组。这些数组作为代码的一部分，被编译进主应用程序二进制文件。

  - 每当程序逻辑发生变化时，这些资源文件都会随之重新编译。
  - 所有资源的大小都会计入 :term:`APP` 镜像，即 APP image ，因此整体空间需求更大（ OTA 时需要更大空间）。

2. bin 文件格式
  资源会被保存为独立的二进制文件，不参与编译过程。

  - 这些文件独立存储和访问，通常需要文件系统等支持。
  - 程序运行时通过文件系统 API 将资源加载到内存中，消耗一定的 RAM 空间。


媒体资源转换工具
-----------------------------

LVGL 在线图片转换工具
~~~~~~~~~~~~~~~~~~~~~~~

- 在线转换工具： `LVGL Image Converter <https://lvgl.io/tools/imageconverter>`_
- 文档说明： `LVGL Images (lv_image) <https://docs.lvgl.io/9.4/main-modules/images/index.html>`_

LVGL 在线图片转换工具为 LVGL 开发团队提供的在线网站，支持上传本地的图片文件，转换输出标准 C 文件，其中以变量方式描述了图片信息。使用步骤请参考 `LVGL Images (lv_image) - Online Converter <https://docs.lvgl.io/9.4/main-modules/images/adding_images.html#online-converter>`_：

1. 选择 LVGL 版本 ``LVGL v9``
2. 选取本地图片文件，支持多选批量转换

   输出 C 文件将与输入文件同名，图片描述变量也会与输入文件同名，因此输入文件名应避免使用中文或其他非法字符。
3. 选择输出文件的颜色格式

   颜色格式的说明请参考 `LVGL Images (lv_image) - Color Format <https://docs.lvgl.io/9.4/main-modules/images/color_formats.html>`_
4. 点击 :guilabel:`Convert` 获取输出文件

在文档 `LVGL Images (lv_image) <https://docs.lvgl.io/9.4/main-modules/images/index.html>`_ 中详细介绍了如何在 LVGL 中使用图片资源和图片转换工具，并提供了简单的使用范例。


LVGL v9 的在线图片转换工具仅支持输出 C 文件格式和有限的颜色格式输出，如需图片压缩、bin 文件和其他的颜色格式输出，请使用 `python 转换脚本 <https://github.com/lvgl/lvgl/blob/master/scripts/LVGLImage.py>`_。
输出 bin 文件格式的图片资源时， bin 文件中数据的存储格式为 ``12 Byte lv_img_header_t + data``, 其中 ``lv_img_header_t`` 中包含有 ``Color format``, ``width`` 和 ``height`` 等信息。

.. code-block:: c

   typedef struct {
       uint32_t magic: 8;          /**< Magic number. Must be LV_IMAGE_HEADER_MAGIC*/
       uint32_t cf : 8;            /**< Color format: See `lv_color_format_t`*/
       uint32_t flags: 16;         /**< Image flags, see `lv_image_flags_t`*/

       uint32_t w: 16;
       uint32_t h: 16;
       uint32_t stride: 16;        /**< Number of bytes in a row*/
       uint32_t reserved_2: 16;    /**< Reserved to be used later*/
   } lv_image_header_t;



Realtek 媒体资源转换工具
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

由 Realtek 开发的资源转换工具 Media Convert Tool ：
   - 集成了 LVGL v9 的 `python 转换脚本 <https://github.com/lvgl/lvgl/blob/master/scripts/LVGLImage.py>`_ ，完整功能引出
   - 支持 JPEG 格式输出，适配硬件解码器
   - 支持 RTZIP 压缩格式输出，适配硬件解码
   - 支持 AVI 视频格式输出，适配硬件解码
   - 支持对文件夹下批量配置，支持单个文件指定配置
   - 支持 User Data 打包，支持链接外部文件夹

Media Convert Tool 在转换生成 bin 文件的同时，还将生成一份资源描述文件 :file:`ui_resource.c`，其中包含了各图片、视频的描述变量，变量类型为 ``lv_image_dsc_t``。对于支持 flash 直接寻址的平台，如 Nor Flash 等，资源 bin 文件下载到 flash 后，通过描述变量直接引用图片，跳过文件系统，可提升效率，减少缓存，降低系统复杂度。

.. code-block:: c

   /**
    * Struct to describe a constant image resource.
    * It's similar to lv_draw_buf_t, but the data is constant.
    */
    typedef struct {
       lv_image_header_t header;   /**< A header describing the basics of the image*/
       uint32_t data_size;         /**< Size of the image in bytes*/
       const uint8_t * data;       /**< Pointer to the data of the image*/
       const void * reserved;      /**< A reserved field to make it has same size as lv_draw_buf_t*/
    } lv_image_dsc_t;


.. note::
   1. 转换工具将使用输入文件名组成变量名，请避免在输入文件名中使用中文等非法字符；
   2. :file:`ui_resource.c` 的使用依赖于打包过程生成的 :file:`ui_resource.h`

LVGL
^^^^^^^^^

该转换功能完全由原生的 LVGL 转换脚本支持，输入文件仅支持 :term:`PNG` 格式图片，支持输出 C 文件和 bin 文件，支持多种颜色格式。输出 bin 文件格式的图片资源时， bin 文件中数据的存储格式为 ``12 Byte lv_img_header_t + data``, 其中 ``lv_img_header_t`` 中包含有 ``Color format``, ``width`` 和 ``height`` 等信息。在文档 `LVGL Images (lv_image) <https://docs.lvgl.io/9.4/main-modules/images/index.html#>`_ 中详细介绍了如何在 LVGL 中使用图片资源和图片转换工具，并提供了简单的使用范例。

.. figure:: https://foruda.gitee.com/images/1753864909573136949/b12eb86a_9218678.png
   :align: center
   :width: 408

   LVGL 图片转换配置界面



RTZIP
^^^^^^^^^
将图片编码转换为 RTZIP 格式，支持多种压缩算法和自定义压缩编码参数。 RTZIP 格式是一种无损压缩，支持对透明图层编码。

Tips:
   - 若输入图片带透明但输出格式不带透明度信息，建议使能 :guilabel:`Mix Alpha Channel`，如输入 PNG 图片，输出 ``RGB565``。

.. figure:: https://foruda.gitee.com/images/1753864936117526804/e64c6628_9218678.png
   :align: center
   :width: 408

   RTZIP 图片转换配置界面


JPG
^^^^^^^^^
将图片重新编码转换为 JPEG 格式，配置图片采样比和图片质量以压缩图片体积。 JPEG 格式是一种能很好地平衡显示画质和文件大小的有损压缩，转换为 JPEG 的图片将丢失透明度信息。


.. figure:: https://foruda.gitee.com/images/1765162484284729238/8b18ba3d_9218678.png
   :align: center
   :width: 408

   JPEG 图片转换配置界面



AVI
^^^^^^^^^
将图片序列或视频文件转换为 AVI 格式，支持帧率和画质设定，不带音频轨。

- 输入图片序列文件夹时，将转换配置应用于文件夹，务必保证图片序列的命名格式为类 ``xxx_%04d.xxx``，从 0 排序，如 frame_0000.jpg , frame_0001.jpg 。
- 输入视频文件时，对视频文件应用转换配置。

.. figure:: https://foruda.gitee.com/images/1753864947902502287/d16f5802_9218678.png
   :align: center
   :width: 408

   AVI 视频转换配置界面


以下为使用示例：

.. code-block:: c

   // lv_conf.h
   /** RTK AVI decoder.
    *  Dependencies: JPEG decoder. */
   #define LV_USE_AVI  1

   // example
   lv_obj_t * video = lv_avi_create(lv_screen_active(), NULL);

   /* From variable */
   lv_avi_set_src(video, &my_avi_dsc);




字体转换器
-----------------------------

LVGL 在线转换工具
~~~~~~~~~~~~~~~~~~~~~~~

- 在线转换工具：`LVGL Font Converter <https://lvgl.io/tools/fontconverter>`_

- 文档说明：`LVGL Font (lv_font) <https://docs.lvgl.io/9.4/main-modules/fonts/index.html>`_

使用步骤请参考  `LVGL Frequently Asked Questions - How to use the font converter? <https://lvgl.io/tools/fontconverter>`_ ：

1. 设定输出字库的名字

2. 设定字号 Size ， 单位是像素高度

3. 设定字体的 bpp(bit-per-piel)

   - 表示采用多少个 bit 来描述一个像素，当数值越大时，字符的抗锯齿效果越好，边缘越平滑，字库占用空间越大

4. 填写字体回调，当字体文件中不包含所需字符时，调用回调字体进行渲染，可以置空

5. 选择输出字库的类型 (C file / Binary)

6. 选择字体文件 (TTF/WOFF)

7. 设定需要转换的字符 Unicode 范围，也可直接列出需要转换的字符


Realtek 字体转换器
~~~~~~~~~~~~~~~~~~~~

Realtek 在封装和扩展 LVGL 字体转换工具的基础上，推出了全新升级的 Realtek 字体转换器。相较于原版工具， Realtek 字体转换器具备以下显著优势：

1. 可视化操作界面，让字体转换过程一目了然，操作便捷高效

2. 支持多字体、多字号及配置标签化设计，大幅提升工程开发的灵活性与效率

3. 新增对 Realtek 平台 GPU 渲染的特性支持，充分发挥硬件加速能力

4. 支持字体数据与描述信息的分离，更好地适配嵌入式平台的存储管理模式

5. 支持配置方案的导入与导出，便于项目移植与团队协作

下图为 Realtek 字体转换器的界面图：

.. figure:: https://foruda.gitee.com/images/1753432503991458121/2c546aae_9325830.png
   :align: center
   :width: 983

   Realtek 字体转换器界面


字体转换步骤：

1. 打开 Realtek 字体转换器界面

2. 点击 " Add new setting " 按钮，新增一个配置标签页

3. 点击 " Add Fonts " 按钮，添加字体文件，可添加多个

4. 填写 bpp 、字号、字符集等配置项，其中字号可以填写多个，使用英文逗号隔开

  - bpp 为 3 时，生成的字体文件无法使用 GPU 加速渲染
  - 选择压缩选项后，生成的字体文件会进行压缩，占用空间更小，但是无法使用 GPU 加速渲染

5. 点击 " Generate Font " 按钮，选择路径并生成当前配置标签页的字体文件

6. 如果存在多个标签页，点击 " Brouse " 按钮，选择字体文件输出路径，点击 " Generate All " 按钮，生成所有配置标签页的字体文件到指定路径

附加功能：

- 支持批量生成字体文件，每个标签页的字体文件名称为 "字体名称_字号_配置项"

- 支持配置标签页的复制、删除、重命名

- 支持导出当前配置标签页的配置信息，导出为 JSON 格式

- 支持导入配置信息，导入 JSON 格式的配置文件

开发资源支持
==========================

在线文档
---------
- `LVGL Document <https://docs.lvgl.io/master/index.html>`_

LVGL 的 `在线文档 <https://docs.lvgl.io/master/index.html>`_ 提供了全面的技术文档和教程，帮助开发者更好地了解和使用 LVGL 图形库。该文档包含以下内容：

- 概述和特性：文档介绍了 LVGL 的基本概念和特性，包括图形对象、屏幕管理、事件处理、主题样式等。用户可以通过阅读文档了解 LVGL 的核心功能和优势。

- 应用开发指南：文档提供了详细的应用开发指南，包括如何初始化和配置 LVGL 、如何创建和管理图形对象、如何处理用户输入和事件、如何添加主题和样式等。这些指南可以帮助用户快速上手使用LVGL并开发自己的应用程序。

- API 文档：文档详细列举了 LVGL 的 API 接口和函数，以及它们的参数和用法。用户可以根据需要查阅 API 文档来了解具体的函数和接口的功能和用法，以便进行更高级的自定义和扩展。

- 示例代码：文档中提供了众多的示例代码，涵盖了常见的应用场景和功能。用户可以借鉴这些示例代码，加快开发速度，并快速实现特定功能的需求。

使用 LVGL 的在线文档可以帮助用户更好地理解和掌握 LVGL 的使用方法和技巧，提高开发效率。用户可以通过逐步学习文档中的内容，从简单的界面构建到复杂的应用开发，逐步掌握 LVGL 的各种功能和特性。同时，文档还提供了示例和代码片段，方便用户更快地开发出具有丰富界面和功能的应用程序。

用户可以通过在网页浏览器中打开 LVGL 的在线文档，并浏览各个章节和内容，根据自己的需要查找和学习相关的知识。另外，用户还可以通过搜索功能来快速查找文档中的具体信息。总之， LVGL 的在线文档是用户理解和使用 LVGL 图形库的重要资源，可以提供全面而详细的指导，帮助用户快速上手和开发出更好的应用程序。


基于文档开发能够完成大部分的 UI 效果，值得注意的是，文档内容并不一定齐全，当文档内容存在疏漏时，最终还是以代码为准。


Github 仓库
-----------------------------
- `Github LVGL <https://github.com/lvgl/lvgl>`_

LVGL 的 GitHub 仓库是开发者使用和贡献 LVGL 的重要平台：

- 获取最新版本： LVGL 的 GitHub 仓库可以获得最新的 LVGL 版本和更新。开发者可以及时获取最新的功能更新、修复和改进，保持应用程序与 LVGL 的同步。

- 参与社区和贡献代码：通过 GitHub 仓库，开发者可以积极参与 LVGL 社区的讨论和交流，了解其他开发者的问题和解决方案。同时，开发者也可以贡献自己的代码和改进，让 LVGL 更加完善和强大。

- 提交问题和 bug 报告： GitHub 仓库提供了问题和 bug 报告的平台，开发者可以提交他们在使用 LVGL 过程中遇到的问题和 bug。这有助于 LVGL 开发团队及时发现和解决问题，提高 LVGL 的稳定性和可靠性。

- 学习示例和文档： GitHub 仓库中还包含示例代码和文档，帮助开发者更好地理解和学习 LVGL 的使用。开发者可以通过浏览仓库中的示例代码和文档，学习 LVGL 的各个功能和特性，提高开发技能。


设计器
-----------------------------
.. <!-- - 什么是设计器（演示，放图）
.. 辅助开发，是否需要付费，移植，限制 -->

Squareline Studio
~~~~~~~~~~~~~~~~~~~~~~~

`Squareline Studio <https://squareline.io/>`_ 是一款新一代可视化 UI 编辑器，专为嵌入式和桌面应用快速开发美观界面，支持个人免费使用，企业灵活付费。它集设计、原型和开发于一体，通过拖拽方式生成适用于 LVGL 的平台无关 C 或 MicroPython 代码，可在任意设备和系统上运行。支持即点即试的像素级预览、自定义组件、动画和事件，极大提升开发效率。适合个人、初创及大型企业多种场景。

UI设计
^^^^^^^^^

使用 Squareline Studio 进行 LVGL 界面设计，可以参考设计工具的 `官方文档 <https://docs.squareline.io/docs/squareline/>`_ 与软件内的开源示例。

Realtek 有对 Squareline Studio 做过完整支持，基于 Realtek 平台的软硬件性能以及 LVGL 库的设计特性，总结了以下几点设计优化原则：

- 减少不必要的圆角设计，例如矩形阴影绘制效率远高于圆角矩形阴影绘制

- 在保证显示内容一致时，减少冗余的图层叠加和背景色填充

- Realtek 平台支持硬件图像解压（ IDU 以及 JPU ），相比于图形绘制，优先使用图像进行 UI 设计，会提高显示帧率

- 为了适配 Realtek 平台的硬件规则，需要使用 Realtek 的资源转换器对资源进行转换，为了便于移植，建议将所有图像和字体设计文件放到单一主目录下，并确保文件名格式的规范性


资源转换
^^^^^^^^^
Squareline Studio 内置了标准的 LVGL 资源转换器，可以生成符合 LVGL 格式的资源文件，并且可以在模拟器以及开发板上编译运行。

由于 Squaremline Studio 使用的 LVGL 资源转换器并不包括 Realtek 平台的图像压缩算法，以及字体转换功能，所以如果客户需要使用 Realtek 平台的硬件解压功能以及 GPU 字体渲染功能，需要使用 Realtek 资源转换器进行转换，然后替换原始文件。

.. note::
   Realtek 资源转换器的使用方法可以参考 :ref:`资源转换器`。

.. note::
   Realtek 资源转换器输出的文件遵守 LVGL 的格式标准，保证直接使用，因此开发者可以直接将 Realtek 资源转换器生成的文件替换 Squareline Studio 生成的资源文件。

Squareline Studio 在导出 UI 设计文件时，可能会修改输出的资源名称，例如：

 - 会强制给图像资源增加前后缀，前缀为原始图像相对路径以及类型名，后缀为原始图像格式

 - 当原始图像名包含非法字符时，会使用随机数字字符串替换原始文件名

 - 当原始字体文件为中文时，会使用对应的拼音或者缩写来生成字体名称

图像资源替换过程中，绝大部分的命名差异可以通过查找替换方式进行调整，当出现异常文件名时，需要手动进行调整。

字体资源替换过程中，由于字体设置的复杂性和多样性，需要手动进行文件匹配。


工程移植
^^^^^^^^^

Squareline Studio 设计的 UI 界面可以直接导出为 Realtek 平台的 C 代码，开发者可以直接将代码导入 Realtek 平台进行编译和调试。

如何在模拟器中运行 Squareline Studio 设计的 UI 界面？

- 首先需要配置好模拟器环境并成功运行 LVGL 的 示例工程，参考 :ref:`在模拟器中运行 LVGL`

- 然后需要将 Squareline Studio 设计的 UI 界面导出为 C 代码以及资源包，并拷贝至 :file:`your lvgl dir\\rtk\\demos\\benchmark`

  常见的 Squareline Studio 导出的文件包括：

  ::

    UI
    |-- components             // 组件设计
    |-- fonts                  // 字体资源
    |-- images                 // 图像资源
    |-- screens                // 页面设计
    |-- CMakeLists.txt
    |-- filelist.txt
    |-- ui.c                   // 入口文件
    |-- ui.h
    |-- ui_events.h
    |-- ui_helpers.c           // 辅助函数
    |-- ui_helpers.h
    |-- ui_theme_manager.c     // 主题管理
    |-- ui_theme_manager.h
    |-- ui_themes.c            // 主题资源
    |-- ui_themes.h

- 如果需要使用图像压缩功能或者 GPU 字体渲染功能，需要使用 Realtek 资源转换器进行转换，然后替换原始文件，并替换 UI 设计文件中的图像和字体名称

- 在原始示例工程的 LVGL 入口文件中通过 :code:`lv_init()` 初始化 LVGL 之后启动 Squareline Studio 工程的 UI 加载函数 :code:`ui_init();`

- 调整示例工程中的 :file:`sconscript` 文件，添加对 Squareline Studio 工程的构建支持

- 使用资源打包工具，将 Squareline Studio 工程的资源文件打包为二进制文件

- 编译运行模拟器，构建编译通过后即可看到模拟器运行 Squareline Studio 示例工程


功能扩展
^^^^^^^^^

Squareline Studio 支持 LVGL 大部分的基础功能，例如 UI 设计、组件设计、动画设计、事件设计等，但欠缺对部分高级功能的支持，例如页面切换的转场机制、外设的逻辑交互、蜂窝表盘等等，因此如果需要使用这些功能，需要手动进行代码编写。

LVGL Editor
~~~~~~~~~~~~~

`LVGL Editor <https://pro.lvgl.io/>`_ 是一款基于免费开源 LVGL 库的专业 GUI 开发工具，支持 Windows、Linux 和 macOS。通过 XML 定义界面，支持自动补全、版本控制、实时预览及 Figma 深度集成，可在线共享和测试 UI。编辑器支持导出为 C 代码、运行时 XML 加载及多种高级组件，并具备 CLI 集成、自动同步样式、数据绑定和事件处理等功能，大大提升了嵌入式 UI 开发效率。


EEZ Studio
~~~~~~~~~~

`EEZ Studio <https://www.envox.eu/studio/studio-introduction/>`_ 支持 LVGL 8.x 和 9.x 版本，允许用户通过拖放和流程图可视化编程，快速设计响应式桌面和嵌入式 GUI 。平台内置丰富模板和示例，适合原型与最终应用开发，支持多平台和多语言。配合 EEZ Flow ，无需编程即可完成复杂逻辑，提升开发效率, 是一个开发 LVGL 项目的高效、开源工具。


GUI Guider
~~~~~~~~~~~~~~~~~
.. <!-- - GUI Guider: `NXP GUI Guider <https://www.nxp.com/design/design-center/software/development-software/gui-guider:GUI-GUIDER>`_ -->


论坛
-----------------------------
- `LVGL Forum <https://forum.lvgl.io/>`_

.. <!-- 开放提问，LVGL 开发者会回复。 -->

LVGL 的官方论坛是一个开发者社区，致力于讨论和分享有关 LVGL 图形库的话题和资源。它提供了一个平台，供开发者之间交流、寻求帮助和分享他们的经验和项目。

LVGL 论坛的一些特点和功能包括：

- 提问和回答：开发者可以在论坛上提出他们在使用 LVGL 时遇到的问题，并获得其他开发者的帮助和回答。这使得论坛成为一个宝贵的知识库，提供了解决问题的经验和技巧。

- 教程和示例：论坛上有许多有用的教程和示例代码，展示了如何使用 LVGL 的不同功能和特性。这些资源对于新手开发者学习和掌握 LVGL 非常有帮助。

- 开发者贡献和项目展示：论坛上的开发者可以分享他们的项目和定制的 LVGL 界面，以及其他开发者可以共享、讨论和参考的贡献。

- 更新和发布通告： LVGL 的开发团队在论坛上发布关于新版本发布和更新的通告和说明。这使得开发者可以及时了解最新功能和改进。

- 社区互动：论坛提供了一个社区互动的平台，开发者可以互相交流、分享和建立联系，加强 LVGL 开发社区的合作和发展。

LVGL 论坛对于使用 LVGL 的开发者来说，是获取支持、解决问题、学习和分享经验的重要资源。


博客
-----------------------------
- `LVGL Blog <https://blog.lvgl.io/>`_

LVGL 的官方博客是一个定期更新的平台，提供关于 LVGL 图形库的最新信息、教程、案例研究和开发者见解。LVGL 的开发团队和社区成员经常在博客上发布有关 LVGL 的各种内容，这些内容可以使开发者更好地了解和使用 LVGL。

LVGL 的博客包含以下内容：

- 更新和新功能介绍：博客上会发布关于 LVGL 最新版本的更新和改进的文章，这些文章介绍了新功能、修复了的问题和性能提升，使开发者可以了解和利用最新的 LVGL 特性。

- 教程和使用指南：博客会提供有关 LVGL 的实用教程和使用指南，涵盖从入门到高级的各种主题。这些教程通常包括示例代码和详细说明，帮助开发者掌握 LVGL 的使用和最佳实践。

- 案例研究和项目展示：博客上会分享一些使用 LVGL 实现的案例研究和项目展示。这些文章介绍了如何使用 LVGL 构建实际应用程序和界面，让开发者从实践中获取灵感和经验。

- 技术深入解析和开发者见解：博客还会涵盖对 LVGL 的深入分析和开发者的见解。这些文章可能探讨 LVGL 的内部工作原理、性能优化技巧、优秀设计实践等方面的主题，给开发者提供更深入的了解和思考。

LVGL 的博客是一个重要的资源，对于 LVGL 的开发者来说是了解和掌握 LVGL 的宝贵来源。通过阅读博客，开发者可以获取到关于 LVGL 最新动态、学习材料和技术见解，帮助他们更好地使用 LVGL 构建出优秀的图形界面。

