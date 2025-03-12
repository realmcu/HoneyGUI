.. _卡片容器 (Cardview):

===================
卡片容器 (Cardview)
===================
卡片效果容器控件，可以嵌套卡片，可以通过滑动来切换卡片。

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1700123394899348792/914aadf9_10641540.png" width= "500" /></div>
   <br>

用法
-----

创建控件
~~~~~~~~
开发者可以调用 :cpp:any:`gui_cardview_create` 来创建卡片容器控件。

回调
~~~~~
当卡片容器的状态改变时，它会触发一个回调，开发者可以在回调函数 :cpp:any:`gui_cardview_status_cb` 来做想做的事情。

设置风格
~~~~~~~~
卡片容器控件有5种风格，开发者可以使用 :cpp:any:`gui_cardview_set_style` 来设置风格。

.. literalinclude:: ../../../realgui/widget/gui_tabview/gui_tabview.h
   :language: c
   :start-after: /* T_SLIDE_STYLE enum start*/
   :end-before: /* T_SLIDE_STYLE enum end*/
   

设置中心对齐
~~~~~~~~~~~~
开发者可以调用函数 :cpp:any:`gui_cardview_alignment` 来设置卡片自动中心对齐。

设置底部间隔
~~~~~~~~~~~~
开发者可以调用函数 :cpp:any:`gui_cardview_set_bottom_space` 来设置卡片的底部间隔。

y轴方向滑动回调
~~~~~~~~~~~~~~~
卡片容器在y轴方向滑动的事件可以添加相应的回调函数 :cpp:any:`gui_cardview_up`。

示例
-----
请参考该章节：:ref:`卡片 (Card)` 


API
-----

.. doxygenfile:: gui_cardview.h


