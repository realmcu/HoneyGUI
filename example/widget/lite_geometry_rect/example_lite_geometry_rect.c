/* gui lite geometry rect example start */
#include "gui_components_init.h"
#include "gui_lite_geometry_round_rect.h"
#include "gui_view.h"
#include "gui_view_instance.h"

#define RECT_VIEW_NAME "rect_round_view"
gui_lite_round_rect_t *round_rect = NULL;
void round_rect_timer_cb(void *arg)
{
    gui_lite_round_rect_set_position(arg, 100, 100);
    gui_fb_change();
}
void rect_click_demo(void)
{
    round_rect = gui_lite_round_rect_create(gui_obj_get_root(), "round_rect", 0, 0, 200, 200, 10,
                                            gui_rgba(255, 0, 0, 255));
    gui_lite_round_rect_set_style(round_rect, 200, 200, 200, 200, 10, gui_rgba(255, 0, 0, 255));
    gui_lite_round_rect_on_click(round_rect, round_rect_timer_cb, NULL);
}
static void app_rect_round_design(gui_view_t *view)
{
    gui_view_switch_on_event(view, "rect_view", SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);
    gui_view_set_animate_step(view, 500);
    uint8_t alpha = 200;
    gui_lite_round_rect_create(gui_obj_get_root(), "bg", 0, 0, 480, 480, 0, gui_rgba(255, 200, 200,
                               255));

    gui_lite_round_rect_create(gui_obj_get_root(), "geometry1", 20, 20, 200, 200, 20, gui_rgba(255, 0,
                               0, alpha));
    gui_lite_round_rect_create(gui_obj_get_root(), "geometry2", 50, 50, 200, 200, 20, gui_rgba(0, 255,
                               0, alpha));
    gui_lite_round_rect_create(gui_obj_get_root(), "geometry3", 80, 80, 200, 200, 20, gui_rgba(0, 0,
                               255, alpha));
    gui_lite_round_rect_create(gui_obj_get_root(), "geometry1", 150, 150, 200, 200, 20, gui_rgba(255, 0,
                               0, alpha));
    gui_lite_round_rect_create(gui_obj_get_root(), "geometry2", 210, 210, 200, 200, 20, gui_rgba(0, 255,
                               0, alpha));

    // rect_click_demo();
}
static int geometry_rect_demo_init(void)
{
    gui_view_create(gui_obj_get_root(), RECT_VIEW_NAME, 0, 0, 0, 0);

    return 0;
}
GUI_VIEW_INSTANCE(RECT_VIEW_NAME, false, app_rect_round_design, 0);
GUI_INIT_APP_EXPORT(geometry_rect_demo_init);
/* gui lite geometry rect example end */
