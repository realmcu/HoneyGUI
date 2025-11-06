#include "gui_view_macro.h"

static void anti_aliasing_switch_in(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("anti_aliasing_view switch in\n");
}

static void anti_aliasing_switch_out(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("anti_aliasing_view switch out\n");
}

GUI_VIEW_INSTANCE("anti_aliasing_view", false, anti_aliasing_switch_in, anti_aliasing_switch_out);

