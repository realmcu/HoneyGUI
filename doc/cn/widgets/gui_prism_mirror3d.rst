==========================================
棱镜模型 (Prism Mirror 3D Model)
==========================================

该控件支持加载由OBJ和MTL文件组成的3D棱柱模型，并支持添加动画特效，可以用在app界面显示。使用之前请阅读： :ref:`棱镜控件使用注意事项` 。

.. raw:: html

   <br>
   <div style="text-align: center;">
       <img src="https://docs.realmcu.com/HoneyGUI/image/widgets/prism_mirror.gif" width="400" alt="Image" />
   </div>
   <br>

.. _GUI加载棱镜模型:

GUI加载棱镜模型
---------------
1. 棱镜模型组成需要的所以文件添加到单独的文件夹内：包含6张PNG图片，3D模型的几何信息文件（ :file:`.obj` ）和对应的材质信息文件（ :file:`.mtl` ）

   + OBJ文件：存储棱镜模型的几何数据，包括顶点、法线、纹理坐标、面等。
   + MTL文件：描述棱镜模型材质的属性，包括颜色、光泽度、透明度和纹理映射等。
   + 图片文件：模型中使用到的贴图。
   + 棱镜信息描述子打包运行exe文件（ :file:`extract_desc.exe` 和 :file:`png2c.py` ）。


2. 棱镜模型解析并生成棱镜信息描述子，调用脚本( :file:`extract_desc.exe` )处理 :file:`.obj` 文件

   + 在棱镜资源包目录中，通过命令行运行以下命令：:kbd:`extract_desc.exe prism.obj`。请根据实际情况将 :file:`prism.obj` 修改为用户自定义的文件名。
   
   + 生成图片和棱镜信息描述子的二进制数组: :file:`desc.txt`

3. GUI加载描述子

   将包含obj解析数据、mtl解析数据和图片数据的 :file:`desc.txt` 文件放入工程目录下，并在 :cpp:any:`gui_prism_mirror3d_create` 中加载。


棱镜控件用法
-------------
创建控件
~~~~~~~~
使用 :cpp:any:`gui_prism_mirror3d_create` 函数来创建棱镜模型。参数 ``desc_addr`` 文件包含从脚本中提取的解析数据。

添加动态效果
~~~~~~~~~~~~~
使用 :cpp:any:`gui_prism_mirror3d_enter_animate` 为棱镜模型添加动态效果，比如自动旋转和交互式旋转。此函数接收创建的 ``prism_mirror3d`` 作为参数，默认情况下模型会绕x轴自动旋转。

添加app切换效果
~~~~~~~~~~~~~~~
使用 :cpp:any:`gui_prism_mirror3d_click_switch_app_add_event` 为棱镜模型添加点击事件响应效果，实现应用切换。参数 ``callback`` 为对应的回调函数。

设置大小
~~~~~~~~
使用 :cpp:any:`gui_prism_mirror3d_set_scale` 来设定棱镜模型的大小。

设置位置
~~~~~~~~
使用 :cpp:any:`gui_prism_mirror3d_set_position` 来设置棱镜模型的位置。

设置方位
~~~~~~~~
使用 :cpp:any:`gui_prism_mirror3d_set_rotation_angles` 来设置棱镜模型的方位。

.. _棱镜控件使用注意事项:

棱镜控件使用注意事项
----------------------

1. 所有图片资源必须为PNG格式。
2. 默认demo效果为454x454的方形显示屏设计。如果使用其他比例的显示屏，为获得更佳的视觉效果，需要重新使用三维软件建模，并导出相应的OBJ文件，制作GUI可以加载的描述文件（具体步骤请参考 :ref:`GUI加载棱镜模型` ）。
3. 三维建模导出的OBJ文件需要配置Y轴为前进轴。
4. 请从以下路径获取棱镜信息描述子所需的转换文件：

   + 文件路径： :file:`HoneyGUI\\realgui\\example\\demo\\3d` 
   + 所需文件： :file:`extract_desc.exe` 和 :file:`png2c.py`

示例
----
棱镜
~~~~~~~~
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