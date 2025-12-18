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
   * - 注册点击事件回调
     - :cpp:any:`gui_rect_on_click`

圆角说明
--------

圆角矩形支持四个角具有相同的圆角半径：

- **radius = 0**: 绘制直角矩形
- **radius > 0**: 绘制圆角矩形，半径值决定圆角弧度

特性亮点
--------

- **高性能**: 采用优化的绘制算法，确保流畅的渲染性能
- **抗锯齿**: 支持边缘抗锯齿，提供平滑的圆角效果
- **灵活配置**: 支持自定义尺寸、圆角半径和颜色
- **透明度支持**: 支持 RGBA 颜色格式，可创建半透明效果
- **轻量级**: 内存占用小，适合嵌入式系统和资源受限环境
- **动态更新**: 支持运行时动态修改位置和样式

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

配置说明
--------

要使用圆角矩形控件，需要在配置文件中启用相应的宏定义：

在 :file:`menu_config.h` 中添加：

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

