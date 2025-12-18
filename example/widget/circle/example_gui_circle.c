/* gui circle example start */
#include "gui_components_init.h"
#include "gui_circle.h"
static void gui_circle_cb(gui_obj_t *obj)
{
    gui_log("gui_circle_cb: %s\n", obj->name);
}
static int geometry_circle_demo_init(void)
{
    const uint32_t W = 480, H = 480;
    const uint32_t big_d   = 368;
    const uint32_t small_d = 66;
    const uint32_t margin  = 36;

    gui_circle_t *big_circle = gui_circle_create(gui_obj_get_root(), "big_circle", W / 2,
                                                 H / 2, big_d / 2, gui_rgba(0, 255,
                                                                            0, 255));

    gui_circle_create(gui_obj_get_root(), "top_left", margin + small_d / 2, margin + small_d / 2,
                      small_d / 2,
                      gui_rgba(255, 0, 0, 255));


    gui_circle_create(gui_obj_get_root(), "top_right", W - margin - small_d / 2,
                      margin + small_d / 2, small_d / 2,
                      gui_rgba(0, 255, 0, 255));


    gui_circle_create(gui_obj_get_root(), "bottom_left", margin + small_d / 2,
                      H - margin - small_d / 2, small_d / 2,
                      gui_rgba(0, 0, 255, 255));


    gui_circle_create(gui_obj_get_root(), "bottom_right", W - margin - small_d / 2,
                      H - margin - small_d / 2, small_d / 2,
                      gui_rgba(0, 0, 255, 150));
    gui_obj_add_event_cb(big_circle, (gui_event_cb_t)gui_circle_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    return 0;
}

GUI_INIT_APP_EXPORT(geometry_circle_demo_init);
/* gui circle example end */
