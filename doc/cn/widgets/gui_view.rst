.. _视图 (View):

=====================
视图 (View)
=====================

视图控件 (View) 类似于不需要容器的选项卡控件，可以通过滑动事件响应实时创建新的视图控件，不切换控件时内存只存在当前显示的视图控件，可以有效降低内存消耗。

.. figure:: https://foruda.gitee.com/images/1739238552753961353/f6edc9f6_10737458.jpeg
   :align: center
   :width: 500px

   View Distribution

用法
-----
创建视图控件
~~~~~~~~~~~~~~
使用 :cpp:any:`gui_view_create` 函数可以创建一个视图控件，其中 ``idx`` 和 ``idy`` 表示创建控件的相对位置。


设置视图控件切换风格
~~~~~~~~~~~~~~~~~~
开发者可以使用 :cpp:any:`gui_view_set_style` 来设置所需的视图控件切出样式。默认情况下，使用的是经典样式 ``CLASSIC``。具体的切换风格请参考下列枚举：

.. literalinclude:: ../../../realgui/widget/gui_view.h
   :language: c
   :start-after: /* VIEW_SLIDE_STYLE enum start*/
   :end-before: /* VIEW_SLIDE_STYLE enum end*/


示例
-----
视图控件
~~~~~~~~~~
可以对每一个视图控件设置不同的切换风格。

.. code-block:: c

   #include <gui_obj.h>
   #include <gui_win.h>
   #include "root_image_hongkong/ui_resource.h"
   #include <gui_app.h>
   #include "app_hongkong.h"
   #include "gui_win.h"
   #include "gui_server.h"
   #include "gui_components_init.h"
   #include <stdio.h>
   #include "guidef.h"
   #include "wheel_algo.h"
   #include "kb_algo.h"
   #include "gui_view.h"
   gui_view_t *view_0 = NULL;
   gui_view_t *view_1 = NULL;
   gui_view_t *view_2 = NULL;
   gui_win_t *win_view = NULL;

   static void create_view_1(void *obj, gui_event_t e, void *param);
   static void create_view_2(void *obj, gui_event_t e, void *param);

   static void create_view_0(void *obj, gui_event_t e, void *param)
   {
      gui_list_t list = GUI_BASE(obj)->parent->child_list;
      if (list.prev != list.next)
      {
         return;
      }
      T_GUI_VIEW_EVENT code = (T_GUI_VIEW_EVENT)e;
      int16_t idx = 0, idy = 0;
      switch (code)
      {
      case VIEW_EVENT_MOVE_LEFT:
         idx = 1;
         break;
      case VIEW_EVENT_MOVE_RIGHT:
         idx = -1;
         break;
      case VIEW_EVENT_MOVE_UP:
         idy = 1;
         break;
      case VIEW_EVENT_MOVE_DOWN:
         idy = -1;
         break;
      default:
         break;
      }
      view_0 = gui_view_create(win_view, "view_0", 0, 0, 0, 0, idx, idy);
      gui_img_t *img = gui_img_create_from_mem(view_0->rte_obj, "img", UI_CLOCK_FACE_MAIN_BIN, 0, 0, 0,
                                                0);
      gui_view_add_change_event(view_0, view_1, create_view_1, VIEW_EVENT_MOVE_RIGHT);
      gui_view_add_change_event(view_0, view_2, create_view_2, VIEW_EVENT_MOVE_LEFT);
   }

   static void create_view_1(void *obj, gui_event_t e, void *param)
   {
      gui_list_t list = GUI_BASE(obj)->parent->child_list;
      if (list.prev != list.next)
      {
         return;
      }
      T_GUI_VIEW_EVENT code = (T_GUI_VIEW_EVENT)e;
      int16_t idx = 0, idy = 0;
      switch (code)
      {
      case VIEW_EVENT_MOVE_LEFT:
         idx = 1;
         break;
      case VIEW_EVENT_MOVE_RIGHT:
         idx = -1;
         break;
      case VIEW_EVENT_MOVE_UP:
         idy = 1;
         break;
      case VIEW_EVENT_MOVE_DOWN:
         idy = -1;
         break;
      default:
         break;
      }
      view_1 = gui_view_create(win_view, "view_1", 0, 0, 0, 0, idx, idy);
      gui_view_set_style(view_1, VIEW_CUBE);
      gui_img_t *img = gui_img_create_from_mem(view_1->rte_obj, "img", WATCHFACE_BUTTERFLY_BIN, 0, 0, 0,
                                                0);
      gui_img_scale(img, 2, 2);
      gui_view_add_change_event(view_1, view_0, create_view_0, VIEW_EVENT_MOVE_LEFT);
   }
   static void create_view_2(void *obj, gui_event_t e, void *param)
   {
      gui_list_t list = GUI_BASE(obj)->parent->child_list;
      if (list.prev != list.next)
      {
         return;
      }
      T_GUI_VIEW_EVENT code = (T_GUI_VIEW_EVENT)e;
      int16_t idx = 0, idy = 0;
      switch (code)
      {
      case VIEW_EVENT_MOVE_LEFT:
         idx = 1;
         break;
      case VIEW_EVENT_MOVE_RIGHT:
         idx = -1;
         break;
      case VIEW_EVENT_MOVE_UP:
         idy = 1;
         break;
      case VIEW_EVENT_MOVE_DOWN:
         idy = -1;
         break;
      default:
         break;
      }
      view_2 = gui_view_create(win_view, "view_2", 0, 0, 0, 0, idx, idy);
      gui_view_set_style(view_2, VIEW_ROTATE);
      gui_img_t *img = gui_img_create_from_mem(view_2->rte_obj, "img", WATCHFACE_RING_BIN, 0, 0, 0, 0);
      gui_img_scale(img, 2, 2);
      gui_view_add_change_event(view_2, view_0, create_view_0, VIEW_EVENT_MOVE_RIGHT);
   }
   static void app_hongkong_ui_design(gui_app_t *app)
   {
      win_view = gui_win_create(app->window, "win_view", 0, 0, 0, 0);
      view_0 = gui_view_create(win_view, "view_0", 0, 0, 0, 0, 0, 0);
      gui_img_create_from_mem(view_0->rte_obj, "img", UI_CLOCK_FACE_MAIN_BIN, 0, 0, 0, 0);
      gui_view_add_change_event(view_0, view_1, create_view_1, VIEW_EVENT_MOVE_RIGHT);
      gui_view_add_change_event(view_0, view_2, create_view_2, VIEW_EVENT_MOVE_LEFT);
      gui_view_set_style(view_0, VIEW_CLASSIC);
   }
   
   
.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1739236228669019223/2005f3cd_10737458.gif" width= "400" /></div>
   <br>

API
-----

.. doxygenfile:: gui_view.h

