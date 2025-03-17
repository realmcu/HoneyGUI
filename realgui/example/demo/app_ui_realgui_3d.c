/* 3d butterfly demo start*/
#include "guidef.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "gui_server.h"
#include "gui_components_init.h"
#include "gui_canvas.h"
#include "gui_3d.h"

#include "butterfly/desc.txt"
#include "math.h"

static int frame_counter = 0;
static float wing_angle = 0.0f;
static float butterfly_x = 0.0f;
static float butterfly_y = 0.0f;
static float butterfly_z = 0.0f;
static float butterfly_rz = 0.0f;

void update_animation()
{
    frame_counter++;
    wing_angle = 50.0f * sinf(frame_counter * 0.1f);

    float radius = 20.0f;
    float theta = frame_counter * 0.01f;

    butterfly_x = radius * cosf(theta);
    butterfly_y = radius * sinf(theta);

    butterfly_z = 10.0f * sinf(frame_counter * 0.05f);

    butterfly_rz = theta * (180.0f / M_PI);
}

static void cb(void *this, size_t face/*face offset*/, gui_3d_world_t *world,
               gui_3d_camera_t *camera, gui_3d_light_t *light)
{
    gui_dispdev_t *dc = gui_get_dc();
    gui_3d_matrix_t face_matrix;
    gui_3d_matrix_t object_matrix;

    gui_3d_camera_UVN_initialize(camera, gui_point_4d(0, 0, 50), gui_point_4d(0, 0, 0), 1, 32767, 90,
                                 dc->screen_width, dc->screen_height);

    gui_3d_world_inititalize(&object_matrix, butterfly_x, butterfly_y, butterfly_z, 0, 0,
                             butterfly_rz + 90,
                             5);


    if (face == 0)
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 1, 0),
                                 wing_angle, 1);
    }
    else if (face == 1)
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 1, 0),
                                 -wing_angle, 1);
    }
    else if (face == 2)
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 1, 0),
                                 wing_angle, 1);
    }
    else if (face == 3)
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 1, 0),
                                 -wing_angle, 1);
    }
    else
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 1, 0), 0,
                                 1);
    }

    *world = gui_3d_matrix_multiply(face_matrix, object_matrix);

}



static int app_init(void)
{
    void *test_3d = gui_3d_create(gui_obj_get_root(), "3d-widget", (void *)_acdesc, 0, 0, 480, 480);

    gui_3d_set_local_shape_transform_cb(test_3d, 0, (gui_3d_shape_transform_cb)cb);

    gui_obj_set_timer(&(((gui_3d_base_t *)test_3d)->base), 1000, true, update_animation);

    return 0;

}
/* 3d butterfly demo end*/

GUI_INIT_APP_EXPORT(app_init);


