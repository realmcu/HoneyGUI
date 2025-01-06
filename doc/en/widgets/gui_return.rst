Return
======

The bubble-shaped curve that can be dragged out on the left side of the screen is used to perform a return operation.

Usage
-----

Create A Return Widget
~~~~~~~~~~~~~~~~~~~~~~
Creating function is :ref:`gui_return_create`. Users need to enter curve animation frame images file array and the return operation function.

.. _gui_return_create:

.. function:: gui_obj_t* gui_return_create(void *parent, const uint32_t *frame_array[], int array_size, void *return_cb, gui_obj_t *ignore_gesture_widget)

   Create a return widget with the given parameters.

   :param parent: Pointer to the parent object.
   :param frame_array: Array of frame images for the animation.
   :param array_size: Number of frames in the array.
   :param return_cb: Callback function to perform the return operation.
   :param ignore_gesture_widget: Pointer to a widget that should ignore gesture events.
   :return: Pointer to the created return widget object.

Example
-------



.. literalinclude:: ../../../realgui/example/screen_454_454/gui_menu/apps_in_menu.c
   :language: c
   :start-after: /*Define gui_app_return_array*/
   :end-before: /*Define gui_app_return_array end*/

.. literalinclude:: ../../../realgui/example/screen_454_454/gui_menu/apps_in_menu.c
   :language: c
   :start-after: /*gui_return_create*/
   :end-before: /*gui_return_create end*/


.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/return.gif" width= "400" /></div>
   <br>
.. note::
   Please refer to ``realgui\example\screen_454_454\gui_menu\apps_in_menu.c`` for details.

API
---

.. doxygenfile:: gui_return.h