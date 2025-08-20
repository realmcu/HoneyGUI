==========
图像 (Img)
==========

图像控件是用于显示图像的基本控件，支持移动、缩放、旋转等功能。

用法
----

创建控件
~~~~~~~~

开发者可以使用 :cpp:any:`gui_img_create_from_mem` 从内存中创建一个图像控件，或者使用 :cpp:any:`gui_img_create_from_fs` 从文件系统中创建一个图像控件。同样，也可以使用 :cpp:any:`gui_img_create_from_ftl` 从闪存中创建一个图像控件。
如果图像控件的宽度或高度设置为 0，那么控件的大小将根据图像源的大小自动设置。

设置焦点
~~~~~~~~

图片控件的默认焦点位置位于图片的左上角 (0, 0)，开发者可以使用 :cpp:any:`gui_img_set_focus` 设置新的图片控件的焦点，设置完成之后，图片控件的变换、旋转、平移操作都会基于新的焦点进行。

设定属性
~~~~~~~~

开发者可以通过 :cpp:any:`gui_img_set_attribute` 来设置图像控件的属性，替换为新图像并设置新坐标。

获取高度/宽度
~~~~~~~~~~~~~

如果想要获取图像控件的高度/宽度，开发者可以使用 :cpp:any:`gui_img_get_height` 或 :cpp:any:`gui_img_get_width`。

刷新大小
~~~~~~~~

开发者可以调用 :cpp:any:`gui_img_refresh_size` 来刷新图像控件大小。

混合模式
~~~~~~~~

开发者可以使用 :cpp:any:`gui_img_set_mode` 来设定图像控件的混合模式。

移动
~~~~

通过 :cpp:any:`gui_img_translate` 来移动图像控件。
开发者可以将图像控件移动到新坐标，而不改变控件属性中的原始坐标。

旋转
~~~~

开发者可以通过 :cpp:any:`gui_img_rotation` 来围绕圆心旋转图像控件。

缩放
~~~~

开发者可以使用 :cpp:any:`gui_img_scale` 调整图像控件的大小以满足需求。

不透明度
~~~~~~~~

图像控件的不透明度值是可调整的，开发者可以调用 :cpp:any:`gui_img_set_opacity` 来调整。

质量
~~~~

开发者可以调用 :cpp:any:`gui_img_set_quality` 来设定图像控件的显示质量。

截屏
~~~~

开发者可以使用 :cpp:any:`gui_img_tree_convert_to_img` 来保存全屏截图。保存的图像会是 RGB 格式。

示例
----

.. literalinclude:: ../../../example/widget/image/example_gui_img.c
    :language: c
    :start-after: /* gui image widget example start*/
    :end-before: /* gui image widget example end*/

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/img.gif" width= "400" /></div>
   <br>

API
---

.. doxygenfile:: gui_img.h
