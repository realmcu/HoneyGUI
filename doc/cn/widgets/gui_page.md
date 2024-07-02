# Page
<br>

## Overview

The page widget enables developers to establish a column of additional controls along the y-axis. These controls are permitted to extend beyond the screen's display boundary. Users can access these controls by swiping up or down.

## Usage

### Create page widget

Developers can utilize the [gui_page_create(parent, filename, x, y, w, h)](#gui_page_create) function to establish a page widget. The page widget is a container oriented vertically, allowing the addition of other widgets to it. The height of the page widget is determined by the number of widgets added to it.

## Example

```c
#include "root_image_hongkong/ui_resource.h"
#include <gui_img.h>
#include "gui_win.h"
#include "gui_text.h"
#include <draw_font.h>
#include "gui_button.h"
#include "gui_page.h"

extern void callback_prism(void *obj, gui_event_t e);

void page_tb_activity(void *parent)
{
    gui_img_create_from_mem(parent, "page1", ACTIVITY_BIN, 0, 0, 0, 0);
    gui_win_t *win_function = gui_win_create(parent, "win_function", 0, 0, 368, 448);
    gui_obj_add_event_cb(win_function, (gui_event_cb_t)callback_prism, GUI_EVENT_TOUCH_LONG, NULL);

    gui_page_t *pg1 = gui_page_create(parent, "page", 0, 0, 0, 0);

    gui_img_t *img1 = gui_img_create_from_mem(pg1, "img1", BUTTON1_BIN, 0, 150, 0, 0);
    gui_img_t *img2 = gui_img_create_from_mem(pg1, "img2", BUTTON2_BIN, 0, 230, 0, 0);
    gui_img_t *img3 = gui_img_create_from_mem(pg1, "img3", BUTTON3_BIN, 0, 300, 0, 0);
    gui_img_t *img4 = gui_img_create_from_mem(pg1, "img4", PLAYER_MUSIC_REWIND_ICON_BIN, 0, 380, 0, 0);
    gui_img_t *img5 = gui_img_create_from_mem(pg1, "img5", PLAYER_MUSIC_WIND_ICON_BIN, 0, 460, 0, 0);
}
```
<br>
<div style="text-align: center"><img src="https://foruda.gitee.com/images/1702980322226942161/b6425e49_10641540.gif " width = "300" /></div>
<br>

<span id = "gui_page_create">

## API

</span>

```eval_rst

.. doxygenfile:: gui_page.h

```
