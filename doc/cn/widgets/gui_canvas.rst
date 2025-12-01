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

1. 使用 ``gui_canvas_create`` 函数创建画布控件。

   此函数用于创建一个基于 NanoVG 的画布控件，返回画布对象指针。

   .. code-block:: c
   
       // 示例：创建200x200大小的画布
       gui_canvas_t* canvas = gui_canvas_create(
           parent,     // 父控件指针
           "my_canvas", // 画布名称
           NULL,       // 地址参数(保留)
           0, 0,       // x,y坐标
           200, 200    // 宽度,高度
       );
       
       if (canvas == NULL) {
           // 错误处理
       }

2. 使用 ``gui_canvas_set_canvas_cb`` 设置绘制回调。

   此函数用于设置画布的重绘回调函数，当需要刷新画布时会调用此回调。

   .. code-block:: c
   
       // 示例绘制函数
       static void my_draw_function(gui_canvas_t* canvas) {
           NVGcontext* vg = canvas->vg;
           
           // 绘制红色矩形
           nvgBeginPath(vg);
           nvgRect(vg, 50, 50, 100, 100);
           nvgFillColor(vg, nvgRGBA(255, 0, 0, 255));
           nvgFill(vg);
       }
       
       // 设置回调
       gui_canvas_set_canvas_cb(canvas, my_draw_function);

触发重绘：
可通过设置 canvas->render = 1 来手动触发重绘，系统会在下一帧调用绘制回调。

图像输出
~~~~~~~~~~

使用 ``gui_canvas_output_buffer`` 函数（预分配缓冲区）。

   此函数需要预先分配缓冲区，适用于需要重复渲染或内存受限的场景。

   .. code-block:: c
   
        // 示例：输出到预分配的RGBA缓冲区
        uint8_t img_head_size = sizeof(gui_rgb_data_head_t);
        uint32_t img_size = img_head_size + 640 * 480 * 4;// RGBA 需分配 width*height*4 字节，加上头部
        uint8_t *buffer = gui_lower_malloc(img_size); 
        gui_canvas_render_to_image_buffer(
            GUI_CANVAS_OUTPUT_RGBA,
            false,
            640, 480,
            my_render_func,
            buffer);


示例代码
--------

完整卡片视图使用示例：

在 :file:`menu_config.h` 中开启宏定义 ``CONFIG_REALTEK_BUILD_CANVAS`` 来运行此示例。

.. literalinclude:: ../../../example/widget/canvas/example_gui_canvas.c
   :language: c
   :start-after: /* canvas example start */
   :end-before: /* canvas example end */

API
-------

.. doxygenfile:: gui_canvas.h


