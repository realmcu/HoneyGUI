#include "gui_view_macro.h"

static void multiple_layer_animation_switch_in(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("multiple_layer_animation_view switch in\n");
}

static void multiple_layer_animation_switch_out(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("multiple_layer_animation_view switch out\n");
}

GUI_VIEW_INSTANCE("multiple_layer_animation_view", false, multiple_layer_animation_switch_in,
                  multiple_layer_animation_switch_out);

