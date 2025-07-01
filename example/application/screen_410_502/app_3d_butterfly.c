/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "root_image_hongkong/ui_resource.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "time.h"
#include "tp_algo.h"
#include <math.h>
#include "cJSON.h"
#include "app_hongkong.h"
#include "gui_view.h"
#include "gui_list.h"
#include "gui_3d.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "butterfly_view"

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void butterfly_app(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;

static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = butterfly_app,
};

/* Animation Variables */
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
static const float move_speed = 0.02f;
static float wing_time = 0.0f;
static float total_flight_distance = 0.0f;
static float flight_progress = 1.0f;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static int gui_view_descriptor_register_init(void)
{
    gui_view_descriptor_register(&descriptor);
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_REGISTER(gui_view_descriptor_register_init);

static int gui_view_get_other_view_descriptor_init(void)
{
    /* you can get other view descriptor point here */
    menu_view = gui_view_descriptor_get("menu_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);


static void update_animation()
{
    touch_info_t *tp = tp_get_info();
    gui_dispdev_t *dc = gui_get_dc();

    if (tp->pressed)
    {
        target_dx = (tp->x - dc->screen_width / 2) / 4.0f;
        target_dy = (tp->y - dc->screen_height / 2) / 4.0f;
        is_moving_to_target = true;

        float dx = target_dx - source_dx;
        float dy = target_dy - source_dy;

        total_flight_distance = sqrtf(dx * dx + dy * dy);
        flight_progress = 0.0f;
    }

    if (is_moving_to_target)
    {
        float current_dx = target_dx - source_dx;
        float current_dy = target_dy - source_dy;

        float remaining_distance = sqrtf(current_dx * current_dx + current_dy * current_dy);
        flight_progress = fminf(1.0f - (remaining_distance / total_flight_distance), 1.0f);

        if (flight_progress < 0.8f)
        {
            // Acceleration and deceleration
            // float speed_factor = fminf(remaining_distance / 40.0f, 1.0f);
            source_dx += current_dx * move_speed * (1.0f - flight_progress);
            source_dy += current_dy * move_speed * (1.0f - flight_progress);

            // Caculate new rotate angle
            float desired_angle = atan2f(current_dy, current_dx) * (180.0f / M_PI_F) + 90;
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
            wing_time += 0.2f + (1.0f - flight_progress) * 0.2f;
            wing_angle = 60.0f * sinf(wing_time);

            butterfly_x = source_dx;
            butterfly_y = source_dy;

            butterfly_z = 30.0f * (4.0f * flight_progress * (0.8f - flight_progress));
        }
        else
        {
            is_moving_to_target = false;
        }
    }
    else
    {
        wing_time += 0.1f;
        wing_angle = 50.0f * sinf(wing_time);
        butterfly_z = -5.0f * sinf(wing_time);
    }

}

static void butterfly_global_cb(gui_3d_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 0), gui_point_4d(0, 0, 40), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, butterfly_x, butterfly_y, 40.0f - butterfly_z, 0, 0,
                             butterfly_rz,
                             5);

}

static gui_3d_matrix_t butterfly_face_cb(gui_3d_t *this, size_t face_index/*face offset*/)
{
    gui_3d_matrix_t face_matrix;
    gui_3d_matrix_t transform_matrix;

    if (face_index == 0 || face_index == 2)
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 1, 0),
                                 wing_angle, 1);
    }
    else if (face_index == 1 || face_index == 3)
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

static void butterfly_app(gui_view_t *view)
{
    gui_obj_t *obj = GUI_BASE(view);
    gui_view_switch_on_event(view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);

    gui_3d_t *butterfly_3d = gui_3d_create(obj, "3d-widget", DESC_BUTTERFLY_BIN,
                                           GUI_3D_DRAW_FRONT_ONLY, 0, 0,
                                           410, 410);

    gui_3d_set_global_transform_cb(butterfly_3d, (gui_3d_global_transform_cb)butterfly_global_cb);
    gui_3d_set_face_transform_cb(butterfly_3d, (gui_3d_face_transform_cb)butterfly_face_cb);

    gui_obj_create_timer(&(butterfly_3d->base), 10, true, update_animation);

}