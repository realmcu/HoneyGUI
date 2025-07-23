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