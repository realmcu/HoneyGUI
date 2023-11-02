# perspective
The perspective is a hexagonal prism-like widget that uses 6 tabs as column faces. The prism will automatically rotate to show all tabs. A single click allows the user to quick jump to the current tab.


## demo
```eval_rst
.. raw:: html

    <iframe src="https://drive.google.com/file/d/1eKNfZbz84aMV3w30YqhFmz-5G646j1n_/preview" width="640" height="480" allow="autoplay"></iframe>
```

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
    uint8_t *array_flash[] = {ACTIVITY_BIN, WEATHER_BIN, HEARTRATE_BIN, CLOCKN_BIN, MUSIC_BIN, QUICKCARD_BIN};
    img_test = gui_perspective_create(canvas, "test", array_flash, 0, 0, 0, 0);

    gui_obj_add_event_cb(win, (gui_event_cb_t)callback_prism_touch_clicked, GUI_EVENT_TOUCH_CLICKED,
                         NULL);
}
```

## API
```eval_rst
.. doxygenfunction:: gui_perspective_create

```

