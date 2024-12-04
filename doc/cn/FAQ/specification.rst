.. _FAQ_Specification_CN:

======
规格
======

图形
----

.. list-table::
   :header-rows: 1

   * - 平台
     - RTL8762D
     - RTL8772F
     - RTL87X2G
     - RTL8763E
     - PC
   * - RGB565
     - 是
     - 是
     - 是
     - 是
     - 是
   * - RGB888
     - 否
     - 是
     - 是
     - 否
     - 是
   * - ARGB8888
     - 否
     - 是
     - 是
     - 否
     - 是
   * - SVG
     - 否
     - 是
     - 否
     - 否
     - 是
   * - TTF
     - 否
     - 是
     - 否
     - 否
     - 是
   * - DOT字体
     - 是
     - 是
     - 是
     - 是
     - 是
   * - 矢量图形
     - 否
     - 是
     - 否
     - 否
     - 是
   * - 线性渐变
     - 否
     - 是
     - 否
     - 否
     - 是
   * - 径向渐变
     - 否
     - 否
     - 否
     - 否
     - 是

内存使用量
----------

RTL8772F示例
~~~~~~~~~~~~~

.. raw:: html

   <div style="text-align: center"><iframe src="https://docs.realmcu.com/HoneyGUI/image/FAQ/8772F_demo.mp4" width="640" height="480" allow="autoplay"></iframe></div><br>

该示例的内存消耗统计如下：

.. list-table::
   :header-rows: 1

   * - 模块
     - 占用
   * - 控件
     - 14.56KB
   * - 帧缓冲区
     - 屏幕宽度 *像素字节数* 行数
   * - 线程堆栈
     - 10KB

控件内存使用量
~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1

   * - 控件
     - 内存(字节)系统芯片
     - 内存(字节)模拟器
   * - 对象(obj)
     - 52
     - 88
   * - 图像(img)
     - 112
     - 178
   * - 窗口(win)
     - 72
     - 112
   * - 页面(page)
     - 124
     - 184
   * - 选项卡(tab)
     - 88
     - 136
   * - 选项卡视图(tabview)
     - 100
     - 160
   * - 按钮(button)
     - 88
     - 160
   * - 文本框(text)
     - 100
     - 176
   * - 滚动文本框(scroll_text)
     - 120
     - 200
   * - 应用(app)
     - 92
     - 152
   * - 画布-圆弧(canvas_arc)
     - 156
     - 264
   * - 画布-矩形(canvas_rect)
     - 64
     - 104
   * - 画布(canvas)
     - 60
     - 104
   * - 卡片(card)
     - 72
     - 112
   * - 卡片视图(cardview)
     - 124
     - 176
   * - 调色盘(colorwheel)
     - 72
     - 112
   * - 立方体(cube)
     - 748
     - 928
   * - 窗帘(curtain)
     - 60
     - 96
   * - 窗帘视图(curtainview)
     - 120
     - 168
   * - 画廊(gallery)
     - 112
     - 184
   * - 网格(grid)
     - 100
     - 144
   * - 动态图像(img_live)
     - 84
     - 144
   * - 范围图像(img_scope)
     - 124
     - 192
   * - stb 图像(stb_img)
     - 76
     - 144
   * - 键盘(kb)
     - 108
     - 192
   * - 地图(map)
     - 196
     - 272
   * - 蜂窝菜单(menu_cellular)
     - 76
     - 120
   * - 多级菜单(multi_level)
     - 60
     - 104
   * - 页面列表(pagelist)
     - 96
     - 160
   * - 页面列表视图(pagelistview)
     - 64
     - 112
   * - 透视(perspective)
     - 736
     - 920
   * - 进度条(progressbar)
     - 80
     - 136
   * - 二维码和条形码(qbcode)
     - 84
     - 136
   * - 滚轮(scroll_wheel)
     - 388
     - 696
   * - 进度条(seekbar)
     - 128
     - 216
   * - 简单图像(simple_img)
     - 68
     - 120
   * - 可缩放矢量图形(svg)
     - 96
     - 144
   * - 轮盘(turn_table)
     - 128
     - 192
   * - 监视渐变点(watch_gradient_spot)
     - 60
     - 96
   * - 波(wave)
     - 72
     - 112
   * - 轮形列表(wheel_list)
     - 64
     - 112
