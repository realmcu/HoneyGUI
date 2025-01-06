Menu Cellular
=============

The menu cellular widget displays a set of icons in a honeycomb shape menu.

Usage
-----

Create Menu Cellular Widget
~~~~~~~~~~~~~~~~~~~~~~~~~~~
Creating function is :ref:`gui_menu_cellular_create`. Users need to enter the icons set's image file array.

.. _gui_menu_cellular_create:

.. function:: gui_obj_t* gui_menu_cellular_create(void *parent, int icon_size, uint32_t *icon_array[], int array_size)

   Create a menu cellular widget with the given parameters.

   :param parent: Pointer to the parent object.
   :param icon_size: The size of each icon in pixels.
   :param icon_array: Array of icon images.
   :param array_size: Number of icons in the array.
   :return: Pointer to the created menu cellular object.

Config Offset
~~~~~~~~~~~~~
Function is :ref:`gui_menu_cellular_offset`. Users are able to adjust horizontal and vertical offset for better display.

.. _gui_menu_cellular_offset:

.. function:: void gui_menu_cellular_offset(gui_obj_t *menu_cellular, int offset_x, int offset_y)

   Adjust the offset of the menu cellular widget.

   :param menu_cellular: Pointer to the menu cellular object.
   :param offset_x: Horizontal offset.
   :param offset_y: Vertical offset.

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