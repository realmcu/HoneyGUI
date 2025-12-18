/* view lime start*/
#include "guidef.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "../../assets/tiger_lime.txt"
#include "gui_server.h"
#include "gui_components_init.h"
#include "gui_view.h"
#include "gui_view_instance.h"

#define CURRENT_VIEW_NAME "lime_view"


static void app_ui_view_lime_design(gui_view_t *view);

GUI_VIEW_INSTANCE(CURRENT_VIEW_NAME, false, app_ui_view_lime_design, NULL);

static void img_cb(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    gui_view_switch_direct(gui_view_get("lime_view"), "blue_view",
                           SWITCH_OUT_ANIMATION_ZOOM,
                           SWITCH_IN_ANIMATION_ZOOM);
}

static void app_ui_view_lime_design(gui_view_t *view)
{
    gui_view_set_animate_step(view, 20);
    gui_img_t *img = gui_img_create_from_mem(view, "img", (void *)_actiger_lime, 200, 100, 0,
                                             0);

    gui_obj_add_event_cb(img, (gui_event_cb_t)img_cb, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_view_switch_on_event(view, "blue_view",
                             SWITCH_OUT_TO_TOP_USE_TRANSLATION,
                             SWITCH_IN_STILL_USE_BLUR,
                             GUI_EVENT_TOUCH_MOVE_UP);

    gui_view_set_opacity(view, 150);


}
/* view lime end*/






