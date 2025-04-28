.. _List:

=====================
List
=====================

The list widget is a container for any number of list table, horizontally or vertically, and each table can have other widgets added to it, such as text widgets, image widgets, etc. Users can swipe up, down, left, or right to access other widgets added to the list table.

.. raw:: html

   <br>
   <div style="text-align: center">
      <img src="https://foruda.gitee.com/images/1745465574757113257/5e8ee605_10737458.gif" width= "400" />
      <p style="margin: 5px 0 0 0; text-align: center;">VERTICAL CLASSIC LIST</p>
   </div>
   <br>


Usage
--------
Create List Widget
~~~~~~~~~~~~~~~~~~~~~
The :cpp:any:`gui_list_create` function can be used to create a list widget, the next level of the list widget can only be a table widget. The total length of the list widget is determined by the number of table added, the length of the table, and the space between the tables; as more tables are added, the length increases.


Create List Table Widget
~~~~~~~~~~~~~~~~~~~~~~~~~~~
The :cpp:any:`gui_list_add_tab` function can be used to create a list table widget. Users can add other widgets to the table.


Set List Widget Style
~~~~~~~~~~~~~~~~~~~~~~~~
The :cpp:any:`gui_list_set_style` function can be used to set list moving style. The default is ``LIST_CLASSIC``, and the available styles include the following:


.. literalinclude:: ../../../realgui/widget/gui_list/gui_list.h
   :language: c
   :start-after: /* LIST_STYLE enum start*/
   :end-before: /* LIST_STYLE enum end*/


Set List Widget Deceleration Factor
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The :cpp:any:`gui_list_set_factor` function can be used to set list deceleration factor.


Set List Widget Offset
~~~~~~~~~~~~~~~~~~~~~~~
The :cpp:any:`gui_list_set_offset` function can be used to set list offset of position.

Example
---------

.. literalinclude:: ../../../example/widget/list/example_gui_list.c
   :language: c
   :start-after: /* gui list widget example start*/
   :end-before: /* gui list widget example end*/


.. raw:: html

   <br>
   <div style="display: flex; justify-content: space-between;">
      <div style="width: 30%; text-align: center;">
         <img src="https://foruda.gitee.com/images/1745398396237626690/156a4565_10737458.gif" style="width: 100%;">
         <p style="margin: 5px 0 0 0; text-align: center;">VERTICAL CIRCLE STYLE</p>
      </div>
      <div style="width: 30%; text-align: center;">
         <img src="https://foruda.gitee.com/images/1745829566941521625/3f80e640_10737458.gif" style="width: 100%;">
         <p style="margin: 5px 0 0 0; text-align: center;">VERTICAL CARD STYLE</p>
      </div>
      <div style="width: 30%; text-align: center;">
         <img src="https://foruda.gitee.com/images/1745398410976550394/fb4282a5_10737458.gif" style="width: 100%;">
         <p style="margin: 5px 0 0 0; text-align: center;">VERTICAL ZOOM STYLE</p>
      </div>
   </div>
   <div style="display: flex; justify-content: space-between;">
      <div style="width: 30%; text-align: center;">
         <img src="https://foruda.gitee.com/images/1745819904731135543/290c9d96_10737458.gif" style="width: 100%;">
         <p style="margin: 5px 0 0 0; text-align: center;">VERTICAL FAN STYLE</p>
      </div>
      <div style="width: 30%; text-align: center;">
         <img src="https://foruda.gitee.com/images/1745819920755179437/13b09db2_10737458.gif" style="width: 100%;">
         <p style="margin: 5px 0 0 0; text-align: center;">VERTICAL HELIX STYLE</p>
      </div>
   </div>
   <br>

API
-----

.. doxygenfile:: gui_list.h

