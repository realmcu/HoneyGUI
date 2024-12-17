.. _Cardview:

=========
Cardview
=========

Card effect container widget, which can nest cards. You can switch cards by swiping.

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1700123394899348792/914aadf9_10641540.png" width= "500" /></div>
   <br>

Usage
-----

Create Widget
~~~~~~~~~~~~~
Using :cpp:any:`gui_cardview_create` to create a cardview widget.

Callback
~~~~~~~~~~~~~
When the state of the cardview changes, it triggers a callback, and the callback function :cpp:any:`gui_cardview_status_cb` can be used to perform specific actions.

Set Style
~~~~~~~~~~~~~
There are five styles in the cardview widget :cpp:any:`gui_cardview_set_style`.

.. literalinclude:: ../../../realgui/widget/gui_tabview.h
   :language: c
   :start-after: /* T_SLIDE_STYLE enum start*/
   :end-before: /* T_SLIDE_STYLE enum end*/
   

Set Center Alignment
~~~~~~~~~~~~~~~~~~~~
This function :cpp:any:`gui_cardview_alignment` sets the card to automatic center alignment.

Set Bottom Space
~~~~~~~~~~~~~~~~~
This function :cpp:any:`gui_cardview_set_bottom_space` sets the bottom spacing of the card.

Y-axis Slide Callback
~~~~~~~~~~~~~~~~~~~~~~
The event of the card container sliding on the Y-axis can add the corresponding callback function :cpp:any:`gui_cardview_up`.

Example
-------
Please refer to the section: :ref:`Card`

API
-----

.. doxygenfile:: gui_cardview.h

