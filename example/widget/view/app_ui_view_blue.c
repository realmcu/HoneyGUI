/* view blue start*/
#include "guidef.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "../../assets/tiger_blue.txt"
#include "../../assets/tiger_white.txt"
#include "../../assets/tiger_yellow.txt"
#include "gui_server.h"
#include "gui_components_init.h"
#include "gui_win.h"
#include "gui_view.h"


#define CURRENT_VIEW_NAME "blue_view"


static gui_view_t *current_view = NULL;
static const gui_view_descriptor_t *yellow_view_descriptor = NULL;
static const gui_view_descriptor_t *white_view_descriptor = NULL;
static const gui_view_descriptor_t *lime_view_descriptor = NULL;
static void switch_in_cb(gui_view_t *view);
static void switch_out_cb(gui_view_t *view);

static const gui_view_descriptor_t descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,

    .on_switch_in = switch_in_cb,
    .on_switch_out = switch_out_cb,

    .keep = true,
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
    yellow_view_descriptor = gui_view_descriptor_get("yellow_view");
    white_view_descriptor = gui_view_descriptor_get("white_view");
    lime_view_descriptor = gui_view_descriptor_get("lime_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void img_cb(void *obj, gui_event_t e, void *param)
{
    gui_view_switch_direct(current_view, white_view_descriptor, SWITCH_OUT_NONE_ANIMATION,
                           SWITCH_OUT_NONE_ANIMATION);
}

static void switch_out_cb(gui_view_t *view)
{
    gui_log("blue view clean\n");
}

static void switch_in_cb(gui_view_t *view)
{
    gui_view_set_animate_step(view, 20);
    gui_img_t *img = gui_img_create_from_mem(view, "img", (void *)_actiger_blue, 200, 200, 0, 0);
    gui_obj_add_event_cb(img, (gui_event_cb_t)img_cb, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_view_switch_on_event(view, yellow_view_descriptor, SWITCH_OUT_TO_RIGHT_USE_CUBE,
                             SWITCH_IN_FROM_LEFT_USE_CUBE,
                             GUI_EVENT_TOUCH_MOVE_RIGHT);

    gui_view_switch_on_event(view, white_view_descriptor, SWITCH_OUT_TO_LEFT_USE_CUBE,
                             SWITCH_IN_FROM_RIGHT_USE_CUBE,
                             GUI_EVENT_TOUCH_MOVE_LEFT);

    gui_view_switch_on_event(view, yellow_view_descriptor, SWITCH_OUT_TO_TOP_USE_TRANSLATION,
                             SWITCH_IN_CENTER_ZOOM_FADE,
                             GUI_EVENT_TOUCH_MOVE_UP);

    gui_view_switch_on_event(view, lime_view_descriptor, SWITCH_OUT_STILL_USE_BLUR,
                             SWITCH_IN_FROM_TOP_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_DOWN);

    gui_view_switch_on_event(view, white_view_descriptor, SWITCH_OUT_ANIMATION_ZOOM,
                             SWITCH_IN_ANIMATION_ZOOM,
                             GUI_EVENT_TOUCH_CLICKED);


}

static int app_init(void)
{
    gui_view_create(gui_obj_get_root(), &descriptor, 0, 0, 0, 0);
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);

/* view blue end*/