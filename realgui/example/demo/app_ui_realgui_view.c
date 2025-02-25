/* view demo start*/
#include "guidef.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include <gui_app.h>
#include "tiger_blue.txt"
// #include "tiger_grey.txt"
// #include "tiger_laven.txt"
// #include "tiger_lime.txt"
// #include "tiger_turk.txt"
#include "tiger_white.txt"
#include "tiger_yellow.txt"
#include "gui_server.h"
#include "gui_components_init.h"
// #include "gui_cube.h"
#include "gui_win.h"
#include "gui_view.h"
#include "gui_fps.h"
gui_view_t *view_0 = NULL;
gui_view_t *view_1 = NULL;
gui_view_t *view_2 = NULL;
gui_win_t *win_view = NULL;

static void create_view_1(void *parent);
static void create_view_2(void *parent);
static void create_view_0(void *parent)
{
    gui_view_t *view = (gui_view_t *)(GUI_BASE(parent)->parent);
    gui_img_t *img = gui_img_create_from_mem(parent, "img", (void *)_actiger_white, 0, 0, 0,
                                             0);
    gui_img_scale(img, 1.875f, 2.034f);
    gui_img_set_mode(img, IMG_BYPASS_MODE);
    gui_view_add_change_event(view, (void **)&view_1, VIEW_REDUCTION, VIEW_REDUCTION, create_view_1,
                              GUI_EVENT_TOUCH_MOVE_RIGHT);
    gui_view_add_change_event(view, (void **)&view_2, VIEW_CUBE, VIEW_CUBE, create_view_2,
                              GUI_EVENT_TOUCH_MOVE_LEFT);
    gui_view_add_change_event(view, NULL, VIEW_STILL, VIEW_TRANSPLATION, create_view_0,
                              GUI_EVENT_TOUCH_MOVE_UP);
    gui_view_add_change_event(view, (void **)&view_2, VIEW_ROTATE_BOOK, VIEW_ROTATE_BOOK, create_view_2,
                              GUI_EVENT_TOUCH_MOVE_DOWN);
    gui_view_add_change_event(view, (void **)&view_1, VIEW_ANIMATION_6, VIEW_ANIMATION_2, create_view_1,
                              GUI_EVENT_TOUCH_CLICKED);
}

static void create_view_1(void *parent)
{
    gui_img_t *img = gui_img_create_from_mem(parent, "img", (void *)_actiger_yellow, 0, 0, 0,
                                             0);
    gui_img_scale(img, 1.875f, 2.034f);
    gui_img_set_mode(img, IMG_BYPASS_MODE);
    gui_view_add_change_event(view_1, (void **)&view_0, VIEW_REDUCTION, VIEW_REDUCTION, create_view_0,
                              GUI_EVENT_TOUCH_MOVE_LEFT);
    gui_view_add_change_event(view_1, (void **)&view_2, VIEW_ROTATE, VIEW_ROTATE, create_view_2,
                              GUI_EVENT_TOUCH_MOVE_RIGHT);
    gui_view_add_change_event(view_1, (void **)&view_1, VIEW_TRANSPLATION, VIEW_TRANSPLATION,
                              create_view_1,
                              GUI_EVENT_TOUCH_MOVE_UP);
    gui_view_add_change_event(view_1, (void **)&view_0, VIEW_STILL, VIEW_TRANSPLATION, create_view_0,
                              GUI_EVENT_TOUCH_MOVE_DOWN);
    gui_view_add_change_event(view_1, (void **)&view_2, VIEW_ANIMATION_7, VIEW_ANIMATION_3,
                              create_view_2,
                              GUI_EVENT_TOUCH_CLICKED);
}
static void create_view_2(void *parent)
{
    gui_img_t *img = gui_img_create_from_mem(parent, "img", (void *)_actiger_blue, 0, 0, 0, 0);
    gui_img_scale(img, 1.875f, 2.034f);
    gui_img_set_mode(img, IMG_BYPASS_MODE);
    gui_view_add_change_event(view_2, (void **)&view_0, VIEW_CUBE, VIEW_CUBE,  create_view_0,
                              GUI_EVENT_TOUCH_MOVE_RIGHT);
    gui_view_add_change_event(view_2, (void **)&view_1, VIEW_ROTATE, VIEW_ROTATE, create_view_1,
                              GUI_EVENT_TOUCH_MOVE_LEFT);
    gui_view_add_change_event(view_2, (void **)&view_2, VIEW_TRANSPLATION, VIEW_TRANSPLATION,
                              create_view_2,
                              GUI_EVENT_TOUCH_MOVE_UP);
    gui_view_add_change_event(view_2, (void **)&view_1, VIEW_TRANSPLATION, VIEW_TRANSPLATION,
                              create_view_1,
                              GUI_EVENT_TOUCH_MOVE_DOWN);
    gui_view_add_change_event(view_2, (void **)&view_0, VIEW_CUBE, VIEW_CUBE, create_view_0,
                              GUI_EVENT_TOUCH_MOVE_UP);
    gui_view_add_change_event(view_2, (void **)&view_0, VIEW_ANIMATION_8, VIEW_ANIMATION_4,
                              create_view_0,
                              GUI_EVENT_TOUCH_CLICKED);
}

static void app_ui_design(gui_app_t *app)
{
    win_view = gui_win_create(app->window, "win_view", 0, 0, 0, 0);
    view_0 = gui_view_create(win_view, "view", 0, 0, 0, 0);
    create_view_0(view_0->rte_obj);
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
    gui_server_init();
    gui_app_startup(&rtk_gui_demo);
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);


