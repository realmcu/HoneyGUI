# Simple image
<br>

## Overview
The simple img widget is a simplified version of the img control, lacking transformation operations such as rotation, translation, and scaling.

## Usage

### Create widget

You can use [gui_simple_img_create(void *parent,  const char *name, void *addr, int16_t x, int16_t y, int16_t w, int16_t h, IMG_SOURCE_MODE_TYPE src_mode_type)](#api) to create an simple  image widget. If src_mode_type is IMG_SRC_MEMADDR, the image is created from memory. If src_mode_type is IMG_SRC_FILESYS, the image is created from the file system using the the image path as `addr`.

### Set location

If you need to update the location of image widget, use [gui_simple_img_set_location(gui_img_t *img, uint16_t x, uint16_t y)](#api) to relocate.
`x, y` is the new coordinate.

### Set attribute

You can use [void gui_simple_img_set_attribute(gui_simple_img_t *img, const char *name, void *addr,int16_t x,int16_t y)](#api) to set the attribute of an image widget, replace it with a new image and set a new coordinate.

### Get height/width

If you want to get the height/width of image widget, you can do so with [gui_simple_img_get_height(gui_simple_img_t *this)](#api) or [gui_simple_img_get_width(gui_simple_img_t *this)](#api).

## Example

<details> <summary>code</summary>

```c
#include "root_image_hongkong/ui_resource.h"
#include "gui_simple_img.h"
#include "gui_text.h"
#include "draw_font.h"

char *tb1_text = "gui_simple_img_create";

void page_tb1(void *parent)
{
    static char array1[50];
    static char array2[50];

    gui_font_mem_init(ARIALBD_SIZE16_BITS4_FONT_BIN);

    gui_simple_img_t *img_test = gui_simple_img_create(parent, "simg", SET_ON_BIN, 0, 0, 0, 0, 0);

    gui_text_t *text1 = gui_text_create(parent, "text1", 10, 100, 300, 30);
    gui_text_set(text1, tb1_text, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, strlen(tb1_text), 16);
    gui_text_mode_set(text1, LEFT);

    gui_text_t *text2 = gui_text_create(parent, "text2", 10, 130, 330, 30);
    gui_text_set(text2, tb1_text, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, strlen(tb1_text), 16);
    gui_text_mode_set(text2, LEFT);
    sprintf(array1, "gui_img_get_height %d", gui_simple_img_get_height(img_test));
    text2->content = array1;
    text2->len = strlen(array1);

    gui_text_t *text3 = gui_text_create(parent, "text3", 10, 160, 330, 30);
    gui_text_set(text3, tb1_text, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, strlen(tb1_text), 16);
    gui_text_mode_set(text3, LEFT);
    sprintf(array2, "gui_img_get_width %d", gui_simple_img_get_width(img_test));
    text3->content = array2;
    text3->len = strlen(array2);
}

void page_tb2(void *parent)
{
    gui_simple_img_t *img_test = gui_simple_img_create(parent, "simg", SET_ON_BIN, 0, 0, 0, 0, 0);
    gui_simple_img_set_location(img_test, 50, 50);

    gui_text_t *text2 = gui_text_create(parent, "text2", 10, 150, 330, 24);
    gui_text_set(text2, "gui_simple_img_set_location", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 27, 16);
    gui_text_mode_set(text2, LEFT);
}

void page_tb3(void *parent)
{
    gui_simple_img_t *img_test = gui_simple_img_create(parent, "simg", SET_ON_BIN, 0, 0, 0, 0, 0);
    gui_simple_img_set_attribute(img_test, "test", SET_OFF_BIN, 20, 20);

    gui_text_t *text3 = gui_text_create(parent, "text3", 10, 120, 330, 24);
    gui_text_set(text3, "gui_simple_img_set_attribute", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 28, 16);
    gui_text_mode_set(text3, LEFT);
}

```

</details>

<br>

<center><img width= "300" src="https://foruda.gitee.com/images/1715074028338417840/9bb1c0dc_13671125.gif" /></center>
<br>

<span id="api">

## API

</span>

```eval_rst

.. doxygenfile:: gui_simple_img.h

```
