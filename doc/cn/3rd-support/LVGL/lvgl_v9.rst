==========================
使用 LVGL V9 设计
==========================


LVGL Benchmark 测试
==========================
.. <!-- - benchmark 介绍
.. - 参考指标，意义
.. - 不同平台上的 benchmark 数据参考
.. -（介绍、演示、分数统计）(性能对比：对比方式/测项，待定) -->


LVGL 的 Benchmark 是一个性能测试工具，用于评估 LVGL 库在各种硬件和软件环境下的图形显示性能。通过运行 Benchmark，用户可以获取帧率、渲染速度和内存使用情况等数据，从而帮助优化显示配置和调试性能问题。Benchmark 包括多种测试场景，如图形绘制、动画和文本渲染，每个场景模拟实际应用中的常见操作。用户可以通过这些测试来比较不同配置和平台的性能表现，从而做出针对性的优化调整。
LVGL 基准测试的官方文档位于 :file:`your HoneyGUI dir/lvgl/demos/README.md`。

参考 Benchmark 
-----------------------------
平台背景，8773G平台，200MHz主频，QSPI410*502屏幕，96KB RAM搭配3M PSRAM，双buffer模式，推屏方式direct。
编译环境：armclang6.22版本，优化方式-Ofast 开启LTO；gcc 12.2版本，优化方式-Ofast。

.. csv-table:: Benchmark 不同加速方式测试结果
   :header: 场景, SW渲染时间, SW+MVE渲染时间, SW+MVE+ARM2D渲染时间, SW+MVE+ARM2D+PPE渲染时间
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


.. csv-table:: Benchmark 不同编译环境测试结果
  :header: 场景, 加速方式, FreeRTOS, Zephyr
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

.. csv-table:: 不同平台渲染加速
  :header: 芯片型号, 处理器主频, 硬件加速器, 图片绘制, 图片透明度, 图片缩放, 图片旋转, 圆角矩形, 矩形填充, RLE 解码, 字符, 线条
  :align: center
  
  RTL8772G, 125MHz, PPE1.0, HW, HW, HW, SW, SW+HW, HW, HW, SW, SW
  RTL8773E, 100MHz, PPE2.0, HW, HW, HW, HW, SW+HW, HW, HW, SW, SW
  RTL8773G, 200MHz, PPE2.0, HW, HW, HW, HW, SW+HW, HW, HW, SW, SW


.. note::
  1. 涉及 LVGL Mask 的效果均需要 SW 处理
  2. RTL8772G 支持 Helium 硬件加速器
