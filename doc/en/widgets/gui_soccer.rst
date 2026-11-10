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

   #include "gui_soccer.h"
   #include "ui_resource.h"

   uint32_t *gui_soccer_array[] =
   {
      SOCCER_P0001_CALL_BIN,
      SOCCER_P0002_SPORTS_BIN,
      SOCCER_P0003_HEARTRATE_BIN,
      SOCCER_P0004_SLEEP_BIN,
      SOCCER_P0005_WEATHER_BIN,
      SOCCER_P0006_ACTIVITIES_BIN,
      SOCCER_P0007_STRESS_BIN,
      SOCCER_P0008_SPO2_BIN,
      SOCCER_P0009_MUSIC_BIN,
      SOCCER_P0010_VOICE_BIN,
      SOCCER_P0011_NOTIFICATION_BIN,
      SOCCER_P0012_SETTINGS_BIN,
      SOCCER_P0013_SPORT_CORECD_BIN,
      SOCCER_P0014_MEASURE_BIN,
      SOCCER_P0015_MOOD_BIN,
      SOCCER_P0016_BREATHE_BIN,
      SOCCER_P0017_ALARM_BIN,
      SOCCER_P0018_PERIOD_BIN,
      SOCCER_P0019_HOME_BIN,
      SOCCER_P0020_MORE_BIN,
   };

   static void app_soccer_cb(void *obj, gui_event_code_t e, void *param)
   {
      gui_soccer_t *soccer = (gui_soccer_t *)obj;
      int index = soccer->press_face;
      switch (soccer->press_face)
      {
      case 0:
         // Handle soccer face click
         break;
      default:
         break;
      }
   }

   static int app_init(void)
   {
      gui_soccer_t *soccer = gui_soccer_create(gui_obj_get_root(), "soccer", gui_soccer_array, 0, 0);
      gui_soccer_set_center(soccer, 227, 227);
      gui_soccer_on_click(soccer, app_soccer_cb, NULL);
      return 0;
   }

   GUI_INIT_APP_EXPORT(app_init);



.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/soccer.gif" width= "400" /></div>
   <br>
   
API
---

.. doxygenfile:: gui_soccer.h
