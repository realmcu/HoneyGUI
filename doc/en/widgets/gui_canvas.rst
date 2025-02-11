==========
Canvas
==========

The canvas widget is the basic widget used to drawing graphics in nanovg.

.. warning::

    Sufficient memory is needed to open a framebuffer.


Usage
------------
Creat Canvas
~~~~~~~~~~~~~
:cpp:any:`gui_canvas_create` creates a canvas.

Add Callback Function
~~~~~~~~~~~~~~~~~~~~~~
:cpp:any:`gui_canvas_set_canvas_cb` sets the callback function for drawing specific shapes.

Example
------------
Rounded Rectangle
~~~~~~~~~~~~~~~~~~

A simple example of drawing three rounded rectangles of different colors.

* Three color refer to `Colors' RGB Data <https://www.rapidtables.com/web/color/RGB_Color.html>`_.
* ``firebrick``
* ``olive drab``
* ``dodger blue`` with 100 value opacity

.. code-block:: c

    #include "gui_canvas.h"
    static void canvas_cb(gui_canvas_t *canvas)
    {
        nvgRoundedRect(canvas->vg, 10, 10, 348, 200, 30);
        nvgFillColor(canvas->vg, nvgRGB(178,34,34));
        nvgFill(canvas->vg);
        nvgBeginPath(canvas->vg);
        nvgRoundedRect(canvas->vg, 10, 220, 348, 200, 30);
        nvgFillColor(canvas->vg, nvgRGB(107,142,35));
        nvgFill(canvas->vg);
        nvgBeginPath(canvas->vg);
        nvgRoundedRect(canvas->vg, 110, 125, 148, 200, 30);
        nvgFillColor(canvas->vg, nvgRGBA(30,144,255, 100));
        nvgFill(canvas->vg);
    }
    static void app_ui_design(gui_app_t *app)
    {
        gui_canvas_t *canvas = gui_canvas_create(&(app->screen), "canvas", 0, 0, 0, 368, 448);
        gui_canvas_set_canvas_cb(canvas, canvas_cb);
    }


.. figure:: https://foruda.gitee.com/images/1698649650262539854/8b1a974f_10088396.png
   :align: center
   :width: 400px

Arc Animation
~~~~~~~~~~~~~

An example of drawing an arc animation. ``arc_cb`` will be triggered every frame.

.. code-block:: c

    #include "math.h"
    #include "gui_canvas.h"
    static void arc_cb(gui_canvas_t *canvas)
    {
        static float  progress;
        progress +=0.01;
        nvgArc(canvas->vg, 368/2, 448/2, 150, 0, 3.14*(sinf(progress)+1), NVG_CCW);
        nvgStrokeWidth(canvas->vg, 20);
        nvgStrokeColor(canvas->vg, nvgRGB(178,34,34));
        nvgStroke(canvas->vg);
    }
    static void app_ui_design(gui_app_t *app)
    {
        gui_canvas_t *canvas = gui_canvas_create(&(app->screen), "canvas", 0, 0, 0, 368, 448);
        gui_canvas_set_canvas_cb(canvas, arc_cb);
    }


.. raw:: html

    <br/>
    <div style="text-align: center"><img width= "400" src="https://docs.realmcu.com/HoneyGUI/image/widgets/canvas_arc.gif"></div>
    <br/>

API
------------
Nanovg API
~~~~~~~~~~~~~
Please refer to

`Nanovg Introduction`_

.. _Nanovg Introduction: https://openplanet.dev/docs/tutorials/nanovg-introduction

`Nanovg Github`_

.. _Nanovg Github: https://github.com/memononen/nanovg

RealGUI API
~~~~~~~~~~~~~

.. doxygenfile:: gui_canvas.h


