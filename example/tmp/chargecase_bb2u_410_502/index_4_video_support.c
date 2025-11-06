#include "gui_view_macro.h"

static void video_support_switch_in(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("video_support_view switch in\n");
}

static void video_support_switch_out(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("video_support_view switch out\n");
}

GUI_VIEW_INSTANCE("video_support_view", false, video_support_switch_in, video_support_switch_out);

