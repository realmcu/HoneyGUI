==========
Canvas
==========

The Canvas widget provides 2D graphics rendering capabilities based on NanoVG, supporting software-accelerated rendering and multiple image output formats.

.. raw:: html

   <br>
   <div style="text-align: center">
   <img src="https://foruda.gitee.com/images/1745388769584294665/63042bc7_13406851.gif" width="500" />
   </div>
   <br>

.. warning::

   Before usage, ensure: Sufficient frame buffer memory.

Component Overview
------------------
:term:`GUI` Canvas is a 2D drawing component based on NanoVG, providing:

- Basic shape drawing (rectangles, circles, arcs, etc.)
- Support for RGBA/RGB565/:term:`PNG`/JPG output formats
- NanoVG software-accelerated rendering (via AGGE backend)
- Direct buffer output functionality
- Blank buffer initialization support

Core Features
----------------

Creation & Initialization
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

1. Using ``gui_canvas_create`` function.

   Creates a NanoVG canvas widget and returns a pointer to the canvas object.

   .. code-block:: c
   
       // Example: Create 200x200 canvas
       gui_canvas_t* canvas = gui_canvas_create(
           parent,     // Parent widget pointer
           "my_canvas", // Canvas name
           NULL,       // Reserved parameter
           0, 0,       // x,y coordinates
           200, 200    // width,height
       );
       
       if (canvas == NULL) {
           // Error handling
       }

2. Using ``gui_canvas_set_canvas_cb`` to set drawing callback.

   Sets the redraw callback function that gets called when canvas needs refreshing.

   .. code-block:: c
   
       // Example drawing function
       static void my_draw_function(gui_canvas_t* canvas) {
           NVGcontext* vg = canvas->vg;
           
           // Draw red rectangle
           nvgBeginPath(vg);
           nvgRect(vg, 50, 50, 100, 100);
           nvgFillColor(vg, nvgRGBA(255, 0, 0, 255));
           nvgFill(vg);
       }
       
       // Set callback
       gui_canvas_set_canvas_cb(canvas, my_draw_function);

Trigger Redraw:
You can manually trigger a redraw by setting canvas->render = 1. The system will call the drawing callback on the next frame.

Image Output
~~~~~~~~~~~~

Using ``gui_canvas_render_to_image_buffer`` function (pre-allocated buffer).

This function requires a pre-allocated buffer and is suitable for scenarios where repeated rendering or memory constraints are important.

   .. code-block:: c

        // Example: Output to a pre-allocated RGBA buffer
        uint8_t img_head_size = sizeof(gui_rgb_data_head_t);
        uint32_t img_size = img_head_size + 640 * 480 * 4; // RGBA requires width*height*4 bytes plus header
        uint8_t *buffer = gui_lower_malloc(img_size);
        gui_canvas_render_to_image_buffer(
            GUI_CANVAS_OUTPUT_RGBA,
            false,
            640, 480,
            my_render_func,
            buffer);

Example Code
------------

Complete card view usage example:

Enable the Kconfig option via ``menuconfig`` to run this example:

.. code-block:: shell

   cd win32_sim
   menuconfig ../Kconfig.gui

Select ``Canvas Demo`` (``CONFIG_REALTEK_BUILD_REAL_CANVAS``), then save to ``win32_sim/.config``.

.. literalinclude:: ../../../example/widget/canvas/example_canvas.c
   :language: c
   :start-after: /* canvas example start */
   :end-before: /* canvas example end */

API
---

.. doxygenfile:: gui_canvas.h
