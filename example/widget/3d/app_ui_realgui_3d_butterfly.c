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

#include "butterfly3d/desc.txt"
#include "math.h"
#include "tp_algo.h"

static int frame_counter = 0;
static float wing_angle = 0.0f;
static float butterfly_x = 0.0f;
static float butterfly_y = 0.0f;
static float butterfly_z = 0.0f;
static float butterfly_rz = 0.0f;

bool is_moving_to_target = false;
static float target_dx = 0.0f;
static float target_dy = 0.0f;
static float source_dx = 0.0f;
static float source_dy = 0.0f;
static float move_speed = 0.02f;
static float wing_time = 0.0f;
static void update_animation()
{
    touch_info_t *tp = tp_get_info();
    gui_dispdev_t *dc = gui_get_dc();

    if (tp->pressed)
    {
        target_dx = (tp->x - dc->screen_width / 2) / 2.5f;
        target_dy = (tp->y - dc->screen_height / 2) / 2.5f;
        is_moving_to_target = true;
    }

    if (is_moving_to_target)
    {
        float dx = target_dx - source_dx;
        float dy = target_dy - source_dy;

        float distance = sqrtf(dx * dx + dy * dy);

        if (distance > 10.0f)
        {
            // Acceleration and deceleration
            float speed_factor = fminf(distance / 40.0f, 1.0f);
            source_dx += dx * move_speed * speed_factor;
            source_dy += dy * move_speed * speed_factor;

            // Caculate new rotate angle
            float desired_angle = atan2f(dy, dx) * (180.0f / M_PI) + 90;
            float angle_difference = desired_angle - butterfly_rz;

            if (angle_difference > 180.0f)
            {
                angle_difference -= 360.0f;
            }
            if (angle_difference < -180.0f)
            {
                angle_difference += 360.0f;
            }
            butterfly_rz += angle_difference * 0.1f;

            // Adjust wing flapping frequency based on speed
            wing_time += 0.2f + speed_factor * 0.2f;
            wing_angle = 60.0f * sinf(wing_time);

            butterfly_x = -source_dx;
            butterfly_y = -source_dy;
        }
        else
        {
            is_moving_to_target = false;
        }
    }
    else
    {
        frame_counter++;
        wing_time += 0.1f;
        wing_angle = 50.0f * sinf(wing_time);
        butterfly_z = 5.0f * sinf(frame_counter * 0.05f);
    }

}

static void butterfly_global_cb(gui_3d_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 80), gui_point_4d(0, 0, 0), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, butterfly_x, butterfly_y, butterfly_z, 0, 0, butterfly_rz,
                             5);

}

static gui_3d_matrix_t butterfly_face_cb(gui_3d_t *this, size_t face_index/*face offset*/)
{
    gui_3d_matrix_t face_matrix;
    gui_3d_matrix_t transform_matrix;

    if (face_index == 0)
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 1, 0),
                                 wing_angle, 1);
    }
    else if (face_index == 1)
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 1, 0),
                                 -wing_angle, 1);
    }
    else if (face_index == 2)
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 1, 0),
                                 wing_angle, 1);
    }
    else if (face_index == 3)
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 1, 0),
                                 -wing_angle, 1);
    }
    else
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 1, 0), 0,
                                 1);
    }

    transform_matrix = gui_3d_matrix_multiply(face_matrix, this->world);

    return transform_matrix;

}



static int app_init(void)
{
    gui_3d_t *butterfly_3d = gui_3d_create(gui_obj_get_root(), "3d-widget", (void *)_acdesc,
                                           GUI_3D_DRAW_FRONT_ONLY, 0, 0,
                                           480, 480);

    gui_3d_set_global_transform_cb(butterfly_3d, (gui_3d_global_transform_cb)butterfly_global_cb);
    gui_3d_set_face_transform_cb(butterfly_3d, (gui_3d_face_transform_cb)butterfly_face_cb);

    gui_obj_create_timer(&(butterfly_3d->base), 17, true, update_animation);
    gui_obj_start_timer(&(butterfly_3d->base));

    return 0;

}
/* 3d butterfly demo end*/

GUI_INIT_APP_EXPORT(app_init);


