===============
画布（Canvas）
===============

画布控件提供基于 NanoVG 的 2D 图形绘制能力，并支持软件加速渲染和多种图像输出格式。

.. raw:: html

   <br>
   <div style="text-align: center">
   <img src="https://foruda.gitee.com/images/1745388769584294665/63042bc7_13406851.gif" width="500" />
   </div>
   <br>

.. warning::

   使用前需确保：帧缓冲区有足够内存。

组件概述
--------
:term:`GUI` Canvas 是基于 NanoVG 的 2D 绘图组件，提供：

- 基础形状绘制（矩形/圆形/弧形等）
- 支持 RGBA/RGB565/ :term:`PNG`/JPG输出格式
- NanoVG 软件加速渲染（通过 AGGE 后端）
- 直接缓冲区输出功能
- 空白缓冲区初始化支持

核心功能
--------

创建与初始化
~~~~~~~~~~~~

1. 使用 ``gui_canvas_create`` 创建画布控件，并通过 ``gui_canvas_set_canvas_cb`` 绑定绘制回调。

   .. literalinclude:: ../../../example/widget/canvas/example_canvas.c
      :language: c
      :start-after: /* canvas create start */
      :end-before: /* canvas create end */

2. 绘制回调函数中通过 NanoVG 接口绘制图形，当需要刷新画布时系统会调用该回调。

   .. literalinclude:: ../../../example/widget/canvas/example_canvas.c
      :language: c
      :start-after: /* canvas draw callback start */
      :end-before: /* canvas draw callback end */

可通过设置 ``canvas->render = 1`` 手动触发重绘，系统会在下一帧调用绘制回调。

图像输出
~~~~~~~~~~

使用 ``gui_canvas_render_to_image_buffer`` 函数可将画布内容渲染到预先分配的缓冲区，适用于需要重复渲染或内存受限的场景。缓冲区大小需按 宽 × 高 × 每像素字节数，再加上头部 ``gui_rgb_data_head_t`` 的大小来计算，示例如下：

.. literalinclude:: ../../../example/application/screen_410_502/app_heartrate.c
   :language: c
   :dedent:
   :start-after: /* canvas render start */
   :end-before: /* canvas render end */

示例代码
--------

完整卡片视图使用示例：

请通过 ``menuconfig`` 启用 Kconfig 选项来运行该示例：

.. code-block:: shell

   cd win32_sim
   menuconfig ../Kconfig.gui

选择画布示例（ ``CONFIG_REALTEK_BUILD_REAL_CANVAS``），保存到 ``win32_sim/.config``。

.. literalinclude:: ../../../example/widget/canvas/example_canvas.c
   :language: c
   :start-after: /* canvas example start */
   :end-before: /* canvas example end */

API
-------

.. doxygenfile:: gui_canvas.h


