====
控件
====

==================                   ==================
控件                                 简介
------------------                   ------------------
`对象 (Obj)`_                        控件树的根控件
`窗口 (Window)`_                     矩形容器控件
`图像 (Img)`_                        屏幕上显示图像
`简单图像 (Simple Img)`_             简化的图像控件
`STB图像 (STB Img)`_                使用stb库解码标准图像格式文件并显示
`文本 (Text)`_                       显示文本，以不同的字体、颜色和大小输出文本
`选项卡容器 (Tabview)`_              屏幕从屏幕中的交叉方向扩展，通过滑动切换当前显示的卡片
`选项卡 (Tab)`_                      屏幕从屏幕中的交叉方向扩展，通过滑动切换当前显示的卡片
`幕布容器 (Curtainview)`_            滑动扩展和收缩窗帘
`幕布 (Curtain)`_                    滑动扩展和收缩窗帘
`卡片容器 (Cardview)`_               显示一个3D卡片集合
`卡片 (Card)`_                       显示一个3D卡片集合
`页面 (Page)`_                       上下滑动显示不同垂直方向控件的创建
`页面列表 (Page List)`_               像页面控件一样滑动，可以使用较少的RAM显示子控件
`开关 (Switch)`_                     监听开关手势
`网格 (Grid)`_                       以网格视图显示控件（垂直和水平均匀布局）
`进度条 (Progressbar)`_              通过显示部分图片、一帧图片或绘制指定长度的进度条来显示进度
`画布 (Canvas)`_                     使用nanovg库绘制2D图形
`立方体 (Cube)`_                     显示一个立方体
`轮形列表 (Wheel List)`_              轮形的应用程序列表
`透视 (Perspective)`_                滑动扩展和收缩窗帘
`可缩放矢量图形 (SVG)`_
`蜂窝菜单 (Menu Celllar)`_           显示成蜂窝形状菜单设置的图标
`返回 (Return)`_                     可以拖出的泡泡形状曲线
`二维码或条形码 (Qbcode)`_           显示二维码或条形码
`画布矩形 (Canvas_rect)`_            显示一个矩形
`画布圆角矩形 (Canvas_round_rect)`_  显示一个圆角矩形
`画布弧形 (Canvas_arc)`_             显示一个弧形
`多级菜单 (Multi Level)`_            管理和操作多层子窗口
`趣戴特效 (LiteGFX)`_                第三方合作特效
`2.5D足球 (2.5D Soccer)`_            2.5D足球特效
==================                   ==================

.. _对象 (Obj): gui_obj.html
.. _窗口 (Window): gui_window.html
.. _图像 (Img): gui_img.html
.. _简单图像 (Simple Img): gui_simple_img.html
.. _STB图像 (STB Img): gui_img_stb.html
.. _文本 (Text): gui_text.html
.. _选项卡容器 (Tabview): gui_tabview.html
.. _选项卡 (Tab): gui_tab.html
.. _幕布容器 (Curtainview): gui_curtainview.html
.. _幕布 (Curtain): gui_curtain.html
.. _卡片容器 (Cardview): gui_cardview.html
.. _卡片 (Card): gui_card.html
.. _页面 (Page): gui_page.html
.. _页面列表 (Page List): gui_pagelist.html
.. _开关 (Switch): gui_switch.html
.. _网格 (Grid): gui_grid.html
.. _进度条 (Progressbar): gui_progressbar.html
.. _画布 (Canvas): gui_canvas.html
.. _立方体 (Cube): gui_cube.html
.. _轮形列表 (Wheel List): gui_wheel_list.html
.. _透视 (Perspective): gui_perspective.html
.. _可缩放矢量图形 (SVG): gui_svg.html
.. _监视点渐变 (Watch Gradient Spot): gui_watch_gradient_spot.html
.. _表盘渐变 (Watchface Gradient): gui_watchface_gradient.html
.. _蜂窝菜单 (Menu Celllar): gui_menu_cellular.html
.. _返回 (Return): gui_return.html
.. _二维码或条形码 (Qbcode): qbcode.html
.. _多级菜单 (Multi Level): gui_multi_level.html
.. _趣戴特效 (LiteGFX): gui_widget_litegfx.html
.. _2.5D足球 (2.5D Soccer): gui_soccer.html

.. csv-table:: 缩略语
   :header: 词,定义
   :widths: 50 100
   :name: 缩略语

   acc,加速
   addr,地址
   att,属性
   ax,x轴上的绝对坐标
   blit,位块图像传输
   buff,缓冲区
   cb,回调函数
   cbsize,立方体大小
   ctor,构造函数
   cur_idx,x方向上的当前索引
   cur_idy,y方向上的当前索引
   cx,x轴上的中心坐标
   dc,显示画布
   dur,持续时间
   dx,触摸板沿x轴的偏移
   fd,文件描述符
   fg,前景
   fs,文件系统
   hw,硬件
   id,索引
   img,图像
   info,信息
   init,初始化
   mem,内存
   mq,消息队列
   nz,平面Z方向的法向量
   obj,对象
   off,偏移
   pic,图片
   pos,位置
   prev,先前的
   rst,结果
   src,源
   sx,x方向缩放
   tmp,临时的
   tx,x方向的平移



.. toctree::
    :maxdepth: 2

    对象 (Obj) <gui_obj>
    窗口 (Window) <gui_window>
    图像 (Img) <gui_img>
    简单图像 (Simple Img) <gui_simple_img>
    STB图像 (STB Img) <gui_img_stb>
    文本 (Text) <gui_text>
    选项卡容器 (Tabview) <gui_tabview>
    选项卡 (Tab) <gui_tab>
    幕布容器 (Curtainview) <gui_curtainview>
    幕布 (Curtain) <gui_curtain>
    卡片容器 (Cardview) <gui_cardview>
    卡片 (Card) <gui_card>
    页面 (Page) <gui_page>
    页面列表 (Page List) <gui_pagelist>
    开关(Switch) <gui_switch>
    网格 (Grid) <gui_grid>
    进度条 (Progressbar) <gui_progressbar>
    画布 (Canvas) <gui_canvas>
    立方体 (Cube) <gui_cube>
    轮形列表 (Wheel List) <gui_wheel_list>
    透视 (Perspective) <gui_perspective>
    可缩放矢量图形 (SVG) <gui_svg>
    监视点渐变 (Watch Gradient Spot) <gui_watch_gradient_spot>
    表盘渐变 (Watchface Gradient) <gui_watchface_gradient>
    蜂窝菜单 (Menu Celllar) <gui_menu_cellular>
    返回 (Return) <gui_return>
	二维码或条形码 (Qbcode) <gui_qbcode>
    多级菜单 (Multi Level) <gui_multi_level>
    趣戴特效 (LiteGFX) <gui_widget_litegfx>
    2.5D足球 (2.5D Soccer) <gui_soccer>
    3D模型 (3D Model) <gui_3D>
    棱镜模型 (Prism Mirror 3D Model) <gui_prism_mirror3d>
