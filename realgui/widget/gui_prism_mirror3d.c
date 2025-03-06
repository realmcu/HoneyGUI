/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_prism_mirror3d.c
  * @brief Prism mirror 3D widget
  * @details This file contains the interface for creating and managing 3D prism mirror widgets, which can transform and animate within a 3D world.
  * @author wenjing_jiang@realsil.com.cn
  * @date 2024/1/7
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/

#include <tp_algo.h>
#include "gui_prism_mirror3d.h"

/*============================================================================*
 *                           Types
 *============================================================================*/

/*============================================================================*
 *                           Constants
 *============================================================================*/

/*============================================================================*
 *                            Macros
 *============================================================================*/

/*============================================================================*
 *                            Variables
 *============================================================================*/

bool enter_flag = true;
static int16_t enter_face_flags = 1;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/

static void prism_mirror3d_render_callback(gui_3d_t *parent, gui_3d_world_t *world,
                                           gui_3d_camera_t *camera, gui_3d_light_t *light)
{
    // Cube specific render logic
    GUI_ASSERT(parent != NULL);

    gui_dispdev_t *dc = gui_get_dc();
    gui_3d_matrix_t face_matrix;
    gui_3d_matrix_t object_matrix;
    gui_prism_mirror3d_t *prism_mirror3d = (gui_prism_mirror3d_t *)parent;

    gui_3d_camera_UVN_initialize(camera,
                                 gui_point_4d(prism_mirror3d->interpolated_state.cameraPosition.x,
                                              prism_mirror3d->interpolated_state.cameraPosition.y,
                                              prism_mirror3d->interpolated_state.cameraPosition.z), gui_point_4d(0, 0, 0), 0, 0, 90,
                                 dc->screen_width, dc->screen_height);

    gui_3d_world_inititalize(&object_matrix, prism_mirror3d->interpolated_state.worldPosition.x,
                             prism_mirror3d->interpolated_state.worldPosition.y,
                             prism_mirror3d->interpolated_state.worldPosition.z,
                             prism_mirror3d->interpolated_state.rot_x,
                             prism_mirror3d->interpolated_state.rot_y, prism_mirror3d->interpolated_state.rot_z,
                             prism_mirror3d->interpolated_state.scale);

    gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 0, 1),
                             0, 1);

    *world = gui_3d_matrix_multiply(face_matrix, object_matrix);
}

static void prism_mirror3d_update_angle_cb(void *param)
{
    GUI_ASSERT(param != NULL);

    touch_info_t *tp = tp_get_info();
    gui_prism_mirror3d_t *prism_mirror3d = (gui_prism_mirror3d_t *)param;

    static float prism_angle_increase = 0.5f;
    static int8_t last_direction = 1;
    int16_t threshold = 2;
    int16_t delta_x = 0;

    if (tp->pressing)
    {
        delta_x = tp->deltaX;

        if (abs(delta_x) > threshold)
        {
            last_direction = (delta_x > 0) ? 1 : -1;
            int16_t smoothed_delta_x = (int16_t)(delta_x * prism_mirror3d->conf.sensitivity);
            prism_mirror3d->interpolated_state.rot_y += smoothed_delta_x;

            while (prism_mirror3d->interpolated_state.rot_y > 360)
            {
                prism_mirror3d->interpolated_state.rot_y -= 360;
            }
            while (prism_mirror3d->interpolated_state.rot_y < -360)
            {
                prism_mirror3d->interpolated_state.rot_y += 360;
            }
        }
    }
    else if (prism_mirror3d->conf.auto_rotation)
    {
        prism_mirror3d->interpolated_state.rot_y += last_direction * prism_angle_increase;
        while (prism_mirror3d->interpolated_state.rot_y > 360)
        {
            prism_mirror3d->interpolated_state.rot_y -= 360;
        }
        while (prism_mirror3d->interpolated_state.rot_y < -360)
        {
            prism_mirror3d->interpolated_state.rot_y += 360;
        }
    }
}

static void gui_prism_mirror3d_swap_states(gui_prism_mirror3d_t *prism)
{
    gui_prism_transform_state_t temp = prism->raw_state;
    prism->raw_state = prism->target_state;
    prism->target_state = temp;
}

static void gui_prism_mirror3d_update_face_flags(gui_prism_mirror3d_t *prism_mirror3d)
{
    if (prism_mirror3d->face_flags_rotation < 0)
    {
        prism_mirror3d->face_flags_rotation += 360;
    }

    // Calculate which face is at front using mid-point threshold
    int section_size = 360 / prism_mirror3d->conf.face_nums;
    int half_section = section_size / 2;
    int effective_flag = (prism_mirror3d->face_flags_rotation + half_section) % 360 / section_size;

    // Adjust 1-based index if needed
    prism_mirror3d->face_flags_rotation = effective_flag + 1;

    // Ensure the flags are within bounds
    if (prism_mirror3d->face_flags_rotation < 1 ||
        prism_mirror3d->face_flags_rotation > prism_mirror3d->conf.face_nums)
    {
        prism_mirror3d->face_flags_rotation = 1; // Assuming default to first face if out of bounds
    }
}

static void prism_mirror3d_render_animate_cb(void *param)
{
    GUI_ASSERT(param != NULL);
    gui_prism_mirror3d_t *prism_mirror3d = (gui_prism_mirror3d_t *)param;
    prism_mirror3d->interpolated_state.worldPosition.x = prism_mirror3d->raw_state.worldPosition.x +
                                                         (prism_mirror3d->target_state.worldPosition.x -
                                                          prism_mirror3d->raw_state.worldPosition.x) *
                                                         prism_mirror3d->render_object.animate->progress_percent;
    prism_mirror3d->interpolated_state.worldPosition.y = prism_mirror3d->raw_state.worldPosition.y +
                                                         (prism_mirror3d->target_state.worldPosition.y -
                                                          prism_mirror3d->raw_state.worldPosition.y) *
                                                         prism_mirror3d->render_object.animate->progress_percent;
    prism_mirror3d->interpolated_state.worldPosition.z = prism_mirror3d->raw_state.worldPosition.z +
                                                         (prism_mirror3d->target_state.worldPosition.z -
                                                          prism_mirror3d->raw_state.worldPosition.z) *
                                                         prism_mirror3d->render_object.animate->progress_percent;
    prism_mirror3d->interpolated_state.rot_x = prism_mirror3d->raw_state.rot_x +
                                               (prism_mirror3d->target_state.rot_x -
                                                prism_mirror3d->raw_state.rot_x) * prism_mirror3d->render_object.animate->progress_percent;
    // prism_mirror3d->interpolated_state.rot_y = prism_mirror3d->raw_state.rot_y + (prism_mirror3d->target_state.rot_y -
    //                                                                 prism_mirror3d->raw_state.rot_y) * prism_mirror3d->render_object.animate->progress_percent;
    prism_mirror3d->interpolated_state.rot_z = prism_mirror3d->raw_state.rot_z +
                                               (prism_mirror3d->target_state.rot_z -
                                                prism_mirror3d->raw_state.rot_z) * prism_mirror3d->render_object.animate->progress_percent;
    prism_mirror3d->interpolated_state.scale = prism_mirror3d->raw_state.scale +
                                               (prism_mirror3d->target_state.scale -
                                                prism_mirror3d->raw_state.scale) * prism_mirror3d->render_object.animate->progress_percent;
    prism_mirror3d->interpolated_state.cameraPosition.x = prism_mirror3d->raw_state.cameraPosition.x +
                                                          (prism_mirror3d->target_state.cameraPosition.x - prism_mirror3d->raw_state.cameraPosition.x) *
                                                          prism_mirror3d->render_object.animate->progress_percent;
    prism_mirror3d->interpolated_state.cameraPosition.y = prism_mirror3d->raw_state.cameraPosition.y +
                                                          (prism_mirror3d->target_state.cameraPosition.y - prism_mirror3d->raw_state.cameraPosition.y) *
                                                          prism_mirror3d->render_object.animate->progress_percent;
    prism_mirror3d->interpolated_state.cameraPosition.z = prism_mirror3d->raw_state.cameraPosition.z +
                                                          (prism_mirror3d->target_state.cameraPosition.z - prism_mirror3d->raw_state.cameraPosition.z) *
                                                          prism_mirror3d->render_object.animate->progress_percent;


    gui_3d_set_global_shape_transform_cb((gui_3d_t *)prism_mirror3d,
                                         prism_mirror3d_render_callback); // Set to the final callback

    if (prism_mirror3d->render_object.animate->progress_percent == 1)
    {

        if (enter_flag)
        {
            gui_prism_mirror3d_swap_states(prism_mirror3d);
            gui_prism_mirror3d_set_animate(prism_mirror3d);
            enter_flag = false;
        }
        else
        {
            gui_obj_enable_event(GUI_BASE(prism_mirror3d), GUI_EVENT_2);//fack click switch
            enter_face_flags = prism_mirror3d->face_flags_rotation;
            enter_flag = true;
        }


    }
}

static void gui_prism_render_interpolated_animate(gui_prism_mirror3d_t *render_object)
{
    GUI_ASSERT(render_object != NULL);
    gui_3d_set_animate((gui_3d_t *)render_object, 800, 0, prism_mirror3d_render_animate_cb,
                       render_object);
}

static void prism_mirror3d_on_face_click_cb(void *obj, gui_event_t e, void *param)
{
    GUI_ASSERT(obj != NULL);
    gui_prism_mirror3d_t *prism_mirror3d = (gui_prism_mirror3d_t *)obj;

    prism_mirror3d->face_flags_rotation = (int32_t)(prism_mirror3d->interpolated_state.rot_y + 0.5f);

    gui_prism_mirror3d_update_face_flags(prism_mirror3d);
    // gui_log("face: %d\n",prism_mirror3d->face_flags_rotation);

    if (prism_mirror3d->face_flags_rotation < 0 ||
        prism_mirror3d->face_flags_rotation > (prism_mirror3d->conf.face_nums - 1))
    {
        prism_mirror3d->face_flags_rotation = 0;
    }
    for (int i = 0; i < prism_mirror3d->conf.face_nums; i++)
    {
        if ((prism_mirror3d->face_flags_rotation == 0) && (i == 0))
        {
            prism_mirror3d->interpolated_state.worldPosition.x = 0;
            prism_mirror3d->interpolated_state.rot_y = (360.0 / prism_mirror3d->conf.face_nums) *
                                                       (prism_mirror3d->conf.face_nums - 1);
            gui_log("%f, %d", prism_mirror3d->interpolated_state.rot_y, prism_mirror3d->face_flags_rotation);
            break;
        }
        else if (i == prism_mirror3d->face_flags_rotation)
        {
            prism_mirror3d->interpolated_state.worldPosition.x = 0;
            prism_mirror3d->interpolated_state.rot_y = (360.0 / prism_mirror3d->conf.face_nums) *
                                                       (prism_mirror3d->face_flags_rotation -
                                                        1);
            gui_log("%f, %d", prism_mirror3d->interpolated_state.rot_y, prism_mirror3d->face_flags_rotation);
            break;
        }

    }
    gui_prism_render_interpolated_animate(
        prism_mirror3d);  // Make sure to call this within the switch case
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void gui_prism_mirror3d_set_animate(gui_prism_mirror3d_t *prism_mirror3d)
{
    GUI_ASSERT(prism_mirror3d != NULL);
    gui_3d_set_animate((gui_3d_t *)prism_mirror3d, 10000, -1, prism_mirror3d_update_angle_cb,
                       prism_mirror3d);
}
void gui_prism_mirror3d_set_scale(gui_prism_mirror3d_t *prism_mirror3d, float scale)
{
    GUI_ASSERT(prism_mirror3d != NULL);
    float scale_temp = scale * prism_mirror3d->raw_state.scale;
    prism_mirror3d->interpolated_state.scale = scale_temp;
    prism_mirror3d->raw_state.scale = scale_temp;
}
void gui_prism_mirror3d_set_position(gui_prism_mirror3d_t *prism_mirror3d, int16_t x, int16_t y,
                                     int16_t z)
{
    GUI_ASSERT(prism_mirror3d != NULL);
    prism_mirror3d->interpolated_state.worldPosition.x = x;
    prism_mirror3d->interpolated_state.worldPosition.y = y;
    prism_mirror3d->interpolated_state.worldPosition.z = z;

    prism_mirror3d->raw_state.worldPosition.x = x;
    prism_mirror3d->raw_state.worldPosition.y = y;
    prism_mirror3d->raw_state.worldPosition.z = z;

}
void gui_prism_mirror3d_set_rotation_angles(gui_prism_mirror3d_t *prism_mirror3d, int16_t x,
                                            int16_t y, int16_t z)
{
    GUI_ASSERT(prism_mirror3d != NULL);
    prism_mirror3d->interpolated_state.rot_x = x;
    prism_mirror3d->interpolated_state.rot_y = y;
    prism_mirror3d->interpolated_state.rot_z = z;

    // Adjust the raw_state and target_state rotation angles to match the new angles
    prism_mirror3d->raw_state.rot_x = x;
    prism_mirror3d->raw_state.rot_y = y;
    prism_mirror3d->raw_state.rot_z = z;

}
void gui_prism_mirror3d_click_switch_app_add_event(gui_prism_mirror3d_t *prism_mirror3d,
                                                   gui_event_cb_t callback)
{
    gui_obj_add_event_cb((gui_obj_t *)prism_mirror3d, callback, GUI_EVENT_2,
                         (gui_prism_mirror3d_t *)prism_mirror3d);
}
void gui_prism_mirror3d_enter_animate(gui_prism_mirror3d_t *prism_mirror3d)
{
    gui_prism_mirror3d_swap_states(prism_mirror3d);
    prism_mirror3d->interpolated_state.rot_y = (360 / prism_mirror3d->conf.face_nums) *
                                               (enter_face_flags - 1);
    gui_prism_render_interpolated_animate(prism_mirror3d);
}
int16_t *gui_prism_mirror3d_get_enter_face(void)
{
    return &enter_face_flags;
}

void gui_prism_mirror3d_set_raw_state(gui_prism_mirror3d_t *prism_mirror3d, float world_position[3],
                                      float camera_position[3], float rot_x, float rot_y, float rot_z, float scale)
{
    if (prism_mirror3d == NULL) { return; }

    prism_mirror3d->raw_state.worldPosition.x = world_position[0];
    prism_mirror3d->raw_state.worldPosition.y = world_position[1];
    prism_mirror3d->raw_state.worldPosition.z = world_position[2];

    prism_mirror3d->raw_state.cameraPosition.x = camera_position[0];
    prism_mirror3d->raw_state.cameraPosition.y = camera_position[1];
    prism_mirror3d->raw_state.cameraPosition.z = camera_position[2];

    prism_mirror3d->raw_state.rot_x = rot_x;
    prism_mirror3d->raw_state.rot_y = rot_y;
    prism_mirror3d->raw_state.rot_z = rot_z;
    prism_mirror3d->raw_state.scale = scale;
}

void gui_prism_mirror3d_set_target_state(gui_prism_mirror3d_t *prism_mirror3d,
                                         float world_position[3],
                                         float camera_position[3], float rot_x, float rot_y, float rot_z, float scale)
{
    if (prism_mirror3d == NULL) { return; }

    prism_mirror3d->target_state.worldPosition.x = world_position[0];
    prism_mirror3d->target_state.worldPosition.y = world_position[1];
    prism_mirror3d->target_state.worldPosition.z = world_position[2];

    prism_mirror3d->target_state.cameraPosition.x = camera_position[0];
    prism_mirror3d->target_state.cameraPosition.y = camera_position[1];
    prism_mirror3d->target_state.cameraPosition.z = camera_position[2];

    prism_mirror3d->target_state.rot_x = rot_x;
    prism_mirror3d->target_state.rot_y = rot_y;
    prism_mirror3d->target_state.rot_z = rot_z;
    prism_mirror3d->target_state.scale = scale;
}
gui_prism_mirror3d_t *gui_prism_mirror3d_create(gui_obj_t *parent, const char *name,
                                                void *desc_addr, uint16_t x, uint16_t y,
                                                uint16_t w, uint16_t h,
                                                prism_mirror3d_config_t *config)
{
    GUI_ASSERT(parent != NULL);

    gui_prism_mirror3d_t *prism_mirror3d = gui_malloc(sizeof(gui_prism_mirror3d_t));
    GUI_ASSERT(prism_mirror3d != NULL);

    memset(prism_mirror3d, 0x00, sizeof(gui_prism_mirror3d_t));
    gui_3d_ctor((void *)prism_mirror3d, (gui_obj_t *)parent, name, desc_addr, x, y, w, h);
    gui_list_init(&(GET_BASE(prism_mirror3d)->child_list));

    if ((GET_BASE(prism_mirror3d)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(prism_mirror3d)->parent)->child_list),
                               &(GET_BASE(prism_mirror3d)->brother_list));
    }

    GET_BASE(prism_mirror3d)->create_done = true;

    if (config != NULL)
    {
        prism_mirror3d->conf = *config;
    }
    else
    {
        prism_mirror3d->conf.face_nums = 6;
        prism_mirror3d->conf.auto_rotation = true;
        prism_mirror3d->conf.sensitivity = 0.05f;
    }

    prism_mirror3d->raw_state = (gui_prism_transform_state_t) {.worldPosition = {0, 10, 100}, .cameraPosition = {0, 3, 60}, .rot_x = 0, .rot_y = 0, .rot_z = 0, .scale = 19};
    prism_mirror3d->target_state = (gui_prism_transform_state_t) {.worldPosition = {0, 11, 109}, .cameraPosition = {0, 0, 80}, .rot_x = 0, .rot_y = 0, .rot_z = 0, .scale = 19};
    prism_mirror3d->interpolated_state = (gui_prism_transform_state_t) {.worldPosition = {0, 0, 0}, .cameraPosition = {0, 0, 0}, .rot_x = 0, .rot_y = 0, .scale = 0};

    prism_mirror3d->face_flags_rotation = enter_face_flags;
    gui_3d_set_global_shape_transform_cb((gui_3d_t *)prism_mirror3d, prism_mirror3d_render_callback);
    gui_3d_on_click((gui_3d_t *)prism_mirror3d, prism_mirror3d_on_face_click_cb, NULL);

    return prism_mirror3d;
}

