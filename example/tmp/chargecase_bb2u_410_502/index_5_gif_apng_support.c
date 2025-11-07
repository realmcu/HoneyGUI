#include "gui_view_macro.h"

static void gif_apng_support_switch_in(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("gif_apng_support_view switch in\n");

    gui_view_switch_on_event(view, gui_view_descriptor_get("blur_glow_effect_view"),
                             SWITCH_OUT_TO_LEFT_USE_TRANSLATION,
                             SWITCH_IN_FROM_RIGHT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_LEFT);

    gui_view_switch_on_event(view, gui_view_descriptor_get("video_support_view"),
                             SWITCH_OUT_TO_RIGHT_USE_TRANSLATION,
                             SWITCH_IN_FROM_LEFT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_RIGHT);
}

static void gif_apng_support_switch_out(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("gif_apng_support_view switch out\n");
}

GUI_VIEW_INSTANCE("gif_apng_support_view", false, gif_apng_support_switch_in,
                  gif_apng_support_switch_out);

