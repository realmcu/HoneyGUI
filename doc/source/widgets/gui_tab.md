# Tab
<br>

## Overview

The tab widget enables you to create an unlimited number of tabs in both the x and y axes of the screen.Before integrating the tab widget, you must first establish a tabview widget to house the tabs.The direction of the generated tabs is denoted by idx and idy.

For an introduction to idx and idy, please refer to:

```eval_rst
`tabview`_

.. _tabview: gui_tabview.html

```

## Usage

### Create tab widget
[gui_tab_create(parent, x, y, w, h, idx, idy)](#gui_tab_create) is used to create a tab widget, where idx and idy represent the direction of the created tab.

### Set tab style
If you wish to exhibit various switching effects when switching tabs, you can utilize the [gui_tab_set_style(this, style)](#gui_tab_create) setting. By default, the classic style is employed. The styles are illustrated in the subsequent enumeration.

```c
typedef enum gui_tab_style
{
    CLASSIC,
    REDUCTION,
    FADE,
    REDUCTION_FADE,
    STACKING,
} SLIDE_STYLE;
```

## Example

### Example for tab

The style of tab switching can be custom-defined, allowing developers to utilize the  [gui_tab_set_style(this, style)](#gui_tab_create) to set the reduction or classic style. In this example, we've designated the first three tabs to have the CLASSIC style, while the last three have the REDUCTION style.


```c
#include <gui_tabview.h>
#include "gui_card.h"
#include <gui_obj.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_curtain.h>
#include "root_image_hongkong/ui_resource.h"
#include <gui_app.h>
#include "gui_tab.h"
#include "app_hongkong.h"
#include "gui_win.h"
#include "gui_server.h"
#include "gui_components_init.h"
#include <stdio.h>

static void app_hongkong_ui_design(gui_app_t *app)
{
    gui_log("app_hongkong_ui_design\n");

    gui_tabview_t *tv = gui_tabview_create(&(app->screen), "tabview", 0, 0, 0, 0);
    gui_win_t *win = gui_win_create(&(app->screen), "window", 0, 0, 0, 0);

    gui_tab_t *tb_clock = gui_tab_create(tv, "tb_clock",           0, 0, 0, 0, 0, 0);
    gui_tab_t *tb_activity = gui_tab_create(tv, "tb_activity",     0, 0, 0, 0, 1, 0);
    gui_tab_t *tb_heart = gui_tab_create(tv, "tb_heart",           0, 0, 0, 0, 2, 0);
    gui_tab_t *tb_cube = gui_tab_create(tv, "tb_cube",             0, 0, 0, 0, 3, 0);
    gui_tab_t *tb_weather = gui_tab_create(tv, "tb_weather",       0, 0, 0, 0, 5, 0);
    gui_tab_t *tb_music = gui_tab_create(tv, "tb_music",           0, 0, 0, 0, 4, 0);
    gui_tab_t *tb_ani = gui_tab_create(tv, "tb_ani",               0, 0, 0, 0, 6, 0);
    page_tb_clock(tb_clock);
    page_tb_activity(tb_activity);
    page_tb_heart(tb_heart);
    page_tb_cube(tb_cube);
    page_tb_weather(tb_weather);
    page_tb_music(tb_music);

    gui_tab_set_style(tb_clock, CLASSIC);
    gui_tab_set_style(tb_activity, CLASSIC);
    gui_tab_set_style(tb_heart, CLASSIC);
    gui_tab_set_style(tb_cube, REDUCTION);
    gui_tab_set_style(tb_weather, REDUCTION);
    gui_tab_set_style(tb_music, REDUCTION);
}
```
<br>

<div style="text-align: center"><img src="https://foruda.gitee.com/images/1701255049073698099/9628deb9_10641540.gif" width = "300" /></div>

<br>

### Example for tabview

In tabview, [gui_tabview_set_style(this, style)](#gui_tabview_set_style) can be utilized to standardize all tab switching styles. For instance, you can set all tab styles to either CLASSIC or REDUCTION. The example below demonstrates the switch to REDUCTION style, which is defaulted to CLASSIC style.

```c
#include <gui_tabview.h>
#include "gui_card.h"
#include <gui_obj.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_curtain.h>
#include "root_image_hongkong/ui_resource.h"
#include <gui_app.h>
#include "gui_tab.h"
#include "app_hongkong.h"
#include "gui_win.h"
#include "gui_server.h"
#include "gui_components_init.h"
#include <stdio.h>

static void app_hongkong_ui_design(gui_app_t *app)
{
    gui_log("app_hongkong_ui_design\n");

    gui_tabview_t *tv = gui_tabview_create(&(app->screen), "tabview", 0, 0, 0, 0);
    gui_win_t *win = gui_win_create(&(app->screen), "window", 0, 0, 0, 0);
    gui_tabview_set_style(tv, REDUCTION);

    gui_tab_t *tb_clock = gui_tab_create(tv, "tb_clock",           0, 0, 0, 0, 0, 0);
    gui_tab_t *tb_activity = gui_tab_create(tv, "tb_activity",     0, 0, 0, 0, 1, 0);
    gui_tab_t *tb_heart = gui_tab_create(tv, "tb_heart",           0, 0, 0, 0, 2, 0);
    gui_tab_t *tb_cube = gui_tab_create(tv, "tb_cube",             0, 0, 0, 0, 3, 0);
    gui_tab_t *tb_weather = gui_tab_create(tv, "tb_weather",       0, 0, 0, 0, 5, 0);
    gui_tab_t *tb_music = gui_tab_create(tv, "tb_music",           0, 0, 0, 0, 4, 0);
    gui_tab_t *tb_ani = gui_tab_create(tv, "tb_ani",               0, 0, 0, 0, 6, 0);
    page_tb_clock(tb_clock);
    page_tb_activity(tb_activity);
    page_tb_heart(tb_heart);
    page_tb_cube(tb_cube);
    page_tb_weather(tb_weather);
    page_tb_music(tb_music);
}
```

<br>

<div style="text-align: center"><img src="https://foruda.gitee.com/images/1701255113760758795/64efc30d_10641540.gif" width = "300" /></div>
<br>

<span id="gui_tab_create">

## API

</span>

```eval_rst
.. doxygenfile:: gui_tab.h
```
