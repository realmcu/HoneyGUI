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

Overview
--------
GUI Canvas is a 2D drawing component based on NanoVG, providing:

- Basic shape drawing (rectangles, circles, arcs, etc.)
- Support for RGBA/RGB565/PNG/JPG output formats
- NanoVG software-accelerated rendering (via AGGE backend)
- Direct buffer output functionality
- Blank buffer initialization support

Core Features
------------

Creation & Initialization
~~~~~~~~~~~~~~~~~~~~~~~~

1. Using ``gui_canvas_create`` function

   Creates a NanoVG-based canvas widget and returns a pointer to the canvas object.

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

2. Using ``gui_canvas_set_canvas_cb`` to set drawing callback

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


Manual Refresh:
Set canvas->render = 1 to manually trigger redraw (callback will be called next frame).

Image Output
~~~~~~~~~~~~

1. Using ``gui_canvas_output`` (dynamically allocated buffer)

   Allocates memory and returns rendering result, suitable for one-time output.

   .. code-block:: c
   
       // Example: Output PNG image
       const uint8_t* png_data = gui_canvas_output(
           GUI_CANVAS_OUTPUT_PNG,
           false,  // PNG doesn't support compression
           640, 480,
           my_render_func);
       
       // Must free memory after use
       free((void*)png_data);

2. Using ``gui_canvas_output_buffer`` (pre-allocated buffer)

   Requires pre-allocated buffer, suitable for repeated rendering or memory-constrained scenarios.

   .. code-block:: c
   
       // Example: Output to RGBA buffer
       uint8_t buffer[640*480*4]; // RGBA requires width*height*4 bytes
       gui_canvas_output_buffer(
           GUI_CANVAS_OUTPUT_RGBA,
           false,   
           640, 480,
           my_render_func,
           buffer);

3. Using ``gui_canvas_output_buffer_blank`` and ``gui_canvas_output_buffer_blank_close``

   This pair of functions creates and destroys blank canvas contexts, suitable for multiple rendering operations.

   .. code-block:: c
   
       // Example: Create blank canvas
       uint8_t buffer[480*480*4];
       NVGcontext* vg = gui_canvas_output_buffer_blank(
           GUI_CANVAS_OUTPUT_RGBA,
           false,
           480, 480,
           buffer);
       
       // Draw on blank canvas
       nvgBeginPath(vg);
       nvgRect(vg, 100, 100, 200, 200);
       nvgFillColor(vg, nvgRGBA(255,0,0,255));
       nvgFill(vg);
       
       // Close canvas when done
       gui_canvas_output_buffer_blank_close(vg);

Example Code
------------

Complete card view usage example:

Enable macro ``CONFIG_REALTEK_BUILD_CANVAS`` in :file:`menu_config.h` to run this example.

.. literalinclude:: ../../../example/widget/canvas/example_gui_canvas.c
   :language: c
   :start-after: /* canvas example start */
   :end-before: /* canvas example end */

API Reference
--------------

.. doxygenfile:: gui_canvas.h