/* gui lite geometry rect example start */
#include "gui_components_init.h"
#include "gui_lite_geometry_round_rect.h"
#include "gui_view.h"
#include "gui_view_instance.h"
#include "gui_lite_geometry.h"

#define RECT_VIEW_NAME "rect_view"

static void app_rect_design(gui_view_t *view)
{
    gui_obj_t *parent = GUI_BASE(view);
    gui_view_set_animate_step(view, 500);
    gui_view_switch_on_event(view, gui_view_descriptor_get("rect_round_view"),
                             SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);
    gui_lite_round_rect_t *geometry1 = gui_lite_round_rect_create(parent, "bg");
    gui_lite_round_rect_set_style(geometry1, 0, 0, 480, 480, 0, gui_rgba(255, 200, 200, 255));
    gui_lite_round_rect_t *geometry2 = gui_lite_round_rect_create(parent, "geometry2");
    gui_lite_round_rect_set_style(geometry2, 100, 200, 200, 200, 20, gui_rgba(0, 255, 0, 200));
    gui_lite_round_rect_t *geometry3 = gui_lite_round_rect_create(parent, "geometry3");
    gui_lite_round_rect_set_style(geometry3, 150, 250, 200, 200, 20, gui_rgba(0, 0, 255, 200));

    gui_lite_round_rect_t *rect_bg = gui_lite_round_rect_create(parent, "canvas_clear");
    gui_lite_round_rect_set_style(rect_bg, 29, 0, 352, 120, 30, gui_rgba(39, 43, 44, 255));


    // gui_lite_geometry_t *rect_bg = gui_lite_geometry_create(gui_obj_get_root(), "rect_bg",0,0,480,480);
    // gui_lite_geometry_add_rounded_rect(rect_bg, 59, 40, 305, 80, 40, gui_rgba(39, 43, 44, 255));
}
GUI_VIEW_INSTANCE(RECT_VIEW_NAME, false, app_rect_design, 0);
/* gui lite geometry rect example end */
