.. _Text:

=====
Text
=====

The text widget is the basic widget used to display text, which can be used to output text in different fonts, different colors, and different sizes to the screen. In order to draw text, the font file can be a standard .ttf file or a customized .bin file.

Features
---------

Text widgets can support the following features.

+ UTF-8/UTF16/UTF-32 support
+ Multi language support
+ Text typesetting support
+ Word wrap and texts scrolling
+ Anti-aliasing
+ Multi fonts support
+ Multi font sizes support
+ Thirty-two bit true color support
+ Emoji support
+ Custom animation effects support
+ Supports bitmap fonts and vector fonts
+ Self-developed font files support

.. note::
    Multilingual support covers standard Latin character languages (such as English, French), major Asian languages (such as Chinese, Japanese, Korean), and languages requiring complex character rendering (such as Arabic, Persian, Thai, etc., including transformed, combined, or modified character texts).

.. note::
    Only certain chips support vector fonts.

Usage
-----

Using functions to load font files and display text.

Font Files
~~~~~~~~~~~~~~
To render text, font files containing font information need to be loaded into the system.

Font files can be generated using the Realtek font converter, and there are two types: bitmap fonts and vector fonts. Font files can be pre-initialized, allowing the font to be automatically retrieved by the control's font size for rendering, or unique fonts can be set individually for text controls.

To initialize custom bin font files, you need to choose to call either :cpp:any:`gui_font_mem_init_mem` or :cpp:any:`gui_font_mem_init_ftl` based on the system's file reading method.

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
    The text length must be less than or equal to the set character length, and the text font size must match the size of the loaded font file.

Font Type
^^^^^^^^^^
The text widget support type setting. This function can be used to set the type. The type is a bin/ttf file address
:cpp:any:`gui_text_type_set`.

Text Content
^^^^^^^^^^^^^
This interface can be used to set the content that needs to be displayed by the text widget :cpp:any:`gui_text_content_set`.

Text Encoding
^^^^^^^^^^^^^^
The text widget supports input formats in UTF-8, UTF-16, and UTF-32 encodings simultaneously. Developers can use :cpp:any:`gui_text_encoding_set` to change the encoding format.

Convert to Img
^^^^^^^^^^^^^^^
By using this function :cpp:any:`gui_text_convert_to_img`, the text in the text widget will be converted into an image, stored in memory, and rendered using the image. It also supports image transformations such as scaling and rotation. This only applies to bitmap fonts.

.. note::
    Because the content and font size information of the text widget is needed, it should be called after set text. If the content, font size, position, and other attributes of the text have been modified, you need to reuse this interface for conversion.

Text Input
^^^^^^^^^^^
Text widget supports the input setting. You can use this function to set input :cpp:any:`gui_text_input_set`.

Text Modes
^^^^^^^^^^^
The text control currently supports 21 types of layout modes, including the following categories:

 + Single-line layout modes (3 types): Suitable for scenarios displaying only one line of text, such as single-line centered, left-aligned, right-aligned, etc.
 + Multi-line layout modes (6 types): Used for displaying multiple lines of text, supporting various arrangements like multi-line centered, multi-line left-aligned, multi-line right-aligned, etc.
 + Scrolling layout modes (4 types): Ideal for displaying content that exceeds the display area by scrolling, such as horizontal auto-scrolling, vertical scrolling, etc.
 + Vertical layout modes (6 types): Supports vertical text display from top to bottom, and can also be used as a general layout method for rotated text, including top alignment, middle alignment, and bottom alignment.
 + RTL (right-to-left) layout modes (6 types): Designed for languages written from right to left, such as Arabic and Hebrew, supporting corresponding layout modes.

Each layout mode can be selected based on the actual application scenario to meet diverse text display needs. Use :cpp:any:`gui_text_mode_set` to set the text layout mode.

All type setting modes are as follows.

.. table:: Text Mode
   :widths: 20 20 20 20 20
   :align: center
   :name: Text_Mode_Table

   +----------------------+---------------+------------------+-------------------------------+--------------------------+
   | Type                 | Line Type     | X Direction      | Y Direction                   | Widget                   |
   +======================+===============+==================+===============================+==========================+
   | `LEFT`               | Single-line   | Left             | Top                           | Text widget (Default)    |
   +----------------------+---------------+------------------+-------------------------------+--------------------------+
   | `CENTER`             | Single-line   | Center           | Top                           | Text widget              |
   +----------------------+---------------+------------------+-------------------------------+--------------------------+
   | `RIGHT`              | Single-line   | Right            | Top                           | Text widget              |
   +----------------------+---------------+------------------+-------------------------------+--------------------------+
   | `MULTI_LEFT`         | Multi-line    | Left             | Top                           | Text widget              |
   +----------------------+---------------+------------------+-------------------------------+--------------------------+
   | `MULTI_CENTER`       | Multi-line    | Center           | Top                           | Text widget              |
   +----------------------+---------------+------------------+-------------------------------+--------------------------+
   | `MULTI_RIGHT`        | Multi-line    | Right            | Top                           | Text widget              |
   +----------------------+---------------+------------------+-------------------------------+--------------------------+
   | `MID_LEFT`           | Multi-line    | Left             | Mid                           | Text widget              |
   +----------------------+---------------+------------------+-------------------------------+--------------------------+
   | `MID_CENTER`         | Multi-line    | Center           | Mid                           | Text widget              |
   +----------------------+---------------+------------------+-------------------------------+--------------------------+
   | `MID_RIGHT`          | Multi-line    | Right            | Mid                           | Text widget              |
   +----------------------+---------------+------------------+-------------------------------+--------------------------+
   | `SCROLL_X`           | Single-line   | Right to Left    | Top                           | Scroll text widget       |
   +----------------------+---------------+------------------+-------------------------------+--------------------------+
   | `SCROLL_Y`           | Multi-line    | Left             | Bottom to Top                 | Scroll text widget       |
   +----------------------+---------------+------------------+-------------------------------+--------------------------+
   | `SCROLL_X_REVERSE`   | Multi-line    | Left to Right    | Top                           | Scroll text widget       |
   +----------------------+---------------+------------------+-------------------------------+--------------------------+
   | `SCROLL_Y_REVERSE`   | Multi-line    | Right            | Top to Bottom                 | Scroll text widget       |
   +----------------------+---------------+------------------+-------------------------------+--------------------------+
   | `VERTICAL_LEFT_TOP`  | Multi-line    | Left             | Top to Bottom, top aligned    | Text widget              |
   +----------------------+---------------+------------------+-------------------------------+--------------------------+
   | `VERTICAL_LEFT_MID`  | Multi-line    | Left             | Top to Bottom, middle aligned | Text widget              |
   +----------------------+---------------+------------------+-------------------------------+--------------------------+
   | `VERTICAL_LEFT_BOT`  | Multi-line    | Left             | Top to Bottom, bottom aligned | Text widget              |
   +----------------------+---------------+------------------+-------------------------------+--------------------------+
   | `VERTICAL_RIGHT_TOP` | Multi-line    | Right            | Bottom to Top, top aligned    | Text widget              |
   +----------------------+---------------+------------------+-------------------------------+--------------------------+
   | `VERTICAL_RIGHT_MID` | Multi-line    | Right            | Bottom to Top, middle aligned | Text widget              |
   +----------------------+---------------+------------------+-------------------------------+--------------------------+
   | `VERTICAL_RIGHT_BOT` | Multi-line    | Right            | Bottom to Top, bottom aligned | Text widget              |
   +----------------------+---------------+------------------+-------------------------------+--------------------------+
   | `RTL_RIGHT`          | Multi-line    | Right            | Top                           | Text widget              |
   +----------------------+---------------+------------------+-------------------------------+--------------------------+
   | `RTL_CENTER`         | Multi-line    | Center           | Top                           | Text widget              |
   +----------------------+---------------+------------------+-------------------------------+--------------------------+
   | `RTL_LEFT`           | Multi-line    | Left             | Top                           | Text widget              |
   +----------------------+---------------+------------------+-------------------------------+--------------------------+
   | `RTL_MULTI_RIGHT`    | Multi-line    | Right            | Top                           | Text widget              |
   +----------------------+---------------+------------------+-------------------------------+--------------------------+
   | `RTL_MULTI_CENTER`   | Multi-line    | Center           | Top                           | Text widget              |
   +----------------------+---------------+------------------+-------------------------------+--------------------------+
   | `RTL_MULTI_LEFT`     | Multi-line    | Left             | Top                           | Text widget              |
   +----------------------+---------------+------------------+-------------------------------+--------------------------+

.. literalinclude:: ../../../realgui/widget/gui_text/gui_text.h
   :language: c
   :start-after: /* TEXT_MODE start */
   :end-before: /* TEXT_MODE end */

Text Move
^^^^^^^^^^

It is possible to use this function :cpp:any:`gui_text_move` to move text to a specified location.

Example
--------

Simple Text Widget
~~~~~~~~~~~~~~~~~~~~~

.. literalinclude:: ../../../example/widget/text/example_gui_text.c
   :language: c
   :start-after: /* gui text widget example start*/
   :end-before: /* gui text widget example end*/


API
-----

.. doxygenfile:: gui_text.h

