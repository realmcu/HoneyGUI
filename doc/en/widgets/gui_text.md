# Text widget

The text widget is the basic widget used to display text, which can be used to output text in different fonts, different colors, and different sizes to the screen. In order to draw text, the font file can be standard .ttf file or customized .bin file.

## Features

Text widgets can support the following features.

+ **UTF-8 support**
+ **Multi language support**
+ **Text typesetting support**
+ **Word wrap and texts scrolling**
+ **Anti-aliasing**
+ **Multi fonts support**
+ **Multi font sizes support**
+ **Thirty-two bit true color support**
+ **Custom animation effects support**
+ **Standrads TTF file support[^1]**
+ **Self-developed font files support**

[^1]: Only part of the chip support this feature

## Usage

Using functions to load font files and display text

### Initialize the font file

In order to draw text, font files containing glyph information need to be loaded into the system.

The font file can be standard .ttf file or customized .bin file. The font file can be initialized ahead of time to avoid having to set the font type for each text widget.

+ To initialize the new version customized bin font file, you need to use [gui_font_mem_init(font_bin_addr)](#api) .

+ To initialize the standrad TTF file to draw text, you need to use [gui_font_stb_init(font_ttf_addr)](#api) .

All customized bin font files are available from RTK technicians.

`FONT_BIN`,`FONT_TTF` are all address of the files stored in flash.

If you want to know more about file storage, please read userdata.md.

### Create text widget

To create a text widget, you can use [gui_text_create(parent, filename, x, y, w, h)](#api)

The coordinates on the screen and text box size have been identified after create.

These attributes also can be modified whenever you want.

Note that text box size should be larger than the string to be shown, out-of-range text will be hidden.

### Set text attributes

#### Set text

To add some texts or characters to a text widget and set text attributes with: [gui_text_set(this, text, text_type, color, length, font_size)](#api)

Note that text length must be the same as the set character length, text frontsize should must be the same as the type size

#### Font type

Text widget support the type setting. You can use this function to set type.Type is bin/ttf file address.
[gui_text_type_set(this, type)](#api).

#### Text content

This interface can be used to set the content that needs to be displayed by the text widget.
[gui_text_content_set(this, text, length)](#api).

#### Text encoding

The text widget supports both UTF-8 encoding and UTF-16 encoding input formats, and this interface can be used to change the decoding method.
[gui_text_encoding_set(this, charset)](#api).

#### Convert to img

By using this interface, the text in the text widget will be converted into an image, stored in memory, and rendered using the image. It also supports image transformations such as scaling and rotation. This only applies to bitmap fonts.
[gui_text_convert_to_img(this, font_img_type)](#api).
Because the content and font size information of the text widget is needed, it should be called after set text.If the content, font size, position and other attributes of the text have been modified, you need to reuse this interface for conversion.

#### Text mode

Text widget support seven typesetting modes, to set text typesetting mode with: [gui_text_mode_set(this, mode)](#api).

All nine typesetting modes are as follows.

|Type|Description|Widget|
|--|--|--|
|`LEFT`|Single-line text. Left align. |Text widget. Default.|
|`CENTER`|Single-line text.Center align.|Text widget.|
|`RIGHT`|Single-line text. Right align.|Text widget.|
|`MULTI_LEFT`|Multiline text. Left align.|Text widget.|
|`MULTI_CENTER`|Multiline text. Center align.|Text widget.|
|`MULTI_RIGHT`|Multiline text. Right align.|Text widget.|
|`SCROLL_X`|Horizontal scroll single-line text. Right to left.|Only on scroll text widget.|
|`SCROLL_Y`|Vertical scroll multiline text. Bottom-up.|Only on scroll text widget.|
|`VERTICAL_LEFT`|Vertical multiline text. To left.|Text widget.|
|`VERTICAL_RIGHT`|Vertical multiline text. To right.|Text widget.|

```C
typedef enum
{
    LEFT = 0,
    CENTER,
    RIGHT,
    MULTI_LEFT,
    MULTI_CENTER,
    MULTI_RIGHT,
    SCROLL_X,
    SCROLL_Y,
    VERTICAL_LEFT,
    VERTICAL_RIGHT,
} TEXT_MODE;
```

### Text move

You can use this function [gui_text_move(this, x, y)](#api) to move text to a specified location, but x and y cannot be larger than w and h of the text

### Set animate

Using this function [gui_text_set_animate(o, dur, repeat_count, callback, p)](#api) to set the animation and implement the animation effect in the corresponding callback function

To use scroll text, you can read scrolltext.md.

## Example

### Example Multiple text widget

An example of the multiple text widget is shown below.

<details> <summary>Example code</summary>

```C
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
    gui_text_type_set(text1, HARMONYOS_SIZE24_BITS1_FONT_BIN);
    gui_text_mode_set(text1, LEFT);

    gui_text_t *text2 = gui_text_create(screen,  "text2",  0, 50, 300, 50);
    gui_text_set(text2, "english", GUI_FONT_SRC_BMP, APP_COLOR_RED, 7, 16);
    gui_text_type_set(text2, HARMONYOS_SIZE16_BITS4_FONT_BIN);
    gui_text_mode_set(text2, LEFT);

    char *string = "TEXT_WIDGET";
    gui_text_t *text3 = gui_text_create(screen,  "text3",  0, 90, 300, 50);
    gui_text_set(text3, string, GUI_FONT_SRC_BMP, APP_COLOR_BLUE, strlen(string), 32);
    gui_text_type_set(text3, HARMONYOS_SIZE32_BITS1_FONT_BIN);
    gui_text_mode_set(text3, CENTER);

    gui_text_t *text4 = gui_text_create(screen,  "text4",  0, 150, 100, 200);
    gui_text_set(text4, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", GUI_FONT_SRC_BMP, gui_rgb(0, 0xff, 0xff), 24, 24);
    gui_text_type_set(text4, SIMKAI_SIZE24_BITS4_FONT_BIN);
    gui_text_mode_set(text4, MULTI_CENTER);
}

```

</details></br>

<center><img  width="300" src= "https://foruda.gitee.com/images/1694429576419596614/3cc7bc43_9325830.png "/></center></br>

### Example Animate text widget

An example of the animate text widget is shown below.

<details> <summary>Example code</summary>

```C
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
        obj->utf_8 = "123456789";
    }
    else if (obj->animate->current_frame > 50 && obj->animate->current_frame < 100)
    {
        gui_text_move(obj, 200, 150);
        obj->utf_8 = "987654321";

    }
    else
    {
        gui_text_move(obj, 125, 50);
        obj->utf_8 = "abcdefghi";
    }
}

void page_tb_activity(void *parent)
{
    gui_font_mem_init(SIMKAI_SIZE24_BITS4_FONT_BIN);

    gui_text_t *text = gui_text_create(parent,  "text",  0, 0, 100, 200);
    gui_text_set(text, "ABCDEFGHI", GUI_FONT_SRC_BMP, APP_COLOR_RED, 9, 24);
    gui_text_type_set(text, SIMKAI_SIZE24_BITS4_FONT_BIN);
    gui_text_mode_set(text, MULTI_CENTER);
    gui_text_set_animate(text, 5000, 15, change_text_cb, text);
}

```

</details></br>

<center><img width="300" src= "(https://docs.realmcu.com/HoneyGUI/image/widgets/text.gif"/></center>

<br>

## API

```eval_rst

.. doxygenfile:: gui_text.h

```
