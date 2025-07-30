===============================
Use LVGL V9 to Design
===============================


LVGL Benchmark Test
==========================
.. <!-- - benchmark introduction
.. - reference metrics, significance
.. - benchmark data reference on different platforms
.. - (introduction, demonstration, score statistics) (performance comparison: comparison methods/test items, TBD) -->


LVGL Benchmark is a performance testing tool used to evaluate the graphics display performance of the LVGL library across various hardware and software environments. By running the Benchmark, users can obtain data such as frame rates, rendering speeds, and memory usage, which helps optimize display configurations and debug performance issues. The Benchmark includes multiple test scenarios, such as graphics drawing, animations, and text rendering, with each scenario simulating common operations in actual applications. Users can compare performance across different configurations and platforms through these tests to make targeted optimization adjustments.
The official documentation for LVGL benchmarking can be found at :file:`your HoneyGUI dir/lvgl/demos/README.md`.

Benchmark Reference
-----------------------------
Platform background: 8773G platform, 200MHz main frequency, QSPI410*502 screen, 96KB RAM with 3M PSRAM, dual buffer mode, direct screen push mode.
Compilation environment: armclang6.22 version, optimization mode -Ofast to enable LTO; gcc 12.2 version, optimization mode -Ofast.

.. csv-table:: Benchmark Test Results with Different Acceleration Methods
   :header: Scenario, SW Rendering Time, SW+MVE Rendering Time, SW+MVE+ARM2D Rendering Time, SW+MVE+ARM2D+PPE Rendering Time
   :align: center

   empty screen, 4, 5, 4, 4
   moving wallpaper, 16, 16, 16, 11
   single rectangle, 0, 0, 0, 0
   multiple rectangles, 4, 5, 4, 2
   multiple RGB images, 5, 5, 5, 2
   multiple ARGB images, 10, 10, 9, 2
   rotated ARGB images, 11, 9, 12, 0
   multiple labels, 8, 9, 9, 9
   screen sized text, 32, 42, 31, 30
   multiple arcs, 6, 7, 6, 5
   containers, 6, 7, 6, 6
   containers with overlay, 27, 27, 21, 24
   containers with opa, 9, 9, 11, 7
   containers with opa_layer, 15, 18, 11, 10
   containers with scrolling, 23, 24, 20, 21
   widget demo, 31, 31, 29, 30
   all screen, 12, 14, 11, 10


.. csv-table:: Benchmark Test Results in Different Compilation Environments
  :header: Scenario, Acceleration Method, FreeRTOS, Zephyr
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
- Documentation: `LVGL Overview Images <https://docs.lvgl.io/9.1/overview/image.html>`_

The LVGL Online Image Converter is a website provided by the LVGL development team that allows you to upload local image files and convert them into standard C files. The converted files describe image information in the form of variables. For usage steps, please refer to `LVGL Overview Images - Online Converter <https://docs.lvgl.io/9.1/overview/image.html#online-converter>`_：

1. Select the LVGL Version: Choose ``LVGL v9``
2. Upload Image Files: Select one or more local image files for batch conversion.

   The output C files will have the same name as the input files, and the variable name describing the image will also match the input file name. Therefore, avoid using Chinese characters or any illegal characters in the file names.
3. Choose Output Color Format:
   
   For an explanation of color formats, refer to  `LVGL Overview Images - Color Format <https://docs.lvgl.io/9.1/overview/image.html#color-formats>`_.
4. Click :guilabel:`Convert` to obtain the output file

The document `LVGL Overview Images <https://docs.lvgl.io/9.1/overview/image.html>`_ provides a detailed introduction on using image resources and the image converter tool in LVGL, as well as simple usage examples.


The online image converter for LVGL v9 only supports output in C file format and a limited selection of color formats. If you need features such as image compression, bin file output, or other color formats, please use the  `Python Image Converter <https://github.com/lvgl/lvgl/blob/master/scripts/LVGLImage.py>`_。
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
For more details on using image resources and the image conversion tool in LVGL, as well as simple usage examples, please refer to the documentation: `LVGL Overview Images <https://docs.lvgl.io/9.1/overview/image.html>`_ 


.. image:: https://foruda.gitee.com/images/1753864909573136949/b12eb86a_9218678.png
   :align: center
   :width: 408



RTZIP
^^^^^^^^^
This configuration encodes images into the RTZIP format, which supports multiple compression algorithms and customizable compression parameters. RTZIP is a lossless compression format and supports encoding of transparent layers.

Tips:
  - If the input image contains transparency but the output format does not support alpha (such as when outputting in RGB565), it is recommended to enable the :guilabel:`Mix Alpha Channel` option. Example: Input: PNG with transparency → Output: RGB565 without alpha.

.. image:: https://foruda.gitee.com/images/1753864936117526804/e64c6628_9218678.png
   :align: center
   :width: 408


JPG
^^^^^^^^^
This configuration re-encodes images into the JPEG format. You can adjust the image sampling ratio and quality settings to reduce the file size. JPEG is a lossy compression format that achieves a good balance between display quality and file size. Please note that images converted to JPEG will lose any transparency (alpha channel) information.

.. image:: https://foruda.gitee.com/images/1753864922836242088/290c1bd8_9218678.png
   :align: center
   :width: 408



AVI
^^^^^^^^^
This configuration converts an image sequence or video file into AVI format, supporting frame rate and image quality settings. Note that the output AVI file does not include an audio track.

- If the input is an image sequence folder, the conversion settings will be applied to all images in the folder. Please ensure that the images are named in the pattern ``xxx_%04d.xxx``, starting from 0 (e.g., frame_0000.jpg, frame_0001.jpg).
- If the input is a video file, the settings will be applied directly to that video file.


.. image:: https://foruda.gitee.com/images/1753864947902502287/d16f5802_9218678.png
   :align: center
   :width: 408


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


