/**Prism3d
 * @file gui_prism_mirror3d.c
 * @author wenjing_jiang@realsil.com.cn
 * @brief
 * @version 1.0
 * @date 2024-12-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <tp_algo.h>
#include "gui_prism_mirror3d.h"
bool enter_flag = true;
uint8_t enter_face_flags = 1;

float angle_increment = 360.0 / PRISM_FACE_NUMS;

void prism_mirror3d_render_callback(gui_3d_t *parent, size_t face, gui_3d_world_t *world,
                                    gui_3d_camera_t *camera, gui_3d_light_t *light)
{
    // Cube specific render logic
    GUI_ASSERT(parent != NULL);

    gui_dispdev_t *dc = gui_get_dc();
    gui_3d_matrix_t face_matrix;
    gui_3d_matrix_t object_matrix;
    gui_prism_mirror3d_t *prism_mirror3d = (gui_prism_mirror3d_t *)parent;

    gui_3d_camera_UVN_initialize(camera, gui_point_4d(0,
                                                      prism_mirror3d->interpolated_state.cameraPosition.y,
                                                      15), gui_point_4d(0, 0, 0), 0, 0, 90,
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

void prism_mirror3d_update_angle_cb(void *param)
{
    GUI_ASSERT(param != NULL);

    touch_info_t *tp = tp_get_info();
    gui_prism_mirror3d_t *prism_mirror3d = (gui_prism_mirror3d_t *)param;

    static float prism_angle_increase = 0.5f;
    static int8_t last_direction = 1;  // -1 for left, 1 for right, 0 for no movement
    float sensitivity = 0.5f;  // Sensitivity coefficient, less than 1 to reduce sensitivity
    int16_t threshold = 2;     // Threshold, only update the rot_y if greater than this value
    int16_t delta_x = 0;
    if (tp->pressing)
    {
        delta_x = tp->deltaX;

        // Check if movement exceeds the threshold
        if (abs(delta_x) > threshold)
        {
            if (delta_x > 0)
            {
                last_direction = 1;
            }
            else
            {
                last_direction = -1;
            }

            // Apply sensitivity to delta_x
            int16_t smoothed_delta_x = (int16_t)(delta_x * sensitivity);
            // Update wing_angle

            prism_mirror3d->interpolated_state.rot_y += smoothed_delta_x;

            // Keep wing_angle within a valid range
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
#if AUTO_ROTATION
    else
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
#endif
}
void prism_mirror3d_render_animate_cb(void *param)
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
    prism_mirror3d->interpolated_state.cameraPosition.y = prism_mirror3d->raw_state.cameraPosition.y +
                                                          (prism_mirror3d->target_state.cameraPosition.y - prism_mirror3d->raw_state.cameraPosition.y) *
                                                          prism_mirror3d->render_object.animate->progress_percent;


    gui_3d_set_shape_transform_cb((gui_3d_t *)prism_mirror3d, 0,
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
            gui_obj_event_set(GUI_BASE(prism_mirror3d), GUI_EVENT_2);//fack click switch
            enter_face_flags = prism_mirror3d->face_flags;
            enter_flag = true;
        }


    }
}
void gui_prism_mirror3d_set_animate(gui_prism_mirror3d_t *prism_mirror3d)
{
    GUI_ASSERT(prism_mirror3d != NULL);
    gui_3d_set_animate((gui_3d_t *)prism_mirror3d, 10000, -1, prism_mirror3d_update_angle_cb,
                       prism_mirror3d);
}
void gui_prism_mirror3d_set_scale(gui_prism_mirror3d_t *prism_mirror3d, float scale)
{
    GUI_ASSERT(prism_mirror3d != NULL);
    prism_mirror3d->interpolated_state.scale = scale;
    prism_mirror3d->raw_state.scale = scale;
    prism_mirror3d->target_state.scale = scale;
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


    prism_mirror3d->target_state.worldPosition.x = x;
    prism_mirror3d->target_state.worldPosition.y = y;
    prism_mirror3d->target_state.worldPosition.z = z;
}
void gui_prism_mirror3d_set_rotation_angles(gui_prism_mirror3d_t *prism_mirror3d, int16_t x,
                                            int16_t y, int16_t z)
{
    GUI_ASSERT(prism_mirror3d != NULL);
    prism_mirror3d->interpolated_state.rot_x = x;
    prism_mirror3d->interpolated_state.rot_y = y;
    prism_mirror3d->interpolated_state.rot_y = z;

    // Adjust the raw_state and target_state rotation angles to match the new angles
    prism_mirror3d->raw_state.rot_x = x;
    prism_mirror3d->raw_state.rot_y = y;
    prism_mirror3d->raw_state.rot_z = z;

    prism_mirror3d->target_state.rot_x = x;
    prism_mirror3d->target_state.rot_y = y;
    prism_mirror3d->target_state.rot_z = z;
}
void gui_prism_render_interpolated_animate(gui_prism_mirror3d_t *render_object)
{
    GUI_ASSERT(render_object != NULL);
    gui_3d_set_animate((gui_3d_t *)render_object, 800, 0, prism_mirror3d_render_animate_cb,
                       render_object);
}

void gui_prism_mirror3d_update_face_flags(gui_prism_mirror3d_t *prism_mirror3d)
{
    if (prism_mirror3d->face_flags < 0)
    {
        prism_mirror3d->face_flags += 360;
    }

    // Calculate which face is at front using mid-point threshold
    int section_size = 360 / PRISM_FACE_NUMS;
    int half_section = section_size / 2;
    int effective_flag = (prism_mirror3d->face_flags + half_section) % 360 / section_size;

    // Adjust 1-based index if needed
    prism_mirror3d->face_flags = effective_flag + 1;

    // Ensure the flags are within bounds
    if (prism_mirror3d->face_flags < 1 || prism_mirror3d->face_flags > PRISM_FACE_NUMS)
    {
        prism_mirror3d->face_flags = 1; // Assuming default to first face if out of bounds
    }
}

void prism_mirror3d_on_face_click_cb(void *obj, gui_event_t e, void *param)
{
    GUI_ASSERT(obj != NULL);
    gui_prism_mirror3d_t *prism_mirror3d = (gui_prism_mirror3d_t *)obj;

    prism_mirror3d->face_flags = (int32_t)(prism_mirror3d->interpolated_state.rot_y + 0.5f);

    gui_prism_mirror3d_update_face_flags(prism_mirror3d);

    if (prism_mirror3d->face_flags < 0 || prism_mirror3d->face_flags > (PRISM_FACE_NUMS - 1))
    {
        prism_mirror3d->face_flags = 0;
    }
    for (int i = 0; i < PRISM_FACE_NUMS; i++)
    {
        if ((prism_mirror3d->face_flags == 0) && (i == 0))
        {
            prism_mirror3d->interpolated_state.worldPosition.x = 0;
            prism_mirror3d->interpolated_state.rot_y = angle_increment * (PRISM_FACE_NUMS - 1);
            break;
        }
        else if (i == prism_mirror3d->face_flags)
        {
            prism_mirror3d->interpolated_state.worldPosition.x = 0;
            prism_mirror3d->interpolated_state.rot_y = angle_increment * (prism_mirror3d->face_flags - 1);
            break;
        }

    }
    gui_prism_render_interpolated_animate(
        prism_mirror3d);  // Make sure to call this within the switch case
}
void gui_prism_mirror3d_swap_states(gui_prism_mirror3d_t *prism)
{
    gui_prism_transform_state_t temp = prism->raw_state;
    prism->raw_state = prism->target_state;
    prism->target_state = temp;
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
    prism_mirror3d->interpolated_state.rot_y = (360 / PRISM_FACE_NUMS) * (enter_face_flags - 1);
    gui_prism_render_interpolated_animate(prism_mirror3d);
}
uint8_t gui_prism_mirror3d_get_enter_face(void)
{
    return enter_face_flags;
}
gui_prism_mirror3d_t *gui_prism_mirror3d_create(gui_obj_t *parent, const char *name,
                                                void *desc_addr,
                                                uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    GUI_ASSERT(parent != NULL);

    if (name == NULL)
    {
        name = "WIdGET_PRISM_MIRROR_3D";
    }

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

    prism_mirror3d->raw_state = (gui_prism_transform_state_t) {.worldPosition = {0, 30, 60}, .cameraPosition = {0, 4, 10}, .rot_x = 90, .rot_y = 0, .rot_z = 0, .scale = 18};
    prism_mirror3d->target_state = (gui_prism_transform_state_t) {.worldPosition = {0, 22, 60}, .cameraPosition = {0, 0, 10}, .rot_x = 90, .rot_y = 0, .rot_z = 0, .scale = 28};
    prism_mirror3d->interpolated_state = (gui_prism_transform_state_t) {.worldPosition = {0, 0, 0}, .cameraPosition = {0, 0, 0}, .rot_x = 0, .rot_y = 0, .scale = 0};

    prism_mirror3d->face_flags = enter_face_flags;
    gui_3d_set_shape_transform_cb((gui_3d_t *)prism_mirror3d, 0, prism_mirror3d_render_callback);
    gui_3d_on_click((gui_3d_t *)prism_mirror3d, prism_mirror3d_on_face_click_cb, NULL);

    return prism_mirror3d;
}

