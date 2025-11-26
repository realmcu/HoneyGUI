===
Img
===

The image widget is the basic widget used to display images. Image widgets support moving, zooming, rotating, etc.

Usage
-----

Create Widget
~~~~~~~~~~~~~

It is possible to use :cpp:any:`gui_img_create_from_mem` to create an image widget from memory, or use :cpp:any:`gui_img_create_from_fs` to create an image widget from a file. Alternatively, :cpp:any:`gui_img_create_from_ftl` can be used to create an image widget from ftl.
If the width or height of the image widget is set to `0`, the widget's size will be set according to the size of the image source automatically.

Set Fouce
~~~~~~~~~~~~~

The default focus position of the image widget is at the top-left corner of the image (0,0). Developers can use :cpp:any:`gui_img_set_focus` to set a new focus point for the image widget. After setting a new focus, all transformations, rotations, and translations of the image widget will be performed based on the new focus point.

Set Attribute
~~~~~~~~~~~~~

It is possible to use :cpp:any:`gui_img_set_attribute` to set the attribute of an image widget, replace it with a new image, and set a new coordinate.

Get Height/Width
~~~~~~~~~~~~~~~~

If you want to get the height/width of image widget, do so with :cpp:any:`gui_img_get_height` or :cpp:any:`gui_img_get_width`.

Refresh
~~~~~~~

Refresh the image size using :cpp:any:`gui_img_refresh_size`.

Blend Mode
~~~~~~~~~~

Set the image's blend mode using :cpp:any:`gui_img_set_mode`.

Translation
~~~~~~~~~~~

Use :cpp:any:`gui_img_translate` to move the image widget.
It can move an image widget to a new coordinate without changing the original coordinate in the widget's attribute.

Rotation
~~~~~~~~

Rotate the image widget around the center of the circle by :cpp:any:`gui_img_rotation`.

Zoom
~~~~

You can adjust the size of the image widget to fit your requirements by :cpp:any:`gui_img_scale`.

Opacity
~~~~~~~

The opacity value of the image is adjustable, and it can be set using :cpp:any:`gui_img_set_opacity`.

Quality
~~~~~~~

The image's quality can be set using :cpp:any:`gui_img_set_quality`.

Screenshot
~~~~~~~~~~

The :cpp:any:`gui_img_tree_convert_to_img` can be used to save a fullscreen screenshot. The saved image will be in RGB format.

Example
-------

.. literalinclude:: ../../../example/widget/image/example_gui_img.c
    :language: c
    :start-after: /* gui image widget example start*/
    :end-before: /* gui image widget example end*/

    

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/img.gif" width= "400" /></div>
   <br>


API
---

.. doxygenfile:: gui_img.h
