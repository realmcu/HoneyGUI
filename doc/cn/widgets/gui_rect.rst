.. _Rounded_Rectangle (Geometry Rounded Rectangle):

圆角矩形 (Geometry Rounded Rectangle)
=====================================

概述
----

圆角矩形控件是一个轻量级的 :term:`GUI` 绘图组件，专门用于在用户界面中绘制带圆角的矩形图形。该控件提供了简洁易用的 :term:`API` ，支持自定义圆角矩形的尺寸、圆角半径、颜色等属性，能够创建现代风格的 :term:`UI` 元素。

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1763457468837106189/45d195d0_13406851.png" width= "400" /></div>
   <br>

核心功能
--------

.. list-table::
   :header-rows: 1

   * - 描述
     - API 
   * - 创建控件
     - :cpp:any:`gui_rect_create`  
   * - 设置属性
     - :cpp:any:`gui_rect_set_style`
   * - 设置位置                      
     - :cpp:any:`gui_rect_set_position` 
   * - 设置尺寸
     - :cpp:any:`gui_rect_set_size` 
   * - 设置颜色
     - :cpp:any:`gui_rect_set_color`
   * - 设置描边
     - :cpp:any:`gui_rect_set_stroke`
   * - 清除描边
     - :cpp:any:`gui_rect_clear_stroke`
   * - 设置外发光
     - :cpp:any:`gui_rect_set_glow`
   * - 清除外发光
     - :cpp:any:`gui_rect_clear_glow`
   * - 设置不透明度
     - :cpp:any:`gui_rect_set_opacity`
   * - 注册点击事件回调
     - :cpp:any:`gui_rect_on_click`
   * - 旋转变换
     - :cpp:any:`gui_rect_rotate`
   * - 缩放变换
     - :cpp:any:`gui_rect_scale`
   * - 平移变换
     - :cpp:any:`gui_rect_translate`
   * - 设置线性渐变
     - :cpp:any:`gui_rect_set_linear_gradient`
   * - 添加渐变色点
     - :cpp:any:`gui_rect_add_gradient_stop`
   * - 清除渐变色
     - :cpp:any:`gui_rect_clear_gradient`

圆角说明
--------

圆角矩形支持四个角具有相同的圆角半径：

- **radius = 0**: 绘制直角矩形
- **radius > 0**: 绘制圆角矩形，半径值决定圆角弧度
- **内部描边**: 描边向矩形内部扩展，不改变控件尺寸

外发光
------

控件可选地绘制一圈外发光。它是独立的一层，画在本体下方：本体的填充、描边
和渐变都不受影响，修改发光也不会触发本体重建。

.. code-block:: c

   gui_color_t glow = gui_rgba(255, 200, 80, 160);
   gui_rect_set_glow(rect, glow, 12);   /* 12 像素模糊半径 */
   gui_rect_clear_glow(rect);

**参数**

- 颜色的 **alpha 即发光强度**，alpha 为 0 等同关闭。
- ``blur_radius`` 单位为像素，钳到 1..32；传 0 等同关闭。
- 改颜色或强度只重绘，不重建掩膜；改尺寸、圆角半径或模糊半径才重建。

**实现说明**

发光掩膜是 A8 的有符号距离场，采样的是本体抗锯齿所用的同一个圆角矩形距离，
因此两者的边缘完全对齐。存储值为 ``F(d) * (1 - c) / (1 - c * a)``，这样它能
与本体自身的 SRC_OVER 正确合成，而不会把边缘覆盖率乘两次。缺少这项补偿时，
本体与发光之间会留下一圈约一像素宽的暗缝。

掩膜 **拆块存储**，不是整块包围盒：一块角块靠镜像画四次，再加一条横边带和
一条竖边带。只有轮廓外那一圈非零，本体内部不占空间，内存取决于圆角半径和
模糊半径而不是控件面积。块之间通过几何缓存共享，所以圆角和模糊半径相同的
两个矩形共用同一份像素，与颜色无关。

**已知限制**

- 发光可以溢出父容器：它只受显示分带裁剪，不受父控件边界限制。命中测试
  仍只算本体。
- 本体移出屏幕时，仍留在屏幕内的那部分发光不会被擦除，要等该区域下一次
  重绘。

渐变色填充
----------

圆角矩形控件支持线性渐变色填充，可以沿着不同方向实现平滑的颜色过渡效果。

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1767064264621594527/6fa7801d_13406851.png" width= "400" /></div>
   <br>

**渐变方向**

支持 4 种线性渐变方向：

- **RECT_GRADIENT_HORIZONTAL**: 水平渐变（从左到右）
- **RECT_GRADIENT_VERTICAL**: 垂直渐变（从上到下）
- **RECT_GRADIENT_DIAGONAL_TL_BR**: 对角线渐变（从左上到右下）
- **RECT_GRADIENT_DIAGONAL_TR_BL**: 对角线渐变（从右上到左下）

**渐变色工作原理**

- 渐变色沿指定方向线性插值
- 支持最多 8 个颜色停止点（color stops）
- 颜色停止点的位置用 0.0 ~ 1.0 的归一化值表示
- 像素颜色通过在相邻颜色停止点之间进行线性插值计算得出
- 采用有序抖动（Ordered Dithering）算法消除 RGB565 格式的色阶问题

**使用步骤**

1. 创建圆角矩形控件
2. 调用 :cpp:any:`gui_rect_set_linear_gradient` 设置渐变方向
3. 调用 :cpp:any:`gui_rect_add_gradient_stop` 添加颜色停止点（至少需要 2 个）
4. 可选：调用 :cpp:any:`gui_rect_clear_gradient` 清除渐变设置

**关键要点**

- **颜色插值**: 支持 RGBA 颜色空间的线性插值，包括透明度通道
- **抗色阶**: 针对 RGB565 显示屏优化，使用抖动算法消除色阶
- **性能优化**: 对非渐变场景几乎无性能影响

特性亮点
--------

- **高性能**: 采用优化的绘制算法，确保流畅的渲染性能
- **抗锯齿**: 支持边缘抗锯齿，提供平滑的圆角效果
- **灵活配置**: 支持自定义尺寸、圆角半径和颜色
- **透明度支持**: 支持 RGBA 颜色格式，可创建半透明效果
- **轻量级**: 内存占用小，适合嵌入式系统和资源受限环境
- **动态更新**: 支持运行时动态修改位置和样式
- **矩阵变换**: 支持旋转、缩放、平移等矩阵变换操作，实现复杂的几何变换效果

应用场景
--------

圆角矩形控件适用于以下场景：

- **现代化按钮**: 创建具有圆角效果的交互按钮
- **卡片式布局**: 实现卡片式 UI 设计元素
- **对话框和面板**: 绘制圆角对话框、信息面板
- **图标背景**: 作为应用图标或功能图标的背景
- **进度条容器**: 创建圆角进度条的背景容器
- **列表项**: 实现圆角列表项或菜单项
- **动画元素**: 支持位置动态变化的动画效果
- **变换动画**: 利用旋转、缩放、平移实现复杂的动画效果

配置说明
--------

要使用圆角矩形控件，需要在配置文件中启用相应的宏定义：

通过 ``menuconfig`` 启用 Kconfig 选项：

.. code-block:: shell

   cd simulator/win32
   menuconfig ../../Kconfig.gui

选择 ``Geometry RECT Demo`` （ ``CONFIG_REALTEK_BUILD_REAL_LITE_RECT`` ），保存到 ``simulator/win32/.config``。

.. code-block:: c

   #define CONFIG_REALTEK_BUILD_REAL_LITE_RECT 1


完整示例
--------

.. literalinclude:: ../../../example/widget/rect/example_gui_rect.c
   :language: c
   :start-after: /* gui rect example start */
   :end-before: /* gui rect example end */

   
API
-------

.. doxygenfile:: gui_rect.h

