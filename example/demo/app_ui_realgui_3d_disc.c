/* 3d disc demo start*/
#include "guidef.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "gui_server.h"
#include "gui_components_init.h"

#include "gui_3d.h"
#include "tp_algo.h"
#include "disc3d/desc.txt"
#include "math.h"

#define CUBE_COUNT 22
#define MAX_ROT_X_ANGLE 65.0f
#define MIN_ROT_X_ANGLE 0.0f
#define STEP_HEIGHT 2.0f

static float rot_x_angle = 0.0f;
static float rot_z_angle = 0.0f;

float shift_z[CUBE_COUNT] = {0}; // cube shift
int step_direction[CUBE_COUNT] = {1};  // cube move direction, 1 rise, -1 fall
int active_cube = 0;

void update_disc_animation()
{
    touch_info_t *tp = tp_get_info();

    if (tp->pressed || tp->pressing)
    {
        rot_z_angle += tp->deltaX / 5.0f;
        rot_x_angle += tp->deltaY / 5.0f;
        rot_x_angle = fmax(MIN_ROT_X_ANGLE, fmin(rot_x_angle, MAX_ROT_X_ANGLE));
    }
    else
    {
        rot_z_angle ++;
    }

    // update all cube states
    for (int i = 0; i < CUBE_COUNT; i++)
    {
        if (step_direction[i] == 1 && shift_z[i] >= STEP_HEIGHT)
        {
            // change direction to fall
            step_direction[i] = -1;
            if (i == active_cube)
            {
                // activate the next cube to rise
                active_cube = (active_cube + 1) % CUBE_COUNT;
                step_direction[active_cube] = 1;
            }
        }

        // Update the current cube height
        shift_z[i] += step_direction[i] * 0.4f;
        if (shift_z[i] < 0.0f)
        {
            shift_z[i] = 0.0f;
        }
    }
}


static void disc_cb(void *this, size_t face_index, gui_3d_world_t *world,
                    gui_3d_camera_t *camera)
{
    gui_dispdev_t *dc = gui_get_dc();
    gui_3d_matrix_t face_matrix;
    gui_3d_matrix_t object_matrix;
    gui_3d_matrix_t local_rotation_matrix;

    gui_3d_camera_UVN_initialize(camera, gui_point_4d(0, 0, 40), gui_point_4d(0, 0, 0), 1, 32767, 90,
                                 dc->screen_width, dc->screen_height);

    gui_3d_world_inititalize(&object_matrix, 0, -5, 90, -65 + rot_x_angle, 0, 0, 5);

    gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 0, 1),
                             rot_z_angle, 1);

    uint8_t cube_index = face_index / 6;
    if (cube_index < CUBE_COUNT)
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, -shift_z[cube_index], gui_3d_point(0, 0, 0),
                                 gui_3d_vector(0, 0, 1),
                                 rot_z_angle, 1);
    }

    *world = gui_3d_matrix_multiply(face_matrix, object_matrix);

}

static int app_init(void)
{
    void *disc_3d = gui_3d_create(gui_obj_get_root(), "3d-widget", (void *)_acdesc, 0, 0, 480, 480);

    gui_3d_set_local_shape_transform_cb(disc_3d, 0, (gui_3d_shape_transform_cb)disc_cb);

    gui_obj_create_timer(&(((gui_3d_base_t *)disc_3d)->base), 1, true, update_disc_animation);
    gui_obj_start_timer(&(((gui_3d_base_t *)disc_3d)->base));


    return 0;

}


GUI_INIT_APP_EXPORT(app_init);


