# Perspective
<br>

## Overview

The perspective is a hexagonal prism-like widget that uses six tabs as column faces. The prism will automatically rotate to show all tabs. A single click allows the user to quick jump to the current tab.

## Usage

### Create perspective widget
[gui_perspective_t *gui_perspective_create(void *parent,  const char *name, gui_perspective_imgfile_t *img_file, int16_t x, int16_t y);](#gui_perspective_create) is used to create a perspective widget. The `img_file` is an struct including image sources for the six pages. Both memory address and file path are supported, `uint8_t flg_fs` should be set `true` when using filesystem.

### Set image mode
By default, the cube's image blend mode is `IMG_SRC_OVER_MODE`, You can change the blend mode of image by calling [void gui_perspective_set_mode(gui_perspective_t *perspective, uint8_t img_index, BLEND_MODE_TYPE mode)](#api).

### Set cube image
The images of cube can be configured by calling [void gui_perspective_set_img(gui_perspective_t *perspective, gui_perspective_imgfile_t *img_file)](#api).

## Example

```c
#include "root_image_hongkong/ui_resource.h"
#include "gui_perspective.h"
#include "gui_canvas.h"
#include "gui_win.h"
#include "gui_obj.h"
#include "gui_app.h"
#include <gui_tabview.h>
#include "app_hongkong.h"

extern gui_tabview_t *tv;
gui_perspective_t *img_test;

static void canvas_cb_black(gui_canvas_t *canvas)
{
    nvgRect(canvas->vg, 0, 0, 368, 448);
    nvgFillColor(canvas->vg, nvgRGBA(0, 0, 128, 255));
    nvgFill(canvas->vg);
}

static void callback_prism_touch_clicked()
{
    gui_app_t *app = get_app_hongkong();
    gui_obj_t *screen = &(app->screen);
    int angle = img_test->release_x;
    if (img_test->release_x < 0)
    {
        angle = img_test->release_x + 360;
    }
    angle = (angle % 360) / (360 / 6);
    if (angle < 0 || angle > 5)
    {
        angle = 0;
    }

    gui_tree_free(screen);
    app->ui_design(get_app_hongkong());


    gui_tabview_jump_tab(tv, angle, 0);
}

void callback_prism(void *obj, gui_event_t e)
{
    gui_app_t *app = get_app_hongkong();
    gui_obj_t *screen = &(app->screen);

    gui_tree_free(screen);

    gui_win_t *win = gui_win_create(screen, "win", 0, 0, 368, 448);
    gui_canvas_t *canvas = gui_canvas_create(win, "canvas", 0, 0, 0, 368, 448);
    gui_canvas_set_canvas_cb(canvas, canvas_cb_black);
    gui_perspective_imgfile_t imgfile =
    {
        .flg_fs = true,
        .img_path[0] = "Clockn.bin",
        .img_path[1] = "Weather.bin",
        .img_path[2] = "Music.bin",
        .img_path[3] = "QuickCard.bin",
        .img_path[4] = "HeartRate.bin",
        .img_path[5] = "Activity.bin"
    };
    //// using memory address
    // gui_perspective_imgfile_t imgfile =
    // {
    //     .flg_fs = false,
    //     .data_addr[0] = CLOCKN_BIN,
    //     .data_addr[1] = WEATHER_BIN,
    //     .data_addr[2] = MUSIC_BIN,
    //     .data_addr[3] = QUICKCARD_BIN,
    //     .data_addr[4] = HEARTRATE_BIN,
    //     .data_addr[5] = ACTIVITY_BIN
    // };
    img_test = gui_perspective_create(canvas, "test", &imgfile, 0, 0);

    gui_obj_add_event_cb(win, (gui_event_cb_t)callback_prism_touch_clicked, GUI_EVENT_TOUCH_CLICKED,NULL);
}
```

<br>
<div style="text-align: center"><img src="https://foruda.gitee.com/images/1699931105543257223/ccac3ca0_10641540.png" width = "300" /></div>
<br>

<span id = "gui_perspective_create">

## API

</span>

```eval_rst

.. doxygenfile:: gui_perspective.h

```
