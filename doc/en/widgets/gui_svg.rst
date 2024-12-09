============
SVG
============


The SVG widget allows you to display an image in SVG format.

+ SVG images are vector graphics based on XML language implementation, which can be used to describe 2D vector graphics.

+ SVG images do not need to be converted into ``bin`` format files using ImageConverter tools, but can be directly packaged in the ``root`` directory.

Usage
-----

Creat Svg Widget
~~~~~~~~~~~~~~~~~~~~~~~~~

You can use :cpp:any:`gui_svg_create_from_mem` to create an SVG widget from memory, or use :cpp:any:`gui_svg_create_from_file` to create from file.
This ``w/h`` are the width and height of the SVG widget, not SVG image. This ``size`` is the data size of the SVG image that must be filled in.

SVG Rotation
~~~~~~~~~~~~~~~~~~~~~~~~~

Using the :cpp:any:`gui_svg_rotation` the image will be rotated by an angle of ``degree``. The ``(c_x, c_y)`` is relative to the origin of the SVG widget, not the origin of the screen.

SVG Scale
~~~~~~~~~~~~~~~~~~~~~~~~~

If you need the object size to be updated to transformed size set :cpp:any:`gui_svg_scale`.

SVG Translate
~~~~~~~~~~~~~~~~~~~~~~~~~

The image will be translated by using the :cpp:any:`gui_svg_translate`.

Opacity
~~~~~~~~~~~~~~~~~~~~~~~~~

If you want the image to fade or opacity set :cpp:any:`gui_svg_set_opacity`.

Example
-----

Creat Simple SVG
~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

    #include "gui_svg.h"
    #include "root_image_hongkong/ui_resource.h"
    void page_tb_svg(void *parent)
    { 
        gui_svg_t* svg1=gui_svg_create_from_mem(parent,"svg1",ACTIVITY_SVG,5184,0,0,100,100);
    }


.. figure:: https://foruda.gitee.com/images/1699598969684310669/2e2a68e7_13671125.png
   :align: center
   :width: 400px


SVG Rotation
~~~~~~~~~~~~~~~~~~~~~~~~~


.. code-block:: c

    #include "gui_svg.h"
    #include "root_image_hongkong/ui_resource.h"
    void page_tb_svg(void *parent)
    { 
        gui_svg_t* svg1=gui_svg_create_from_mem(parent,"svg1",ACTIVITY_SVG,5184,0,0,100,100);
        gui_svg_rotation(svg1,90,50,50);
    }


.. figure:: https://foruda.gitee.com/images/1699598974541085137/fcc74440_13671125.png
   :align: center
   :width: 400px


SVG Scale
~~~~~~~~~~~~~~~~~~~~~~~~~


.. code-block:: c

    #include "gui_svg.h"
    #include "root_image_hongkong/ui_resource.h"
    void page_tb_svg(void *parent)
    { 
        gui_svg_t* svg1=gui_svg_create_from_mem(parent,"svg1",ACTIVITY_SVG,5184,0,0,100,100);
        gui_svg_scale(svg1,2,1);
    }


.. figure:: https://foruda.gitee.com/images/1699598982107316449/af526d67_13671125.png
   :align: center
   :width: 400px


SVG Translate
~~~~~~~~~~~~~~~~~~~~~~~~~


.. code-block:: c

    #include "gui_svg.h"
    #include "root_image_hongkong/ui_resource.h"
    void page_tb_svg(void *parent)
    { 
        gui_svg_t* svg1=gui_svg_create_from_mem(parent,"svg1",ACTIVITY_SVG,5184,0,0,100,100);
        gui_svg_translate(svg1,100,100);
    }


.. figure:: https://foruda.gitee.com/images/1699598986751661244/ff6ea9cf_13671125.png
   :align: center
   :width: 400px


API
-----


.. doxygenfile:: gui_svg.h

