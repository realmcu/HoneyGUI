#include "gui_view_macro.h"

static void fps_show_switch_in(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("fps_show_view switch in\n");


    gui_view_switch_on_event(view, gui_view_descriptor_get("anti_aliasing_view"),
                             SWITCH_OUT_TO_LEFT_USE_TRANSLATION,
                             SWITCH_IN_FROM_RIGHT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_LEFT);
}

static void fps_show_switch_out(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("fps_show_view switch out\n");
}

GUI_VIEW_INSTANCE("fps_show_view", false, fps_show_switch_in, fps_show_switch_out);






