Menu Cellular
=============

The menu cellular widget displays a set of icons in a honeycomb shape menu.

Usage
-----

Create Menu Cellular Widget
~~~~~~~~~~~~~~~~~~~~~~~~~~~
Creating function is :cpp:any:`gui_menu_cellular_create`. Users need to enter the icons set's image file array.


Config Offset
~~~~~~~~~~~~~
Function is :cpp:any:`gui_menu_cellular_offset`. Users are able to adjust horizontal and vertical offset for better display.



Example
-------


.. literalinclude:: ../../../realgui/example/screen_454_454/gui_menu/apps_in_menu.c
   :language: c
   :start-after: /*Define APP_MENU's entry func */
   :end-before: /*Define APP_MENU's entry func end*/


.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/menu_cellular.gif" width= "400" /></div>
   <br>

API
---

.. doxygenfile:: gui_menu_cellular.h