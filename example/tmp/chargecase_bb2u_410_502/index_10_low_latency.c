#include "gui_view_macro.h"

static void low_latency_switch_in(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("low_latency_view switch in\n");
}

static void low_latency_switch_out(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("low_latency_view switch out\n");
}

GUI_VIEW_INSTANCE("low_latency_view", false, low_latency_switch_in, low_latency_switch_out);

