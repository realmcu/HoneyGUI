/* view yellow start*/
#include "guidef.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "../../assets/tiger_yellow.txt"
#include "gui_server.h"
#include "gui_components_init.h"
#include "gui_view.h"
#include "gui_view_instance.h"

#define CURRENT_VIEW_NAME "yellow_view"

static void app_ui_view_yellow_design(gui_view_t *view);

GUI_VIEW_INSTANCE(CURRENT_VIEW_NAME, false, app_ui_view_yellow_design, NULL);

static void app_ui_view_yellow_design(gui_view_t *view)
{
    gui_view_set_animate_step(view, 20);
    gui_img_create_from_mem(view, "img", (void *)_actiger_yellow, 0, 0, 0,
                            0);

    gui_view_switch_on_event(view, "white_view",
                             SWITCH_OUT_TO_RIGHT_USE_ROTATE,
                             SWITCH_IN_FROM_LEFT_USE_ROTATE,
                             GUI_EVENT_TOUCH_MOVE_RIGHT);

    gui_view_switch_on_event(view, "blue_view", SWITCH_OUT_TO_LEFT_USE_CUBE,
                             SWITCH_IN_FROM_RIGHT_USE_CUBE,
                             GUI_EVENT_TOUCH_MOVE_LEFT);

    gui_view_switch_on_event(view, "white_view",
                             SWITCH_OUT_TO_TOP_USE_TRANSLATION,
                             SWITCH_IN_FROM_BOTTOM_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_UP);

    gui_view_switch_on_event(view, "blue_view",
                             SWITCH_OUT_TO_BOTTOM_USE_TRANSLATION,
                             SWITCH_IN_FROM_TOP_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_DOWN);

    gui_view_switch_on_event(view, "blue_view", SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_BOUNCE_FROM_RIGHT,
                             GUI_EVENT_TOUCH_CLICKED);

}
/* view yellow end*/






