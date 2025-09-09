蜂窝菜单 (Menu cellular)
========================
蜂窝菜单控件以蜂窝形状显示一组图标。

用法
-----

创建蜂窝菜单控件
~~~~~~~~~~~~~~~~~
使用 :cpp:any:`gui_menu_cellular_create` 函数创建蜂窝菜单控件，用户需要提供代表图标集的图像文件数组。

添加点击回调事件
~~~~~~~~~~~~~~~~
使用 :cpp:any:`gui_menu_cellular_on_click` 函数添加点击图标的回调事件。

设置偏移
~~~~~~~~
使用 :cpp:any:`gui_menu_cellular_offset` 函数可以调整水平和垂直偏移以获得更好的显示效果。

开发示例
--------
蜂窝菜单控件的开发示例如下，首先使用画布控件绘制一组纯色图标，再将这些图标传递给蜂窝菜单控件进行显示：

.. literalinclude:: ../../../example/widget/menu_cellular/example_gui_menu_cellular.c
   :language: c
   :start-after: /* gui menu cellular widget example start*/
   :end-before: /* gui menu cellular widget example end*/

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1751456456790100801/2b635428_10737458.gif" width= "400" /></div>
   <br>

API
-----

.. doxygenfile:: gui_menu_cellular.h