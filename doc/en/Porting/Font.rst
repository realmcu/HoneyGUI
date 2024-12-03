.. _Font_Porting_EN:

============
Font Porting
============

This chapter will analyze the font library code segment and explain how to replace HoneyGUI's native font library with a custom one provided by the developer, or how to add customized features.

Dot Matrix Font Library Porting
-------------------------------

Glyph Loading
~~~~~~~~~~~~~

Text Encoding Conversion
^^^^^^^^^^^^^^^^^^^^^^^^

In the file ``font_mem.c``, within the function ``gui_font_get_dot_info``, ``process_content_by_charset`` parses the text content of the text control and saves it as Unicode (UTF-32) in ``unicode_buf``. The number of Unicode characters is returned in ``unicode_len``.

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

For the specific implementation of ``process_content_by_charset``, please refer to ``draw_font.c``.

.. note::

    The parsing process supports UTF-8, UTF-16, and UTF-32.

Subsequently, Unicode information in ``unicode_buf`` will be used to index text data from the font library.

Text encoding conversion for minor languages, such as Arabic character concatenation and other calculations involving Unicode, can be performed either before or after the encoding conversion. If the conversion is done later, ``unicode_len`` must be updated accordingly.

.. note::

    The unit of ``unicode_len`` is bytes, not the number of characters.

Font Library Indexing
^^^^^^^^^^^^^^^^^^^^^

In the file ``font_mem.c``, within the function ``gui_font_get_dot_info``, the Unicode value is parsed and then used to index glyph information from the font library designated by the text widget.

Since the font library tool has the ``crop`` attribute and two indexing modes, different parsing code is used to find text data and dot matrix data in the font library file using the Unicode value.

The purpose of the font library parsing code is to populate the ``chr`` structure array, which is structured as follows:

.. literalinclude:: ../../../realgui/engine/font_mem.h
   :language: c
   :start-after: /** @brief  mem char struct start */
   :end-before: /** @brief  mem char struct end */

Each member has the following meanings:

- ``Unicode``: The Unicode of the dot matrix text, expressed in UTF-32LE format.
- ``x``: The X-coordinate of the upper-left corner of the dot matrix text boundary, determined during layout, used to set the drawing coordinates of the text.
- ``y``: The Y-coordinate of the upper-left corner of the dot matrix text boundary, determined during layout, used to set the drawing coordinates of the text.
- ``w``: The data width of the character in the dot matrix data. Due to byte alignment and compression characteristics, this value is not always equal to the font size.
- ``h``: The height of the dot matrix text, which is always equal to the font size, used to define the basic drawing area and for multi-line layout.
- ``char_y``: The number of blank rows above the character, representing the Y-coordinate distance between the topmost pixel of the text dot matrix and the upper boundary, used to constrain the drawing area.
- ``char_w``: The pixel width of the character, representing the difference in the X-coordinate between the leftmost boundary (starting point) and the rightmost pixel of the text. This value is used to constrain the drawing area during drawing and represents the text width during layout.
- ``char_h``: The pixel height of the character, representing the Y-coordinate distance between the bottommost pixel of the text dot matrix and the upper boundary. The value of ``char_h`` minus ``char_y`` gives the actual pixel height of the dot matrix.
- ``dot_addr``: The starting address of the dot matrix data corresponding to the text.
- ``emoji_img``: The pointer to the widget corresponding to the Emoji image. This value is NULL if the Emoji feature is not used.

.. figure:: https://foruda.gitee.com/images/1729762447610163035/4ae24c0c_9325830.png
   :width: 636
   :align: center

   Glyph Example

During the font library indexing phase, all members of ``chr`` except for the ``x`` and ``y`` coordinates will be populated to prepare for the next step of layout.

.. note::

    Due to differences in data storage rules under different modes, the drawing areas also vary. For example, ``char_y`` and ``char_h`` are only effective when ``crop=1`` and ``index_method=0``.

Since this stage involves using the Unicode to look up width information for the dot matrix text and the dot matrix data pointer, it's best to complete the Unicode-level text transformations before this step. For example, Arabic script ligatures should be handled in this stage, whereas Thai glyph fusion should be handled during the layout stage.

If you are porting using your custom font library, you can populate the ``chr`` data structures using information from your custom font library. The default parts can be used for the subsequent layout and drawing stages.

Layout
~~~~~~

The text widget supports various layout modes. For more details, see `Text Widget Mode <#Text_Widget_Mode_EN>`_.

The specific layout functionality is located in the file ``font_mem.c`` in the function ``gui_font_mem_layout``. Each layout mode has a different layout logic; however, all depend on the glyph information ``chr`` and the boundary information ``rect`` provided by the text widget.

The ``rect`` struct array is structured as follows:

.. literalinclude:: ../../../realgui/engine/draw_font.h
   :language: c
   :start-after: /** @brief  text rect struct start */
   :end-before: /** @brief  text rect struct end */

The ``rect`` is the display range of the widget passed from the control layer. In this structure, ``x1`` and ``x2`` represent the X-coordinates of the left and right borders, respectively, while ``y1`` and ``y2`` represent the Y-coordinates of the top and bottom borders, respectively. These values are calculated internally by the widget based on its position and size at the time of creation. From the four coordinates of ``rect``, you can calculate ``rect_w`` (width) and ``rect_h`` (height).

There are also four ``bound`` values used by the scrolling text widget (``scroll_text``) to handle display boundaries. These ``bound`` values are currently not used by the regular text widget (``text``).

Developers can add new layout modes as per their requirements.

By enabling the English word wrapping feature (``wordwrap``) via the function ``gui_text_wordwrap_set``, the multi-line layout will adhere to English word wrapping rules to prevent words from being split across lines.

Character Rendering
~~~~~~~~~~~~~~~~~~~~

The code for rendering bitmap characters is located in the ``rtk_draw_unicode`` function in ``font_mem.c``. You can enable matrix operations for the text widget to support text scaling effects; the rendering code for this feature is in ``rtk_draw_unicode_matrix`` in ``font_mem_matrix.c``. Additionally, you can enable a feature to convert text into an image for achieving complex effects; this rendering code is found in ``gui_font_bmp2img_one_char`` in ``font_mem_img.c``.

The character rendering stage does not involve any layout information; it only reads the glyph information and renders it to the screen buffer.

Each character's rendering is constrained by three boundaries: the widget's boundary, the screen's boundary, and the current character's boundary.

If developers wish to use a special font library for rendering, they need to modify the bitmap data parsing code and draw the pixels into the screen buffer.