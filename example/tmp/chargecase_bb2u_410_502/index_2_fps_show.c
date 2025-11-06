#include "gui_view_macro.h"

static void fps_show_switch_in(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("fps_show_view switch in\n");
}

static void fps_show_switch_out(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("fps_show_view switch out\n");
}

GUI_VIEW_INSTANCE("fps_show_view", false, fps_show_switch_in, fps_show_switch_out);






