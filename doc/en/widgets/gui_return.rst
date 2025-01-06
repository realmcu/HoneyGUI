Return
======

The bubble-shaped curve that can be dragged out on the left side of the screen is used to perform a return operation.

Usage
-----

Create A Return Widget
~~~~~~~~~~~~~~~~~~~~~~
Creating function is :cpp:any:`gui_return_create`. Users need to enter curve animation frame images file array and the return operation function.


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