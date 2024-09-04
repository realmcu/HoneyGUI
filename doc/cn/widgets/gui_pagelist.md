# 页面列表控件

<br>

页面列表控件具有与页面控件类似的功能，并且预计使用更少的内存。页面列表控件在 y 轴上表现得像一个滚动条。当它在 y 轴上移动时，其子节点也会随之移动。可以设置两个切换控件，分别指示第一个和最后一个位置，以及两个边界线，分别指示顶部和底部。当第一个切换控件到达顶部时，它将从顶部移动到底部。最后一个切换控件到达底部时，它将从底部移动到顶部。这一特性使得子控件看起来比实际创建的数量更多。

## 用法

### 增加页面控件

开发者可以通过 [gui_pagelist_increase_real_list_max(gui_pagelist_t *this, int16_t real_list_max)](#gui_pagelist_increase_real_list_max)来增加真实列表的数量。

### 减少页面控件
开发者可以使用[gui_pagelist_decrease_real_list_max(gui_pagelist_t *this, int16_t real_list_max)](#gui_pagelist_decrease_real_list_max)来减少真实列表的数量。

### 添加回调函数

此API [gui_pagelist_add_list_update_cb(gui_pagelist_t *this, gui_pagelist_update_cb_t list_first_cb, gui_pagelist_update_cb_t list_last_cb)](#gui_pagelist_add_list_update_cb) 用于在页列表上下滑动时添加更新回调，通常用于更新文本。

### 设定属性

开发者可以调用 [gui_pagelist_set_att(gui_pagelist_t *this, int16_t real_list_max, int16_t show_list_max, int16_t list_gap_y, gui_switch_t *list_first, gui_switch_t *list_last)](#gui_pagelist_set_att)来设定页面列表控件的属性。

### 增加图片

通过此API [gui_pagelist_add_scroll_bar(gui_pagelist_t *this, void *bar_pic, IMG_SOURCE_MODE_TYPE src_mode)](#gui_pagelist_add_scroll_bar), 可以为pagelist控件添加滚动条图像，文件来源可以是内存地址。

### 创建控件

开发者可以利用 [gui_pagelist_create(parent, filename, x, y, w, h)](#gui_pagelist_create) 函数来创建一个页面列表控件。它的高度将根据子控件的总高度进行设置。

## 示例

<details> <summary>代码</summary>

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

</details><br>

<span id = "api">

## API

</span>

```eval_rst

.. doxygenfile:: gui_pagelist.h

```
