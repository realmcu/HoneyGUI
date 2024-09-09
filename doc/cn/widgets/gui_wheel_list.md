# 旋转列表

旋转列表组件是一个应用程序列表界面，主要由多个圆形层排列的应用程序图标组成。每层可以独立旋转，每个图标都可以有一个关联的回调。

## 使用方法

### 创建一个旋转列表

[gui_wheel_list_t *gui_wheel_list_create(void *parent, uint8_t layer, uint8_t radius, uint8_t distence)](#gui_wheel_list_create) 用于创建一个旋转列表组件。

`layer` 是圆轮的层数，1表示除了中心图标外还有1圈外部图标。

`radius` 是图标的半径。如果图标是正方形的话，它是图标图像边长的一半。这个值需要精确，否则在图标排列计算和位置计算中会出现错误。

`distance` 是相邻两层图标中心之间的距离，需要大于图标的直径或边长。

在第0层，有一个中心图标，其中心位于屏幕中心。这个点作为计算其他图标位置的参考点。
每层的图标数量为6*n，其中n代表图标层数。因此，越靠近外层，图标越多，同层相邻图标中心与屏幕中心的夹角越小。

### 添加图标

#### 默认方式

创建旋转列表组件后，可以使用默认接口添加图标。在轮子中，可以从内向外一层层添加相应的图标。
[void gui_wheel_list_add_icon_default(gui_wheel_list_t *this, void *icon_addr,gui_event_cb_t event_cb)](#gui_wheel_list_add_icon_default)

`this` 是组件的指针。

`icon_addr` 是图像地址的指针。

`event_cb` 是此图像的回调函数，由点击触发。


<details> <summary>代码</summary>

```c
#include "gui_wheel_list.h"
#include "gui_img.h"
#include "gui_app.h"
#include "root_image_8762g/ui_resource.h"

static void app_launcher_ui_design(gui_app_t *app);
static gui_app_t app_launcher =
{
    .screen =
    {
        .name = "app_launcher",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_launcher_ui_design,
    .active_ms = 1000 * 5,
};
gui_app_t *get_launcher_app(void)
{
    return &app_launcher;
}

static void app_wheel_ui_design(gui_app_t *app);
static gui_app_t app_wheel =
{
    .screen =
    {
        .name = "app_wheel",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_wheel_ui_design,
    .active_ms = 1000 * 5,
};
gui_app_t *get_wheel_app(void)
{
    return &app_wheel;
}

static void wheel_cb(void *obj, gui_event_t event)
{

}
static void switch_to_launcher(void *obj, gui_event_t event)
{
    gui_switch_app(get_wheel_app(), get_launcher_app());
}
static void app_wheel_ui_design(gui_app_t *app)
{
    gui_wheel_list_t *hc = gui_wheel_list_create(&app->screen, 2, 32, 80);
    gui_wheel_list_add_icon_default(hc, ICMENUALARM_BIN, switch_to_launcher);

    gui_wheel_list_add_icon_default(hc, ICMENUBIRD_BIN, wheel_cb);
    gui_wheel_list_add_icon_default(hc, ICMENUALBUM_BIN, wheel_cb);
    gui_wheel_list_add_icon_default(hc, ICMENUHEARTRATE_BIN, wheel_cb);
    gui_wheel_list_add_icon_default(hc, ICMENUMUSIC_BIN, wheel_cb);
    gui_wheel_list_add_icon_default(hc, ICMENUALARM_BIN, wheel_cb);
    gui_wheel_list_add_icon_default(hc, ICMENUBIRD_BIN, wheel_cb);

    gui_wheel_list_add_icon_default(hc, ICMENUALBUM_BIN, NULL);
    gui_wheel_list_add_icon_default(hc, ICMENUHEARTRATE_BIN, NULL);
    gui_wheel_list_add_icon_default(hc, ICMENUMUSIC_BIN, NULL);
    gui_wheel_list_add_icon_default(hc, ICMENUALARM_BIN, NULL);
    gui_wheel_list_add_icon_default(hc, ICMENUBIRD_BIN, NULL);
    gui_wheel_list_add_icon_default(hc, ICMENUALBUM_BIN, NULL);
    gui_wheel_list_add_icon_default(hc, ICMENUHEARTRATE_BIN, NULL);
    gui_wheel_list_add_icon_default(hc, ICMENUMUSIC_BIN, NULL);
    gui_wheel_list_add_icon_default(hc, ICMENUALARM_BIN, NULL);
    gui_wheel_list_add_icon_default(hc, ICMENUBIRD_BIN, NULL);
    gui_wheel_list_add_icon_default(hc, ICMENUALBUM_BIN, NULL);
    gui_wheel_list_add_icon_default(hc, ICMENUHEARTRATE_BIN, NULL);
}

static void app_launcher_ui_design(gui_app_t *app)
{

}

```

</details></br>

<center><img width="456" src= "https://docs.realmcu.com/HoneyGUI/image/widgets/wheel_list1.gif"/></center>

<br/>

#### 自定义方式

创建旋转列表组件后，可以使用自定义接口添加图标。在轮子中，可以从内向外一层层添加相应的图标。
[gui_wheel_list_set_icon(gui_wheel_list_t *this, void *icon_addr, gui_event_cb_t event_cb, uint8_t layer, uint8_t index)](#gui_wheel_list_set_icon)

`this` 是组件的指针。

`icon_addr` 是图像地址的指针。

`event_cb` 是此图像的回调函数，由点击触发。

`layer` 是图标所在的层数。


<details> <summary>代码</summary>

```c
#include "gui_wheel_list.h"
#include "gui_img.h"
#include "gui_app.h"
#include "root_image_8762g/ui_resource.h"

static void app_launcher_ui_design(gui_app_t *app);
static gui_app_t app_launcher =
{
    .screen =
    {
        .name = "app_launcher",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_launcher_ui_design,
    .active_ms = 1000 * 5,
};
gui_app_t *get_launcher_app(void)
{
    return &app_launcher;
}

static void app_wheel_ui_design(gui_app_t *app);
static gui_app_t app_wheel =
{
    .screen =
    {
        .name = "app_wheel",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_wheel_ui_design,
    .active_ms = 1000 * 5,
};
gui_app_t *get_wheel_app(void)
{
    return &app_wheel;
}

static void wheel_cb(void *obj, gui_event_t event)
{

}
static void switch_to_launcher(void *obj, gui_event_t event)
{
    gui_switch_app(get_wheel_app(), get_launcher_app());
}
static void app_wheel_ui_design(gui_app_t *app)
{
    gui_wheel_list_t *hc = gui_wheel_list_create(&app->screen, 2, 32, 80);
    gui_wheel_list_set_icon(hc, ICMENUALARM_BIN, switch_to_launcher, 0, 0);

    gui_wheel_list_set_icon(hc, ICMENUBIRD_BIN, wheel_cb, 1, 0);
    gui_wheel_list_set_icon(hc, ICMENUHEARTRATE_BIN, wheel_cb, 1, 2);
    gui_wheel_list_set_icon(hc, ICMENUALARM_BIN, wheel_cb, 1, 4);

    gui_wheel_list_set_icon(hc, ICMENUHEARTRATE_BIN, NULL, 2, 1);
    gui_wheel_list_set_icon(hc, ICMENUMUSIC_BIN, NULL, 2, 2);
    gui_wheel_list_set_icon(hc, ICMENUALARM_BIN, NULL, 2, 3);
    gui_wheel_list_set_icon(hc, ICMENUALBUM_BIN, NULL, 2, 5);
    gui_wheel_list_set_icon(hc, ICMENUHEARTRATE_BIN, NULL, 2, 6);
    gui_wheel_list_set_icon(hc, ICMENUMUSIC_BIN, NULL, 2, 7);
    gui_wheel_list_set_icon(hc, ICMENUBIRD_BIN, NULL, 2, 9);
    gui_wheel_list_set_icon(hc, ICMENUALBUM_BIN, NULL, 2, 10);
    gui_wheel_list_set_icon(hc, ICMENUHEARTRATE_BIN, NULL, 2, 11);
}

static void app_launcher_ui_design(gui_app_t *app)
{

}

```

</details></br>

<center><img width="456" src= "https://docs.realmcu.com/HoneyGUI/image/widgets/wheel_list2.gif"/></center>

<br/>

## API

```eval_rst

.. doxygenfile:: gui_wheel_list.h

```
