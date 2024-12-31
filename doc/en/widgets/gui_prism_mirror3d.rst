========================
Prism Mirror 3D Model
========================

This control supports loading 3D prism mirror models consisting of OBJ and MTL files and supports adding animation effects, which can be displayed on app interfaces. Please read before use: :ref:`Prism Mirror Control Usage Notes`.

.. raw:: html

   <br>
   <div style="text-align: center;">
       <img src="https://docs.realmcu.com/HoneyGUI/image/widgets/prism_mirror.gif" width="400" alt="Image" />
   </div>
   <br>

.. _GUI Load Prism Mirror Model:
GUI Load Prism Mirror Model
-----------------------------
1. Add all necessary files for the prism model to a separate folder, including 6 PNG images, geometric information files for the 3D model (:file:`.obj`), and corresponding material information files (:file:`.mtl`).

   + OBJ file: Stores geometric data for the prism mirror model, including vertices, normals, texture coordinates, and faces.
   + MTL file: Describes the material properties of the prism mirror model, including color, glossiness, transparency, and texture mapping.
   + Image files: Textures used in the model.
   + Packaged executable file for generating prism info descriptor: :file:`extract_desc.exe` and :file:`png2c.py`.

2. Parse the prism mirror model and generate prism mirror information descriptor, using a script to process :file:`extract_desc.exe` obj files

   + In the prism mirror resource package directory, run the following command through the command line: :kbd:`extract_desc.exe prism.obj`. Please modify :file:`prism.obj` to the user's custom file name as needed.
   
   + Generate binary array of images and prism mirror information descriptor: :file:`desc.txt`

3. GUI Load Descriptor

   Place the :file:`desc.txt` file containing obj parsed data, mtl parsed data, and image data into the project directory, and load it in :cpp:any:`gui_prism_mirror3d_create`.

Prism Mirror Control Usage
---------------------------
Create Control
~~~~~~~~~~~~~~
Use the function :cpp:any:`gui_prism_mirror3d_create` to create a prism mirror mirror model. The parameter ``desc_addr`` contains the parsed data extracted from the script.

Add Dynamic Effects
~~~~~~~~~~~~~~~~~~~
Use the function :cpp:any:`gui_prism_mirror3d_enter_animate` to add dynamic effects to the prism mirror model, such as auto-rotation and interactive rotation. This function takes the created ``prism_mirror3d`` as a parameter, and by default, the model will rotate around the x-axis automatically.

Add App Switch Effect
~~~~~~~~~~~~~~~~~~~~~
Use the function :cpp:any:`gui_prism_mirror3d_click_switch_app_add_event` to add click event response effects for the prism mirror model, enabling app switching. The parameter ``callback`` is the corresponding callback function.

Set Size
~~~~~~~~
Use the function :cpp:any:`gui_prism_mirror3d_set_scale` to set the size of the prism mirror model.

Set Position
~~~~~~~~~~~~
Use the function :cpp:any:`gui_prism_mirror3d_set_position` to set the position of the prism mirror model.

Set Orientation
~~~~~~~~~~~~~~~
Use the function :cpp:any:`gui_prism_mirror3d_set_rotation_angles` to set the orientation of the prism mirror model.

.. _Prism Mirror Control Usage Notes:
Prism Mirror Control Usage Notes
---------------------------------

1. All image resources must be in PNG format.
2. The default demo effect is a 454 by 454 square display design. If you use a display with other ratios, in order to obtain better visual effects, you need to use 3D software to re-model, export the corresponding OBJ file, and make a description file that can be loaded by the GUI (for specific steps, please refer to :ref:`GUI Load Prism Mirror Model`).
3. The OBJ file exported by 3D modeling needs to be configured with the Y-axis as the forward axis.
4. Please obtain the necessary files for the prism information descriptor from the following path:
  
   + File Path: :file:`HoneyGUI\\realgui\\example\\demo\\3d`
   + Required Files: :file:`extract_desc.exe` and :file:`png2c.py`

Example
-------
Prism
~~~~~
.. code-block:: c

   #include "guidef.h"
   #include "gui_tabview.h"
   #include "gui_tab.h"
   #include "gui_img.h"
   #include "gui_obj.h"
   #include "string.h"
   #include "stdio.h"
   #include "stdlib.h"
   #include <gui_app.h>
   #include "gui_server.h"
   #include "gui_components_init.h"
   #include "gui_canvas.h"
   #include "def_3d.h"
   #include "gui_3d.h"
   #include "math.h"

   #include "gui_prism_mirror3d.h"
   #include <tp_algo.h>
   #include "prism3d/desc.txt"

   #include "prism3d/root/homelist_dog.c"
   #include "prism3d/root/homelist_line_black.c"
   #include "prism3d/root/homelist_line_orange.c"
   #include "prism3d/root/homelist_number.c"
   #include "prism3d/root/homelist_watch_black.c"
   #include "prism3d/root/homelist_watch_white.c"


   void callback_touch_clike_return();
   void app_cb(void *p);

   static void app_ui_design(gui_app_t *app)
   {
      gui_dispdev_t *dc = gui_get_dc();
      touch_info_t *tp = tp_get_info();
      gui_prism_mirror3d_t *prism_demo = gui_prism_mirror3d_create(&(app->screen), "prism_3d", (void *)_acdesc, 0, 0,
                                                   dc->screen_width,
                                                   dc->screen_height);

      gui_prism_mirror3d_click_switch_app_add_event(prism_demo, app_cb);
      gui_prism_mirror3d_enter_animate(prism_demo);

   }
   uint8_t face_nums_flags = 0;
   static void app_ui_design_switch(gui_app_t *app)
   {
      touch_info_t *tp = tp_get_info();
      gui_img_t *image;
      if (face_nums_flags == 0)
      {
         image = gui_img_create_from_mem(&(app->screen), "image0", (void *)_achomelist_dog, 0, 0, 454, 454);
         gui_img_scale(image, 2.27, 1.89);
      }
      if (face_nums_flags == 1)
      {
         image = gui_img_create_from_mem(&(app->screen), "image0", (void *)_achomelist_line_black, 0, 0, 454,
                                          454);
         gui_img_scale(image, 2.27, 1.89);
      }
      if (face_nums_flags == 2)
      {
         image = gui_img_create_from_mem(&(app->screen), "image0", (void *)_achomelist_line_orange, 0, 0,
                                          454, 454);
         gui_img_scale(image, 2.27, 1.89);
      }
      if (face_nums_flags == 3)
      {
         image = gui_img_create_from_mem(&(app->screen), "image0", (void *)_achomelist_number, 0, 0, 454,
                                          454);
         gui_img_scale(image, 2.27, 1.89);
      }
      if (face_nums_flags == 4)
      {
         image = gui_img_create_from_mem(&(app->screen), "image0", (void *)_achomelist_watch_black, 0, 0,
                                          454, 454);
         gui_img_scale(image, 2.27, 1.89);
      }
      if (face_nums_flags == 5)
      {
         image = gui_img_create_from_mem(&(app->screen), "image0", (void *)_achomelist_watch_white, 0, 0,
                                          454, 454);
         gui_img_scale(image, 2.27, 1.89);
      }

      gui_obj_add_event_cb(image, callback_touch_clike_return, GUI_EVENT_1, NULL);

      return;

   }

   static gui_app_t rtk_gui_demo =
   {
      .screen = {
         .name = "rtk_gui_demo",
         .x    = 0,
         .y    = 0,
      },
      .ui_design = app_ui_design,
      .active_ms = 1000 * 60 * 60,
   };

   static gui_app_t rtk_gui_demo_switch_image1 =
   {
      .screen = {
         .name = "rtk_gui_demo_switch_image1",
         .x    = 0,
         .y    = 0,
      },
      .ui_design = app_ui_design_switch,
      .active_ms = 1000 * 60 * 60,
   };

   void *get_app_rtk_gui_demo(void)
   {
      return &rtk_gui_demo;
   }

   static int app_init(void)
   {
      gui_server_init();
      gui_app_startup(&rtk_gui_demo);
      return 0;
   }

   void app_cb(void *p)
   {
      gui_prism_mirror3d_t *prism_3d = (gui_prism_mirror3d_t *)p;
      face_nums_flags = gui_prism_mirror3d_get_enter_face();
      switch (prism_3d->face_flags)
      {
      case 0:
         gui_app_switch(gui_current_app(), &rtk_gui_demo_switch_image1);
         break;
      case 1:
         gui_app_switch(gui_current_app(), &rtk_gui_demo_switch_image1);
         break;
      case 2:
         gui_app_switch(gui_current_app(), &rtk_gui_demo_switch_image1);
         break;
      case 3:
         gui_app_switch(gui_current_app(), &rtk_gui_demo_switch_image1);
         break;
      case 4:
         gui_app_switch(gui_current_app(), &rtk_gui_demo_switch_image1);
         break;
      case 5:
         gui_app_switch(gui_current_app(), &rtk_gui_demo_switch_image1);
         break;
      default:
         break;
      }
   }
   void callback_touch_clike_return()
   {
      gui_app_switch(gui_current_app(), &rtk_gui_demo);
   }

   GUI_INIT_APP_EXPORT(app_init);



API
---

.. doxygenfile:: gui_prism_mirror3d.h