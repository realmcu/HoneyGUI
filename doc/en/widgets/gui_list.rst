.. _List:

=====================
List
=====================

The list widget is a container for any number of list note, horizontally or vertically. And each note, which is dynamic created for saving memory and CPU usage, can have other widgets added to it, such as text widgets, image widgets, etc. Users can swipe up, down, left, or right to access other widgets added to the list note.

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
The :cpp:any:`gui_list_create` function can be used to create a list widget, in which the scroll bar is optional, and the next level of the list widget can only be a note widget. In ``note_design``, the corresponding child widget could be designed according to the ``index`` of the list widget to dynamically create the note widget. The total length of the list widget is determined by the number of note added, the length of the note, and the space between the tables; as more tables are added, the length increases.


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


Set List Widget Scroll Bar Color
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The :cpp:any:`gui_list_set_bar_color` function can be used to set scroll bar color of list widget.


Set List Widget Note Number
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The :cpp:any:`gui_list_set_note_num` function can be used to set note number of list widget。

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
         <img src="https://foruda.gitee.com/images/1750920685777460993/280f951c_10737458.gif" style="width: 100%;">
         <p style="margin: 5px 0 0 0; text-align: center;">VERTICAL CARD STYLE</p>
      </div>
      <div style="width: 30%; text-align: center;">
         <img src="https://foruda.gitee.com/images/1745398410976550394/fb4282a5_10737458.gif" style="width: 100%;">
         <p style="margin: 5px 0 0 0; text-align: center;">HORIZONTAL ZOOM STYLE</p>
      </div>
   </div>
   <div style="display: flex; justify-content: space-between;">
      <div style="width: 30%; text-align: center;">
         <img src="https://foruda.gitee.com/images/1745819904731135543/290c9d96_10737458.gif" style="width: 100%;">
         <p style="margin: 5px 0 0 0; text-align: center;">VERTICAL FAN STYLE</p>
      </div>
      <div style="width: 30%; text-align: center;">
         <img src="https://foruda.gitee.com/images/1750916286131886532/8af671b7_10737458.gif" style="width: 100%;">
         <p style="margin: 5px 0 0 0; text-align: center;">VERTICAL HELIX STYLE</p>
      </div>
      <div style="width: 30%; text-align: center;">
         <img src="https://foruda.gitee.com/images/1745890329636631265/cef196cd_10737458.gif" style="width: 100%;">
         <p style="margin: 5px 0 0 0; text-align: center;">VERTICAL CRUL STYLE</p>
      </div>
   </div>
   <br>

API
-----

.. doxygenfile:: gui_list.h

