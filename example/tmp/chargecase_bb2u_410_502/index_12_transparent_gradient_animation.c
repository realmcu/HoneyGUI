#include "gui_view_macro.h"

static void transparent_gradient_animation_switch_in(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("transparent_gradient_animation_view switch in\n");
}

static void transparent_gradient_animation_switch_out(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("transparent_gradient_animation_view switch out\n");
}

GUI_VIEW_INSTANCE("transparent_gradient_animation_view", false,
                  transparent_gradient_animation_switch_in, transparent_gradient_animation_switch_out);

