=======================
2.5D足球 (2.5D Soccer)
=======================

2.5D足球由12个顶点和20个面组成，这些顶点和面可以构成一个近似3D效果的球形。其中，顶点定义了球体的形状和位置，面是由多个顶点连接而成的平面，用于填充球的表面。

用法
-----
创建控件
^^^^^^^^
使用 :cpp:any:`gui_soccer_create` 创建一个2.5D足球控件。其中 ``addr`` 是用于填充足球白色面片的图像。

设置大小
^^^^^^^^
足球顶点之间的默认距离是100，可以通过 :cpp:any:`gui_soccer_set_size` 改变顶点之间的距离，从而改变足球控件大小。

设置中心
^^^^^^^^
足球控件的默认中心是 ``((dc->fb_width - this->scsize) / 2.0f)``，其中 ``this->scsize`` 为足球顶点距离。使用 :cpp:any:`gui_soccer_set_center` 可以改变足球的中心坐标。

设置图像模式
^^^^^^^^^^^^
足球控件的默认图像混合模式是 ``IMG_SRC_OVER_MODE``，可以通过 :cpp:any:`gui_soccer_set_mode` 来改变图像混合模式。

设置图像透明度
^^^^^^^^^^^^^^^^
可以通过 :cpp:any:`gui_soccer_set_opacity` 来改变图像的透明度。

设置足球面片图像
^^^^^^^^^^^^^^^^^^
可以通过 :cpp:any:`gui_soccer_set_img` 来设置足球的面片图像。


示例
----

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