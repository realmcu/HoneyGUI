===============
画布（Canvas）
===============

画布控件提供基于NanoVG的2D图形绘制能力，并支持软件加速渲染和多种图像输出格式。

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
GUI Canvas是基于NanoVG的2D绘图组件，提供：

- 基础形状绘制（矩形/圆形/弧形等）
- 支持RGBA/RGB565/PNG/JPG输出格式
- NanoVG软件加速渲染（通过AGGE后端）
- 直接缓冲区输出功能
- 空白缓冲区初始化支持

核心功能
--------

创建与初始化
~~~~~~~~~~~~

1. 使用 ``gui_canvas_create`` 函数创建画布控件

   此函数用于创建一个基于NanoVG的画布控件，返回画布对象指针。

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

2. 使用 ``gui_canvas_set_canvas_cb`` 设置绘制回调

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
可通过设置canvas->render = 1来手动触发重绘，系统会在下一帧调用绘制回调。

图像输出
~~~~~~~~~~

1. 使用 ``gui_canvas_output`` 函数（动态分配缓冲区）

   此函数会动态分配内存并返回渲染结果，适用于需要一次性输出的场景。

   .. code-block:: c
   
       // 示例：输出PNG格式图像
       const uint8_t* png_data = gui_canvas_output(
           GUI_CANVAS_OUTPUT_PNG,
           false,  // PNG格式不支持压缩参数
           640, 480,
           my_render_func);
       
       // 使用后需要手动释放内存
       free((void*)png_data);

2. 使用 ``gui_canvas_output_buffer`` 函数（预分配缓冲区）

   此函数需要预先分配缓冲区，适用于需要重复渲染或内存受限的场景。

   .. code-block:: c
   
       // 示例：输出到预分配的RGBA缓冲区
       uint8_t buffer[640*480*4]; // RGBA格式需要 width*height*4 字节
       gui_canvas_output_buffer(
           GUI_CANVAS_OUTPUT_RGBA,
           false,   
           640, 480,
           my_render_func,
           buffer);

3. 使用 ``gui_canvas_output_buffer_blank`` 和 ``gui_canvas_output_buffer_blank_close`` 函数

   这对函数用于创建和销毁空白画布上下文，适用于需要多次渲染操作的场景。

   .. code-block:: c
   
       // 示例：创建空白画布
       uint8_t buffer[480*480*4];
       NVGcontext* vg = gui_canvas_output_buffer_blank(
           GUI_CANVAS_OUTPUT_RGBA,
           false,
           480, 480,
           buffer);
       
       // 在空白画布上绘制
       nvgBeginPath(vg);
       nvgRect(vg, 100, 100, 200, 200);
       nvgFillColor(vg, nvgRGBA(255,0,0,255));
       nvgFill(vg);
       
       // 完成后关闭画布
       gui_canvas_output_buffer_blank_close(vg);

示例代码
--------

完整卡片视图使用示例：

在 :file:`menu_config.h` 中开启宏定义 ``CONFIG_REALTEK_BUILD_CANVAS`` 来运行此示例。

.. literalinclude:: ../../../example/widget/canvas/example_gui_canvas.c
   :language: c
   :start-after: /* canvas example start */
   :end-before: /* canvas example end */

API参考
-------

.. doxygenfile:: gui_canvas.h


