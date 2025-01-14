返回 (Return)
=============
可以从屏幕左侧拖出的泡泡形状曲线用于执行返回操作。

用法
-----

创建返回控件
~~~~~~~~~~~~
创建函数是 :cpp:any:`gui_return_create` 。用户需要提供曲线动画帧图像文件数组，以及返回操作函数。

示例
----

.. literalinclude:: ../../../realgui/example/screen_454_454/gui_menu/apps_in_menu.c
   :language: c
   :start-after: /*Define gui_app_return_array*/
   :end-before: /*Define gui_app_return_array end*/

.. literalinclude:: ../../../realgui/example/screen_454_454/gui_menu/apps_in_menu.c
   :language: c
   :start-after: /*gui_return_create*/
   :end-before: /*gui_return_create end*/


.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/return.gif" width= "400" /></div>
   <br>
.. note::
   详细信息请参阅 ``realgui\example\screen_454_454\gui_menu\apps_in_menu.c`` 。

API
---

.. doxygenfile:: gui_return.h
