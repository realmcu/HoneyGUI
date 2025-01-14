蜂窝菜单 (Menu cellular)
========================
蜂窝菜单组件以蜂窝形状显示一组图标。

用法
-----

创建蜂窝菜单组件
~~~~~~~~~~~~~~~
要创建蜂窝菜单组件，使用 :cpp:any:`gui_menu_cellular_create` 函数。用户需要提供代表图标集的图像文件数组。

配置偏移
~~~~~~~~
使用 :cpp:any:`gui_menu_cellular_offset` 函数可以调整水平和垂直偏移以获得更好的显示效果。

示例
----

.. literalinclude:: ../../../realgui/example/screen_454_454/gui_menu/apps_in_menu.c
   :language: c
   :start-after: /*Define APP_MENU's entry func */
   :end-before: /*Define APP_MENU's entry func end*/


.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/menu_cellular.gif" width= "400" /></div>
   <br>

API
---

.. doxygenfile:: gui_menu_cellular.h