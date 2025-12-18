.. _圆弧 (Geometry Arc):

圆弧 (Geometry Arc)
========================

概述
----

圆弧控件是一个轻量级的 :term:`GUI` 绘图组件，专门用于在用户界面中绘制圆弧和圆环图形。该控件提供了简洁易用的 :term:`API` ，支持自定义圆弧的起始角度、结束角度、线宽、颜色等属性，能够满足各种圆弧绘制需求。

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1763432727908123796/8b4e718c_13406851.png" width= "400" /></div>
   <br>

核心功能
--------

.. list-table::
   :header-rows: 1

   * - 描述
     - API 
   * - 创建控件
     - :cpp:any:`gui_arc_create`  
   * - 设置位置
     - :cpp:any:`gui_arc_set_position` 
   * - 设置半径
     - :cpp:any:`gui_arc_set_radius`
   * - 设置颜色
     - :cpp:any:`gui_arc_set_color`
   * - 设置起始角度
     - :cpp:any:`gui_arc_set_start_angle`
   * - 设置结束角度
     - :cpp:any:`gui_arc_set_end_angle`
   * - 设置线宽
     - :cpp:any:`gui_arc_set_line_width`
   * - 注册点击事件回调
     - :cpp:any:`gui_arc_on_click`

角度说明
--------

圆弧的角度系统采用标准的数学坐标系：

- **0°**: 3点钟方向（正东方向）
- **90°**: 6点钟方向（正南方向）
- **180°**: 9点钟方向（正西方向）
- **270°**: 12点钟方向（正北方向）

特性亮点
--------

- **高性能**: 采用优化的绘制算法，确保流畅的渲染性能
- **抗锯齿**: 支持边缘抗锯齿，提供平滑的视觉效果
- **灵活配置**: 支持自定义角度范围、线宽和颜色
- **轻量级**: 内存占用小，适合嵌入式系统和资源受限环境
- **圆环支持**: 当起始角度和结束角度相差360°时，自动绘制完整圆环

应用场景
--------

圆弧控件适用于以下场景：

- **进度指示器**: 用于显示加载进度、电池电量等
- **仪表盘**: 构建速度表、温度计等仪表界面
- **数据可视化**: 展示比例数据、统计图表
- **用户界面装饰**: 作为界面的装饰元素
- **状态指示**: 表示系统状态、连接状态等

配置说明
--------

要使用圆弧控件，需要在配置文件中启用相应的宏定义：

在 :file:`menu_config.h` 中添加：

.. code-block:: c

   #define CONFIG_REALTEK_BUILD_REAL_LITE_ARC 1

完整示例
--------

.. literalinclude:: ../../../example/widget/arc/example_gui_arc.c
   :language: c
   :start-after: /* gui arc example start */
   :end-before: /* gui arc example end */

API
-------
.. doxygenfile:: gui_arc.h
