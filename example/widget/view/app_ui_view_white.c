/* view white start*/
#include "guidef.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "../../assets/tiger_blue.txt"
#include "../../assets/tiger_grey.txt"
#include "../../assets/tiger_laven.txt"
#include "../../assets/tiger_lime.txt"
#include "../../assets/tiger_turk.txt"
#include "../../assets/tiger_white.txt"
#include "../../assets/tiger_yellow.txt"
#include "gui_server.h"
#include "gui_components_init.h"
#include "gui_view.h"

#define CURRENT_VIEW_NAME "white_view"


static gui_view_t *current_view = NULL;
static const gui_view_descriptor_t *blue_view = NULL;
static const gui_view_descriptor_t *yellow_view = NULL;
static void app_ui_view_white_design(gui_view_t *view);

static const gui_view_descriptor_t descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,

    .on_switch_in = app_ui_view_white_design,
    .on_switch_out = NULL,

    .keep = false,
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
    blue_view = gui_view_descriptor_get("blue_view");
    yellow_view = gui_view_descriptor_get("yellow_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void img_cb()
{
    gui_view_switch_direct(current_view, blue_view, SWITCH_OUT_NONE_ANIMATION,
                           SWITCH_OUT_NONE_ANIMATION);
}

static void app_ui_view_white_design(gui_view_t *view)
{
    gui_img_t *img = gui_img_create_from_mem(view, "img", (void *)_actiger_white, 0, 0, 0,
                                             0);

    gui_obj_add_event_cb(img, (gui_event_cb_t)img_cb, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_view_switch_on_event(view, blue_view, SWITCH_OUT_TO_RIGHT_USE_CUBE,
                             SWITCH_IN_FROM_LEFT_USE_CUBE,
                             GUI_EVENT_TOUCH_MOVE_RIGHT);

    gui_view_switch_on_event(view, yellow_view, SWITCH_OUT_TO_LEFT_USE_ROTATE,
                             SWITCH_IN_FROM_RIGHT_USE_ROTATE,
                             GUI_EVENT_TOUCH_MOVE_LEFT);

    gui_view_switch_on_event(view, blue_view, SWITCH_OUT_TO_TOP_USE_TRANSLATION,
                             SWITCH_IN_FROM_BOTTOM_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_UP);

    gui_view_switch_on_event(view, yellow_view, SWITCH_OUT_TO_BOTTOM_USE_TRANSLATION,
                             SWITCH_IN_FROM_TOP_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_DOWN);

    gui_view_switch_on_event(view, yellow_view, SWITCH_OUT_ANIMATION_ZOOM,
                             SWITCH_IN_ANIMATION_ZOOM,
                             GUI_EVENT_TOUCH_CLICKED);

    gui_view_set_animate_step(view, 30);

}
/* view white end*/






