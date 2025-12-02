/* view blue start*/
#include "guidef.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "../../assets/tiger_blue.txt"
#include "gui_server.h"
#include "gui_components_init.h"
#include "gui_win.h"
#include "gui_view.h"
#include "gui_view_instance.h"

#define CURRENT_VIEW_NAME "blue_view"


static void switch_in_cb(gui_view_t *view);
static void switch_out_cb(gui_view_t *view);

GUI_VIEW_INSTANCE(CURRENT_VIEW_NAME, true, switch_in_cb, switch_out_cb);

static void img_cb(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    gui_view_switch_direct(gui_view_get("blue_view"), "white_view",
                           SWITCH_OUT_NONE_ANIMATION,
                           SWITCH_IN_NONE_ANIMATION);
}

static void switch_out_cb(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("blue view clean\n");
}

static void switch_in_cb(gui_view_t *view)
{
    gui_view_set_animate_step(view, 20);
    gui_img_t *img = gui_img_create_from_mem(view, "img", (void *)_actiger_blue, 200, 200, 0, 0);
    gui_obj_add_event_cb(img, (gui_event_cb_t)img_cb, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_view_switch_on_event(view, "yellow_view", SWITCH_OUT_TO_RIGHT_USE_CUBE,
                             SWITCH_IN_FROM_LEFT_USE_CUBE,
                             GUI_EVENT_TOUCH_MOVE_RIGHT);

    gui_view_switch_on_event(view, "white_view", SWITCH_OUT_TO_LEFT_USE_CUBE,
                             SWITCH_IN_FROM_RIGHT_USE_CUBE,
                             GUI_EVENT_TOUCH_MOVE_LEFT);

    gui_view_switch_on_event(view, "yellow_view",
                             SWITCH_OUT_TO_TOP_USE_TRANSLATION,
                             SWITCH_IN_CENTER_ZOOM_FADE,
                             GUI_EVENT_TOUCH_MOVE_UP);

    gui_view_switch_on_event(view, "lime_view", SWITCH_OUT_STILL_USE_BLUR,
                             SWITCH_IN_FROM_TOP_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_DOWN);

    gui_view_switch_on_event(view, "white_view", SWITCH_OUT_ANIMATION_ZOOM,
                             SWITCH_IN_ANIMATION_ZOOM,
                             GUI_EVENT_TOUCH_CLICKED);


}


static int app_init(void)
{
    gui_view_create(gui_obj_get_root(), CURRENT_VIEW_NAME, 0, 0, 0, 0);
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);

/* view blue end*/