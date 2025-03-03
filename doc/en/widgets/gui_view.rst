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
Register Descriptor of View
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The :cpp:any:`gui_view_descriptor_register` function can be used to register descriptor of view in the descriptor list for other view to read and use as a parameter to create the view, via passing in the descriptor's address. The ``gui_view_descriptor`` structure is defined as follows:

.. literalinclude:: ../../../realgui/widget/gui_view/gui_view.h
   :language: c
   :start-after: /* gui_view_descriptor start*/
   :end-before: /* gui_view_descriptor end*/


Get Descriptor of View by Name
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The :cpp:any:`gui_view_descriptor_get` function can be used to get the view descriptor with the corresponding ``name``  by passing in the string.


Create View Widget
~~~~~~~~~~~~~~~~~~~
The :cpp:any:`gui_view_create` function can be used to establish a view widget.


Set Switch View Event
~~~~~~~~~~~~~~~~~~~~~~~~
The :cpp:any:`gui_view_switch_on_event` function can be used to set switch view event. Repeatable settings for a particular event will use the latest descriptor. Specific events include ``GUI_EVENT_TOUCH_CLICKED`` 、 ``GUI_EVENT_KB_SHORT_CLICKED`` 、 ``GUI_EVENT_TOUCH_MOVE_LEFT``、 ``GUI_EVENT_TOUCH_MOVE_RIGHT`` and so on. The available switching styles include the following:

.. literalinclude:: ../../../realgui/widget/gui_view/gui_view.h
   :language: c
   :start-after: /* VIEW_SWITCH_STYLE enum start*/
   :end-before: /* VIEW_SWITCH_STYLE enum end*/


Switch View Directly
~~~~~~~~~~~~~~~~~~~~~~~
The :cpp:any:`gui_view_switch_direct` function can be used to switch view directly, which can be used in conjunction with events or animations of the child widgets based on view. Note that the switching style is limited to the animation style and cannot be set to the sliding style.

Get Current View Pointer
~~~~~~~~~~~~~~~~~~~~~~~~~~
The :cpp:any:`gui_view_get_current_view` function can be used to get current view pointer, and can be used with :cpp:any:`gui_view_switch_direct` to switch the current view.

Example
---------
View
~~~~~~~~~~
Below are three separate C files, each containing a descriptor for the view and the design function.

.. literalinclude:: ../../../realgui/example/demo/view_demo/app_ui_view_blue.c
   :language: c
   :start-after: /* view blue start*/
   :end-before: /* view blue end*/


.. literalinclude:: ../../../realgui/example/demo/view_demo/app_ui_view_white.c
   :language: c
   :start-after: /* view white start*/
   :end-before: /* view white end*/


.. literalinclude:: ../../../realgui/example/demo/view_demo/app_ui_view_yellow.c
   :language: c
   :start-after: /* view yellow start*/
   :end-before: /* view yellow end*/


.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1739865862383021432/c0d24c3a_10737458.gif" width= "400" /></div>
   <br>

API
-----

.. doxygenfile:: gui_view.h

