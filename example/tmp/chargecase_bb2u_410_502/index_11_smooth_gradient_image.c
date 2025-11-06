#include "gui_view_macro.h"

static void smooth_gradient_image_switch_in(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("smooth_gradient_image_view switch in\n");
}

static void smooth_gradient_image_switch_out(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("smooth_gradient_image_view switch out\n");
}

GUI_VIEW_INSTANCE("smooth_gradient_image_view", false, smooth_gradient_image_switch_in,
                  smooth_gradient_image_switch_out);

