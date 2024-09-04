# 选项卡
<br>

选项卡控件`(tab)`是一种创建在x轴或者y轴上的控件，理论上来说可以创建无数个选项卡控件。选项卡控件的生成方向由idx和idy来表示，idx表示x轴方向，idy表示y轴方向，在建立选项卡控件之前必须要建立一个选项卡视图控件`(tabview)`来容纳这些选项卡。

关于idx和idy的介绍，请参阅：[选项卡容器](./gui_tabview.md)


## 用法

### 创建选项卡控件

使用 [gui_tab_create(parent, x, y, w, h, idx, idy)](#gui_tab_create) 函数创建选项卡控件，其中idx和idy表示创建选项卡控件的方向。

### 设置选项卡切换风格

如果想要在切换不同的选项卡控件时，展现不同的切换效果，可以使用 [gui_tab_set_style(this, style)](#gui_tab_create) 函数来设置。在默认情况下使用经典风格，切换效果有以下几种：

```eval_rst

.. literalinclude:: ../../../gui_engine/widget/gui_tabview.h
   :language: c
   :start-after: /* T_SLIDE_STYLE enum start*/
   :end-before: /* T_SLIDE_STYLE enum end*/

```

## 示例

### 选项卡控件

选项卡切换的样式可以自定义，开发者可以使用 [gui_tabview_set_style(this, style)](#gui_tabview_set_style) 这个函数设置选项卡的切换风格。如 `REDUCTION`、`CLASSIC`、`FADE`、`REDUCTION_FADE`、`TAB_ROTATE`、`TAB_CUBE`、`TAB_PAGE`等。`FADE` 样式表示在切换过程中调整选项卡的透明度，而 `REDUCTION_FADE` 样式则可以在选项卡切换过程中同时改变图像大小和透明度。在这个例子中，我们将前三个选项卡指定为 `TAB_CUBE` 样式，后三个选项卡指定为 `REDUCTION` 样式。效果如下列动画所示，开发人员可以通过调用 `gui_tab_set_style(this, style)` 修改不同的选项卡切换风格。

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
    gui_obj_add_event_cb(win, (gui_event_cb_t)kb_button_cb, GUI_EVENT_KB_UP_PRESSED, NULL);
    gui_tabview_enable_pre_load(tv, true);

    gui_tab_t *tb_clock = gui_tab_create(tv, "tb_clock",           0, 0, 0, 0, 0, 0);
    gui_tab_t *tb_activity = gui_tab_create(tv, "tb_activity",     0, 0, 0, 0, 1, 0);
    gui_tab_t *tb_heart = gui_tab_create(tv, "tb_heart",           0, 0, 0, 0, 2, 0);
    gui_tab_t *tb_cube = gui_tab_create(tv, "tb_cube",           0, 0, 0, 0, 3, 0);
    gui_tab_t *tb_weather = gui_tab_create(tv, "tb_weather",       0, 0, 0, 0, 5, 0);
    gui_tab_t *tb_music = gui_tab_create(tv, "tb_music",           0, 0, 0, 0, 4, 0);
    gui_tab_t *tb_ani = gui_tab_create(tv, "tb_ani",          0, 0, 0, 0, 6, 0);
    page_tb_clock(gui_tab_get_rte_obj(tb_clock));
    page_tb_activity(gui_tab_get_rte_obj(tb_activity));
    page_tb_heart(gui_tab_get_rte_obj(tb_heart));
    page_tb_cube(gui_tab_get_rte_obj(tb_cube));
    page_tb_weather(gui_tab_get_rte_obj(tb_weather));
    page_tb_music(gui_tab_get_rte_obj(tb_music));

    gui_tab_set_style(tb_clock, TAB_CUBE);
    gui_tab_set_style(tb_activity, TAB_CUBE);
    gui_tab_set_style(tb_heart, TAB_CUBE);
    gui_tab_set_style(tb_cube, REDUCTION);
    gui_tab_set_style(tb_weather, REDUCTION);
    gui_tab_set_style(tb_music, REDUCTION);
}
```
<br>

<div style="text-align: center"><img src="https://foruda.gitee.com/images/1721901439083426379/8f8f9db8_10641540.gif" width = "300" /></div>

<br>

### 选项卡控件翻转

不同于单个选项卡的风格切换，在选项卡视图控件`(tabview)`中可以使用 [gui_tabview_set_style(this, style)](#gui_tabview_set_style)  来统一所有选项卡的切换样式。例如，可以将所有选项卡的样式统一设置为 `REDUCTION`、`CLASSIC`、`FADE`、`REDUCTION_FADE`、`TAB_ROTATE`、`TAB_CUBE`、`TAB_PAGE`等。下面的例子展示了如何切换到 `TAB_ROTATE`样式，默认情况下是 `CLASSIC`式。其余的选项卡切换风格开发人员可以使用[gui_tabview_set_style(this, style)](#gui_tabview_set_style) 函数进行设置。

<details> <summary>代码</summary>

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
    gui_obj_add_event_cb(win, (gui_event_cb_t)kb_button_cb, GUI_EVENT_KB_UP_PRESSED, NULL);
    gui_tabview_set_style(tv, TAB_ROTATE);
    gui_tabview_enable_pre_load(tv, true);

    gui_tab_t *tb_clock = gui_tab_create(tv, "tb_clock",           0, 0, 0, 0, 0, 0);
    gui_tab_t *tb_activity = gui_tab_create(tv, "tb_activity",     0, 0, 0, 0, 1, 0);
    gui_tab_t *tb_heart = gui_tab_create(tv, "tb_heart",           0, 0, 0, 0, 2, 0);
    gui_tab_t *tb_cube = gui_tab_create(tv, "tb_cube",           0, 0, 0, 0, 3, 0);
    gui_tab_t *tb_weather = gui_tab_create(tv, "tb_weather",       0, 0, 0, 0, 5, 0);
    gui_tab_t *tb_music = gui_tab_create(tv, "tb_music",           0, 0, 0, 0, 4, 0);
    gui_tab_t *tb_ani = gui_tab_create(tv, "tb_ani",          0, 0, 0, 0, 6, 0);
    page_tb_clock(gui_tab_get_rte_obj(tb_clock));
    page_tb_activity(gui_tab_get_rte_obj(tb_activity));
    page_tb_heart(gui_tab_get_rte_obj(tb_heart));
    page_tb_cube(gui_tab_get_rte_obj(tb_cube));
    page_tb_weather(gui_tab_get_rte_obj(tb_weather));
    page_tb_music(gui_tab_get_rte_obj(tb_music));
}
```
</details>
<br>

<div style="text-align: center"><img src="https://foruda.gitee.com/images/1721902043698157443/1cb14773_10641540.gif" width = "300" /></div>
<br>

### 选项卡循环显示

在选项卡视图中，可以使用函数 [gui_tabview_loop_x(tabview, loop)](#gui_tabview_create) (请参考 tabview) 来决定选项卡是否连续在x方向上循环切换和显示。同理，[gui_tabview_loop_y(tabview, loop)](#gui_tabview_create) 决定选项卡是否连续在y方向上循环切换和显示。loop 参数是布尔类型，用于指定是否希望启用循环显示功能，如果是true，则选项卡将会循环显示，如果是false，则选项卡将不会循环显示。

<details> <summary>代码</summary>

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
    gui_obj_add_event_cb(win, (gui_event_cb_t)kb_button_cb, GUI_EVENT_KB_UP_PRESSED, NULL);
    gui_tabview_set_style(tv, TAB_CUBE);
    gui_tabview_enable_pre_load(tv, true);
    gui_tabview_loop_x(tv, true);

    gui_tab_t *tb_clock = gui_tab_create(tv, "tb_clock",           0, 0, 0, 0, 0, 0);
    gui_tab_t *tb_activity = gui_tab_create(tv, "tb_activity",     0, 0, 0, 0, 1, 0);
    gui_tab_t *tb_heart = gui_tab_create(tv, "tb_heart",           0, 0, 0, 0, 2, 0);
    gui_tab_t *tb_cube = gui_tab_create(tv, "tb_cube",           0, 0, 0, 0, 3, 0);
    gui_tab_t *tb_weather = gui_tab_create(tv, "tb_weather",       0, 0, 0, 0, 5, 0);
    gui_tab_t *tb_music = gui_tab_create(tv, "tb_music",           0, 0, 0, 0, 4, 0);
    gui_tab_t *tb_ani = gui_tab_create(tv, "tb_ani",          0, 0, 0, 0, 6, 0);
    page_tb_clock(gui_tab_get_rte_obj(tb_clock));
    page_tb_activity(gui_tab_get_rte_obj(tb_activity));
    page_tb_heart(gui_tab_get_rte_obj(tb_heart));
    page_tb_cube(gui_tab_get_rte_obj(tb_cube));
    page_tb_weather(gui_tab_get_rte_obj(tb_weather));
    page_tb_music(gui_tab_get_rte_obj(tb_music));
}
```
</details>
<br>
<div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/tabview_loop.gif" width = "300" /></div>
<br>


<span id="gui_tab_create">

## API
</span>

```eval_rst
.. doxygenfile:: gui_tab.h
```
