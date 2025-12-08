/* gui lite geometry arc example start */
#include "gui_components_init.h"
#include "gui_lite_geometry_arc.h"

static void create_activity_rings(float center_x, float center_y)
{
    // bg track color
    gui_color_t track_color_move     = gui_rgba(52, 14, 11, 200);
    gui_color_t track_color_exercise = gui_rgba(16, 37, 17, 200);
    gui_color_t track_color_stand    = gui_rgba(17, 32, 52, 200);
    // fg color
    gui_color_t col_move     = gui_rgba(255, 0, 0, 255);  // Move red
    gui_color_t col_exercise = gui_rgba(0, 255, 0, 255);  // Exercise green
    gui_color_t col_stand    = gui_rgba(0, 0, 255, 255);  // Stand blue

    // circle radius and thickness
    float outer_radius = 163.0f; float outer_thickness = 22.0f;
    float mid_radius   = 136.5f; float mid_thickness   = 22.0f;
    float inner_radius = 110.0f; float inner_thickness = 22.0f;

    uint16_t start_angle = 0;
    // background rings
    gui_lite_arc_t *ring_move = gui_lite_arc_create(gui_obj_get_root(), "test_ring_move", center_x,
                                                    center_y, outer_radius, 0, 360, outer_thickness,
                                                    track_color_move);
    gui_lite_arc_t *ring_exercise = gui_lite_arc_create(gui_obj_get_root(), "test_ring_exercise",
                                                        center_x, center_y, mid_radius, 0, 360, mid_thickness,
                                                        track_color_exercise);
    gui_lite_arc_t *ring_stand = gui_lite_arc_create(gui_obj_get_root(), "test_ring_stand", center_x,
                                                     center_y, inner_radius, 0, 360, inner_thickness,
                                                     track_color_stand);
    gui_lite_arc_set_style(ring_stand, center_x, center_y, inner_radius, 0, 360, inner_thickness,
                           track_color_stand);


    // arc style
    gui_lite_arc_t *arc_move = gui_lite_arc_create(gui_obj_get_root(), "test_arc_move", center_x,
                                                   center_y, outer_radius,
                                                   start_angle, start_angle + 252.0f, outer_thickness, col_move);
    gui_lite_arc_t *arc_exercise = gui_lite_arc_create(gui_obj_get_root(), "test_arc_exercise",
                                                       center_x, center_y, mid_radius,
                                                       start_angle, start_angle + 72.0f, mid_thickness, col_exercise);
    gui_lite_arc_t *arc_stand = gui_lite_arc_create(gui_obj_get_root(), "test_arc_stand", center_x,
                                                    center_y, inner_radius,
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
