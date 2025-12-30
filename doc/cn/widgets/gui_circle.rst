.. _Circle (Geometry Circle):

圆形 (Geometry Circle)
=========================

概述
----

圆形控件是一个轻量级的 :term:`GUI` 绘图组件，专门用于在用户界面中绘制圆形图形。该控件提供了简洁易用的 :term:`API` ，支持自定义圆形的中心位置、半径、颜色等属性，能够创建各种圆形 :term:`UI` 元素。

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1763463135933874666/f2c5f5f8_13406851.png" width= "400" /></div>
   <br>

核心功能
--------

.. list-table::
   :header-rows: 1

   * - 描述
     - API 
   * - 创建控件
     - :cpp:any:`gui_circle_create`  
   * - 设置属性
     - :cpp:any:`gui_circle_set_style`
   * - 设置位置
     - :cpp:any:`gui_circle_set_position` 
   * - 设置半径
     - :cpp:any:`gui_circle_set_radius`
   * - 设置颜色
     - :cpp:any:`gui_circle_set_color`
   * - 设置不透明度
     - :cpp:any:`gui_circle_set_opacity`
   * - 注册点击事件回调
     - :cpp:any:`gui_circle_on_click`
   * - 旋转变换
     - :cpp:any:`gui_circle_rotate`
   * - 缩放变换
     - :cpp:any:`gui_circle_scale`
   * - 平移变换
     - :cpp:any:`gui_circle_translate`
   * - 设置径向渐变
     - :cpp:any:`gui_circle_set_radial_gradient`
   * - 设置角度渐变
     - :cpp:any:`gui_circle_set_angular_gradient`
   * - 添加渐变色点
     - :cpp:any:`gui_circle_add_gradient_stop`
   * - 清除渐变色
     - :cpp:any:`gui_circle_clear_gradient`

圆形特性
--------

圆形控件具有以下几何特性：

- **圆心定位**: 通过中心点坐标精确定位
- **半径控制**: 支持任意大小的半径值
- **完美圆形**: 确保绘制完美的几何圆形
- **边界处理**: 自动处理圆形与边界的裁剪

渐变色填充
----------

圆形控件支持径向渐变和角度渐变两种渐变色填充方式，可以创建丰富的视觉效果。

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1767064721463984141/294aaa5b_13406851.png" width= "400" /></div>
   <br>

**渐变类型**

支持 2 种渐变类型：

- **CIRCLE_GRADIENT_RADIAL**: 径向渐变（从圆心向外）
- **CIRCLE_GRADIENT_ANGULAR**: 角度渐变（沿圆周方向）

**渐变色工作原理**

- 径向渐变：颜色从圆心向边缘线性插值
- 角度渐变：颜色沿圆周角度方向线性插值
- 支持最多 8 个颜色停止点（color stops）
- 颜色停止点的位置用 0.0 ~ 1.0 的归一化值表示
- 像素颜色通过在相邻颜色停止点之间进行线性插值计算得出
- 采用有序抖动（Ordered Dithering）算法消除 RGB565 格式的色阶问题

**使用步骤**

1. 创建圆形控件
2. 调用 :cpp:any:`gui_circle_set_radial_gradient` 设置径向渐变，或调用 :cpp:any:`gui_circle_set_angular_gradient` 设置角度渐变
3. 调用 :cpp:any:`gui_circle_add_gradient_stop` 添加颜色停止点（至少需要 2 个）
4. 可选：调用 :cpp:any:`gui_circle_clear_gradient` 清除渐变设置

**关键要点**

- **径向渐变**: 适合创建光晕、太阳等从中心向外发散的效果
- **角度渐变**: 适合创建彩虹圆环、色轮等沿圆周变化的效果
- **无缝循环**: 对于角度渐变，起始颜色和结束颜色应相同，以实现无缝的颜色循环
- **颜色插值**: 支持 RGBA 颜色空间的线性插值，包括透明度通道
- **抗色阶**: 针对 RGB565 显示屏优化，使用抖动算法消除色阶
- **性能优化**: 对非渐变场景几乎无性能影响

特性亮点
--------

- **高性能**: 采用优化的圆形绘制算法，确保流畅的渲染性能
- **抗锯齿**: 支持边缘抗锯齿，提供平滑的圆形边缘效果
- **灵活配置**: 支持自定义半径、位置和颜色
- **透明度支持**: 支持 RGBA 颜色格式，可创建半透明圆形效果
- **轻量级**: 内存占用小，适合嵌入式系统和资源受限环境
- **动态更新**: 支持运行时动态修改位置和样式
- **矩阵变换**: 支持旋转、缩放、平移等矩阵变换操作，可创建椭圆和复杂变换效果

应用场景
--------

圆形控件适用于以下场景：

- **状态指示器**: 创建圆形状态指示灯
- **用户头像**: 实现圆形用户头像显示
- **图标设计**: 绘制各种圆形图标元素
- **进度指示**: 创建圆形进度条或加载动画
- **按钮设计**: 实现圆形交互按钮
- **装饰元素**: 作为 UI 界面的装饰性圆形元素
- **数据可视化**: 用于饼图、雷达图等数据可视化组件
- **椭圆绘制**: 利用缩放变换创建椭圆形状
- **变换动画**: 利用旋转、缩放、平移实现复杂的动画效果

配置说明
--------

要使用圆形控件，需要在配置文件中启用相应的宏定义：

通过 ``menuconfig`` 启用 Kconfig 选项：

.. code-block:: shell

   cd win32_sim
   menuconfig ../Kconfig.gui

选择 ``Geometry CIRCLE Demo`` ( ``CONFIG_REALTEK_BUILD_REAL_LITE_CIRCLE`` )，保存到 ``win32_sim/.config``。

.. code-block:: c

   #define CONFIG_REALTEK_BUILD_REAL_LITE_CIRCLE 1

完整示例
--------

.. literalinclude:: ../../../example/widget/circle/example_gui_circle.c
   :language: c
   :start-after: /* gui circle example start */
   :end-before: /* gui circle example end */



API
-------

.. doxygenfile:: gui_circle.h
