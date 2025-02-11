.. _Tabview:

==========
Tabview
==========

The tabview widget is the container to store and depends on user selection to change the tab for different functions. As shown in the following figure, each rectangle represents a different tab widget, and 9 tab widgets together form a tab container, with the blue area representing the current display area. The ``idx`` stands for the index on the X-axis, and the ``idy`` stands for the index on Y-axis.

.. figure:: https://foruda.gitee.com/images/1727058851305177488/b3542c64_13408154.png
   :align: center
   :width: 600px

   Tab Distribution

Usage
-----
Create Tabview Widget
~~~~~~~~~~~~~~~~~~~~~~
The :cpp:any:`gui_tabview_create` function can be used to establish a tabview.

Switch the Tab
~~~~~~~~~~~~~~~~
In usage scenarios, tab switching is generally accomplished by horizontal or vertical swiping. However, if users wish to quickly navigate to a specific tab page, they can use the function :cpp:any:`gui_tabview_jump_tab` to achieve this. This function allows for jumping to a specified tab page by using the indices ``idx`` and ``idy`` to specify the position of the tab page.

Set Tabview Style
~~~~~~~~~~~~~~~~~
Developers can use :cpp:any:`gui_tabview_set_style` to set the desired style of tabview. By default, the classic style is ``CLASSIC``. The available switching effects include the following:


.. literalinclude:: ../../../realgui/widget/gui_tabview.h
   :language: c
   :start-after: /* T_SLIDE_STYLE enum start*/
   :end-before: /* T_SLIDE_STYLE enum end*/


Set Tabview Loop
~~~~~~~~~~~~~~~~~
The tabview loop function is a clever feature that allows users to easily navigate through all the tabs that have been created. By enabling the reverse loop display function using :cpp:any:`gui_tabview_loop_x` for the x-axis and :cpp:any:`gui_tabview_loop_y` for the y-axis, as shown in Figure-a, you will see a blue outer box indicating the visible area of the screen for the tab widget. The direction of the arrow represents the direction of tab switching. When swiping left, once all tabs have moved to the left side of the visible area, another left swipe will cycle back to the leftmost tab that was created. Conversely, as shown in Figure-b, when swiping right again, the display will cycle from the rightmost tab.


.. figure:: https://foruda.gitee.com/images/1737357436952016270/569e58cc_13408154.png
   :align: center
   :width: 400px

   Figure-a


.. figure:: https://foruda.gitee.com/images/1737357500617800758/e845c1e5_13408154.png
   :align: center
   :width: 400px

   Figure-b


Example
-------
Please refer to the section: :ref:`Tab`


API
-----

.. doxygenfile:: gui_tabview.h

