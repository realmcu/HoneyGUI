/* gui lite geometry circle example start */
#include "gui_components_init.h"
#include "gui_lite_geometry_circle.h"


gui_lite_circle_t *circle = NULL;
void circle_timer_cb(void *arg)
{
    gui_lite_circle_set_position(arg, 100, 100);
    gui_fb_change();
}
void circle_click_demo(void)
{
    circle = gui_lite_circle_create(gui_obj_get_root(), "circle");
    gui_lite_circle_set_style(circle, 200, 200, 100, RGBA_color(255, 0, 0, 255));
    gui_lite_circle_on_click(circle, circle_timer_cb, NULL);
}
static int geometry_circle_demo_init(void)
{
    const uint32_t W = 480, H = 480;
    const uint32_t big_d   = 368;
    const uint32_t small_d = 66;
    const uint32_t margin  = 36;

    gui_lite_circle_t *big_circle = gui_lite_circle_create(gui_obj_get_root(), "big_circle");
    gui_lite_circle_set_style(big_circle, W / 2, H / 2, big_d / 2, RGBA_color(0, 255, 0, 255));

    gui_lite_circle_t *top_left = gui_lite_circle_create(gui_obj_get_root(), "top_left");
    gui_lite_circle_set_style(top_left, margin + small_d / 2, margin + small_d / 2, small_d / 2,
                              RGBA_color(255, 0, 0, 255));


    gui_lite_circle_t *top_right = gui_lite_circle_create(gui_obj_get_root(), "top_right");
    gui_lite_circle_set_style(top_right, W - margin - small_d / 2, margin + small_d / 2, small_d / 2,
                              RGBA_color(0, 255, 0, 255));


    gui_lite_circle_t *bottom_left = gui_lite_circle_create(gui_obj_get_root(), "bottom_left");
    gui_lite_circle_set_style(bottom_left, margin + small_d / 2, H - margin - small_d / 2, small_d / 2,
                              RGBA_color(0, 0, 255, 255));


    gui_lite_circle_t *bottom_right = gui_lite_circle_create(gui_obj_get_root(), "bottom_right");
    gui_lite_circle_set_style(bottom_right, W - margin - small_d / 2, H - margin - small_d / 2,
                              small_d / 2, RGBA_color(0, 0, 255, 150));
    return 0;
}

GUI_INIT_APP_EXPORT(geometry_circle_demo_init);
/* gui lite geometry circle example end */
