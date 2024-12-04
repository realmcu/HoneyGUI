===========
2.5D Soccer
===========

2.5D soccer consists of 12 vertices and 20 faces, which can form an approximately 3D spherical shape. 
Among them, the vertices define the shape and position of the sphere, and the face is a plane formed by connecting multiple vertices, used to fill the surface of the sphere.

Usage
-----

Create Widget
~~~~~~~~~~~~~~

Use :cpp:any:`gui_soccer_create` to create a 2.5D soccer. The ``addr`` parameter is the image used to fill the white faces of the soccer ball.

Set Size
~~~~~~~~~~

The default distance between the vertices of the soccer is 100. You can change the size of the soccer by using :cpp:any:`gui_soccer_set_size` to modify the distance between the vertices.

Set Center
~~~~~~~~~~~~

The default center of the soccer is ``((dc->fb_width - this->scsize) / 2.0f)``, where ``this->scsize`` is the distance between the vertices of the soccer. 
You can change the center coordinates of the soccer by using :cpp:any:`gui_soccer_set_center`.

Set Image Mode
~~~~~~~~~~~~~~

The default image blending mode for the soccer is ``IMG_SRC_OVER_MODE``. You can change the image blending mode by using :cpp:any:`gui_soccer_set_mode`.

Set Image Opacity
~~~~~~~~~~~~~~~~~~

You can change the opacity of the image by using :cpp:any:`gui_soccer_set_opacity`.

Set Image for Soccer Faces
~~~~~~~~~~~~~~~~~~~~~~~~~~~

You can set the image for the soccer ball faces by using :cpp:any:`gui_soccer_set_img`.

Example
-------

.. code-block:: c

   #include "gui_app.h"
   #include "gui_soccer.h"
   #include "green_hexagon.txt"

   static void app_dialing_ui_design(gui_app_t *app)
   {
       gui_soccer_t *soccer = gui_soccer_create(&(app->screen), "soccer", (void *)_acgreen_hexagon, 0, 0);
       // gui_soccer_set_size(soccer, 80);
       gui_soccer_set_center(soccer, 240, 240);
   }


.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/soccer.gif" width= "400" /></div>
   <br>
   
API
---

.. doxygenfile:: gui_soccer.h
