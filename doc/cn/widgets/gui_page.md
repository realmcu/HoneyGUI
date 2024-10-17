# 页面 (Page)

页面控件是一个容器控件，可以让开发人员能够沿着y轴建立一个包含其他控件的页面。创建页面控件后，可以在该控件上添加其他的控件，如按钮控件，图片控件等等。页面控件可以超出屏幕显示边界，用户可以通过向上或向下滑动来访问添加到页面控件上的其他控件。

## 用法

### 创建页面控件

开发者可以使用 [gui_page_create(void *parent, const char *name, int16_t x, int16_t y, int16_t w, int16_t h)](#gui_page_create) 函数来创建一个页面控件。页面控件是一个垂直排列的容器，允许向其中添加其他控件。页面控件的高度由添加的控件数量决定，当添加的其他控件越多时，其高度也就越高。

## 示例


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
<div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/page.gif" width = "300" /></div>
<br>

<span id = "gui_page_create">

## API

</span>

```eval_rst

.. doxygenfile:: gui_page.h

```
