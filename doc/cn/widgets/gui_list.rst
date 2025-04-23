.. _列表 (List):

=====================
列表 (List)
=====================

列表控件 (List) 作为容器可以在水平或垂直方向上添加任意数量的表格，每个表格都可以添加其他的控件，如按钮控件，图片控件等等，用户可以通过上下左右滑动来访问添加到列表控件上的其他控件。

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1745398337318690441/61f817f2_10737458.gif" width= "400" /></div>
   <br>


用法
-----
创建列表控件
~~~~~~~~~~~~~~~~~~
使用 :cpp:any:`gui_list_create` 函数来创建一个列表控件，列表控件的下一级只能是表格控件。列表控件的总长度由添加的表格数量、表格长度以及表格间隔决定，当添加的表格越多时，其长度也越大。


创建表格控件
~~~~~~~~~~~~~~~~~~
使用 :cpp:any:`gui_list_add_tab` 函数创建表格，用户可以在表格中添加其他控件。


设置列表控件的样式
~~~~~~~~~~~~~~~~~~
使用 :cpp:any:`gui_list_set_style` 函数可以设置列表控件的样式，默认为 ``LIST_CLASSIC``, 具体有以下几种：


.. literalinclude:: ../../../realgui/widget/gui_list/gui_list.h
   :language: c
   :start-after: /* LIST_STYLE enum start*/
   :end-before: /* LIST_STYLE enum end*/


示例
-----

.. literalinclude:: ../../../example/widget/list/example_gui_list.c
   :language: c
   :start-after: /* gui list widget example start*/
   :end-before: /* gui list widget example end*/


.. raw:: html

   <br>
   <div style="display: flex; justify-content: space-between;">
      <img src="https://foruda.gitee.com/images/1745398396237626690/156a4565_10737458.gif" style="width: 30%;">
      <img src="https://foruda.gitee.com/images/1745398373494570427/8336b58a_10737458.gif" style="width: 30%;">
      <img src="https://foruda.gitee.com/images/1745398410976550394/fb4282a5_10737458.gif" style="width: 30%;">
   </div>
   <br>

API
-----

.. doxygenfile:: gui_list.h

