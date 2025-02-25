.. _View:

=====================
View
=====================

The view widget is a kind of container that makes switching more convenient. Any new view widget can be created in real time in response to an event(clicking and sliding in all four directions...) and multiple switching effects can be selected. During the switching process, there will be two views in the memory, and after the switching is completed, the undisplayed view will be automatically cleaned up, which can effectively reduce the memory consumption.

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1739867379534372641/4058df7c_10737458.gif" width= "400" /></div>
   <br>


Usage
-------
Create View Widget
~~~~~~~~~~~~~~~~~~~
The :cpp:any:`gui_view_create` function can be used to establish a view widget.

Add Change Event
~~~~~~~~~~~~~~~~~~
The :cpp:any:`gui_view_add_change_event` function can be used to add a event for this view widget to change to another view. Specific events include ``GUI_EVENT_TOUCH_CLICKED`` 、 ``GUI_EVENT_TOUCH_MOVE_LEFT``、 ``GUI_EVENT_TOUCH_MOVE_LEFT`` and so on. The available switching styles include the following:

.. literalinclude:: ../../../realgui/widget/gui_view.h
   :language: c
   :start-after: /* VIEW_CHANGE_STYLE enum start*/
   :end-before: /* VIEW_CHANGE_STYLE enum end*/

Enable View Caching
~~~~~~~~~~~~~~~~~~~~~
The :cpp:any:`gui_view_enable_pre_load` function can be used to enable view widget caching。

Example
---------
View
~~~~~~~~~~
You can set a different switching style for each view widget.

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

