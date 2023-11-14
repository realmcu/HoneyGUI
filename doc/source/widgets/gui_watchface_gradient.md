# Watchface_gradient
<br>

## Overview

The watchface_gradient widget allows you to create a pointer watchface with a scanning effect for each pointer.

## Features

- Drawing using the NanoVG library

## Usage

### Create watchface_gradient widget

use [gui_watchface_gradient_create](#gui_watchface_gradient_create) to create widget

```c
canvas = gui_watchface_gradient_create(parent, "watchface_gradient", (368 - 368) / 2,(448 - 448) / 2, 368, 448);
```

## Example

<details> <summary>code</summary>

```c
#include "root_image_hongkong/ui_resource.h"
#include <gui_img.h>
#include "gui_win.h"
#include "gui_watch_gradient_spot.h"
#include "gui_text.h"
#include "gui_watchface_gradient.h"
#include "gui_cardview.h"
#include "gui_card.h"
#include "gui_tab.h"
#include "gui_app.h"
gui_win_t *win_watch;
gui_img_t *img;
gui_watch_gradient_spot_t *watch;
gui_text_t *rate;
static gui_watchface_gradient_t *canvas;
gui_tabview_t *tablist_tab;

void page_ct_clock(void *parent)
{
    win_watch = gui_win_create(parent, "win", 0, 0, 368, 448);
    gui_obj_add_event_cb(win_watch, (gui_event_cb_t)callback_touch_long, GUI_EVENT_TOUCH_LONG, NULL);
    img = gui_img_create_from_mem(parent, "page0", CLOCKN_BIN, 0, 0, 0, 0);
    watch = gui_watch_gradient_spot_create(win_watch, "watchface", 0, 0, 0, 0);
    gui_watch_gradient_spot_set_center(watch, 368 / 2, 448 / 2);
    canvas = gui_watchface_gradient_create(parent, "watchface_gradient", (368 - 368) / 2,
                                           (448 - 448) / 2, 368, 448);
    GET_BASE(watch)->not_show = true;
    GET_BASE(canvas)->not_show = true;
}
```

</details>
<br><br>

<div align=center><img src="https://foruda.gitee.com/images/1699931787525761928/f8a2202f_10641540.png"></div>

<span id = "gui_watchface_gradient_create">

## API

</span>

```eval_rst

.. doxygenfile:: gui_watchface_gradient.h
.. doxygenfunction:: gui_watchface_gradient_create
```
