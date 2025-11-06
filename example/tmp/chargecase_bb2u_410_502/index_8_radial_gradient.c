#include "gui_view_macro.h"

static void radial_gradient_switch_in(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("radial_gradient_view switch in\n");
}

static void radial_gradient_switch_out(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("radial_gradient_view switch out\n");
}

GUI_VIEW_INSTANCE("radial_gradient_view", false, radial_gradient_switch_in,
                  radial_gradient_switch_out);

