===============================
Use LVGL V9 to Design
===============================

Introduction to LVGL
==========================
- `LVGL Official Website <https://lvgl.io/>`_
- `LVGL Online Documentation <https://docs.lvgl.io/9.3/>`_
- `LVGL Introduction <https://docs.lvgl.io/9.3/intro/introduction.html>`_

LVGL (Light and Versatile Graphics Library) is a free and open-source graphics library in C language, providing all the necessary tools to create embedded GUIs with easy-to-use graphical elements, attractive visual effects, and low memory usage.

LVGL offers a GUI engine that handles all interactions between the application and the end user. This includes not only drawing display content and accepting user input to convert it into events that the application can capture, but also providing over 30 built-in widgets, more than 100 style properties, scrolling, animations, advanced software rendering, and built-in GPU support ranging from MCU to OpenGL, among other features. This combination of features allows you to efficiently develop user interfaces without reinventing the wheel.

LVGL showcases demo effects on its official website to demonstrate LVGL's UI building capabilities. The online documentation serves as the primary development resource for LVGL, detailing the design and operational logic of LVGL, usage methods for various controls, a wealth of example programs, and porting methods. Whether you are a beginner or an experienced developer, you can quickly get started and gain an in-depth understanding of LVGL's functions and features based on the online documentation.

- `LVGL Demo <https://lvgl.io/demos>`_
- `LVGL Example <ttps://docs.lvgl.io/9.3/examples.html>`_

Simulator
==========================

A simulator is a powerful tool used in UI development to simulate the UI interface of embedded devices on a computer. It can mimic the behavior and appearance of real hardware platforms, providing developers with a convenient environment to quickly create, debug, and test UI designs.

The main function of the simulator is to showcase and interactively test the UI interface of a design in real time, thus reducing the time and cost associated with repetitive testing on actual hardware. By using an simulator, developers can quickly iterate designs, view results in real time, and perform debugging and validation. This greatly accelerates the speed and quality of UI development and improves work efficiency.

Advantages of using an simulator include:

- Real-time preview: The simulator can instantly display the effects of the UI interface, allowing developers to quickly see the appearance and functionality of the design, facilitating adjustments and modifications.

- Cross-platform support: The simulator can run on a computer, so developers do not need to rely on specific hardware platforms.

- Time and resource savings: Using an simulator avoids the need for frequent flashing and testing of the UI on actual hardware, reducing extra time and cost expenditures.

- Debugging and testing: The simulator provides rich debugging and testing features, allowing for the inspection of UI element interactions, event handling, and layout effects, helping to solve issues and optimize performance.

.. _Running LVGL in the Simulator:

Running LVGL in the Simulator
-----------------------------------
The LVGL simulator is based on the scons tool and the MinGW-w64 toolchain, running and debugging in VScode. For specific environment configuration and startup operations, please refer to the :ref:`Get Started` section.

After completing the simulator environment installation, starting it will show the simulator's default LVGL project. Modify the simulator configuration file to run different LVGL projects. The configuration file for the simulator is located at :file:`your lvgl dir/rtk/win32_sim/` under :file:`menu_config.h`. You can select the example project for the simulator to run by modifying the value of ``LVGL_DEMO_APP``. Start it again in VScode, and after a successful build and compile, you will see the simulator running the selected example project.

.. figure:: https://foruda.gitee.com/images/1753343343333452747/20abc72e_9325830.png
   :align: center
   :width: 408

   Simulator Configuration File

When you need to modify the screen size, change the :file:`SConscript` file located under :file:`your lvgl dir/rtk/demos/xxxxxx/`. Modify the screen width ``DRV_LCD_WIDTH`` and screen height ``DRV_LCD_HEIGHT``, both in pixel units.

.. figure:: https://foruda.gitee.com/images/1753343552380283109/09329a68_9325830.png
   :align: center
   :width: 408

   Simulator Screen Size Configuration File

LVGL Structure
----------------

Below are the directories and files related to LVGL:

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
    |  |        |-- root                         // File system root directory
    |  |        |-- _bin_mkromfs.py
    |  |        |-- mkromfs_0x704D1400.bat       // User Data packaging script
    |  |        |-- root(0x704D1400).bin         // Packaged User Data
    |  |        :
    |  |        |__ ui_resource.h                // Address mapping of packaged file resources
    |  |--tool
    |  |__win32_sim
    |     :
    |     |__ port
    |        |__ lvgl_port                       // Simulator LVGL porting
    |           |-- lv_conf.h                    // Simulator LVGL configuration definitions
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


1. The file structure of LVGL in the Realtek SDK is the same as the official version of LVGL V9.3.

- demos: Contains some comprehensive built-in examples of LVGL, some of which can be experienced at `LVGL Demo <https://lvgl.io/demos>`_ .

- docs: Contains development documents for LVGL, which can be read online at the LVGL documentation site: `LVGL Document <https://docs.lvgl.io/master/index.html>`_ .

- env_support: Support for some environments or platforms.

- examples: Contains built-in examples of LVGL, which can be experienced at `LVGL Example <https://docs.lvgl.io/9.3/examples.html>`_ .

- libs: Contains library files used by LVGL.

- rtk: Contains files related to LVGL in the Realtek SDK, including simulator-related files and example projects.

- scripts: Contains some processing scripts for LVGL.

- src: Contains the actual source code of LVGL. When developing with LVGL, the code files here are used. Realtek has made slight modifications to the LVGL source code, and all changes are recorded in the :file:`your lvgl dir/rtk/` under the :file:`LVGL_SourceCode_Changelog.md` file.

- tests: Contains LVGL test files.

- zephyr: Contains configuration files related to Zephyr.

2. When running LVGL on the simulator, LVGL will start running from the :file:`your lvgl dir/rtk/demos/xxxxxx/` under the :file:`app_sim_port.c`.

3. When running LVGL on the simulator, the root directory pointed to by the LVGL file system interface is :file:`your lvgl dir/rtk/demos/xxxxxx/root/`.

Actual Device Porting
==========================

LVGL offers extensive porting support, enabling developers to easily integrate it into various embedded systems and platforms. It supports drivers for various display devices, touch screens, input devices, and custom GPUs. Developers can configure the porting based on project requirements, such as adjusting display parameters when changing display devices or adapting input interfaces when replacing input devices. This article uses display devices, input devices, and file systems as examples to introduce the porting process and methods. For more details, please refer to `LVGL Integration and Drivers <https://docs.lvgl.io/9.3/details/integration/index.html>`_ and `LVGL Main Modules <https://docs.lvgl.io/9.3/details/main-modules/index.html>`_.

.. note::
    The following examples do not include the specific implementation of hardware device drivers, but simply demonstrate how to connect the drivers to LVGL's interfaces. Developers can complete the driver functions within the same API framework as the example driver to connect to the driver layer interface, and then reuse the porting interfaces of the example project.

Display
-----------------------------
- Documentation: `LVGL Display (lv_display) <https://docs.lvgl.io/9.3/details/main-modules/display/index.html>`_, `LVGL Setting Up Your Display(s) <https://docs.lvgl.io/9.3/details/main-modules/display/setup.html>`_

After developers complete the debugging of display device driver functions, the device can communicate normally with the display device and show colors. This section introduces how to connect the driver to LVGL's display interface to display LVGL's UI.

LVGL's display interface is implemented in the file :file:`lv_port_disp.c`, and display parameters are configured in the initialization function ``void lv_port_disp_init(void)``, such as screen size and frame buffer preparation. The display refresh function is ``static void disp_flush(lv_display_t *disp_drv, const lv_area_t *area, uint8_t *px_map)``.

LVGL supports three rendering modes for display buffers, each suitable for different memory and performance needs:

- LV_DISPLAY_RENDER_MODE_PARTIAL (Partial Rendering)

  - Uses a smaller buffer to divide the screen into several small parts for gradual rendering.
  - Recommended buffer size is at least 1/10 of the screen.
  - Advantage: Saves RAM, suitable for scenarios with limited memory.

- LV_DISPLAY_RENDER_MODE_DIRECT (Direct Write Rendering)

  - Buffer size is the same as the screen, LVGL directly renders to the corresponding buffer location.
  - Buffer always contains the complete image, only updating the changed area.
  - If there are two buffers, contents will automatically synchronize, requiring only buffer address switching during refresh.

- LV_DISPLAY_RENDER_MODE_FULL (Full Screen Redraw)

  - Each refresh redraws the entire screen, even if only one pixel changes.
  - When using double buffering, refresh also only requires buffer address switching.
  - Advantage: Simple implementation, suitable for scenarios with special requirements for refresh speed or compatibility.


Based on actual development needs, different memory resources, and rendering modes, the SDK file :file:`lv_port_disp.c` has configured four recommended rendering schemes for reference. Configure ``ACTIVE_DISPLAY_SCHEME`` to switch modes:

- ``SCHEME_RAM_PARTIAL`` Region Drawing Scheme

  - Uses partial rendering scheme
  - Configures two RAM area caches
  - Suitable for MCUs without PSRAM
  - This scheme only supports display ICs with RAM

- ``SCHEME_RAM_PSRAM_PARTIAL`` Region Drawing Scheme with Full Screen Cache

  - Uses partial rendering scheme
  - Configures two RAM area caches and one PSRAM full screen cache
  - This scheme only supports MCUs with PSRAM
  - No special requirements for display IC

- ``SCHEME_PSRAM_DIRECT`` Full Screen Direct Write Scheme

  - Uses direct write rendering scheme
  - Configures two PSRAM area caches
  - This scheme only supports MCUs with PSRAM
  - No special requirements for display IC

- ``SCHEME_PSRAM_FULL`` Full Screen Redraw Scheme

  - Uses full screen redraw scheme
  - Configures two PSRAM area caches
  - This scheme only supports MCUs with PSRAM
  - No special requirements for display IC


Input Devices
-----------------------------
- Documentation: `LVGL Input Device (lv_indev) <https://docs.lvgl.io/9.3/details/main-modules/indev.html>`_

Once developers have completed the debugging of the input device driver functionality, the device can communicate normally with the input device. This section introduces how to interface the driver with LVGL's input interface to interact with LVGL's UI.

LVGL's input interface is implemented in the file :file:`lv_port_indev.c`, and input device parameters are configured in the initialization function ``void lv_port_indev_init(void)``, such as selecting the device type, configuring the data read callback function, and pointer binding.

For detailed input device porting methods and precautions, please refer to the documentation `LVGL Input Device (lv_indev) <https://docs.lvgl.io/9.3/details/main-modules/indev.html>`_ .

- In the initialization function ``void lv_port_indev_init(void)``, select and register the corresponding type of input device, such as **Touchpad** for touch screen devices, and implement the corresponding interface.

- LVGL will periodically obtain the current input device data through the callback function pointer ``indev_drv.read_cb``. For touch screen devices, this would be the function ``static void touchpad_read(lv_indev_t *indev_drv, lv_indev_data_t *data)``, which does not need modification.

- Developers need to fill in the ``static bool touchpad_is_pressed(void)`` function and the ``static void touchpad_get_xy(int32_t *x, int32_t *y)`` function to obtain the current status information from the touchpad, including the coordinates of the touch point and the touch status.

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


File System
-----------------------------

 - Documentation: `LVGL File System (lv_fs_drv) <https://docs.lvgl.io/9.3/details/main-modules/fs.html>`_

LVGL Benchmark Test
==========================

LVGL Benchmark is a performance testing tool used to evaluate the graphics display performance of the LVGL library across various hardware and software environments. By running the Benchmark, users can obtain data such as frame rates, rendering speeds, and memory usage, which helps optimize display configurations and debug performance issues. The Benchmark includes multiple test scenarios, such as graphics drawing, animations, and text rendering, with each scenario simulating common operations in actual applications. Users can compare performance across different configurations and platforms through these tests to make targeted optimization adjustments.

The official documentation for LVGL benchmarking can be found at :file:`your lvgl dir/lvgl/demos/README.md`.

Benchmark Reference
-----------------------------

Platform background: 8773G platform, 200MHz main frequency, QSPI410*502 screen, 96KB RAM with 3M PSRAM, dual buffer mode, direct screen push mode.

Compilation environment: armclang6.22 version, optimization mode -Ofast to enable LTO; gcc 12.2 version, optimization mode -Ofast.

.. csv-table:: Benchmark Test Results with Different Acceleration Methods
   :header: Scenario, SW Rendering Time, SW+MVE+ARM2D Rendering Time, SW+MVE+ARM2D+PPE Rendering Time
   :widths: 60 50 50 50
   :align: center

   empty screen, 4, 4, 4
   moving wallpaper, 16, 16, 11
   single rectangle, 0, 0, 0
   multiple rectangles, 4, 4, 2
   multiple RGB images, 5, 5, 2
   multiple ARGB images, 10, 9, 2
   rotated ARGB images, 11, 12, 0
   multiple labels, 8, 9, 9
   screen sized text, 32, 31, 30
   multiple arcs, 6, 6, 5
   containers, 6, 6, 6
   containers with overlay, 27, 21, 24
   containers with opa, 9, 11, 7
   containers with opa_layer, 15, 11, 10
   containers with scrolling, 23, 20, 21
   widget demo, 31, 29, 30
   all screen, 12, 11, 10


.. csv-table:: Benchmark Test Results in Different Compilation Environments
  :header: Scenario, Acceleration Method, ARMCLANG, GCC
  :widths: 60 50 50 50
  :align: center

    empty screen, SW+MVE+PPE, 5, 5
    moving wallpaper, SW+MVE+PPE, 16, 16
    single rectangle, SW+MVE+PPE, 0, 0
    multiple rectangles, SW+MVE+PPE, 2, 4
    multiple RGB images, SW+MVE+PPE, 2, 5
    multiple ARGB images, SW+MVE+PPE, 3, 3
    rotated ARGB images, SW+MVE+PPE, 1, 0
    multiple labels, SW+MVE+PPE, 11, 13
    screen sized text, SW+MVE+PPE, 38, 37
    multiple arcs, SW+MVE+PPE, 6, 8
    containers, SW+MVE+PPE, 6, 6
    containers with overlay, SW+MVE+PPE, 24, 25
    containers with opa, SW+MVE+PPE, 6, 6
    containers with opa_layer, SW+MVE+PPE, 11, 10
    containers with scrolling, SW+MVE+PPE, 21, 22
    widget demo, SW+MVE+PPE, 30, 34
    all screen, SW+MVE+PPE, 11, 12

.. csv-table:: Rendering Acceleration on Different Platforms
  :header: Chip Model, Processor Frequency, Hardware Accelerator, Image Drawing, Image Transparency, Image Scaling, Image Rotation, Rounded Rectangle, Rectangle Fill, RLE Decoding, Character, Line
  :align: center

  RTL8772G, 125MHz, PPE1.0, HW, HW, HW, SW, SW+HW, HW, HW, SW, SW
  RTL8773E, 100MHz, PPE2.0, HW, HW, HW, HW, SW+HW, HW, HW, SW, SW
  RTL8773G, 200MHz, PPE2.0, HW, HW, HW, HW, SW+HW, HW, HW, SW, SW


.. note::
  1. Effects involving LVGL Mask require SW processing
  2. RTL8772G supports Helium hardware accelerator


Getting Started with Demo Development
=========================================

LVGL Native Demo
-------------------

- `LVGL Demo <https://lvgl.io/demos>`_
- `LVGL Example <https://docs.lvgl.io/9.3/examples.html>`_

It is recommended that developers read and understand the `LVGL Introduction <https://docs.lvgl.io/9.3/intro/introduction.html>`_ and `LVGL Widget Basics <https://docs.lvgl.io/9.3/details/common-widget-features/basics.html>`_ sections before starting development to grasp the design concepts and logic of LVGL.

LVGL offers a wealth of demos and examples to help developers become familiar with the use of various controls and features.

- The `LVGL Demo <https://lvgl.io/demos>`_ showcases comprehensive demos, with their source code stored in the directory :file:`your lvgl dir/lvgl/src/demo`. Developers can directly call the corresponding ``lv_demo_xxx()`` functions to familiarize themselves.

- The online documentation `LVGL Example <https://docs.lvgl.io/9.3/examples.html>`_ displays the running effects of various examples, with their source code stored in the directory :file:`your lvgl dir/example`. Developers can directly call the corresponding ``lv_example_xxx()`` functions to familiarize themselves with controls and understand features.

RTK Extension Demo
-------------------

RTK, based on the existing LVGL framework, encapsulates some commonly used UI components into standard LVGL controls or control extension modules, allowing developers to quickly build UI interfaces by directly calling these controls and modules.

To help developers quickly understand the RTK extension controls, RTK has created an independent demo for the extension features, making the demo as concise and easy to understand as possible.

The entry function for the RTK extension demo is located at :file:`your lvgl dir rtk/demos/single_demo/ui_init.c`, which includes some LVGL native demos and RTK extension demos. Users can choose any demo to execute and view the effects.

All extension demos can be run using a simulator, allowing developers to directly view the demo's running effect in the simulator, as well as modify and quickly see the differences in the modified effects.

The source code for RTK extension demos is stored in the directory :file:`your lvgl dir rtk/demos/single_demo/xxx/rtk_demo_xxx.c`. Developers can directly call the corresponding ``rtk_demo_xxx()`` function to familiarize themselves with it.


Card Demo
~~~~~~~~~~~~

Users can use the ``lv_card_view_create`` function to create a card view container widget. The child widget can only be the card widget. This function can configure the display style, the height, interval, total number of the card widget, and the design function. The design of the UI content in the card widget is completed through the user-defined design function. It supports dynamic creation, that is, the card widget will be created and design function will be called when entering the display area, and when exiting display area the card widget will be cleaned.

Users can use the ``lv_card_view_set_number`` function to set the number of card widgets.

Users can use the ``lv_card_view_set_offset`` function to set the offset of the card view widget.

.. raw:: html

   <br>
   <div style="text-align: center">
   <img src="https://foruda.gitee.com/images/1757557061445981954/fe093ccb_9325830.gif" width="502" />
   </div>
   <br>


Cellular Demo
~~~~~~~~~~~~~~~~

Users can use the ``lv_cellular_create_with_icon`` function to create a cellular container widget, and you need to provide an array of image description pointers and an array of callback function pointers.

Users can use the ``lv_cellular_set_offset`` function to set the offset of the cellular widget.

.. raw:: html

   <br>
   <div style="text-align: center">
   <img src="https://foruda.gitee.com/images/1757557520907101802/95926e8f_9325830.gif" width="502" />
   </div>
   <br>


Tileview Transition Demo
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. raw:: html

   <br>
   <div style="text-align: center">
   <img src="https://foruda.gitee.com/images/1757557892430171667/0183d5bd_9325830.gif" width="502" />
   </div>
   <br>

Tileview is a container widget whose child elements are tiles, which can be arranged in a grid format. Users can navigate between tiles by swiping. If the tileview is the size of the screen, the user interface resembles that on a smartwatch.

The RTK Extension Demo includes a tileview transition example, which creates a set of tileviews and adds independent transition effects to each tile, such as scaling, fading, etc.

Unlike the native LVGL tileview, the tileview transition process in this demo supports drag-and-swipe interactions, making it more in line with user operation habits.

If you want to use tile transition effects, developers need to set transition effects for each tile and add transition event callbacks to the tileview.

The transition effects and callback functions are already encapsulated in the software package and can be used directly. The source code is saved in :file:`lv_custom_tileview_slide.c`.

For the complete code of the tileview transition example, please refer to the ``rtk_demo_tileview_slide()`` function in :file:`rtk_demo_tileview_slide.c`. Here is a simplified version for understanding in correspondence with the above introduction:

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


Tileview Transition Demo with 2.5D Transition Effects and Snapshot
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. raw:: html

   <br>
   <div style="text-align: center">
   <img src="https://foruda.gitee.com/images/1757559644406993741/db4f4ff2_9325830.gif" width="502" />
   </div>
   <br>

This example, along with the previous one, creates the same set of tiled views and additionally incorporates 2.5D transition effects and a snapshot caching mechanism.

The 2.5D transition effects currently support three types: box, rotation, and cube. Developers can refer to the existing effect code in :file:`lv_custom_tileview_slide.c` for further expansion.

To use the 2.5D transition effects, you need to set the transition effect for each tile and enable the configuration option ``LV_DRAW_TRANSFORM_USE_MATRIX`` in LVGL.

To enable the snapshot mechanism, developers must create a control under each tile to manage the snapshot cache image and the actual page content, and add two new snapshot events to handle the creation and destruction of snapshots during transitions.

Developers can refer to the :file:`rtk_demo_tileview_slide_snapshot()` function in ``rtk_demo_tileview_slide.c`` for further development. Here is a simplified version to correspond with the understanding introduced above:

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
  1. The 2.5D transition effects require an LVGL version that supports matrix rendering and a hardware platform that supports matrix rendering.
  2. To use 2.5D transition effects, you need to enable the snapshot caching mechanism for tiled views.
  3. The snapshot caching mechanism can be enabled independently to optimize performance during transitions.
  4. The snapshot caching mechanism requires a large amount of memory space.

3D Model Demo
~~~~~~~~~~~~~~~~

LVGL integrates Realtek's self-developed Lite3D engine, encapsulated as the ``lv_lite3d`` widget. Users can use this widget to load and render 3D models. For a detailed introduction to the Lite3D engine, please refer to :ref:`3D Model`.

Usage Steps:

  1. Prepare the description file for the 3D model.
  2. Call the ``l3_create_model()`` function from the Lite3D library to create the 3D model.
  3. Use the ``l3_set_global_transform()`` function to apply a global transformation to the 3D model, including initializing the world and camera coordinate systems.
  4. Use the ``l3_set_face_transform()`` function to apply local transformations to different faces of the 3D model. (Optional)
  5. Call the ``lv_lite3d_create()`` function to create the ``lv_lite3d`` widget and bind the 3D model to it.
  6. Use the ``lv_lite3d_set_click_cb()`` function to set a click event callback for the ``lv_lite3d`` widget. (Optional)

The following provides an example of a 3D disc model. The sample code is saved in :file:`rtk_demo_lite3d_disc.c`:

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
   The configuration option ``LV_DRAW_TRANSFORM_USE_MATRIX`` in LVGL needs to be enabled.

.. _Resource Converter:

Resource Converter
==========================
To use images and fonts in LVGL-based UIs, you must first convert them into LVGL-recognizable formats using dedicated tools. LVGL supports two types of resource formats:

1. C Array Format
  Resources (images/fonts) are converted into C arrays. These arrays become part of the code and are compiled into the main application binary.

  - Every time you change your application logic, these resource files will be recompiled.
  - The combined resource size is included in the APP image, meaning a larger space is required, particularly during OTA (Over-The-Air) updates.

2. Binary (bin) File Format
  Resources are saved as separate binary files and do not participate in the compilation process.

  - These files are stored and accessed independently, which usually requires support for a file system.
  - At runtime, the program uses the file system's API to load resources into memory, which does consume RAM.

Media Converter
-----------------------------


LVGL Image Converter
~~~~~~~~~~~~~~~~~~~~~~~

- Online Image Converter: `LVGL Image Converter <https://lvgl.io/tools/imageconverter>`_
- Documentation: `LVGL Images (lv_image) <https://docs.lvgl.io/9.3/details/main-modules/image.html>`_

The LVGL Online Image Converter is a website provided by the LVGL development team that allows you to upload local image files and convert them into standard C files. The converted files describe image information in the form of variables. For usage steps, please refer to `LVGL Images (lv_image) - Online Converter <https://docs.lvgl.io/9.3/details/main-modules/image.html#online-converter>`_:

1. Select the LVGL Version: Choose ``LVGL v9``
2. Upload Image Files: Select one or more local image files for batch conversion.

   The output C files will have the same name as the input files, and the variable name describing the image will also match the input file name. Therefore, avoid using Chinese characters or any illegal characters in the file names.
3. Choose Output Color Format:

   For an explanation of color formats, refer to  `LVGL Images (lv_image) - Color Format <https://docs.lvgl.io/9.3/details/main-modules/image.html#color-formats>`_.
4. Click :guilabel:`Convert` to obtain the output file

The document `LVGL Images (lv_image) <https://docs.lvgl.io/9.3/details/main-modules/image.html>`_ provides a detailed introduction on using image resources and the image converter tool in LVGL, as well as simple usage examples.


The online image converter for LVGL v9 only supports output in C file format and a limited selection of color formats. If you need features such as image compression, bin file output, or other color formats, please use the  `Python Image Converter <https://github.com/lvgl/lvgl/blob/master/scripts/LVGLImage.py>`_.
When exporting image resources in bin file format, the data in the bin file is stored as ``12 Byte lv_img_header_t + data``, where ``lv_img_header_t`` includes information such as ``Color format``, ``width``, and ``height``.

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



Realtek Media Convert Tool
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The Media Convert Tool, developed by Realtek, offers the following features:
  - Integrated LVGL v9 Python Conversion Script:
    Fully incorporates the `Python Image Converter <https://github.com/lvgl/lvgl/blob/master/scripts/LVGLImage.py>`_ from LVGL v9, exposing its complete functionality.
  - Supports JPEG Output:
    Compatible with hardware decoders for JPEG format.
  - Supports RTZIP Compression Output:
    Compatible with hardware decoding for RTZIP compressed formats.
  - Supports AVI Video Output:
    Hardware decoding support for AVI video format.
  - Batch and Single File Configuration:
    Allows batch configuration for entire folders, as well as specifying configurations for individual files.
  - User Data Packaging and External Folder Linking:
    Supports packaging of user data and linking to external folders.

When converting and generating bin files, the Media Convert Tool also creates a resource description file, :file:`ui_resource.c`. This file contains descriptor variables for each image and video, with the variable type set to ``lv_image_dsc_t``.

For platforms that support direct flash addressing, such as Nor Flash, the resource bin files can be downloaded directly to flash memory. After that, images can be referenced directly through these descriptor variables, bypassing the file system. This improves efficiency, reduces buffering requirements, and lowers overall system complexity.


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
  1. The conversion tool generates variable names based on the input file names. Please avoid using Chinese characters or other illegal characters in the input file names.
  2. The usage of :file:`ui_resource.c` depends on the :file:`ui_resource.h` file, which is generated during the packaging process.




LVGL
^^^^^^^^^

This conversion feature is fully supported by the native LVGL conversion script. The tool only accepts PNG images as input files and can output both C files and bin files, supporting multiple color formats.
When outputting image resources in the bin file format, the data in the bin file is stored as ``12 Byte lv_img_header_t + data``, where the ``lv_img_header_t`` contains information such as the ``color format``, ``width``, and ``height``.
For more details on using image resources and the image conversion tool in LVGL, as well as simple usage examples, please refer to the documentation: `LVGL Images (lv_image) <https://docs.lvgl.io/9.3/details/main-modules/image.html>`_


.. figure:: https://foruda.gitee.com/images/1753864909573136949/b12eb86a_9218678.png
   :align: center
   :width: 408

   LVGL Image Conversion Configuration Interface


RTZIP
^^^^^^^^^
This configuration encodes images into the RTZIP format, which supports multiple compression algorithms and customizable compression parameters. RTZIP is a lossless compression format and supports encoding of transparent layers.

Tips:
  - If the input image contains transparency but the output format does not support alpha (such as when outputting in RGB565), it is recommended to enable the :guilabel:`Mix Alpha Channel` option. Example: Input: PNG with transparency → Output: ``RGB565``.

.. figure:: https://foruda.gitee.com/images/1753864936117526804/e64c6628_9218678.png
   :align: center
   :width: 408

   RTZIP Image Conversion Configuration Interface



JPG
^^^^^^^^^
This configuration re-encodes images into the JPEG format. You can adjust the image sampling ratio and quality settings to reduce the file size. JPEG is a lossy compression format that achieves a good balance between display quality and file size. Please note that images converted to JPEG will lose any transparency (alpha channel) information.

.. figure:: https://foruda.gitee.com/images/1753864922836242088/290c1bd8_9218678.png
   :align: center
   :width: 408

   JPEG Image Conversion Configuration Interface


AVI
^^^^^^^^^
This configuration converts an image sequence or video file into AVI format, supporting frame rate and image quality settings. Note that the output AVI file does not include an audio track.

- If the input is an image sequence folder, the conversion settings will be applied to all images in the folder. Please ensure that the images are named in the pattern ``xxx_%04d.xxx``, starting from 0 (e.g., frame_0000.jpg, frame_0001.jpg).
- If the input is a video file, the settings will be applied directly to that video file.


.. figure:: https://foruda.gitee.com/images/1753864947902502287/d16f5802_9218678.png
   :align: center
   :width: 408

   AVI Video Conversion Configuration Interface


Below is the usage example:

.. code-block:: c

   // lv_conf.h
   /** RTK AVI decoder.
    *  Dependencies: JPEG decoder. */
   #define LV_USE_AVI  1

   // example
   lv_obj_t * video = lv_avi_create(lv_screen_active(), NULL);

   /* From variable */
   lv_avi_set_src(video, &my_avi_dsc);


Font Converter
-----------------------------

LVGL Online Conversion Tool
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- Online Conversion Tool: `LVGL Font Converter <https://lvgl.io/tools/fontconverter>`_

- Documentation: `LVGL Font (lv_font) <https://docs.lvgl.io/9.3/details/main-modules/font.html>`_

Please refer to `LVGL Frequently Asked Questions - How to use the font converter? <https://lvgl.io/tools/fontconverter>`_ for usage steps:

1. Set the name of the output font library

2. Set the font size, measured in pixel height

3. Set the font's bpp (bit-per-pixel)

   - Indicates how many bits are used to describe a pixel; the larger the value, the better the anti-aliasing effect and smoother edges, but the larger the font library space required

4. Fill in the font callback. When the font file does not contain the required characters, the callback font can be called for rendering; it can be left blank

5. Choose the type of output font library (C file / Binary)

6. Select the font file (TTF/WOFF)

7. Set the Unicode range of characters to be converted, or directly list the characters to be converted


Realtek Font Converter
~~~~~~~~~~~~~~~~~~~~~~~

Realtek has launched a newly upgraded Realtek Font Converter based on the packaging and extension of the LVGL font conversion tool. Compared to the original tool, the Realtek Font Converter has the following significant advantages:

1. Visual operation interface makes the font conversion process clear and easy, enhancing operational efficiency

2. Supports multi-font, multi-size, and tag-based configuration design, greatly improving the flexibility and efficiency of engineering development

3. Adds support for Realtek platform GPU rendering features, fully leveraging hardware acceleration capabilities

4. Supports separation of font data and description information, better adapting to the storage management mode of embedded platforms

5. Supports import and export of configuration schemes, facilitating project migration and team collaboration

Below is the interface image of the Realtek Font Converter:

.. figure:: https://foruda.gitee.com/images/1753432503991458121/2c546aae_9325830.png
   :align: center
   :width: 983

   Realtek Font Converter Interface


Font Conversion Steps:

1. Open the Realtek Font Converter interface

2. Click the Add new setting button to add a new configuration tab

3. Click the Add Fonts button to add font files; multiple fonts can be added

4. Fill in configuration items such as bpp, font size, and character set, where multiple font sizes can be entered, separated by commas

  - When bpp is 3, the generated font file cannot use GPU accelerated rendering
  - After selecting the compression option, the generated font file will be compressed, occupying less space, but cannot use GPU accelerated rendering

5. Click the Generate Font button, choose the path, and generate the font file for the current configuration tab

6. If there are multiple tabs, click the Browse button, select the font file output path, and click the Generate All button to generate all configuration tab font files to the specified path

Additional Features:

- Supports batch generation of font files, with each tab's font file named Font Name_Size_Configuration Item

- Supports copying, deleting, and renaming configuration tabs

- Supports exporting configuration information of the current tab, exported in JSON format

- Supports importing configuration information, importing configuration files in JSON format

Development Resources Support
==============================

Online Documentation
---------------------
- `LVGL Document <https://docs.lvgl.io/master/index.html>`_

The `online documentation <https://docs.lvgl.io/master/index.html>`_ for LVGL provides comprehensive technical documentation and tutorials to help developers better understand and use the LVGL graphics library. The documentation includes the following:

- Overview and Features: The documentation introduces the basic concepts and features of LVGL, including graphical objects, screen management, event handling, theme styling, and more. Users can read the documentation to understand the core functions and advantages of LVGL.

- Application Development Guide: The documentation provides detailed application development guides, including how to initialize and configure LVGL, create and manage graphical objects, handle user input and events, add themes and styles, and more. These guides can help users quickly get started with LVGL and develop their own applications.

- API Documentation: The documentation lists LVGL's API interfaces and functions in detail, along with their parameters and usage. Users can refer to the API documentation as needed to understand the specific functions and interfaces for advanced customization and extension.

- Sample Code: The documentation provides numerous sample codes covering common application scenarios and features. Users can refer to these sample codes to speed up development and quickly implement specific functional requirements.

Using LVGL's online documentation can help users better understand and master the use of LVGL, improving development efficiency. Users can gradually learn the content in the documentation, from simple interface building to complex application development, gradually mastering the various functions and features of LVGL. Additionally, the documentation provides examples and code snippets, making it easier for users to develop applications with rich interfaces and features.

Users can open LVGL's online documentation in a web browser, browse through various sections and content, and search for and learn the relevant knowledge according to their needs. Moreover, users can use the search function to quickly find specific information in the documentation. Overall, LVGL's online documentation is an important resource for users to understand and use the LVGL graphics library, providing comprehensive and detailed guidance to help users quickly get started and develop better applications.

Developing based on the documentation can achieve most of the UI effects. It is worth noting that the documentation content may not be complete, and when there are omissions in the documentation, the code shall prevail.

GitHub Repository
-----------------------------
- `GitHub LVGL <https://github.com/lvgl/lvgl>`_

LVGL's GitHub repository is an important platform for developers to use and contribute to LVGL:

- Get the Latest Version: LVGL's GitHub repository provides access to the latest LVGL versions and updates. Developers can timely access the latest feature updates, fixes, and improvements to keep applications in sync with LVGL.

- Participate in the Community and Contribute Code: Through the GitHub repository, developers can actively participate in discussions and exchanges within the LVGL community, learning about other developers' issues and solutions. Developers can also contribute their own code and improvements to make LVGL more complete and powerful.

- Submit Issues and Bug Reports: The GitHub repository provides a platform for submitting issues and bug reports. Developers can submit problems and bugs they encounter while using LVGL, helping the LVGL development team to promptly identify and resolve issues, thereby improving LVGL's stability and reliability.

- Learn from Examples and Documentation: The GitHub repository also contains sample code and documentation to help developers better understand and learn how to use LVGL. Developers can browse the sample code and documentation in the repository to learn about LVGL's various functions and features, improving their development skills.

Designer
-----------------------------

Squareline Studio
~~~~~~~~~~~~~~~~~~~~~~~

`Squareline Studio <https://squareline.io/>`_ is a next-generation visual UI editor designed for rapid development of beautiful interfaces for embedded and desktop applications. It supports free personal use and flexible payment for enterprises. It integrates design, prototyping, and development, generating platform-independent C or MicroPython code suitable for LVGL through drag-and-drop, which can run on any device and system. It supports pixel-level preview, custom components, animations, and events, greatly enhancing development efficiency. It is suitable for a variety of scenarios for individuals, startups, and large enterprises.

UI Design
^^^^^^^^^

When designing LVGL interfaces using Squareline Studio, you can refer to the design tool's `official documentation <https://docs.squareline.io/docs/squareline/>`_ and the open-source examples within the software.

Realtek has provided complete support for Squareline Studio, based on the software and hardware capabilities of the Realtek platform and the design characteristics of the LVGL library. The following design optimization principles have been summarized:

- Reduce unnecessary rounded designs, as rectangular shadow rendering is much more efficient than rounded rectangle shadow rendering.

- Minimize redundant layer stacking and background color filling while ensuring consistent display content.

- The Realtek platform supports hardware image decompression (IDU and JPU). Compared to graphic rendering, prioritizing image use in UI design will improve display frame rates.

- To adapt to the hardware rules of the Realtek platform, use the Realtek resource converter to convert resources. For ease of porting, it is recommended to place all image and font design files in a single main directory and ensure the standardization of file name formats.

Resource Conversion
^^^^^^^^^^^^^^^^^^^^
Squareline Studio includes a standard LVGL resource converter that can generate resource files in LVGL format, which can be compiled and run on simulators and development boards.

Since the LVGL resource converter used by Squareline Studio does not include Realtek's image compression algorithms and font conversion features, customers who need to use Realtek's hardware decompression and GPU font rendering functions need to use the Realtek resource converter for conversion and then replace the original files.

.. note::
   Instructions for using the Realtek resource converter can be found at :ref:`Resource Converter`.

.. note::
   The files output by the Realtek resource converter comply with LVGL's format standards, ensuring direct use. Therefore, developers can directly replace the resource files generated by Squareline Studio with those generated by the Realtek resource converter.

When exporting UI design files from Squareline Studio, it may modify the output resource names, such as:

 - Adding prefixes and suffixes to image resources, with the prefix being the original image's relative path and type name, and the suffix being the original image format.

 - Replacing original file names containing illegal characters with random numeric strings.

 - Generating font names using corresponding pinyin or abbreviations when the original font file is in Chinese.

During the image resource replacement process, most naming differences can be adjusted using find-and-replace methods. When encountering abnormal file names, manual adjustments are required.

During font resource replacement, due to the complexity and diversity of font settings, manual file matching is necessary.

Project Porting
^^^^^^^^^^^^^^^^^^^^

UI interfaces designed with Squareline Studio can be directly exported as C code for the Realtek platform, allowing developers to directly import the code into the Realtek platform for compilation and debugging.

How to run the UI interface designed with Squareline Studio in a simulator?

- First, configure the simulator environment and successfully run the LVGL example project, referring to :ref:`Running LVGL in the Simulator`.

- Then export the UI interface designed with Squareline Studio as C code and resource packages, and copy them to :file:`your lvgl dir/rtk/demos/benchmark`.

  Common files exported by Squareline Studio include:

  ::

    UI
    |-- components             // Component design
    |-- fonts                  // Font resources
    |-- images                 // Image resources
    |-- screens                // Page design
    |-- CMakeLists.txt
    |-- filelist.txt
    |-- ui.c                   // Entry file
    |-- ui.h
    |-- ui_events.h
    |-- ui_helpers.c           // Helper functions
    |-- ui_helpers.h
    |-- ui_theme_manager.c     // Theme management
    |-- ui_theme_manager.h
    |-- ui_themes.c            // Theme resources
    |-- ui_themes.h

- If you need to use image compression or GPU font rendering functions, use the Realtek resource converter for conversion, then replace the original files and replace the image and font names in the UI design files.

- After initializing LVGL with :code:`lv_init()` in the original example project's LVGL entry file, start the UI loading function of the Squareline Studio project with :code:`ui_init();`.

- Adjust the :file:`sconscript` file in the example project to add build support for the Squareline Studio project.

- Use the resource packaging tool to package the resource files of the Squareline Studio project into binary files.

- Compile and run the simulator. Once the build and compilation pass, you can see the simulator running the Squareline Studio example project.

Feature Extensions
^^^^^^^^^^^^^^^^^^^^

Squareline Studio supports most of LVGL's basic features, such as UI design, component design, animation design, and event design. However, it lacks support for some advanced features, such as page transition mechanisms, peripheral logic interaction, and cellular dials. Therefore, if you need to use these features, manual code writing is required.

LVGL Editor
~~~~~~~~~~~~~

`LVGL Editor <https://pro.lvgl.io/>`_ is a professional GUI development tool based on the free open-source LVGL library, supporting Windows, Linux, and macOS. It defines interfaces through XML, supports auto-completion, version control, real-time preview, and deep integration with Figma, allowing online sharing and testing of UIs. The editor supports exporting to C code, runtime XML loading, and various advanced components, with CLI integration, automatic style synchronization, data binding, and event handling, greatly enhancing embedded UI development efficiency.

EEZ Studio
~~~~~~~~~~~~~

`EEZ Studio <https://www.envox.eu/studio/studio-introduction/>`_ supports LVGL 8.x and 9.x versions, allowing users to quickly design responsive desktop and embedded GUIs through drag-and-drop and flowchart visualization programming. The platform comes with rich templates and examples, suitable for prototyping and final application development, supporting multiple platforms and languages. With EEZ Flow, complex logic can be completed without programming, improving development efficiency, making it an efficient, open-source tool for developing LVGL projects.

GUI Guider
~~~~~~~~~~~~~~~~~
.. <!-- - GUI Guider: `NXP GUI Guider <https://www.nxp.com/design/design-center/software/development-software/gui-guider:GUI-GUIDER>`_ -->

Forum
-----------------------------
- `LVGL Forum <https://forum.lvgl.io/>`_

.. <!-- Open for questions, LVGL developers will respond. -->

The official LVGL forum is a developer community dedicated to discussing and sharing topics and resources related to the LVGL graphics library. It provides a platform for developers to communicate, seek help, and share their experiences and projects.

Some features and functions of the LVGL forum include:

- Questions and Answers: Developers can post their issues encountered while using LVGL on the forum and receive assistance and answers from other developers. This makes the forum a valuable knowledge base, offering problem-solving experiences and techniques.

- Tutorials and Examples: The forum contains numerous useful tutorials and example codes, demonstrating how to use various features and functionalities of LVGL. These resources are very helpful for novice developers to learn and master LVGL.

- Developer Contributions and Project Showcases: Developers on the forum can share their projects and customized LVGL interfaces, as well as contributions that other developers can share, discuss, and reference.

- Updates and Release Announcements: The LVGL development team posts announcements and explanations about new version releases and updates on the forum. This allows developers to stay informed about the latest features and improvements.

- Community Interaction: The forum provides a platform for community interaction, where developers can communicate, share, and build connections, enhancing the collaboration and development of the LVGL community.

The LVGL forum is an important resource for developers using LVGL to get support, solve problems, learn, and share experiences.

Blog
-----------------------------
- `LVGL Blog <https://blog.lvgl.io/>`_

The official LVGL blog is a regularly updated platform that provides the latest information, tutorials, case studies, and developer insights about the LVGL graphics library. The development team and community members of LVGL frequently publish various content on the blog, allowing developers to better understand and utilize LVGL.

The LVGL blog contains the following content:

- Updates and New Features Introduction: The blog posts articles about updates and improvements in the latest version of LVGL, introducing new features, fixed issues, and performance enhancements, enabling developers to understand and leverage the latest LVGL features.

- Tutorials and Usage Guides: The blog provides practical tutorials and usage guides on LVGL, covering topics from beginner to advanced levels. These tutorials typically include sample code and detailed instructions to help developers master the use of LVGL and best practices.

- Case Studies and Project Showcases: The blog shares case studies and project showcases that implement LVGL. These articles demonstrate how to use LVGL to build actual applications and interfaces, inspiring and providing experience to developers through practice.

- Technical Deep Dives and Developer Insights: The blog also covers in-depth analyses and developer insights on LVGL. These articles may explore topics such as the internal workings of LVGL, performance optimization techniques, and excellent design practices, offering developers a deeper understanding and reflection.

The LVGL blog is an important resource for LVGL developers, serving as a valuable source for understanding and mastering LVGL. By reading the blog, developers can gain access to the latest trends, learning materials, and technical insights about LVGL, aiding them in better utilizing LVGL to create outstanding graphical interfaces.
