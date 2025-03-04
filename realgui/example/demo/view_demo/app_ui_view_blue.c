/* view blue start*/
#include "guidef.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include <gui_app.h>
#include "tiger_blue.txt"
#include "tiger_white.txt"
#include "tiger_yellow.txt"
#include "gui_server.h"
#include "gui_components_init.h"
#include "gui_win.h"
#include "gui_view.h"
#include "gui_fps.h"

#define CURRENT_VIEW_NAME "blue_view"


static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *yellow_view = NULL;
const static gui_view_descriptor_t *white_view = NULL;
static void app_ui_view_blue_design(gui_view_t *view);


static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .design_cb = app_ui_view_blue_design,
};

static int gui_view_descriptor_register_init(void)
{
    gui_view_descriptor_register(&descriptor);
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_REGISTER(gui_view_descriptor_register_init);


static int gui_view_get_other_view_descriptor_init(void)
{
    /* you can get other view descriptor point here */
    yellow_view = gui_view_descriptor_get("yellow_view");
    white_view = gui_view_descriptor_get("white_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);


static void app_ui_design(gui_app_t *app)
{
    gui_win_t *win_view = gui_win_create(app->window, "win_view", 0, 0, 0, 0);
    current_view = gui_view_create(win_view, &descriptor, 0, 0, 0, 0);
    gui_fps_create(app->window);
}
/* view demo end*/

static gui_app_t rtk_gui_demo =
{
    .screen =
    {
        .name = "rtk_gui_demo",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_ui_design,
    .active_ms = 1000 * 60 * 60,
};


static int app_init(void)
{
    gui_app_startup(&rtk_gui_demo);
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);

static void app_ui_view_blue_design(gui_view_t *view)
{
    gui_img_t *img = gui_img_create_from_mem(view, "img", (void *)_actiger_blue, 0, 0, 0,
                                             0);
    gui_img_scale(img, 1.875f, 2.034f);
    gui_img_set_mode(img, IMG_BYPASS_MODE);

    gui_view_switch_on_event(view, yellow_view, VIEW_CUBE, VIEW_CUBE,
                             GUI_EVENT_TOUCH_MOVE_RIGHT);

    gui_view_switch_on_event(view, white_view, VIEW_CUBE, VIEW_CUBE, GUI_EVENT_TOUCH_MOVE_LEFT);

    gui_view_switch_on_event(view, white_view, VIEW_CUBE, VIEW_CUBE,
                             GUI_EVENT_TOUCH_MOVE_UP);

    gui_view_switch_on_event(view, yellow_view, VIEW_CUBE, VIEW_CUBE,
                             GUI_EVENT_TOUCH_MOVE_DOWN);

    gui_view_switch_on_event(view, white_view, VIEW_ANIMATION_7, VIEW_ANIMATION_3,
                             GUI_EVENT_TOUCH_CLICKED);
    gui_view_switch_on_event(view, yellow_view, VIEW_ANIMATION_7, VIEW_ANIMATION_3,
                             GUI_EVENT_KB_SHORT_CLICKED);
}
/* view blue end*/