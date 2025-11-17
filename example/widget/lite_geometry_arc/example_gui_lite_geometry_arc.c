/* gui lite geometry arc example start */
#include "gui_components_init.h"
#include "gui_lite_geometry_arc.h"
gui_lite_arc_t *arc = NULL;
void circle_timer_cb(void *arg)
{
    gui_lite_arc_set_position(arg, 100, 100);
    gui_fb_change();
}

void arc_test_click_demo(void)
{
    arc = gui_lite_arc_create(gui_obj_get_root(), "arc");
    gui_lite_arc_set_style(arc, 200, 200, 100, 0, 90, 20, RGBA_color(255, 0, 0, 255));
    gui_lite_arc_on_click(arc, circle_timer_cb, NULL);
}
static void create_activity_rings(float center_x, float center_y)
{

    // bg track color
    PixelColor track_color_move     = 0xC8340E0B;
    PixelColor track_color_exercise = 0xC8102511;
    PixelColor track_color_stand    = 0xC8112034;
    // fg color
    PixelColor col_move     = RGBA_color(255, 0, 0, 255);  // Move red
    PixelColor col_exercise = RGBA_color(0, 255, 0, 255);  // Exercise green
    PixelColor col_stand    = RGBA_color(0, 0, 255, 255);  // Stand blue

    // circle radius and thickness
    float outer_radius = 163.0f; float outer_thickness = 22.0f;
    float mid_radius   = 136.5f; float mid_thickness   = 22.0f;
    float inner_radius = 110.0f; float inner_thickness = 22.0f;

    uint16_t start_angle = 0;
    // background rings
    gui_lite_arc_t *ring_move = gui_lite_arc_create(gui_obj_get_root(), "test_ring_move");
    gui_lite_arc_set_style(ring_move, center_x, center_y, outer_radius, 0, 360, outer_thickness,
                           track_color_move);
    gui_lite_arc_t *ring_exercise = gui_lite_arc_create(gui_obj_get_root(), "test_ring_exercise");
    gui_lite_arc_set_style(ring_exercise, center_x, center_y, mid_radius, 0, 360, mid_thickness,
                           track_color_exercise);
    gui_lite_arc_t *ring_stand = gui_lite_arc_create(gui_obj_get_root(), "test_ring_stand");
    gui_lite_arc_set_style(ring_stand, center_x, center_y, inner_radius, 0, 360, inner_thickness,
                           track_color_stand);


    // arc style
    gui_lite_arc_t *arc_move = gui_lite_arc_create(gui_obj_get_root(), "test_arc_move");
    gui_lite_arc_set_style(arc_move, center_x, center_y, outer_radius,
                           start_angle, start_angle + 252.0f, outer_thickness, col_move);
    gui_lite_arc_t *arc_exercise = gui_lite_arc_create(gui_obj_get_root(), "test_arc_exercise");
    gui_lite_arc_set_style(arc_exercise, center_x, center_y, mid_radius,
                           start_angle, start_angle + 72.0f, mid_thickness, col_exercise);
    gui_lite_arc_t *arc_stand = gui_lite_arc_create(gui_obj_get_root(), "test_arc_stand");
    gui_lite_arc_set_style(arc_stand, center_x, center_y, inner_radius,
                           start_angle, start_angle + 198.0f, inner_thickness, col_stand);
}
static int geometry_demo_init(void)
{

    float center_x = 480 / 2.0f;
    float center_y = 480 / 2.0f;
    create_activity_rings(center_x, center_y);

    return 0;
}

GUI_INIT_APP_EXPORT(geometry_demo_init);
/* gui lite geometry arc example end */
