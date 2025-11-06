#include "gui_view_macro.h"

static void gif_apng_support_switch_in(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("gif_apng_support_view switch in\n");
}

static void gif_apng_support_switch_out(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("gif_apng_support_view switch out\n");
}

GUI_VIEW_INSTANCE("gif_apng_support_view", false, gif_apng_support_switch_in,
                  gif_apng_support_switch_out);

