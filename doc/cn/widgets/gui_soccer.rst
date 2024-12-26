=======================
2.5D足球 (2.5D Soccer)
=======================

2.5D足球由12个顶点和20个面组成，这些顶点和面可以构成一个近似3D效果的球形。其中，顶点定义了球体的形状和位置，面是由多个顶点连接而成的平面，用于填充球的表面。

用法
-----
创建控件
~~~~~~~~
使用 :cpp:any:`gui_soccer_create` 创建一个2.5D足球控件。其中 ``addr`` 是用于填充足球白色面片的图像。

设置大小
~~~~~~~~
足球顶点之间的默认距离是100，可以通过 :cpp:any:`gui_soccer_set_size` 改变顶点之间的距离，从而改变足球控件大小。

设置中心
~~~~~~~~
足球控件的默认中心是 ``((dc->fb_width - this->scsize) / 2.0f)``，其中 ``this->scsize`` 为足球顶点距离。使用 :cpp:any:`gui_soccer_set_center` 可以改变足球的中心坐标。

设置图像模式
~~~~~~~~~~~~
足球控件的默认图像混合模式是 ``IMG_SRC_OVER_MODE``，可以通过 :cpp:any:`gui_soccer_set_mode` 来改变图像混合模式。

设置图像透明度
~~~~~~~~~~~~~~
可以通过 :cpp:any:`gui_soccer_set_opacity` 来改变图像的透明度。

设置足球面片图像
~~~~~~~~~~~~~~~~
可以通过 :cpp:any:`gui_soccer_set_img` 来设置足球的面片图像。


示例
----

.. code-block:: c

   #include "gui_app.h"
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
   static void app_call_ui_design(gui_app_t *app);
   static gui_app_t app_call =
   {
      .screen =
      {
         .name = "app_call",
         .x    = 0,
         .y    = 0,
      },
      .ui_design = app_call_ui_design,
      .active_ms = 1000 * 5,
   };

   gui_app_t *get_call_app(void)
   {
      return &app_call;
   }
   static void app_call_ui_design(gui_app_t *app)
   {
      gui_img_create_from_mem(&(app->screen), "call", SOCCER_P0001_CALL_BIN, 100, 100, 100, 100);
   }
   static void app_soccer_cb(void *obj, gui_event_t e, void *param)
   {
      gui_soccer_t *soccer = (gui_soccer_t *)obj;
      int index = soccer->press_face;
      switch (soccer->press_face)
      {
      case 0:
         gui_switch_app(gui_current_app(), get_call_app());
         break;
      default:
         break;
      }
   }
   GUI_APP_ENTRY(APP_SOCCER)
   {
      gui_soccer_t *soccer = gui_soccer_create(&(app->screen), "soccer", gui_soccer_array, 0, 0);
      gui_soccer_set_center(soccer, 227, 227);
      gui_soccer_on_click(soccer, app_soccer_cb, NULL);

      gui_return_create(GUI_APP_ROOT_SCREEN, gui_app_return_array,
                        sizeof(gui_app_return_array) / sizeof(uint32_t *), win_cb, (void *)0);
   }


.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/soccer.gif" width= "400" /></div>
   <br>

API
---

.. doxygenfile:: gui_soccer.h