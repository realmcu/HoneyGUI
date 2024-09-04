# Pagelist

The pagelist widget has the similar function as page widget, and it is expected to use less ram size. The pagelist widget bahaves like a scroll at y-axis. When it moves at y-axis, the child nodes will also move with pagelist widget. you can set two switch widget which indicate the first and the ast, and two border line which indicate top and bottom. The first one will move from top to bottom when it reaches top. The last one will move from bottom to top when it reaches bottom. This characteristic makes the child widgets look like more than that they are really created.

## Usage

### Increase

You can increase the number of real lists using [gui_pagelist_increase_real_list_max(gui_pagelist_t *this, int16_t real_list_max)](#gui_pagelist_increase_real_list_max)[gui_pagelist_t](#gui_pagelist_t).

### Decrease
You can decrease the number of real lists using [gui_pagelist_decrease_real_list_max(gui_pagelist_t *this, int16_t real_list_max)](#gui_pagelist_decrease_real_list_max).

### Add callback

The API [gui_pagelist_add_list_update_cb(gui_pagelist_t *this, gui_pagelist_update_cb_t list_first_cb, gui_pagelist_update_cb_t list_last_cb)](#gui_pagelist_add_list_update_cb) is used to add an update callback when the pagelist slides up and down, typically for updating text.

### Set attribute
You can set attributes for a pagelist widget using [gui_pagelist_set_att(gui_pagelist_t *this, int16_t real_list_max, int16_t show_list_max, int16_t list_gap_y, gui_switch_t *list_first, gui_switch_t *list_last)](#gui_pagelist_set_att).

### Add picture

By using this API [gui_pagelist_add_scroll_bar(gui_pagelist_t *this, void *bar_pic, IMG_SOURCE_MODE_TYPE src_mode)](#gui_pagelist_add_scroll_bar), you can add a scroll bar image for a pagelist widget, with the file source being a memory address.

### Create widget

You can utilize the [gui_pagelist_create(parent, filename, x, y, w, h)](#gui_pagelist_create) function to establish a pagelist widget. It's height will be set according to the total height of child widgets.

## Example

```c
#include "root_image/ui_resource.h"
#include "gui_img.h"
#include "gui_text.h"
#include "gui_switch.h"
#include "gui_pagelistview.h"
#include "gui_pagelist.h"

int8_t show_num = 4;
int16_t real_num = 20;
int16_t list_gap_y = 20;
int16_t index_first = 0;
int16_t index_last = 0;

void design_pagelist_test(void *parent)
{
    pagelist_test = gui_pagelist_create(pagelistview_test, "pagelist", 0, 0, LCD_W, LCD_H);
    for (int8_t i = 0; i < show_num; i++)
    {
        switch_pagelist_test[i] = gui_switch_create(pagelist_test, 83, 111 + i * (list_gap_y + 64), 288, 64,
                                                    ICON_TEXT_BASE_DARK_BIN, ICON_TEXT_BASE_DARK_BIN);
        switch_pagelist_test[i]->off_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
        switch_pagelist_test[i]->on_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
        scroll_text_pagelist_test[i] = gui_scroll_text_create(switch_pagelist_test[i],
                                                              "scroll_text_record_files", 0, 0, 128, FONT_H_32);
        gui_scroll_text_set(scroll_text_pagelist_test[i], files_name_test[i],
                            GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX), strlen(files_name_test[i]),
                            FONT_H_32);
        gui_scroll_text_scroll_set(scroll_text_pagelist_test[i], SCROLL_X, 0, 0, 5000, 0);
        gui_obj_add_event_cb(switch_pagelist_test[i], (gui_event_cb_t)swtich_pagelist_touch_cb, GUI_EVENT_1,
                             NULL);
        gui_obj_add_event_cb(switch_pagelist_test[i], (gui_event_cb_t)swtich_pagelist_touch_cb, GUI_EVENT_2,
                             NULL);
    }
    index_first = 0;
    index_last = MAX_SHOW_FILE_LIST_NUM - 1;
    gui_pagelist_set_att(pagelist_test, real_num, show_num, list_gap_y, switch_pagelist_test[0],
                         switch_pagelist_test[MAX_SHOW_FILE_LIST_NUM - 1]);
    gui_pagelist_add_list_update_cb(pagelist_test,
                                    (gui_pagelist_update_cb_t)pagelist_test_update_list_first_cb,
                                    (gui_pagelist_update_cb_t)pagelist_test_update_list_last_cb);
}
```
<br>

<br>

<span id = "api">

## API

</span>

```eval_rst

.. doxygenfile:: gui_pagelist.h

```
