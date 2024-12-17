.. _Text:

=====
Text
=====

The text widget is the basic widget used to display text, which can be used to output text in different fonts, different colors, and different sizes to the screen. In order to draw text, the font file can be a standard .ttf file or a customized .bin file.

Features
---------

Text widgets can support the following features.

+ **UTF-8/UTF16/UTF-32 support**
+ **Multi language support**
+ **Text typesetting support**
+ **Word wrap and texts scrolling**
+ **Anti-aliasing**
+ **Multi fonts support**
+ **Multi font sizes support**
+ **Thirty-two bit true color support**
+ **Emoji support**
+ **Custom animation effects support**
+ **Standards TTF file support**\ :sup:`①`
+ **Self-developed font files support**

\ :sup:`①`: Only part of the chip support this feature.

Usage
-----

Using functions to load font files and display text.

Initialize the Font File
~~~~~~~~~~~~~~~~~~~~~~~~~

In order to draw text, font files containing glyph information need to be loaded into the system.

The font file can be a standard .ttf file or a customized .bin file. The font file can be initialized ahead of time to avoid having to set the font type for each text widget.

+ To initialize the new version customized bin font file, use ``gui_font_mem_init(uint8_t *font_bin_addr)``.

+ To initialize the standard TTF file to draw text, use ``gui_font_stb_init(void *font_ttf_addr)``.

All customized bin font files are available from RTK technicians.

``FONT_BIN``, ``FONT_TTF`` are all addresses of the files stored in flash.

Create Text Widget
~~~~~~~~~~~~~~~~~~~
To create a text widget, you can use :cpp:any:`gui_text_create`, The coordinates on the screen and text box size have been identified after create. These attributes also can be modified whenever you want.

.. note::
    The size of the text box should be larger than the string to be shown; out-of-range text will be hidden.

Set Text Attributes
~~~~~~~~~~~~~~~~~~~~

Set Text
^^^^^^^^^
To add some texts or characters to a text widget and set text attributes with :cpp:any:`gui_text_set`.

.. note::
    The text length must be the same as the set character length, and the font size of the text must be the same as the size of the loaded font file.

Font Type
^^^^^^^^^^
The text widget support type setting. This function can be used to set the type. The type is a bin/ttf file address
:cpp:any:`gui_text_type_set`.

Text Content
^^^^^^^^^^^^^
This interface can be used to set the content that needs to be displayed by the text widget :cpp:any:`gui_text_content_set`.

Text Encoding
^^^^^^^^^^^^^^
The text control supports input formats in UTF-8, UTF-16, and UTF-32 encodings simultaneously. Developers can use :cpp:any:`gui_text_encoding_set` to change the encoding format.

Convert to Img
^^^^^^^^^^^^^^^
By using this function :cpp:any:`gui_text_convert_to_img`, the text in the text widget will be converted into an image, stored in memory, and rendered using the image. It also supports image transformations such as scaling and rotation. This only applies to bitmap fonts.

.. note::
    Because the content and font size information of the text widget is needed, it should be called after set text. If the content, font size, position, and other attributes of the text have been modified, you need to reuse this interface for conversion.

Text Input
^^^^^^^^^^^
Text widget supports the input setting. You can use this function to set input :cpp:any:`gui_text_input_set`.

Text Click
^^^^^^^^^^^
Text widget supports click. You can use this function to add the click event for text :cpp:any:`gui_text_click`.

Text Mode
^^^^^^^^^^
Text widget supports seven typesetting modes. To set text typesetting mode, use: :cpp:any:`gui_text_mode_set`.

All typesetting modes are as follows.

.. table:: Text_Mode_Table
   :widths: 20 20 20 20 20
   :align: center
   :name: Text_Mode_Table
+--------------------+---------------+------------------+------------------+--------------------------+
| Type               | Line Type     | X Direction      | Y Direction      | Widget                   |
+====================+===============+==================+==================+==========================+
| `LEFT`             | Single-line   | Left             | Top              | Text widget (Default)    |
+--------------------+---------------+------------------+------------------+--------------------------+
| `CENTER`           | Single-line   | Center           | Top              | Text widget              |
+--------------------+---------------+------------------+------------------+--------------------------+
| `RIGHT`            | Single-line   | Right            | Top              | Text widget              |
+--------------------+---------------+------------------+------------------+--------------------------+
| `MULTI_LEFT`       | Multi-line    | Left             | Top              | Text widget              |
+--------------------+---------------+------------------+------------------+--------------------------+
| `MULTI_CENTER`     | Multi-line    | Center           | Top              | Text widget              |
+--------------------+---------------+------------------+------------------+--------------------------+
| `MULTI_RIGHT`      | Multi-line    | Right            | Top              | Text widget              |
+--------------------+---------------+------------------+------------------+--------------------------+
| `MID_LEFT`         | Multi-line    | Left             | Mid              | Text widget              |
+--------------------+---------------+------------------+------------------+--------------------------+
| `MID_CENTER`       | Multi-line    | Center           | Mid              | Text widget              |
+--------------------+---------------+------------------+------------------+--------------------------+
| `MID_RIGHT`        | Multi-line    | Right            | Mid              | Text widget              |
+--------------------+---------------+------------------+------------------+--------------------------+
| `SCROLL_X`         | Single-line   | Right to Left    | Top              | Scroll text widget       |
+--------------------+---------------+------------------+------------------+--------------------------+
| `SCROLL_Y`         | Multi-line    | Left             | Bottom to Top    | Scroll text widget       |
+--------------------+---------------+------------------+------------------+--------------------------+
| `SCROLL_Y_REVERSE` | Multi-line    | Right            | Top to Bottom    | Scroll text widget       |
+--------------------+---------------+------------------+------------------+--------------------------+
| `VERTICAL_LEFT`    | Multi-line    | Left             | Top to Bottom    | Text widget              |
+--------------------+---------------+------------------+------------------+--------------------------+
| `VERTICAL_RIGHT`   | Multi-line    | Right            | Bottom to Top    | Text widget              |
+--------------------+---------------+------------------+------------------+--------------------------+

.. literalinclude:: ../../../realgui/widget/gui_text.h
   :language: c
   :start-after: /* TEXT_MODE start */
   :end-before: /* TEXT_MODE end */

Text Move
^^^^^^^^^^

It is possible to use this function :cpp:any:`gui_text_move` to move text to a specified location, but x and y cannot be larger than w and h of the text.

Set Animate
^^^^^^^^^^^^

Using this function :cpp:any:`gui_text_set_animate` to set the animation and implement the animation effect in the corresponding callback function.

Example
--------

Multiple Text Widget
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

    #include "string.h"
    #include "gui_obj.h"
    #include "guidef.h"
    #include "gui_text.h"
    #include "draw_font.h"
    #include "gui_app.h"
    #include "rtk_gui_resource.h"

    static char chinese[6] =
    {
        0xE4, 0xB8, 0xAD,
        0xE6, 0x96, 0x87
    };
    static void app_launcher_ui_design(gui_app_t *app)
    {
        gui_font_mem_init(HARMONYOS_SIZE24_BITS1_FONT_BIN);
        gui_font_mem_init(HARMONYOS_SIZE16_BITS4_FONT_BIN);
        gui_font_mem_init(HARMONYOS_SIZE32_BITS1_FONT_BIN);
        gui_font_mem_init(SIMKAI_SIZE24_BITS4_FONT_BIN);

        void *screen = &app->screen;

        gui_text_t *text1 = gui_text_create(screen,  "text1",  10, 10, 100, 50);
        gui_text_set(text1, chinese, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, strlen(chinese), 24);
        gui_text_type_set(text1, HARMONYOS_SIZE24_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text1, LEFT);

        gui_text_t *text2 = gui_text_create(screen,  "text2",  0, 50, 300, 50);
        gui_text_set(text2, "english", GUI_FONT_SRC_BMP, APP_COLOR_RED, 7, 16);
        gui_text_type_set(text2, HARMONYOS_SIZE16_BITS4_FONT_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text2, LEFT);

        char *string = "TEXT_WIDGET";
        gui_text_t *text3 = gui_text_create(screen,  "text3",  0, 90, 300, 50);
        gui_text_set(text3, string, GUI_FONT_SRC_BMP, APP_COLOR_BLUE, strlen(string), 32);
        gui_text_type_set(text3, HARMONYOS_SIZE32_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text3, CENTER);

        gui_text_t *text4 = gui_text_create(screen,  "text4",  0, 150, 100, 200);
        gui_text_set(text4, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", GUI_FONT_SRC_BMP, gui_rgb(0, 0xff, 0xff), 24, 24);
        gui_text_type_set(text4, SIMKAI_SIZE24_BITS4_FONT_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text4, MULTI_CENTER);
    }


.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1694429576419596614/3cc7bc43_9325830.png" width= "400" /></div>
   <br>


Animate Text Widget
~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

    #include "root_image_hongkong/ui_resource.h"
    #include "string.h"
    #include "gui_obj.h"
    #include "guidef.h"
    #include "gui_text.h"
    #include "draw_font.h"

    void change_text_cb(gui_text_t *obj)
    {
        if (obj->animate->current_frame > 0 && obj->animate->current_frame < 50)
        {
            gui_text_move(obj, 50, 150);
            gui_text_content_set(obj, "123456789", 9);
        }
        else if (obj->animate->current_frame > 50 && obj->animate->current_frame < 100)
        {
            gui_text_move(obj, 200, 150);
            gui_text_content_set(obj, "987654321", 9);

        }
        else
        {
            gui_text_move(obj, 125, 50);
            gui_text_content_set(obj, "abcdefghi", 9);
        }
    }

    void page_tb_activity(void *parent)
    {
        gui_font_mem_init(SIMKAI_SIZE24_BITS4_FONT_BIN);

        gui_text_t *text = gui_text_create(parent,  "text",  0, 0, 100, 200);
        gui_text_set(text, "ABCDEFGHI", GUI_FONT_SRC_BMP, APP_COLOR_RED, 9, 24);
        gui_text_type_set(text, SIMKAI_SIZE24_BITS4_FONT_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, MULTI_CENTER);
        gui_text_set_animate(text, 5000, 15, change_text_cb, text);
    }


.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/text.gif" width= "400" /></div>
   <br>


API
-----

.. doxygenfile:: gui_text.h

