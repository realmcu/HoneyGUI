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

1. Use ``gui_canvas_create`` to create a canvas widget, and bind a drawing callback with ``gui_canvas_set_canvas_cb``.

   .. literalinclude:: ../../../example/widget/canvas/example_canvas.c
      :language: c
      :start-after: /* canvas create start */
      :end-before: /* canvas create end */

2. Inside the drawing callback, draw graphics through the NanoVG interface; the callback is invoked whenever the canvas needs refreshing.

   .. literalinclude:: ../../../example/widget/canvas/example_canvas.c
      :language: c
      :start-after: /* canvas draw callback start */
      :end-before: /* canvas draw callback end */

You can manually trigger a redraw by setting ``canvas->render = 1``; the system will call the drawing callback on the next frame.

Image Output
~~~~~~~~~~~~

Use ``gui_canvas_render_to_image_buffer`` to render the canvas content into a pre-allocated buffer, which is suitable for scenarios involving repeated rendering or limited memory. The buffer size is computed as width x height x bytes-per-pixel plus the size of the ``gui_rgb_data_head_t`` header, as shown below:

.. literalinclude:: ../../../example/application/screen_410_502/app_heartrate.c
   :language: c
   :dedent:
   :start-after: /* canvas render start */
   :end-before: /* canvas render end */

Example Code
------------

Complete card view usage example:

Enable the Kconfig option via ``menuconfig`` to run this example:

.. code-block:: shell

   cd win32_sim
   menuconfig ../Kconfig.gui

Select canvas demo (``CONFIG_REALTEK_BUILD_REAL_CANVAS``), then save to ``win32_sim/.config``.

.. literalinclude:: ../../../example/widget/canvas/example_canvas.c
   :language: c
   :start-after: /* canvas example start */
   :end-before: /* canvas example end */

API
---

.. doxygenfile:: gui_canvas.h
