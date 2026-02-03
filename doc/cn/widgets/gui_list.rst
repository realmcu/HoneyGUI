.. _列表 (List):

=====================
列表 (List)
=====================

列表控件 (List) 作为容器可以在水平或垂直方向上添加任意数量的表格（动态创建，节省内存和 CPU 开销），每个表格都可以添加其他的控件，如文本控件，图片控件等等，用户可以通过上下左右滑动来访问添加到列表控件上的其他控件。

.. figure:: https://foruda.gitee.com/images/1745465574757113257/5e8ee605_10737458.gif
   :align: center
   :width: 400

   Vertical Classic List

用法
-----
创建列表控件
~~~~~~~~~~~~~~~~~~
使用 :cpp:any:`gui_list_create` 函数来创建一个列表控件（可选滚动条），列表控件的下一级只能是表格控件，在 ``note_design`` 中根据表格控件的 ``index`` 进行对应的子控件设计，实现动态创建表格控件。列表控件的总长度由添加的表格数量、表格长度以及表格间隔决定，当添加的表格越多时，其长度也越大。


设置列表控件的样式
~~~~~~~~~~~~~~~~~~
使用 :cpp:any:`gui_list_set_style` 函数可以设置列表控件的样式，默认为 ``LIST_CLASSIC``, 具体有以下几种：


.. literalinclude:: ../../../realgui/widget/gui_list/gui_list.h
   :language: c
   :start-after: /* LIST_STYLE enum start*/
   :end-before: /* LIST_STYLE enum end*/


设置列表控件的减速系数
~~~~~~~~~~~~~~~~~~~~~~~
使用 :cpp:any:`gui_list_set_factor` 函数设置列表控件的减速系数。


设置列表控件的偏移位置
~~~~~~~~~~~~~~~~~~~~~~~
使用 :cpp:any:`gui_list_set_offset` 函数设置列表控件的偏移位置。


设置列表控件滚动条颜色
~~~~~~~~~~~~~~~~~~~~~~~
使用 :cpp:any:`gui_list_set_bar_color` 函数设置列表控件的滚动条颜色。


设置列表控件表格数量
~~~~~~~~~~~~~~~~~~~~~
使用 :cpp:any:`gui_list_set_note_num` 函数设置列表控件的表格数量。


设置卡片样式堆叠位置
~~~~~~~~~~~~~~~~~~~~~
使用 :cpp:any:`gui_list_set_card_stack_location` 函数设置卡片样式堆叠位置。


设置列表控件超出区域范围
~~~~~~~~~~~~~~~~~~~~~~~~~
使用 :cpp:any:`gui_list_set_out_scope` 函数设置列表控件超出区域范围。


设置列表控件自动对齐
~~~~~~~~~~~~~~~~~~~~~
使用 :cpp:any:`gui_list_set_auto_align` 函数设置列表控件自动对齐。


设置列表控件惯性滚动
~~~~~~~~~~~~~~~~~~~~~
使用 :cpp:any:`gui_list_set_inertia` 函数设置列表控件惯性滚动。


设置列表循环滚动
~~~~~~~~~~~~~~~~~~~~~
使用 :cpp:any:`gui_list_enable_loop` 函数设置列表控件循环滚动，当列表总长度大于列表宽度或高度时生效。

设置列表区域显示
~~~~~~~~~~~~~~~~~~~~~
使用 :cpp:any:`gui_list_enable_area_display` 函数设置列表控件区域显示，当列表的起始点不在屏幕边缘时，启用区域显示可以将表格限制在更为合理的区域而不是更个屏幕进行动态创建。


示例
-----

.. literalinclude:: ../../../example/widget/list/example_gui_list.c
   :language: c
   :start-after: /* gui list widget example start*/
   :end-before: /* gui list widget example end*/


.. list-table::
   :widths: 33 33 33

   * - .. figure:: https://foruda.gitee.com/images/1745398396237626690/156a4565_10737458.gif
          :width: 100%
          :align: center

          Vertical Circle Style

     - .. figure:: https://foruda.gitee.com/images/1750920685777460993/280f951c_10737458.gif
          :width: 100%
          :align: center

          Vertical Card Style

     - .. figure:: https://foruda.gitee.com/images/1745398410976550394/fb4282a5_10737458.gif
          :width: 100%
          :align: center

          Horizontal Zoom Style

   * - .. figure:: https://foruda.gitee.com/images/1745819904731135543/290c9d96_10737458.gif
          :width: 100%
          :align: center

          Vertical Fan Style

     - .. figure:: https://foruda.gitee.com/images/1750916286131886532/8af671b7_10737458.gif
          :width: 100%
          :align: center

          Vertical Helix Style

     - .. figure:: https://foruda.gitee.com/images/1745890329636631265/cef196cd_10737458.gif
          :width: 100%
          :align: center
          
          Vertical Curl Style

API
-----

.. doxygenfile:: gui_list.h