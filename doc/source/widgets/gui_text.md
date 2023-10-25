# text widget

## Overview

The text widget is the basic widget used to display text, which can be used to output text in different fonts, different colors, and different sizes to the screen.

In order to draw text, .

The font file can be standard .ttf file or customized .bin file.

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

### Initialize the font file

In order to draw text, font files containing glyph information need to be loaded into the system.

The font file can be standard .ttf file or customized .bin file. It must to be initialized before use the text widget.

+ To initialize the old version customized bin font file, you need to use  `gui_set_font_mem_resourse(24, DOT_BIN, TABLE_BIN);`. The font size and two files must be matched.

+ To initialize the new version customized bin font file, you need to use `gui_font_mem_init(FONT_BIN);`.

+ To initialize the standrad TTF file to draw text, you need to use `gui_font_stb_init(FONT_TTF)`.

All customized bin font files are available from RTK technicians.

`DOT_BIN`,`TABLE_BIN`,`FONT_BIN`,`FONT_TTF` are all address of the files stored in flash.
If you want to know more about file storage, please read userdata.md.

### Creat text widget

To creat a text widget, you can use

``` C
gui_text_t *time = gui_text_create(curtain_center,  "time",  0, 0, 100, 30);
```

The coordinates on the screen and text box size have been identified after creat.

These attributes also can be modified whenever you want.

Note that, text box size should be larger than the string to be shown, out-of-range text will be hidden.

### Set text attributes

#### Set text

To add some texts or characters to a text widget and set text attributes with:

``` C
gui_text_set(time, "8:30 PM", "rtk_font_mem", 0xffffffff, 7, 24);
```

#### Font type

``` C
gui_text_type_set(time, DOT_BIN);
gui_text_type_set(time, FONT_BIN);
gui_text_type_set(time, FONT_TTF);
```

#### Text mode

Text widget support seven typesetting modes, to set text typesetting mode with:

``` C
gui_text_mode_set(time, CENTER);
```

All nine typesetting modes are as follows.

|Type|Description|Widget|
|--|--|--|
|`LEFT`|Single-line text. Left align. |Text widget. Default.|
|`CENTER`|Single-line text.Center align.|Text widget.|
|`RIGHT`|Single-line text. Right align.|Text widget.|
|`MUTI_LEFT`|Multiline text. Left align.|Text widget.|
|`MUTI_CENTER`|Multiline text. Center align.|Text widget.|
|`MUTI_RIGHT`|Multiline text. Right align.|Text widget.|
|`SCROLL_X`|Horizontal scroll single-line text. Right to left.|Only on scroll text widget.|
|`SCROLL_Y`|Vertical scroll multiline text. Bottom-up.|Only on scroll text widget.|

```C
typedef enum
{
    LEFT = 0,
    CENTER,
    RIGHT,
    MUTI_LEFT,
    MUTI_CENTER,
    MUTI_RIGHT,
    SCROLL_X,
    SCROLL_Y,
} TEXT_MODE;
```

To use scroll text, you can read scrolltext.md.

## Example

### Simple text widget

An example image of the text widget is shown below.

![text widget example1](https://foruda.gitee.com/images/1694429674838659868/2912dcda_9325830.png "text_widget_example1.png")

<details> <summary>Example code</summary>

```C
#include "string.h"
#include "gui_obj.h"
#include "guidef.h"
#include "gui_text.h"
#include "draw_font.h"
#include "gui_app.h"
#include "rtk_gui_resource.h"

static void app_launcher_ui_design(gui_app_t *app)
{
    gui_font_mem_init(HARMONYOS_SIZE24_BITS1_FONT_BIN);

    gui_text_t *time = gui_text_create(&app->screen,  "time", 20, 20, 100, 30);
    gui_text_set(time, "15:30", "rtk_font_mem", 0xff0000ff, 5, 24);
    gui_text_type_set(time, HARMONYOS_SIZE24_BITS1_FONT_BIN);
    gui_text_mode_set(time, LEFT);
}
```

</details>

### Multiple text widget

![text widget example2](https://foruda.gitee.com/images/1694429576419596614/3cc7bc43_9325830.png "text_widget_example2.png")

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
    gui_font_mem_init(SIMKAI_SIZE24_BITS4_FONT_BIN);
    gui_set_font_mem_resourse(32, GBK_32_32_DOT_BIN, GBK_UNICODE_TABLE_BIN);
    gui_set_font_mem_resourse(28, QUICKSAND_SEMIBOLD_28_BIN, CP500_TABLE_BIN);

    void *screen = &app->screen;

    gui_text_t *text1 = gui_text_create(screen,  "text1",  10, 10, 100, 50);
    gui_text_set(text1, chinese, "rtk_font_mem", 0xffffffff, strlen(chinese), 24);
    gui_text_type_set(text1, HARMONYOS_SIZE24_BITS1_FONT_BIN);
    gui_text_mode_set(text1, LEFT);

    gui_text_t *text2 = gui_text_create(screen,  "text2",  0, 50, 300, 50);
    gui_text_set(text2, "english", "rtk_font_mem", 0xff0000ff, 7, 16);
    gui_text_type_set(text2, HARMONYOS_SIZE16_BITS4_FONT_BIN);
    gui_text_mode_set(text2, LEFT);

    char *string = "TEXT_WIDGET";
    gui_text_t *text3 = gui_text_create(screen,  "text3",  0, 90, 300, 50);
    gui_text_set(text3, string, "rtk_font_mem", 0x0000ffff, strlen(string), 32);
    gui_text_type_set(text3, GBK_32_32_DOT_BIN);
    gui_text_mode_set(text3, CENTER);

    gui_text_t *text4 = gui_text_create(screen,  "text4",  0, 150, 100, 200);
    gui_text_set(text4, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", "rtk_font_mem", 0xffff0000, 24, 24);
    gui_text_type_set(text4, SIMKAI_SIZE24_BITS4_FONT_BIN);
    gui_text_mode_set(text4, MUTI_CENTER);
}

```

</details>

## Struct

text widget struct

```C
typedef struct gui_text
{
    gui_obj_t base;
    const struct rtgui_font_engine *engine;
    uint32_t color;
    TEXT_MODE mode;
    uint16_t len;
    uint16_t font_len;
    uint16_t text_offset;
    uint8_t font_height;
    uint8_t *utf_8;
    void *data;
    char *text_type;
    gui_animate_t *animate;
    void *path;
} gui_text_t;
```

## APIs

|Parameter|Description|Type|
|--|--|--|
|`font_size`|the font size of this font file|`unsigned char`|
|`font_bitmap_addr`|the bitmap file address of this font type|`void *`|
|`font_table_addr`|the table file address of this font type|`void *`|

```C
/**
 * @brief Initialize the character binary file and store the font and corresponding information in the font list
 * @note This API works with older font files
 * @param font_size the font size of this font file
 * @param font_bitmap_addr the bitmap file address of this font type
 * @param font_table_addr the table file address of this font type
 */
void gui_set_font_mem_resourse(unsigned char font_size, void *font_bitmap_addr,
                               void *font_table_addr);
```

```C
/**
 * @brief Initialize the character binary file and store the font and corresponding information in the font list
 * @note The area of the text box should be larger than that of the string to be shown, otherwise, part of
 * @param font_bin_addr the binary file address of this font type
 */
void gui_font_mem_init(uint8_t *font_bin_addr);
```

```C
/**
 * @brief Initialize a standard ttf file and send the font and corresponding information to the parser
 *
 * @param font_ttf_addr the address of ttf file
 */
void gui_font_stb_init(void *font_ttf_addr);
```

### Create and modify text widgets

```C
/**
 * @brief create a text box widget.
 * @note The area of the text box should be larger than that of the string to be shown, otherwise, part of the text will be hidden.
 * @param parent the father widget which the text nested in.
 * @param filename the widget's name.
 * @param x the X-axis coordinate of the text box.
 * @param y the Y-axis coordinate of the text box.
 * @param w the width of the text box.
 * @param h the hight of the text box.
 * @return return the widget object pointer
 *
 */
gui_text_t *gui_text_create(void *parent, const char *filename, int16_t x, int16_t y, int16_t w, int16_t h);
```

```C
/**
* @brief set the string in a text box widget.
* @note The font size must match the font file!
* @param this the text box widget pointer.
* @param text the text string.
* @param text_type font file type, "rtk_font_mem" match FONT_BIN or "rtk_font_stb" match FONT_TTF.
* @param color the text's color.
* @param length the text string's length.
* @param font_size the text string's font size.
* @return void
*/
void gui_text_set(gui_text_t *this, const char *text, char *text_type, uint32_t color, uint16_t length, uint8_t font_size);
```

```C
/**
 * @brief move the text widget
 *
 * @param this the text box widget pointer.
 * @param x the X-axis coordinate of the text box.
 * @param y the Y-axis coordinate of the text box.
 */
void gui_text_move(gui_text_t *this, int16_t x, int16_t y);
```

```C
/**
 * @brief set text mode of this text widget
 * @note if text line count was more than one, it will display on the left even if it was set lft or right
 * @param this the text widget pointer.
 * @param mode there was three mode: LEFT, CENTER and RIGHT.
 */
void gui_text_mode_set(gui_text_t *this, TEXT_MODE mode);
```

```C
/**
 * @brief set font size or width and height
 * @note if use freetype, width and height is effective, else height will be applied as font size
 * @param this the text widget pointer.
 * @param height font height or font size.
 * @param width font width(only be effective when freetype was used).
 */
void gui_text_size_set(gui_text_t *this, uint8_t height, uint8_t width);
```

```C
/**
 * @brief set font type
 * @note The type must match the font size!
 * @param this the text widget pointer
 * @param type the addr of .ttf or .bin
 */
void gui_text_type_set(gui_text_t *this, void *type);
```

```C
/**
 * @brief set animate
 *
 * @param o text widget
 * @param dur durtion. time length of the animate
 * @param repeatCount  0:one shoot -1:endless
 * @param callback happens at every frame
 * @param p callback's parameter
 */
void gui_text_set_animate(void *o, uint32_t dur, int repeatCount, void *callback, void *p);
```
```eval_rst


.. doxygenfile:: gui_text.h

```
