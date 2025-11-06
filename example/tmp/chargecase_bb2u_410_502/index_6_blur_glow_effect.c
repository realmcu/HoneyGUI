#include "gui_view_macro.h"

static void blur_glow_effect_switch_in(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("blur_glow_effect_view switch in\n");
}

static void blur_glow_effect_switch_out(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("blur_glow_effect_view switch out\n");
}

GUI_VIEW_INSTANCE("blur_glow_effect_view", false, blur_glow_effect_switch_in,
                  blur_glow_effect_switch_out);

