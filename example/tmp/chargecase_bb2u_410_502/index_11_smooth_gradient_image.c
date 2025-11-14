#include "gui_view_macro.h"
#include "gui_img.h"
static void smooth_gradient_image_switch_in(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("smooth_gradient_image_view switch in\n");
    gui_view_switch_on_event(view, gui_view_descriptor_get("low_latency_view"),
                             SWITCH_OUT_TO_RIGHT_USE_TRANSLATION,
                             SWITCH_IN_FROM_LEFT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_RIGHT);
    gui_view_switch_on_event(view, gui_view_descriptor_get("transparent_gradient_animation_view"),
                             SWITCH_OUT_TO_LEFT_USE_TRANSLATION,
                             SWITCH_IN_FROM_RIGHT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_LEFT);
    gui_img_t *img =
        gui_img_create_from_mem(view,
                                0, (void *)FILE_POINTER(BOTTOM_BG_BIN), 0, 0, 0, 0);
    gui_img_set_mode(img, IMG_SRC_OVER_MODE);
}

static void smooth_gradient_image_switch_out(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("smooth_gradient_image_view switch out\n");
}

GUI_VIEW_INSTANCE("smooth_gradient_image_view", 1, smooth_gradient_image_switch_in,
                  smooth_gradient_image_switch_out);

