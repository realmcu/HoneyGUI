.. _视图 (View):

=====================
视图 (View)
=====================

视图控件 (View) 是一种切换更为便利的容器控件，通过事件响应（点击以及四个方向的滑动等）可以实时创建任一视图控件并可选择多种切换效果。切换过程中内存中会存在两个view，切换完成后会自动清理未显示的view，可以有效降低内存消耗。

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1753407359429529327/73931f5b_10737458.jpeg" width= "1000" /></div>
   <p style="margin: 5px 0 0 0; text-align: center;">View流程图</p>
   <br>

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1739867379534372641/4058df7c_10737458.gif" width= "400" /></div>
   <p style="margin: 5px 0 0 0; text-align: center;">View切换示意图</p>
   <br>

用法
-----
注册视图控件描述子
~~~~~~~~~~~~~~~~~~
使用 :cpp:any:`gui_view_descriptor_register` 函数传入视图控件描述子地址将其注册在描述子列表中供其它视图控件读取使用，作为创建视图控件的参数，其中 ``gui_view_descriptor`` 结构体定义如下：

.. literalinclude:: ../../../realgui/widget/gui_view/gui_view.h
   :language: c
   :start-after: /* gui_view_descriptor start*/
   :end-before: /* gui_view_descriptor end*/


获得视图控件描述子
~~~~~~~~~~~~~~~~~~
使用 :cpp:any:`gui_view_descriptor_get` 函数通过传入字符串获取对应 ``name`` 的视图控件描述子。


创建视图控件
~~~~~~~~~~~~~~
使用 :cpp:any:`gui_view_create` 函数可以根据描述子创建一个视图控件。


设置视图切换事件
~~~~~~~~~~~~~~~~~~
使用 :cpp:any:`gui_view_switch_on_event` 设置视图切换事件，对某一个事件可以重复设置，会使用最新的描述子。具体的事件类型包括 ``GUI_EVENT_TOUCH_CLICKED`` 、 ``GUI_EVENT_KB_SHORT_CLICKED`` 、 ``GUI_EVENT_TOUCH_MOVE_LEFT``、 ``GUI_EVENT_TOUCH_MOVE_RIGHT`` 等。具体的切换风格请参考下列枚举：


.. literalinclude:: ../../../realgui/widget/gui_view/gui_view.h
   :language: c
   :start-after: /* VIEW_SWITCH_STYLE enum start*/
   :end-before: /* VIEW_SWITCH_STYLE enum end*/


立即切换视图
~~~~~~~~~~~~~~~~~~
使用 :cpp:any:`gui_view_switch_direct` 立即切换视图，可以配合视图控件中子控件的事件或动画使用，注意切换风格仅限于动画风格，不可设置滑动风格。


设置动画步进长度
~~~~~~~~~~~~~~~~~~~~
使用 :cpp:any:`gui_view_set_animate_step` 设置切换控件时的动画步进长度。


设置透明度
~~~~~~~~~~~~~
使用 :cpp:any:`gui_view_set_opacity` 设置控件透明度，可实现view重叠显示效果。


获取当前显示的视图控件指针
~~~~~~~~~~~~~~~~~~~~~~~~~~
使用 :cpp:any:`gui_view_get_current` 函数可以获取当前显示的视图控件指针，可以搭配 :cpp:any:`gui_view_switch_direct` 使用，将当前view切换。


获取切换过程中新创建视图控件指针
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
使用 :cpp:any:`gui_view_get_next` 函数可以获取切换过程中新创建视图控件指针。


示例
-----
视图控件
~~~~~~~~~~
以下是四个单独的C文件，每个C文件中包含view控件的描述子以及design函数。

.. literalinclude:: ../../../example/widget/view/app_ui_view_blue.c
   :language: c
   :start-after: /* view blue start*/
   :end-before: /* view blue end*/


.. literalinclude:: ../../../example/widget/view/app_ui_view_white.c
   :language: c
   :start-after: /* view white start*/
   :end-before: /* view white end*/


.. literalinclude:: ../../../example/widget/view/app_ui_view_yellow.c
   :language: c
   :start-after: /* view yellow start*/
   :end-before: /* view yellow end*/


.. literalinclude:: ../../../example/widget/view/app_ui_view_lime.c
   :language: c
   :start-after: /* view lime start*/
   :end-before: /* view lime end*/


.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1753407801497338482/48e631cd_10737458.gif" width= "400" /></div>
   <br>

API
-----

.. doxygenfile:: gui_view.h

