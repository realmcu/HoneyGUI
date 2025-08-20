.. _Font_Porting_CN:

========
字库移植
========

本章节会解析字库部分代码，并介绍如何使用开发者的字库替换替换 HoneyGUI 的原生字库，或者加入定制化功能。

点阵字库移植
------------

字形加载
~~~~~~~~

文本编码转换
^^^^^^^^^^^^

在文件 ``font_mem.c`` ，函数 :cpp:any:`gui_font_get_dot_info` 中， :cpp:any:`process_content_by_charset` 会解析文本控件的文本内容，
并保存为 unicode (UTF-32) 储存到 ``unicode_buf`` ，Unicode 数量作为返回值输入 ``unicode_len`` 。

.. code-block:: c

    uint32_t *unicode_buf = NULL;
    uint16_t unicode_len = 0;
    unicode_len = process_content_by_charset(text->charset, text->content, text->len, &unicode_buf);
    if (unicode_len == 0)
    {
        gui_log("Warning! After process, unicode len of text: %s is 0!\n", text->base.name);
        text->font_len = 0;
        return;
    }

:cpp:any:`process_content_by_charset` 的具体实现请查阅 ``draw_font.c`` 。

.. note::

   解析过程支持 UTF-8、UTF-16 和 UTF-32。

后续会根据 unicode_buf 中的 Unicode 信息索引字库中的文本数据。

编码转换前后都可以进行小语种的文本编码转换，例如阿拉伯语的字符拼接及其他对 Unicode 进行计算的部分。如果在后面进行转换，需要同步修改 ``unicode_len`` 。

.. note::

   ``unicode_len`` 的单位是字节，而非字符数量。

字库索引
^^^^^^^^

在文件 ``font_mem.c`` ，函数 :cpp:any:`gui_font_get_dot_info` 中，解析得到 Unicode 之后，会使用 Unicode 去文本控件指定的字库中索引字形信息。

由于字库工具具有 ``crop`` 属性，以及两种索引模式，因此在使用 unicode 在字库文件中寻找文本数据以及点阵数据时，使用了不同的解析代码。

字库解析代码的目的是填充 ``chr`` 结构体数组，其结构如下：

.. literalinclude:: ../../../realgui/engine/font_mem.h
   :language: c
   :start-after: /** @brief  mem char struct start */
   :end-before: /** @brief  mem char struct end */

每个成员的含义如下：

- ``Unicode`` 是点阵文本的 Unicode，使用 UTF-32LE 格式表达；
- ``x`` 是点阵文本边框左上角的 X 坐标，排版时确定，用来确定文本绘制坐标；
- ``y`` 是点阵文本边框左上角的 Y 坐标，排版时确定，用来确定文本绘制坐标；
- ``w`` 是点阵数据中该字符的数据位宽，由于存在字节对齐以及压缩的特性，因此该值并不总等于字号；
- ``h`` 是点阵文本的高度，恒等于字号，用来限制基础绘制区域以及多行排版；
- ``char_y`` 是字符的上方空白行数，代表文本点阵图中最上方像素点 Y 坐标与上边框的距离，用来限制绘制区域；
- ``char_w`` 是字符的像素宽度，代表文本边框最左侧（起始点）与文本最右侧像素 X 方向坐标差值，绘制时使用该值限制绘制区域，排版时使用该值代表文本宽度；
- ``char_h`` 是字符的像素高度，代表文本点阵图中最下方像素点 Y 坐标与上边框的距离，用来限制绘制区域； ``char_h`` 减去 ``char_y`` 的数值为点阵的实际像素高度；
- ``dot_addr`` 是该文本对应的点阵数据的起始地址；
- ``emoji_img`` 是 Emoji 图片对应的控件指针，未使用 Emoji 功能时，该值为空；

.. figure:: https://foruda.gitee.com/images/1729762447610163035/4ae24c0c_9325830.png
   :width: 636
   :align: center

   字形示例

在字库索引阶段，会填充所有 ``chr`` 的除 ``x`` ``y`` 坐标以外的全部成员，为下一步排版做准备。

.. note::

   由于不同模式下的数据存储规则有差异，绘制区域也有差异。
   例如 ``char_y`` 与 ``char_h`` 仅在 ``crop=1`` 并且 ``index_method=0`` 时才生效。

由于该阶段会使用 Unicode 去查找点阵文本的宽度信息以及点阵数据指针，因此最好在这一步骤之前完成 Unicode 级别的变形文本的融合过程，例如阿拉伯语的拼接，而泰语的字形融合则属于排版阶段的图形融合。

如果使用自己的定制字库进行移植，可以利用定制字库的信息填充至 ``chr`` 数据结构中，在后续的排版和绘制阶段，使用默认部分。

排版
~~~~

文本控件支持多种不同的排版模式。

具体的排版功能在文件 ``font_mem.c`` 的函数 :cpp:any:`gui_font_mem_layout` 中，每种排版模式具有不同的排版逻辑，但是都依赖于字形信息 ``chr`` 和文本控件提供的边框信息 ``rect`` 。

``rect`` 结构体数组结构如下：

.. literalinclude:: ../../../realgui/engine/draw_font.h
   :language: c
   :start-after: /** @brief  text rect struct start */
   :end-before: /** @brief  text rect struct end */

``rect`` 为控件层传入的控件显示范围，其中 ``x1`` 和 ``x2`` 分别代表左边框和右边框的 X 坐标， ``y1`` 和 ``y2`` 分别代表上边框和下边框的 Y 坐标，其数值是内部控件计算生成，依赖控件创建时的位置和大小。

通过 ``rect`` 的四个坐标计算出 ``rect_w`` 和 ``rect_h`` 。

四组 ``bound`` 值是滚动文本控件 ``scroll_text`` 用来处理显示边界的，文本控件 ``text`` 暂时没有使用。

开发者可以根据需求，添加新的排版模式。

通过 :cpp:any:`gui_text_wordwrap_set` 使能了英文单词换行功能（``wordwrap``）后，多行排版会增加英文单词的换行规则，防止英文单词的截断。

字符绘制
~~~~~~~~~

点阵字符的绘制代码位于 ``font_mem.c`` 中的 :cpp:any:`rtk_draw_unicode` 中。

可以指定文本控件开启矩阵运算功能以适配文本缩放效果，这部分字符的绘制代码位于 ``font_mem_matrix.c`` 中的 :cpp:any:`rtk_draw_unicode_matrix` 中。

可以指定文本控件开启转图片功能，将文本转化成图片，可以实现复杂特效，这部分字符的绘制代码位于 ``font_mem_img.c`` 中的 :cpp:any:`gui_font_bmp2img_one_char` 中。

字符绘制阶段不涉及任何排版信息，只会读取字形信息，并绘制到屏幕缓存中。

每个字的绘制都会使用控件边框、屏幕的边框以及当前字符的边框三重限制绘制区域。

如果开发者想要使用特殊的字库进行绘制，需要修改点阵数据解析代码，并将像素绘制到屏幕缓存中。

API
---

.. doxygenfile:: font_mem.h
.. doxygenfile:: draw_font.h