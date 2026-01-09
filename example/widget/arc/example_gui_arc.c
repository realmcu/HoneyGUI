/* gui arc example start */
#include "guidef.h"
#include "gui_components_init.h"
#include "gui_arc.h"
#include "gui_arc_group.h"
#include "gui_rect.h"
#include "gui_circle.h"

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
    gui_arc_t *ring_move = gui_arc_create(gui_obj_get_root(), "test_ring_move", center_x,
                                          center_y, outer_radius, 0, 360, outer_thickness,
                                          track_color_move);
    gui_arc_t *ring_exercise = gui_arc_create(gui_obj_get_root(), "test_ring_exercise",
                                              center_x, center_y, mid_radius, 0, 360, mid_thickness,
                                              track_color_exercise);
    gui_arc_t *ring_stand = gui_arc_create(gui_obj_get_root(), "test_ring_stand", center_x,
                                           center_y, inner_radius, 0, 360, inner_thickness,
                                           track_color_stand);

    GUI_UNUSED(ring_move);
    GUI_UNUSED(ring_exercise);
    GUI_UNUSED(ring_stand);
    // arc style
    gui_arc_t *arc_move = gui_arc_create(gui_obj_get_root(), "test_arc_move", center_x,
                                         center_y, outer_radius,
                                         start_angle, start_angle + 252.0f, outer_thickness, col_move);
    gui_arc_t *arc_exercise = gui_arc_create(gui_obj_get_root(), "test_arc_exercise",
                                             center_x, center_y, mid_radius,
                                             start_angle, start_angle + 72.0f, mid_thickness, col_exercise);
    gui_arc_t *arc_stand = gui_arc_create(gui_obj_get_root(), "test_arc_stand", center_x,
                                          center_y, inner_radius,
                                          start_angle, start_angle + 198.0f, inner_thickness, col_stand);
    GUI_UNUSED(arc_move);
    GUI_UNUSED(arc_exercise);
    GUI_UNUSED(arc_stand);
}
void example_arc_gradient(void *parent)
{
    // Example 1: Simple two-color angular gradient (red to blue)
    // NOTE: Arc is 0-360 (full circle), but gradient is 0-361 (exceeds 360)
    // This allows the end cap to show gradient color for smooth transition
    gui_arc_t *arc1 = gui_arc_create(parent, "arc_gradient_1",
                                     120, 120, 80,    // center x, y, radius
                                     270, 270,          // start angle, end angle (FULL CIRCLE)
                                     20,              // line width
                                     gui_rgba(255, 0, 0, 255)); // fallback color (red)

    // Set angular gradient with span > 360 for end cap gradient
    gui_arc_set_angular_gradient(arc1, 270, 270);
    gui_arc_add_gradient_stop(arc1, 0.0f, gui_rgba(255, 0, 0, 255));     // Red
    gui_arc_add_gradient_stop(arc1, 1.0f, gui_rgba(0, 0, 255, 255));     // Blue


    // Example 2: Multi-color rainbow gradient (full circle, seamless loop)
    gui_arc_t *arc2 = gui_arc_create(parent, "arc_gradient_2",
                                     320, 120, 80,
                                     0, 360,
                                     20,
                                     gui_rgba(255, 0, 0, 255)); // fallback color (red)

    gui_arc_set_angular_gradient(arc2, 0, 360);

    // Redistribute color stops with 7 intervals, step ~0.142
    // Key point: 0.0f and 1.0f must be the same color (Red) for seamless loop

    // 1. Red (start)
    gui_arc_add_gradient_stop(arc2, 0.00f, gui_rgba(255, 0, 0, 255));

    // 2. Orange
    gui_arc_add_gradient_stop(arc2, 0.14f, gui_rgba(255, 127, 0, 255));

    // 3. Yellow
    gui_arc_add_gradient_stop(arc2, 0.28f, gui_rgba(255, 255, 0, 255));

    // 4. Green
    gui_arc_add_gradient_stop(arc2, 0.42f, gui_rgba(0, 255, 0, 255));

    // 5. Blue
    gui_arc_add_gradient_stop(arc2, 0.57f, gui_rgba(0, 0, 255, 255));

    // 6. Indigo
    gui_arc_add_gradient_stop(arc2, 0.71f, gui_rgba(75, 0, 130, 255));

    // 7. Violet - not at 1.0f to leave space for transition back to red
    gui_arc_add_gradient_stop(arc2, 0.85f, gui_rgba(148, 0, 211, 255));

    // 8. Red (end) - must return to red to eliminate seam
    gui_arc_add_gradient_stop(arc2, 1.00f, gui_rgba(255, 0, 0, 255));



    // Example 3: Gradient with transparency (fade effect)
    gui_arc_t *arc3 = gui_arc_create(parent, "arc_gradient_3",
                                     120, 320, 80,
                                     45, 315,
                                     25,
                                     gui_rgba(0, 255, 0, 255)); // fallback color (green)

    gui_arc_set_angular_gradient(arc3, 45, 315);
    gui_arc_add_gradient_stop(arc3, 0.0f, gui_rgba(0, 255, 0, 255));     // Opaque green
    gui_arc_add_gradient_stop(arc3, 0.5f, gui_rgba(0, 255, 255, 255));   // Cyan
    gui_arc_add_gradient_stop(arc3, 1.0f, gui_rgba(0, 255, 0, 51));      // Transparent green (alpha=51)


    // Example 4: Progress bar style gradient
    gui_arc_t *arc4 = gui_arc_create(parent, "arc_gradient_4",
                                     320, 320, 80,
                                     -90, 180,        // Start from top
                                     30,
                                     gui_rgba(0, 255, 0, 255)); // fallback color (green)

    gui_arc_set_angular_gradient(arc4, -90, 180);
    gui_arc_add_gradient_stop(arc4, 0.0f, gui_rgba(255, 0, 0, 255));     // Red (start)
    gui_arc_add_gradient_stop(arc4, 0.5f, gui_rgba(255, 255, 0, 255));   // Yellow (middle)
    gui_arc_add_gradient_stop(arc4, 1.0f, gui_rgba(0, 255, 0, 255));     // Green (end)
}

void example_arc_group(void *parent)
{
    // Example: Arc Group for batch rendering multiple static arcs
    // This is useful for background rings that don't change frequently
    // Performance benefit: Reduces DMA/GPU transfer calls by combining multiple arcs into one buffer

    float center_x = 240.0f;
    float center_y = 240.0f;

    // Calculate bounding box for the arc group
    float outer_radius = 163.0f;
    float outer_thickness = 22.0f;
    int box_size = (int)(outer_radius + outer_thickness / 2 + 2) * 2 + 4;
    int box_x = (int)center_x - box_size / 2;
    int box_y = (int)center_y - box_size / 2;

    // Create arc group for background rings
    gui_arc_group_t *bg_group = gui_arc_group_create(parent, "bg_rings",
                                                     box_x, box_y, box_size, box_size);

    float group_cx = box_size / 2.0f, group_cy = group_cx;

    // Add first arc (outer ring) with gradient
    int arc0 = gui_arc_group_add_arc(bg_group, group_cx, group_cy,
                                     163.0f, 0, 360, 22.0f, gui_rgba(50, 50, 50, 255));
    gui_arc_group_set_gradient(bg_group, arc0, 0, 360);
    gui_arc_group_add_gradient_stop(bg_group, arc0, 0.0f, gui_rgba(60, 0, 0, 100));
    gui_arc_group_add_gradient_stop(bg_group, arc0, 0.5f, gui_rgba(80, 20, 20, 100));
    gui_arc_group_add_gradient_stop(bg_group, arc0, 1.0f, gui_rgba(60, 0, 0, 100));

    // Add second arc (middle ring) with gradient
    int arc1 = gui_arc_group_add_arc(bg_group, group_cx, group_cy,
                                     136.5f, 0, 360, 22.0f, gui_rgba(50, 50, 50, 255));
    gui_arc_group_set_gradient(bg_group, arc1, 0, 360);
    gui_arc_group_add_gradient_stop(bg_group, arc1, 0.0f, gui_rgba(0, 60, 0, 100));
    gui_arc_group_add_gradient_stop(bg_group, arc1, 0.5f, gui_rgba(20, 80, 20, 100));
    gui_arc_group_add_gradient_stop(bg_group, arc1, 1.0f, gui_rgba(0, 60, 0, 100));

    // Add third arc (inner ring) with gradient
    int arc2 = gui_arc_group_add_arc(bg_group, group_cx, group_cy,
                                     110.0f, 0, 360, 22.0f, gui_rgba(50, 50, 50, 255));
    gui_arc_group_set_gradient(bg_group, arc2, 0, 360);
    gui_arc_group_add_gradient_stop(bg_group, arc2, 0.0f, gui_rgba(0, 20, 60, 100));
    gui_arc_group_add_gradient_stop(bg_group, arc2, 0.5f, gui_rgba(20, 40, 80, 100));
    gui_arc_group_add_gradient_stop(bg_group, arc2, 1.0f, gui_rgba(0, 20, 60, 100));

    // Create foreground arcs separately (these can change dynamically)
    gui_arc_t *arc_move = gui_arc_create(parent, "fg_arc_move", center_x, center_y,
                                         163.0f, 0, 252.0f, 22.0f, gui_rgba(255, 0, 0, 255));
    gui_arc_set_angular_gradient(arc_move, 0, 252.0f);
    gui_arc_add_gradient_stop(arc_move, 0.0f, gui_rgba(250, 17, 79, 255));
    gui_arc_add_gradient_stop(arc_move, 1.0f, gui_rgba(255, 100, 150, 255));

    gui_arc_t *arc_exercise = gui_arc_create(parent, "fg_arc_exercise", center_x, center_y,
                                             136.5f, 0, 72.0f, 22.0f, gui_rgba(0, 255, 0, 255));
    gui_arc_set_angular_gradient(arc_exercise, 0, 72.0f);
    gui_arc_add_gradient_stop(arc_exercise, 0.0f, gui_rgba(164, 255, 4, 255));
    gui_arc_add_gradient_stop(arc_exercise, 1.0f, gui_rgba(255, 255, 0, 255));

    gui_arc_t *arc_stand = gui_arc_create(parent, "fg_arc_stand", center_x, center_y,
                                          110.0f, 0, 198.0f, 22.0f, gui_rgba(0, 0, 255, 255));
    gui_arc_set_angular_gradient(arc_stand, 0, 198.0f);
    gui_arc_add_gradient_stop(arc_stand, 0.0f, gui_rgba(0, 255, 255, 255));
    gui_arc_add_gradient_stop(arc_stand, 1.0f, gui_rgba(0, 150, 255, 255));

    GUI_UNUSED(bg_group);
    GUI_UNUSED(arc_move);
    GUI_UNUSED(arc_exercise);
    GUI_UNUSED(arc_stand);
}

static int geometry_demo_init(void)
{

    // float center_x = 480 / 2.0f;
    // float center_y = 480 / 2.0f;
    // create_activity_rings(center_x, center_y);
    // example_arc_gradient(gui_obj_get_root());
    example_arc_group(gui_obj_get_root());

    return 0;
}

GUI_INIT_APP_EXPORT(geometry_demo_init);
/* gui arc example end */
