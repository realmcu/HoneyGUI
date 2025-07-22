===============================
Use LVGL V8 to Design
===============================

LVGL introduction
==========================
- `LVGL Website <https://lvgl.io/>`_
- `LVGL Document <https://docs.lvgl.io/master/intro/index.html>`_
- `LVGL Intro <https://docs.lvgl.io/8.3/intro/index.html>`_

LVGL (Light and Versatile Graphics Library) is the most popular free and open-source embedded graphics library to create beautiful UIs for any MCU, MPU and display type. LVGL  provides everything you need to create an embedded GUI with easy-to-use graphical elements, beautiful visual effects and a low memory footprint.

LVGL showcases Demo effects on its official website to demonstrate the UI building capabilities of LVGL. The online documentation serves as the primary development resource for LVGL, providing detailed information on the design and operational logic of LVGL, instructions on using various widgets, a wide range of example programs, and guidelines for porting LVGL. Whether you are a beginner or an experienced developer, you can quickly get started and gain a deep understanding of LVGL's functionality and features based on the online documentation.

- `LVGL Demo <https://lvgl.io/demos>`_
- `LVGL Example <https://docs.lvgl.io/8.3/examples.html>`_





HoneyGUI Simulator
==========================

A simulator is a powerful tool used for developing UI that simulates the UI interface of embedded devices on a computer. It can mimic the behavior and appearance of a real hardware platform, providing developers with a convenient environment to quickly create, debug, and test UI designs.

The primary purpose of a simulator is to display and interactively test the designed UI interface in real-time, thereby reducing the time and cost of repetitive testing on actual hardware. By using a simulator, developers can iterate designs quickly, view the effects in real-time, and perform debugging and validation. This greatly speeds up UI development and improves workflow efficiency.

Using a simulator has the following advantages:

- Real-time preview: The simulator can show the immediate effects of the UI interface, allowing developers to quickly see the appearance and functionality of their design, facilitating adjustments and modifications.

- Cross-platform support: Simulators can run on computers, eliminating the need for specific hardware platforms.

- Time and resource-saving: Simulators help avoid frequent flashing and testing of UI on actual hardware, reducing additional time and cost overhead.

- Debugging and testing: Simulators provide rich debugging and testing capabilities to inspect the interaction, event handling, and layout effects of UI elements, aiding problem-solving and performance optimization.


Run LVGL in HoneyGUI Simulator
--------------------------------
HoneyGUI Simulator is based on the scons tool and MinGW-w64 toolchain. It can be run and debugged in VScode. For specific environment setup and running instructions, please refer to the :ref:`Get Started` section.

After completing the environment setup for the HoneyGUI Simulator, when you start running it, you will see the default HoneyGUI project in the simulator. To modify the simulator configuration file to run an LVGL project, go to the path :file:`your HoneyGUI dir/win32_sim/` and open the file :file:`menu_config.h`, which is the configuration file for the simulator. Under the section **HoneyGUI Demo Select**, comment out all the demos. Under the section **HoneyGUI Enable LVGL**, enable :c:macro:`CONFIG_REALTEK_BUILD_LVGL_GUI`. Then, start running it again in VScode. After the build is successful, you will see the default LVGL demo project running in the simulator.

.. image:: https://foruda.gitee.com/images/1729750006337280334/445a33a3_9218678.png
   :align: center
   :width: 800

1. If you need to modify the screen size, open the file :file:`SConscript` under the directory :file:`your HoneyGUI dir/realgui/example/demo/`, and modify the values of :c:macro:`DRV_LCD_WIDTH` and :c:macro:`DRV_LCD_HEIGHT` to the desired pixel values.

.. image:: https://foruda.gitee.com/images/1727161740835693997/89fd9c57_9218678.png
   :align: center
   :width: 400


HoneyGUI LVGL 
----------------
The directories and files related to LVGL in HoneyGUI are as follows:

::

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
    |     |  |__ app_ui_lvgl.c               // simulator LVGL UI entrance
    |     :
    |     :
    |     |__ screen_lvgl    
    |        |-- assets                       // LVGL user image and font C files    
    |        |  |__ lvgl_example_assets.c     // assets example                
    |        |
    |        |-- root                         // file system root folder
    |        |-- _bin_mkromfs.py
    |        |-- mkromfs_0x4600000.bat        // User Data pack script
    |        |-- resource.h                   // resource files packed address
    |        |__ root(0x4600000).bin          // packed User Data
    |
    |-- keil_sim
    |-- lib
    |-- lvgl_v8                               // LVGL v8.3
    |  |-- demos                              // LVGL demo source files
    |  |  |-- benchmark
    |  |  |-- keypad_encoder
    |  |  |-- music
    |  |  |-- stress
    |  |  |__ widgets
    |  |
    |  |-- docs
    |  |-- env_support
    |  |-- examples                           // LVGL example source files
    |  |  |-- anim
    |  |  |-- arduino
    |  |  |-- assets
    |  |  |-- event
    |  |  |-- get_started
    |  |  |-- layouts
    |  |  |-- libs
    |  |  |-- others
    |  |  |-- porting                         // LVGL porting template
    |  |  |-- scroll
    |  |  |-- styles
    |  |  |__ widgets                         // LVGL example widges 
    |  |
    |  |-- rlottie
    |  |-- scripts
    |  |-- src
    |  |  :
    |  |  |-- widgets 
    |  |  |__ font                            // LVGL internal font
    |  |
    |  |__ tests
    |
    |-- lvgl_v9                               // LVGL v9
    :
    :
    |__ win32_sim
        :
        |__ port                              // Simulator porting
            |-- realgui_port                  // Simulator HoneyGUI porting
            |-- lvgl_port                     // Simulator LVGLv8 porting 
            |    |-- lv_conf.h                // Simulator LVGL configuration
            |    |-- lv_port_disp.c                  
            |    |-- lv_port_disp.h
            |    |-- lv_port_fs.c
            |    |-- lv_port_fs.h
            |    |-- lv_port_indev.c
            |    |__ lv_port_indev.h                      
            |
            |__ lvglv9_port                    // Simulator LVGLv9 porting 




1. In HoneyGUI, the LVGL source files are located in the directory :file:`your HoneyGUI dir/lvgl`:

   - demos: Contains various comprehensive built-in examples of LVGL. Some examples can be experienced on  `LVGL Demo <https://lvgl.io/demos>`_ .

   - docs: Contains the development documentation for LVGL. It can be read online on the LVGL documentation site: `LVGL Document <https://docs.lvgl.io/master/intro/index.html>`_.

   - env_support: Provides support for various environments or platforms.

   - examples: Stores the built-in examples of LVGL. They can be experienced on `LVGL Example <https://docs.lvgl.io/8.3/examples.html>`_ .

   - scripts: Contains some processing scripts that are not typically used when using LVGL.

   - src: Stores the actual source code of LVGL. When developing with LVGL, the code files from this directory are used.

   - tests: Contains some CI testing files that are not used when using LVGL.

2. When running LVGL with the HoneyGUI simulator, the LVGL UI will start running from the file :file:`app_ui_lvgl.c` under the directory :file:`your HoneyGUI dir/realgui/example/demo`.

3. When running LVGL with the HoneyGUI simulator, the root directory pointed to by the LVGL file system interface is :file:`your HoneyGUI dir/realgui/example/screen_lvgl/root/`.


Porting
==========================
- Documentation: `LVGL Porting <https://docs.lvgl.io/8.3/porting/index.html>`_

LVGL provides extensive porting support, allowing developers to easily integrate it into various embedded systems and platforms. It supports drivers for various display devices, touchscreens, input devices, and custom GPUs. Developers can configure the porting according to the requirements of their projects, such as adjusting the display parameters when changing display devices, or adapting the input interface when replacing input devices. This article focuses on the porting process and methods for display devices, input devices, and file systems. For more details, please refer to `LVGL Porting <https://docs.lvgl.io/8.3/porting/index.html>`_.

.. note:: 
    The following examples do not include the specific implementation of hardware device drivers. They only illustrate how to integrate drivers with the LVGL interface. When implementing hardware device drivers, developers can complete the driver functionality under a consistent API framework with the example driver, in order to interface with the HoneyGUI driver layer. The porting interfaces of the example projects can be reused in higher layers.

Display
----------------
- Documentation:  `LVGL Porting Display <https://docs.lvgl.io/8.3/porting/display.html>`_, `LVGL Overview Display <https://docs.lvgl.io/8.3/overview/display.html>`_

Once the developers have completed the debugging of the display device driver, and the device can communicate properly with the display device and show colors. This section explains how to interface the driver with LVGL's display interface to render LVGL's UI.”

The display interface of LVGL is implemented in the file :file:`lv_port_disp.c`. Display parameters are configured in the initialization function ``void lv_port_disp_init(void)``, such as screen size and frame buffer configuration. The display refresh function is defined as ``void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)``.

The file :file:`lv_port_disp.c` has been configured with different rendering and screen-pushing methods for reference. Configure :c:macro:`DISPLAY_FLUSH_TYPE` to switch modes, where :c:macro:`RAMLESS_XXX` is suitable for display ICs without RAM, :c:macro:`RAM_XXX` is suitable for display ICs with RAM, :c:macro:`XXX_FULL_SCREEN_XXX` indicates pushing the entire screen each time, and :c:macro:`XXX_TWO_SEC` indicates rendering only the changed display content, with the unit being the size of two buffers. The pixel height of the buffer is defined by :c:macro:`SECTION_HEIGHT`.

For detailed display device porting methods and considerations, please refer to the documentation `LVGL Porting Display <https://docs.lvgl.io/8.3/porting/display.html>`_. The following code snippet demonstrates porting a display IC without RAM:

- When using a display IC without RAM, a frame buffer that covers the entire screen size needs to be allocated. Therefore, two frame buffers with a size equal to the screen size are allocated on the PSRAM for display. The macro definitions for display parameters are defined in the file :file:`lv_conf.h`.
- If the display IC used has RAM, the size of the frame buffer does not need to be the same as the screen size. Due to different screen update methods, the :c:macro:`LVGL_USE_EDPI` in :file:`lv_port_disp.c` needs to be configured as not enabled (0) to switch the ``disp_flush`` function for screen update adaptation.

.. code-block:: c

    // flush func 1
    #define RAMLESS_TWO_FULL_SCREEN         0  // double buffer, full refresh

    // flush func 2
    #define RAM_TWO_FULL_SCREEN_NO_SEC      1  // double buffer, full refresh
    #define RAM_ONE_FULL_SCREEN_TWO_SEC     2  // two buffer
    #define RAM_DIRECT_TWO_SEC              3  // two buffer


    // two buffer: section height
    #define SECTION_HEIGHT                  40


    #define DISPLAY_FLUSH_TYPE              RAMLESS_TWO_FULL_SCREEN

    #if (DISPLAY_FLUSH_TYPE == RAMLESS_TWO_FULL_SCREEN)
    #define LVGL_USE_EDPI       1
    #else
    #define LVGL_USE_EDPI       0
    #endif


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
    #if (DISPLAY_FLUSH_TYPE == RAMLESS_TWO_FULL_SCREEN || DISPLAY_FLUSH_TYPE == RAM_TWO_FULL_SCREEN_NO_SEC)
        static lv_disp_draw_buf_t draw_buf_dsc_3;
        lv_color_t *buf_3_1 = (lv_color_t *)LV_PORT_BUF1;           /*A screen sized buffer*/
        lv_color_t *buf_3_2 = (lv_color_t *)LV_PORT_BUF2;           /*Another screen sized buffer*/
        lv_disp_draw_buf_init(&draw_buf_dsc_3, buf_3_1, buf_3_2,
                            MY_DISP_VER_RES * MY_DISP_HOR_RES);   /*Initialize the display buffer*/

        /*Set a display buffer*/
        disp_drv.draw_buf = &draw_buf_dsc_3;

        /*Required for Example 3)*/
        disp_drv.full_refresh = 1;

    #elif (DISPLAY_FLUSH_TYPE == RAM_DIRECT_TWO_SEC || DISPLAY_FLUSH_TYPE == RAM_ONE_FULL_SCREEN_TWO_SEC)
    #if 1
        static uint8_t __attribute__((aligned(4))) disp_buff1[MY_DISP_HOR_RES * SECTION_HEIGHT *
                                                                            LV_COLOR_DEPTH / 8];
        static uint8_t __attribute__((aligned(4))) disp_buff2[MY_DISP_HOR_RES * SECTION_HEIGHT *
                                                                            LV_COLOR_DEPTH / 8];
    #else
        uint8_t *disp_buff1 = lv_mem_alloc(MY_DISP_HOR_RES * SECTION_HEIGHT * LV_COLOR_DEPTH / 8);
        uint8_t *disp_buff2 = lv_mem_alloc(MY_DISP_HOR_RES * SECTION_HEIGHT * LV_COLOR_DEPTH / 8);
    #endif
        static lv_disp_draw_buf_t draw_buf_dsc_2;
        lv_color_t *buf_2_1 = (lv_color_t *)disp_buff1;
        lv_color_t *buf_2_2 = (lv_color_t *)disp_buff2;

        if (!buf_2_1 || !buf_2_2)
        {
            DBG_DIRECT("LVGL frame buffer is NULL");
            while (1);
        }
        lv_disp_draw_buf_init(&draw_buf_dsc_2, buf_2_1, buf_2_2,
                            MY_DISP_HOR_RES * SECTION_HEIGHT);   /*Initialize the display buffer*/

        /*Set a display buffer*/
        disp_drv.draw_buf = &draw_buf_dsc_2;

        /*Required for Example 2)*/
        disp_drv.full_refresh = 0;

        // disp_drv.rounder_cb = rounder_cb;

    #endif

        /*Finally register the driver*/
        lv_disp_drv_register(&disp_drv);
    }



Input Device
-----------------------------

- Documentation: `LVGL Porting Input Devices <https://docs.lvgl.io/8.3/porting/indev.html>`_

Once the developers have completed the debugging of the input device driver, and the device can communicate properly with the input device. This section explains how to interface the driver with LVGL's input interface to interact with LVGL's UI.

The input interface of LVGL is implemented in the file :file:`lv_port_indev.c`. Input device parameters are configured in the initialization function ``void lv_port_indev_init(void)``, including selecting the device type, etc. The input data acquisition function is configured in the function pointer ``indev_drv.read_cb``, which depends on the type of input device and is integrated in :file:`lv_port_indev.c`.

For detailed input device porting methods and considerations, please refer to the documentation `LVGL Porting Input Devices <https://docs.lvgl.io/8.3/porting/indev.html>`_. The following code snippet demonstrates porting a touch IC:

- In the initialization function ``void lv_port_indev_init(void)``, select and register the corresponding type of input device. For example, for a touchpad device, select **Touchpad**.

- LVGL will retrieve the input data through the function pointer ``indev_drv.read_cb``. Developers need to provide the input data in the function it points to. For a touch screen device, it would be the function ``void touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)``. For a touch screen input device, you only need to provide the coordinates of the touch point and the touch state.

.. code-block:: c

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





File System
----------------------------
- Documentation: `LVGL Overview File System <https://docs.lvgl.io/8.3/overview/file-system.html>`_

Using a file system to manage storage media makes data more organized and easier to maintain. It can improve compatibility and cross-platform support for external storage devices. Through the file system interface, developers can easily manipulate file data, making it more flexible and efficient. Integrating the file system with LVGL allows resource data to be stored separately from project code, reducing compilation time, improving development efficiency, and enhancing the flexibility of UI design.

The file system interface of LVGL is implemented in the file :file:`lv_port_fs.c`. The file system is configured in the initialization function ``void lv_port_fs_init(void)``, which includes initializing the file system and mounting drive letters. Developers need to integrate the interfaces of various file system functions into the corresponding LVGL fs porting functions, ensuring that the input and output data formats are consistent with the interface definitions.

For detailed file system porting methods and considerations, please refer to the documentation `LVGL Overview File System <https://docs.lvgl.io/8.3/overview/file-system.html>`_. The following example demonstrates the porting of **ROMFS**. 

.. note::
  ROMFS is a read-only file system, thus it does not support file writing.


.. code-block:: c

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



ROMFS File System Image
~~~~~~~~~~~~~~~~~~~~~~~~~

HoneyGUI provides support for packaging `ROMFS` file system images:

1. The working directory is :file:`your HoneyGUI dir/realgui/example/screen_lvgl/`. The packaging process requires Python environment support. The external file resources used in the project need to be packaged as a file system image and downloaded as :guilabel:`User Data`.

2. Open the working directory and place the files to be packaged in the :file:`root/` folder. Double-click the :file:`mkromfs_0x4600000.bat` script to generate the file system image :file:`root(0x4600000).bin` and the resource mapping address :file:`resource.h`. The default :guilabel:`base address` of the files is `0x4600000`. :file:`resource.h` records the mapping address of the packaged files. Since `ROMFS` supports direct access using physical addresses, developers can access the resource files directly through the mapping address.

3. Use the :guilabel:`User Data` feature of the MP Tool to download and burn the file system image to flash. The burn address should match the :guilabel:`base address`. If you need to modify the :guilabel:`base address`, you can modify the **"--addr <number>"** parameter in the :file:`mkromfs_0x4600000.bat` script. For example, the following example changes the :guilabel:`base address` from `0x4600000` to `0x4000000`.

.. code-block:: console

    # before - base address: 0x4600000, image: root(0x4600000).bin
    python _bin_mkromfs.py --binary --addr 0x4600000 root root(0x4600000).bin


    # after  - base address: 0x4000000, image: root(0x4000000).bin
    python _bin_mkromfs.py --binary --addr 0x4000000 root root(0x4000000).bin


.. note::
    1. This packaging tool is only applicable for creating filesystem images of ROMFS.
    2. The packaging process is not a simple concatenation of files; it also records the directory information and file details of the filesystem.



LittleFS File System Image
~~~~~~~~~~~~~~~~~~~~~~~~~

The LittleFS file system supports read and write operations and features power-loss protection. HoneyGUI provides packaging support for LittleFS file system images:

1. The working directory is :file:`your HoneyGUI dir/realgui/example/screen_lvgl/root_lfs`. External file resources used by the project will be packaged into a file system image and ultimately downloaded as :guilabel:`User Data`.
2. Open the working directory and place the files you need to package under the :file:`root/` folder. Double-click the script :file:`mklittlefs_img.bat` to generate the file system image :file:`root.bin`.
3. Use the :guilabel:`User Data` function in MP Tool to download and write the file system image to flash. To change the size of the file system, modify the **"-s <number>"** parameter in the script :file:`mklittlefs_img.bat`. When using interfaces from :file:`rtk_fs.c` for file operations, ensure that :c:macro:`RTK_FS_MNT_ADDR` matches the write address, and :c:macro:`MAX_LFS_SIZE` matches the file system size.
4. If you need to unpack a file system image, double-click the script :file:`unpack_littlefs_img.bat` to unpack :file:`root.bin` into the :file:`root_up/` folder.


.. code-block:: console

    # pack image:
    # -c <pack_dir>,  --create <pack_dir>
    # create littlefs image from a directory
    #
    # -b <number>,  --block <number>
    # fs block size, in bytes
    #
    # -p <number>,  --page <number>
    # fs page size, in bytes
    # 
    # -s <number>,  --size <number>
    # fs image size, in bytes

    mklittlefs.exe -c root/  root.bin  -b 4096  -s 512000 -p 16


    # unpack image:
    # -l,  --list
    # list files in littlefs image
    #
    # -u <dest_dir>,  --unpack <dest_dir>
    # unpack littlefs image to a directory

    mklittlefs.exe root.bin  -l
    mklittlefs.exe root.bin  -u root_up/

.. note::
    1. This packaging tool is only applicable for creating filesystem images of LittleFS.



LVGL Benchmark
==========================

LVGL Benchmark is a performance testing tool designed to evaluate the graphical display performance of the LVGL library in various hardware and software environments. By running the Benchmark, users can obtain data on frame rate, rendering speed, and memory usage, helping to optimize display configurations and debug performance issues. The Benchmark includes various test scenarios such as graphical drawing, animations, and text rendering, each simulating common operations in real applications. Users can use these tests to compare the performance of different configurations and platforms, enabling targeted optimization adjustments.
The official documentation for the LVGL benchmark test is located at :file:`your HoneyGUI dir/lvgl/demos/benchmark/README.md`.

Benchmark for Reference
-----------------------------

.. csv-table:: Benchmark Result
  :header: Chip Model, CPU CLK, Accelerator, Display Size, Buffering Configurations, Result
  :align: center
  
  RTL8762E, 40MHz, SW, 240*280, Double buffing, Weighted FPS:15; Opa. speed: 100%
  RTL8762E, 40MHz, SW, 80*160,  Double buffing, Weighted FPS:34; Opa. speed: 95%
  RTL8762D, 90MHz, SW, 240*280, Double buffing, Weighted FPS:161; Opa. speed: 77%
  RTL8762D, 90MHz, SW, 80*160, Double buffing, Weighted FPS:337; Opa. speed: 95%
  RTL8772G, 125MHz, PPE1.0, 480*480, Two buffer, Weighted FPS:20; Opa. speed: 100%
  RTL8772G, 125MHz, PPE1.0, 240*280, Double buffing, Weighted FPS:721; Opa. speed: 77%
  RTL8773E, 100MHz, PPE2.0, 390*450, Double buffing, Weighted FPS:159; Opa. speed: 86%

.. csv-table:: Render acceleration on different platforms
  :header: Chip Model, CPU CLK, Hardware Accelerator, Image Rendering, Image Transparency, Image Scaling, Image Rotation, Rounded Rectangle, Rectangle Filling, RLE Decoding, Characters, Lines
  :align: center
  
  RTL8772G, 125MHz, PPE1.0, HW, HW, HW, SW, SW+HW, HW, HW, SW, SW
  RTL8773E, 100MHz, PPE2.0, HW, HW, HW, HW, SW+HW, HW, HW, SW, SW




.. note::
  1. Effects involving LVGL Mask require SW processing.
  2. RTL8772G supports the Helium hardware accelerator.



Start with Demo
==========================
- `LVGL Demo <https://lvgl.io/demos>`_
- `LVGL Example <https://docs.lvgl.io/8.3/examples.html>`_

It is recommended for developers to read and understand the `LVGL Overview <https://docs.lvgl.io/8.3/overview/index.html>`_ and `LVGL Widgets - Base Object <https://docs.lvgl.io/8.3/widgets/obj.html>`_ sections before starting development. This will help them grasp the design concepts and logic of LVGL.

LVGL provides a rich set of demos and examples to help developers understand and familiarize themselves with the usage of various widgets and features.

- The `LVGL Demo <https://lvgl.io/demos>`_ showcases comprehensive demos with their source code stored in the directory :file:`your HoneyGUI dir/lvgl/src/demo`. Developers can directly invoke the corresponding ``lv_demo_xxx()`` function to explore and understand them.

- The online documentation `LVGL Example <https://docs.lvgl.io/8.3/examples.html>`_ demonstrates the running effects of various examples, with their source code stored in the directory :file:`your HoneyGUI dir/lvgl/src/example`. Developers can directly call the corresponding ``lv_example_xxx()`` function to familiarize themselves with widgets and understand their features.

Resource Converter
==========================
To use images and fonts in LVGL, they need to be converted to formats that LVGL can recognize using specific tools. LVGL supports converting resources to C array format and bin binary file format. 

In the C array format, the resources will be included in the compilation process. They will be compiled every time the program logic changes, and the size of the resources will be included in the APP image. 

In the bin binary file format, the resources are not included in the compilation. They are stored separately and require a file system or other means to access them. An example :file:`lvgl_example_assets.c` is provided in the path :file:`your HoneyGUI dir/realgui/example/screen_lvgl/assets/` to demonstrate how to configure resources of different formats for the widgets.

Image Converter
-----------------------------


LVGL Image Converter
~~~~~~~~~~~~~~~~~~~~~~~

- Online conversion tool: `LVGL Image Converter <https://lvgl.io/tools/imageconverter>`_
- Documentation: `LVGL Overview Images <https://docs.lvgl.io/8.3/overview/image.html>`_

Please refer to the following steps for usage in `LVGL Overview Images - Online Converter <https://docs.lvgl.io/8.3/overview/image.html#online-converter>`_:

1. Select the LVGL version.
2. Choose the image file.
3. Select the color format for the output file. 

   For color format details, please refer to  `LVGL Overview Images - Color Format <https://docs.lvgl.io/8.3/overview/image.html#color-formats>`_.
4. Choose the type of output image (C array/binary file).
5. Click :guilabel:`Convert` to obtain the output file.


The `LVGL Overview Images <https://docs.lvgl.io/8.3/overview/image.html>`_ document provides detailed instructions on how to use image resources and the image conversion tool in LVGL, along with simple usage examples. To automatically build image resources generated as C arrays, place them under the directory :file:`your HoneyGUI dir/realgui/example/screen_lvgl/assets/` directory.

It's worth mentioning that when using the bin file as an image resource, the data in the bin file follows the format of ``4 Byte header + data``. The ``lv_img_header_t`` contains information such as ``color format``, ``width``, and ``height``. To construct a complete ``lv_img_dsc_t`` to describe the image, you can calculate the ``data_size`` using the information from the `:`lv_img_header_t``.

.. code-block:: c

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


HoneyGUI Image Convert Tool
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
- Download link for the conversion tool: `HoneyGUI Image Convert Tool <https://docs.realmcu.com/HoneyGUI/cn/latest/tool/index.html>`_
- Documentation: `HoneyGUI Image Convert Tool - Doc <https://docs.realmcu.com/HoneyGUI/cn/latest/tool/Resource/image.html>`_

When further compression of image resource space is needed, the HoneyGUI Image Convert Tool supports compressing and converting images. The IC supports both software and hardware decoding. The HoneyGUI Image Convert Tool uses RLE (Run-length Encoding) compression, a simple lossless algorithm that reduces storage space by encoding consecutive repeated pixel values and the number of repetitions. It has low computational complexity and high compression rates, making it ideal for compressing GUI resources.

Compressing Images
^^^^^^^^^^^^^^^^^^^^^^
Users can utilize the HoneyGUI Image Convert Tool to convert image resources into RLE-compressed binary file format. For detailed usage steps, please refer to  `HoneyGUI Image Converter - Doc <https://docs.realmcu.com/HoneyGUI/cn/latest/tool/Resource/image.html>`_:

1. Select the image file to be compressed (supports PNG, JPEG, etc.)
2. Configure the image conversion parameters: enable :guilabel:`Compress`, choose :guilabel:`Compress Mode` as :guilabel:`RLE`, enable :guilabel:`Color Head`, and select :guilabel:`Color Space` as needed
3. Click to :guilabel:`Convert` and generate a compressed binary file

Importing into LVGL
^^^^^^^^^^^^^^^^^^^^^^^^^^

The binary files generated by the HoneyGUI Image Convert Tool can be imported into LVGL for use:

1. If importing as a file

 **Note**: Modify the file extension to **.rle**, then place it into the file system at :file:`your HoneyGUI dir/realgui/example/screen_lvgl/root`

   .. code-block:: c

    // file: lvgl_example_assets.c
    void load_img_rle_file(void)
    {
        lv_obj_t *icon = lv_img_create(lv_scr_act());
        lv_img_set_src(icon, "F:/logo_lvgl.rle");
        lv_obj_set_pos(icon, 0, 0);
    }


 **Note**: When using RLE + ROMFS, the decoder will directly retrieve images from the file system, i.e. FLASH, without additional caching. For situations that require caching processing, please read file from filesystem to memory, and use it as a array.

2. If imported as a C array format

 - a. Open the LVGL image conversion tool and upload the compressed file to be converted, please refer to `LVGL Image Converter`_.

 - b. In the :guilabel:`Color format` option, be sure to select **CF_RAW**

 - c. Export the converted image file as a C file, for example, :file:`logo_lvgl_rle.c`

  **Note1: The storage path of the converted file:** Place the converted C file in the following reference path: :file:`your HoneyGUI dir/realgui/example/screen_lvgl/assets` 

  **Note2: Modify the color format (cf) in the image descriptor:** The exported C file, for example :file:`logo_lvgl_rle.c`, needs to be modified to ensure ``cf: LV_IMG_CF_RAW``:
    
    .. code-block:: c

        // file:logo_lvgl_rle.c
        const lv_img_dsc_t logo_lvgl_rle = {
            .header.cf = LV_IMG_CF_RAW,
            .header.always_zero = 0,
            .header.reserved = 0,
            .header.w = 0,
            .header.h = 0,
            .data_size = 1889,
            .data = logo_lvgl_rle_map,
        };


 - d. Include the generated C file in your project and create the image object:

    .. code-block:: c

        // file:lvgl_example_assets.c
        void load_img_rle_c_file(void)
        {
            LV_IMG_DECLARE(logo_lvgl_rle);
            lv_obj_t *icon = lv_img_create(lv_scr_act());
            lv_img_set_src(icon, &logo_lvgl_rle);
            lv_obj_set_pos(icon, 0, 0);
        }


3. If importing as a file, accessing image resources using file addresses

 - a. Construct the LVGL image header ``lv_img_dsc_t``, for example:

    .. code-block:: c

        // file: lvgl_example_assets.c
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


 **Note: Set the color format in the image descriptor to cf = LV_IMG_CF_RAW**
 
 - b. Access the image resources and create the widget:

    .. code-block:: c

        // file: lvgl_example_assets.c
        void load_img_rle_dataAddr_file(void)
        {
            lv_obj_t *icon = lv_img_create(lv_scr_act());
            lv_img_set_src(icon, &lvgl_test_img_rle);
            lv_obj_set_pos(icon, 0, 0);
        }


Enabling RLE Decoder in LVGL
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
To decode RLE compressed image resources in LVGL, you need to enable the RLE decoder and allocate cache space for it.

1. Enable the RLE decoder: in the configuration file :file:`lv_conf.h`, locate the :c:macro:`LV_USE_RTK_IDU` macro definition and  set it to enable (1)

2. Allocate decoding cache: Configure the following parameters in the :file:`lv_conf.h` file:

- :c:macro:`LV_SSRAM_START`: The starting address of the cache

- :c:macro:`LV_SSRAM_SIZE`: Cache space size, ensuring that this size is sufficient to accommodate the decoding data of the largest entire image used


.. code-block:: c

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


.. note::
   When using the RLE decoder along with ROMFS, the decoder will directly obtain images from the file system, i.e., FLASH, without additional caching.

Font Converter
---------------------
- Online conversion tool: `LVGL Font Converter <https://lvgl.io/tools/fontconverter>`_
- Documentation: `LVGL Overview Fonts <https://docs.lvgl.io/8.3/overview/font.html>`_

Please refer to the following steps for usage in `LVGL Overview Font - Add a New Font <https://docs.lvgl.io/8.3/overview/font.html#add-a-new-font>`_ :

1. Set the name of the output font.
2. Set the height of the font in pixels.
3. Set the bpp (bits per pixel) of the font. 
   
   It represents how many bits are used to describe each pixel. Higher values result in better anti-aliasing and smoother edges, but larger font file size.
4. Choose the type of output font (C array/bin file).
5. Select the font file (TTF/WOFF).
6. Set the Unicode range of characters to convert, or directly list the characters that need to be converted.

The `LVGL Overview Fonts <https://docs.lvgl.io/8.3/overview/font.html>`_ document provides detailed instructions on how to use font resources and the font conversion tool in LVGL, along with simple usage examples. In the example, ``lv_example_label_3()`` demonstrates how to configure a specific font for a label widget. To automatically build font resources generated as C arrays, place them under the directory :file:`your HoneyGUI dir/realgui/example/screen_lvgl/assets/` directory.

LVGL provides built-in fonts, which are saved as arrays in the directory :file:`your HoneyGUI dir/lvgl/src/font/`. Each font file specifies the included characters at the beginning of the file. The built-in fonts include a Chinese font, :file:`lv_font_simsun_16_cjk.c`, which is a CJK (Chinese, Japanese, and Korean) 16px font, but it is a single font size with a limited character set.

Development Resources
==========================

Online Doucument
------------------
- `LVGL Document <https://docs.lvgl.io/master/intro/index.html>`_

The `LVGL Document <https://docs.lvgl.io/master/intro/index.html>`_ provides comprehensive technical documentation and tutorials to help developers better understand and use the LVGL graphics library. The documentation includes the following:

- Overview and features: The documentation introduces the basic concepts and features of LVGL, including graphical objects, screen management, event handling, theme styles, and more. Users can read the documentation to understand the core functionality and advantages of LVGL.

- Application development guide: The documentation provides detailed application development guides, including how to initialize and configure LVGL, how to create and manage graphical objects, how to handle user input and events, and how to add themes and styles. These guides can help users quickly get started with LVGL and develop their own applications.

- API documentation: The documentation provides a comprehensive list of LVGL's API interfaces and functions, along with their parameters and usage. Users can consult the API documentation to understand the specific functions and usage of individual functions and interfaces, enabling more advanced customization and extension.

- Example code: The documentation provides numerous example codes covering common application scenarios and functionalities. Users can leverage these example codes to accelerate development and quickly implement specific functionality requirements.

Using the LVGL online documentation can help users better understand and master the usage and techniques of LVGL, improving development efficiency. Users can gradually learn the contents of the documentation, starting from simple interface construction to complex application development, gradually mastering the various features and capabilities of LVGL. Additionally, the documentation provides examples and code snippets, making it easier for users to develop applications with rich interfaces and functionality.

Users can access the LVGL online documentation through a web browser and browse through the chapters and contents to find and learn relevant knowledge according to their needs. Additionally, the documentation provides a search function to quickly find specific information within the documentation. In summary, the LVGL online documentation is an important resource for users to understand and use the LVGL graphics library. It provides comprehensive and detailed guidance to help users quickly get started and develop better applications.

It is worth noting that while developing based on the documentation can complete most of the UI effects, the documentation may not be exhaustive. When there are omissions in the documentation, the code should be considered the most reliable source.

Github Repo
-----------------------------
- `Github LVGL <https://github.com/lvgl/lvgl>`_

The LVGL GitHub repository is an important platform for developers to use and contribute to LVGL:

- Getting the latest version: The LVGL GitHub repository provides access to the latest LVGL versions and updates. Developers can stay up-to-date with the latest feature updates, bug fixes, and improvements, keeping their applications in sync with LVGL.

- Engaging in the community and contributing code: Through the GitHub repository, developers can actively participate in LVGL community discussions and exchanges, learning about other developers' issues and solutions. At the same time, developers can contribute their own code and improvements, making LVGL more robust and powerful.

- Submitting issues and bug reports: The GitHub repository offers a platform for issue and bug reporting, allowing developers to submit problems and bugs encountered during their use of LVGL. This helps the LVGL development team promptly discover and resolve issues, improving the stability and reliability of LVGL.

- Learning from examples and documentation: The GitHub repository also includes example code and documentation to help developers better understand and learn how to use LVGL. By browsing the repository's example code and documentation, developers can learn about the various features and capabilities of LVGL, enhancing their development skills.


Designer
-----------------------------

.. <!-- - GUI Guider: [NXP GUI Guider](https://www.nxp.com/design/design-center/software/development-software/gui-guider:GUI-GUIDER) -->

- GUI Guider: Free
- Squareline: `Squareline Studio <https://squareline.io/>`_, Paid


The LVGL Designer is a visual tool for designing and developing interfaces for the LVGL graphics library. It provides an intuitive and user-friendly interface that allows developers to quickly create and edit GUI interfaces using LVGL.

The LVGL Designer has the following features and functionalities:

- Visual interface design: The designer provides an intuitive visual interface where developers can create and edit GUI interfaces using mouse and simple drag-and-drop operations. It allows adding and adjusting various graphical objects, labels, buttons, text boxes, images, and more, while setting their size, position, style, and other attributes.

- Real-time preview and debugging: The designer supports real-time preview, allowing developers to see the appearance and behavior of the designed interface at any time. This helps developers quickly adjust and optimize the interface to achieve the desired effect.

- Event and interaction management: The designer enables developers to conveniently add and manage events and interaction behaviors. Developers can add click, scroll, drag, and other events to graphical objects and configure their response behaviors through simple configurations.

- Theme and style customization: The designer supports customization of themes and styles, allowing developers to easily select and apply different themes and styles to make the interface more personalized and visually appealing.

- Code export: The designer allows exporting the designed interface as LVGL code, providing the necessary initialization and configuration. This enables developers to directly use the exported code for LVGL application development, eliminating the need for manual code writing.

Using the LVGL Designer greatly accelerates the design and development process of GUI interfaces, especially for non-professional UI designers or developers. With simple drag-and-drop and configuration operations, developers can quickly create attractive and interactive interfaces, improving development efficiency and user experience. Additionally, the designer provides a convenient way to export the designed interface as usable LVGL code, allowing developers to easily integrate it into their applications.


Forum
-----------------------------
- `LVGL Forum <https://forum.lvgl.io/>`_

The official LVGL forum is a developer community dedicated to discussing and sharing topics and resources related to the LVGL graphics library. It provides a platform for developers to exchange ideas, seek help, and share their experiences and projects.

Some features and functionalities of the LVGL forum include:

- Questions and answers: Developers can ask questions about their LVGL usage on the forum and receive help and answers from other developers. This makes the forum a valuable knowledge base, providing experience and tips for problem-solving.

- Tutorials and examples: The forum contains many useful tutorials and example code, demonstrating how to use different features and functionalities of LVGL. These resources are helpful for novice developers to learn and master LVGL.

- Developer contributions and project showcases: Developers on the forum can share their projects and customized LVGL interfaces, as well as contributions that other developers can share, discuss, and reference.

- Updates and release announcements: The LVGL development team provides announcements and explanations about new version releases and updates on the forum. This allows developers to stay informed about the latest features and improvements.

- Community interaction: The forum provides a platform for community interaction, where developers can communicate, share, and establish connections, enhancing collaboration and development within the LVGL community.

The LVGL forum is an important resource for developers using LVGL to receive support, solve problems, learn, and share experiences.



Blog
-----------------------------
- `LVGL Blog <https://blog.lvgl.io/>`_

The official LVGL blog is a regularly updated platform that provides the latest information, tutorials, case studies, and developer insights about the LVGL graphics library. The LVGL development team and community members frequently publish various content related to LVGL on the blog, helping developers better understand and use LVGL.

The LVGL blog covers the following content:

- Updates and new feature introductions: The blog publishes articles on the latest version of LVGL, highlighting new features, bug fixes, and performance improvements. This allows developers to stay up-to-date and leverage the latest LVGL capabilities.

- Tutorials and guides: The blog provides practical tutorials and guides on LVGL, covering various topics ranging from beginner to advanced. These tutorials often include example code and detailed explanations, helping developers master the usage of LVGL and best practices.

- Case studies and project showcases: The blog shares case studies and project showcases implemented with LVGL. These articles demonstrate how to use LVGL to build real-world applications and interfaces, providing developers with inspiration and experience from practical implementations.

- Technical deep dives and developer insights: The blog also covers in-depth analyses of LVGL and insights from developers. These articles may explore topics such as the internal workings of LVGL, performance optimization techniques, and excellent design practices, providing developers with a deeper understanding and food for thought.

The LVGL blog is an important resource for developers to understand and master LVGL. By reading the blog, developers can gain insights on the latest LVGL developments, learning materials, and technical insights, helping them utilize LVGL to build excellent graphical interfaces.

FAQ
=======
- `LVGL FAQ <https://docs.lvgl.io/8.3/intro/index.html#faq>`_

HoneyGUI vs LVGL Picture Drawing Frame Rate
--------------------------------------------


GRAM Screen (280x456) RAM Block Drawing
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Background: RTL8772G, RGB565, uncompressed images, test for the performance of displaying a single image (HoneyGUI rectangle fill data is temporarily unavailable; LVGL has not adapted PPE hardware acceleration for image scaling yet).

.. csv-table:: RAM Block Drawing
  :header: Test Case, HoneyGUI FPS (SW), HoneyGUI FPS (PPE), LVGL FPS (SW), LVGL FPS (PPE)
  :align: center
  
  Draw Image,             73,     74,     70,     73
  Fill Rectangle,          3,     85,     74,     74
  Rotate Image 45°,        3,      3,      4,      4
  Scale Up 1.5x ,          3,     31,      3,     25
  Scale Down 0.5x ,        9,     73,     12,     25


.. csv-table:: RAM Block Drawing Test Data
  :header: Section, HoneyGUI FPS, LVGL FPS
  :align: center
  
  10,   70,     45
  20,   73,     73
  30,   74,     73

PSRAM Full Frame Buffer Drawing (800x480)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Background: RTL8772G, RGB565, image size 315x316, uncompressed images, RGB screen, test for the performance of displaying a single image.


.. csv-table:: PSRAM Full Frame Buffer Drawing
  :header: Test Case, HoneyGUI FPS (SW), HoneyGUI FPS (PPE), LVGL FPS (SW), LVGL FPS (PPE)
  :align: center
  
  Draw Image,             76,     76,     17,     25
  Fill Rectangle,          4,     78,     25,     26
  Rotate Image 45°,        3,      3,      6,      4
  Scale Up 1.5x ,          2,     23,      3,     13
  Scale Down 0.5x ,       10,     82,     13,     50


Analysis
~~~~~~~~~~~~~~~~~~~~~~~~~

Extra PSRAM is required for RGB screen as a cache buffer. LVGL uses PSRAM completely as its image cache buffer compared to HoneyGUI which combines RAM and PSRAM. LVGL performs worse overall.


HoneyGUI vs LVGL RAM Consumption
----------------------------------


.. csv-table:: GRAM Screen (280x456) Dynamic RAM Consumption
  :header: Test Case, HoneyGUI (Bytes), LVGL Widget Consumption (Bytes)
  :align: center
  
  Draw Image,          156,     176
  Fill Rectangle,       64,     200
  Rotate Image 45°,    156,     208
  Scale Up 1.5x ,      156,     208
  Scale Down 0.5x ,    156,     176



.. csv-table:: GRAM Screen (280x456) Static RAM Consumption
  :header: Test Case, HoneyGUI (Bytes), LVGL Widget Consumption (Bytes)
  :align: center
  
  Draw Image,           41892(40KB),     55300(54KB)
  Fill Rectangle,       41892(40KB),     55300(54KB)
  Rotate Image 45°,     41892(40KB),     55300(54KB)
  Scale Up 1.5x ,       41892(40KB),     55300(54KB)
  Scale Down 0.5x ,     41892(40KB),     55300(54KB)



Conclusion
------------

- **Applicable Scenarios**: For large screen sizes (e.g. 800x480) and full-frame drawing, HoneyGUI is recommended. For frequent partial screen refresh projects, LVGL is recommended. For block drawing when RAM resources are tight, HoneyGUI is recommended, with section recommended parameters set to 10.
- **Rotation, Scaling**: LVGL performs faster in 2D rendering using a 2x2 matrix compared to HoneyGUI's 3x3 matrix, which handles more data for 2D rendering. For 2.5D or pseudo-3D effects, HoneyGUI will perform better.
- In practical projects, select the suitable framework based on specific frame rate requirements, system resources, and other functional needs. Conduct specific performance testing and evaluation if possible.

This analysis provides valuable insights for selecting the appropriate display framework and assists decision-makers in making the best choice based on actual requirements.
