.. _FAQ_Specification_EN:

==============
Specification
==============

Graphics
--------

.. list-table::
   :header-rows: 1

   * - Platform
     - RTL8762D
     - RTL8772F
     - RTL87X2G
     - RTL8763E
     - term:`PC`
   * - RGB565
     - Y
     - Y
     - Y
     - Y
     - Y
   * - RGB888
     - N
     - Y
     - Y
     - N
     - Y
   * - ARGB8888
     - N
     - Y
     - Y
     - N
     - Y
   * - SVG
     - N
     - Y
     - N
     - N
     - Y
   * - TTF
     - N
     - Y
     - N
     - N
     - Y
   * - DOT font
     - Y
     - Y
     - Y
     - Y
     - Y
   * - Vector Graphics
     - N
     - Y
     - N
     - N
     - Y
   * - Linear gradient
     - N
     - Y
     - N
     - N
     - Y
   * - Radial gradient
     - N
     - N
     - N
     - N
     - Y

Memory Usage
------------

RTL8772F Demo
~~~~~~~~~~~~~

.. raw:: html

   <div style="text-align: center">
       <video width="640" height="480" controls>
           <source src="https://docs.realmcu.com/HoneyGUI/image/FAQ/8772F_demo.mp4" type="video/mp4">
           Your browser does not support HTML5 video.
       </video>
   </div>
   <br>


The memory consumption statistics of this demo are as follows.

.. list-table::
   :header-rows: 1

   * - Module
     - Cost
   * - Widget
     - 14.56KB
   * - Framebuffer
     - screenWidth * pixelBytes * Lines
   * - Thread stack
     - 10KB

Widget Memory Usage
~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1

   * - Widget
     - Memory(Byte) on ARM SoC
     - Memory(Byte) on Win_32 SIM
   * - obj
     - 52
     - 88
   * - img
     - 112
     - 178
   * - win
     - 72
     - 112
   * - page
     - 124
     - 184
   * - tab
     - 88
     - 136
   * - tabview
     - 100
     - 160
   * - button
     - 88
     - 160
   * - text
     - 100
     - 176
   * - scroll_text
     - 120
     - 200
   * - app
     - 92
     - 152
   * - canvas_arc
     - 156
     - 264
   * - canvas_rect
     - 64
     - 104
   * - canvas
     - 60
     - 104
   * - card
     - 72
     - 112
   * - cardview
     - 124
     - 176
   * - colorwheel
     - 72
     - 112
   * - cube
     - 748
     - 928
   * - curtain
     - 60
     - 96
   * - curtainview
     - 120
     - 168
   * - gallery
     - 112
     - 184
   * - grid
     - 100
     - 144
   * - img_live
     - 84
     - 144
   * - img_scope
     - 124
     - 192
   * - stb_img
     - 76
     - 144
   * - kb
     - 108
     - 192
   * - map
     - 196
     - 272
   * - menu_cellular
     - 76
     - 120
   * - multi_level
     - 60
     - 104
   * - pagelist
     - 96
     - 160
   * - pagelistview
     - 64
     - 112
   * - perspective
     - 736
     - 920
   * - progressbar
     - 80
     - 136
   * - qbcode
     - 84
     - 136
   * - scroll_wheel
     - 388
     - 696
   * - seekbar
     - 128
     - 216
   * - simple_img
     - 68
     - 120
   * - svg
     - 96
     - 144
   * - turn_table
     - 128
     - 192
   * - watch_gradient_spot
     - 60
     - 96
   * - wave
     - 72
     - 112
   * - wheel_list
     - 64
     - 112
