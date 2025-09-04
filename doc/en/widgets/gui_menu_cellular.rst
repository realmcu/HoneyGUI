Menu Cellular
=============

The menu cellular widget displays a set of icons in a honeycomb shape menu.

Usage
-----

Create Menu Cellular Widget
~~~~~~~~~~~~~~~~~~~~~~~~~~~
The :cpp:any:`gui_menu_cellular_create` function can be used to create a menu cellular widget. Users need to enter the icons set's image file array.

Add Click Callback Event
~~~~~~~~~~~~~~~~~~~~~~~~~~~
The :cpp:any:`gui_menu_cellular_on_click` function can be used to add click callback event.

Set Offset
~~~~~~~~~~~~~
The :cpp:any:`gui_menu_cellular_offset` function can be used to adjust horizontal and vertical offset for better display.



Example
-------
Example of using cellular menus is as follows. First, use the canvas widget to draw a set of solid-color icons, and then pass these icons to the cellular menu widget to display.

.. literalinclude:: ../../../example/widget/menu_cellular/example_gui_menu_cellular.c
   :language: c
   :start-after: /* gui menu cellular widget example start*/
   :end-before: /* gui menu cellular widget example end*/


.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1751456456790100801/2b635428_10737458.gif" width= "400" /></div>
   <br>

API
---

.. doxygenfile:: gui_menu_cellular.h