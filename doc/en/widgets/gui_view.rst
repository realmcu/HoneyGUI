.. _View:

=====================
View
=====================

The view widget is a kind of container that makes switching more convenient. Any new view widget can be created in real time in response to an event and multiple switching effects can be selected, and when the widget is not switched, only the currently displayed view widget exists in memory, which can effectively reduce memory consumption.

.. figure:: https://foruda.gitee.com/images/1739771693617989699/15c8e9e5_10737458.jpeg
   :align: center
   :width: 500px

   View Distribution

Usage
-----
Create View Widget
~~~~~~~~~~~~~~
The :cpp:any:`gui_view_create` function can be used to establish a view widget.

Set View Style
~~~~~~~~~~~~~~~~~~
The :cpp:any:`gui_view_set_style` function can be used to add a event for this view widget to change to another view. The available switching styles include the following:

.. literalinclude:: ../../../realgui/widget/gui_view.h
   :language: c
   :start-after: /* VIEW_CHANGE_STYLE enum start*/
   :end-before: /* VIEW_CHANGE_STYLE enum end*/

Enable View Caching
~~~~~~~~~~~~~~
The :cpp:any:`gui_view_enable_pre_load` function can be used to enable view widget caching。

Example
-----
View
~~~~~~~~~~
You can set a different switching style for each view widget.

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

   static void create_view_1(void *parent);
   static void create_view_2(void *parent);
   static void create_view_0(void *parent)
   {
      gui_view_t *view = (gui_view_t *)(GUI_BASE(parent)->parent);
      gui_img_t *img = gui_img_create_from_mem(parent, "img", UI_CLOCK_FACE_MAIN_BIN, 0, 0, 0,
                                                0);
      gui_view_add_change_event(view, (void **)&view_1, VIEW_REDUCTION, VIEW_REDUCTION,create_view_1,
                                 VIEW_EVENT_MOVE_RIGHT);
      gui_view_add_change_event(view, (void **)&view_2, VIEW_CUBE, VIEW_CUBE,create_view_2,
                                 VIEW_EVENT_MOVE_LEFT);
      gui_view_add_change_event(view, NULL, VIEW_ROTATE_BOOK, VIEW_ROTATE_BOOK, create_view_0,
                                 VIEW_EVENT_MOVE_UP);
      gui_view_add_change_event(view, (void **)&view_2, VIEW_ROTATE_BOOK, VIEW_ROTATE_BOOK, create_view_2,
                                 VIEW_EVENT_MOVE_DOWN);
      gui_view_add_change_event(view, (void **)&view_1, VIEW_ANIMATION_6, VIEW_ANIMATION_2, create_view_1,
                                 VIEW_EVENT_CLICK);
   }

   static void create_view_1(void *parent)
   {
      gui_img_t *img = gui_img_create_from_mem(parent, "img", WATCHFACE_BUTTERFLY_BIN, 0, 0, 0,
                                                0);
      gui_img_scale(img, 2, 2);
      gui_view_add_change_event(view_1, (void **)&view_0, VIEW_REDUCTION, VIEW_REDUCTION, create_view_0, VIEW_EVENT_MOVE_LEFT);
      gui_view_add_change_event(view_1, (void **)&view_2, VIEW_ROTATE, VIEW_ROTATE, create_view_2,
                                 VIEW_EVENT_MOVE_RIGHT);
      gui_view_add_change_event(view_1, (void **)&view_1, VIEW_CLASSIC, VIEW_CLASSIC, create_view_1,
                                 VIEW_EVENT_MOVE_UP);
      gui_view_add_change_event(view_1, (void **)&view_0, VIEW_ROTATE, VIEW_ROTATE, create_view_0,
                                 VIEW_EVENT_MOVE_DOWN);
      gui_view_add_change_event(view_1, (void **)&view_2, VIEW_ANIMATION_7, VIEW_ANIMATION_3, create_view_2,
                                 VIEW_EVENT_CLICK);
   }
   static void create_view_2(void *parent)
   {
      gui_img_t *img = gui_img_create_from_mem(parent, "img", WATCHFACE_RING_BIN, 0, 0, 0, 0);
      gui_img_scale(img, 2, 2);
      gui_view_add_change_event(view_2, (void **)&view_0, VIEW_CUBE, VIEW_CUBE,  create_view_0,
                                 VIEW_EVENT_MOVE_RIGHT);
      gui_view_add_change_event(view_2, (void **)&view_1, VIEW_ROTATE, VIEW_ROTATE, create_view_1,
                                 VIEW_EVENT_MOVE_LEFT);
      gui_view_add_change_event(view_2, (void **)&view_2, VIEW_CLASSIC, VIEW_CLASSIC, create_view_2,
                                 VIEW_EVENT_MOVE_UP);
      gui_view_add_change_event(view_2, (void **)&view_1, VIEW_CLASSIC, VIEW_CLASSIC, create_view_1,
                                 VIEW_EVENT_MOVE_DOWN);
      gui_view_add_change_event(view_2, (void **)&view_0, VIEW_CLASSIC, VIEW_CLASSIC, create_view_0,
                                 VIEW_EVENT_MOVE_UP);
      gui_view_add_change_event(view_2, (void **)&view_0, VIEW_ANIMATION_8, VIEW_ANIMATION_4, create_view_0,
                                 VIEW_EVENT_CLICK);
   }

   void app_hongkong_ui_design(gui_app_t *app)
   {
      win_view = gui_win_create(app->window, "win_view", 0, 0, 0, 0);
      view_0 = gui_view_create(win_view, "view", 0, 0, 0, 0);
      create_view_0(view_0->rte_obj);
      gui_fps_create(app->window);
   }
   
   
.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1739771196758337648/38c43e3a_10737458.gif" width= "400" /></div>
   <br>

API
-----

.. doxygenfile:: gui_view.h

