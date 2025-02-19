.. _视图 (View):

=====================
视图 (View)
=====================

视图控件 (View) 是一种切换更为便利的容器控件，通过事件响应（点击以及四个方向的滑动等）可以实时创建任一视图控件并可选择多种切换效果。切换过程中内存中会存在两个view，切换完成后会自动清理未显示的view，可以有效降低内存消耗。

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1739867379534372641/4058df7c_10737458.gif" width= "400" /></div>
   <br>


用法
-----
创建视图控件
~~~~~~~~~~~~~~
使用 :cpp:any:`gui_view_create` 函数可以创建一个视图控件。


设置视图切换事件
~~~~~~~~~~~~~~~~~~
开发者可以使用 :cpp:any:`gui_view_add_change_event` 来设置视图切换事件。具体的事件类型包括 ``GUI_EVENT_TOUCH_CLICKED`` 、 ``GUI_EVENT_TOUCH_MOVE_LEFT``、 ``GUI_EVENT_TOUCH_MOVE_LEFT`` 等。具体的切换风格请参考下列枚举：

.. literalinclude:: ../../../realgui/widget/gui_view.h
   :language: c
   :start-after: /* VIEW_CHANGE_STYLE enum start*/
   :end-before: /* VIEW_CHANGE_STYLE enum end*/


使能视图缓存功能
~~~~~~~~~~~~~~~~~
使用 :cpp:any:`gui_view_enable_pre_load` 函数使能视图缓存功能。

示例
-----
视图控件
~~~~~~~~~~
可以对每一个视图控件设置多种切换事件。

.. literalinclude:: ../../../realgui/example/demo/app_ui_realgui_view.c
   :language: c
   :start-after: /* view demo start*/
   :end-before: /* view demo end*/



.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1739865862383021432/c0d24c3a_10737458.gif" width= "400" /></div>
   <br>

API
-----

.. doxygenfile:: gui_view.h

