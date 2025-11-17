/* gui lite geometry rect example start */
#include "gui_components_init.h"
#include "gui_lite_geometry_round_rect.h"

gui_lite_round_rect_t *round_rect = NULL;
void round_rect_timer_cb(void *arg)
{
    gui_lite_round_rect_set_position(arg, 100, 100);
    gui_fb_change();
}
void rect_click_demo(void)
{
    round_rect = gui_lite_round_rect_create(gui_obj_get_root(), "round_rect");
    gui_lite_round_rect_set_style(round_rect, 200, 200, 200, 200, 10, RGBA_color(255, 0, 0, 255));
    gui_lite_round_rect_on_click(round_rect, round_rect_timer_cb, NULL);
}
static int geometry_rect_demo_init(void)
{

    gui_lite_round_rect_t *geometry_rect = gui_lite_round_rect_create(gui_obj_get_root(), "geometry1");
    gui_lite_round_rect_set_style(geometry_rect, 20, 20, 200, 200, 0, RGBA_color(255, 0, 0, 255));
    gui_lite_round_rect_t *geometry_rect1 = gui_lite_round_rect_create(gui_obj_get_root(), "geometry2");
    gui_lite_round_rect_set_style(geometry_rect1, 50, 50, 200, 200, 0, RGBA_color(0, 255, 0, 255));
    gui_lite_round_rect_t *geometry_rect2 = gui_lite_round_rect_create(gui_obj_get_root(), "geometry3");
    gui_lite_round_rect_set_style(geometry_rect2, 80, 80, 200, 200, 0, RGBA_color(0, 0, 255, 255));


    gui_lite_round_rect_t *geometry1 = gui_lite_round_rect_create(gui_obj_get_root(), "geometry1");
    gui_lite_round_rect_set_style(geometry1, 150, 150, 200, 200, 20, RGBA_color(255, 0, 0, 200));
    gui_lite_round_rect_t *geometry2 = gui_lite_round_rect_create(gui_obj_get_root(), "geometry2");
    gui_lite_round_rect_set_style(geometry2, 200, 200, 200, 200, 20, RGBA_color(0, 255, 0, 200));
    gui_lite_round_rect_t *geometry3 = gui_lite_round_rect_create(gui_obj_get_root(), "geometry3");
    gui_lite_round_rect_set_style(geometry3, 250, 250, 200, 200, 20, RGBA_color(0, 0, 255, 200));
    // rect_click_demo();
    return 0;
}

GUI_INIT_APP_EXPORT(geometry_rect_demo_init);
/* gui lite geometry rect example end */
