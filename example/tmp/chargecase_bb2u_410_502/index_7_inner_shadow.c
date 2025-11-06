#include "gui_view_macro.h"

static void inner_shadow_switch_in(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("inner_shadow_view switch in\n");
}

static void inner_shadow_switch_out(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("inner_shadow_view switch out\n");
}

GUI_VIEW_INSTANCE("inner_shadow_view", false, inner_shadow_switch_in, inner_shadow_switch_out);

