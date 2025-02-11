.. _Curtain:

========
Curtain
========

The curtain widget enables you to create curtains in five directions: up, down, left, right, and middle (refer to :ref:`T_GUI_CURTAIN_ENUM1`). Before creating the curtain widget, you must first create a curtainview widget to accommodate the curtain.

Usage
-----
Create Curtain Widget
~~~~~~~~~~~~~~~~~~~~~~
If you want to create a curtain widget with a defined direction, you may opt to utilize the :cpp:any:`gui_curtain_create` function.
``orientation`` represents direction and can create curtain widgets in five different directions: up, down, left, right, and center.
``Scope`` refers to the curtain range that can be extended, which falls between 0 and 1.

.. _T_GUI_CURTAIN_ENUM1:

Orientation
~~~~~~~~~~~

There are five orientations in curtain widget.

.. literalinclude:: ../../../realgui/widget/gui_curtainview.h
   :language: c
   :start-after: /* T_GUI_CURTAIN_ENUM start*/
   :end-before: /* T_GUI_CURTAIN_ENUM end*/


Example
--------

.. literalinclude:: ../../../realgui/example/screen_448_368/app_tb_clock.c
   :language: c
   :start-after: /* curtain example start*/
   :end-before: /* curtain example end*/


.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1699869962427925475/4a382788_10641540.png" width= "400" /></div>
   <br>


API
-----

.. doxygenfile:: gui_curtain.h
