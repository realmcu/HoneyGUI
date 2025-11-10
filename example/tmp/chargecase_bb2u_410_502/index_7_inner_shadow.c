#include "gui_view_macro.h"

static void inner_shadow_switch_in(gui_view_t *view)
{
    gui_view_switch_on_event(view, gui_view_descriptor_get("blur_glow_effect_view"),
                             SWITCH_OUT_TO_RIGHT_USE_TRANSLATION,
                             SWITCH_IN_FROM_LEFT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_RIGHT);
    gui_view_switch_on_event(view, gui_view_descriptor_get("radial_gradient_view"),
                             SWITCH_OUT_TO_LEFT_USE_TRANSLATION,
                             SWITCH_IN_FROM_RIGHT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_LEFT);
    GUI_UNUSED(view);
    gui_log("inner_shadow_view switch in\n");
}

static void inner_shadow_switch_out(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("inner_shadow_view switch out\n");
}

GUI_VIEW_INSTANCE("inner_shadow_view", false, inner_shadow_switch_in, inner_shadow_switch_out);

