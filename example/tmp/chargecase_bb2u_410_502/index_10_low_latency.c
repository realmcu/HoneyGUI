#include "gui_view_macro.h"
#include "gui_lite3d.h"
#include <math.h>
#include "root_image/desc_face.txt"

/* Animation Variables */
#define ANIMATION_SPEED 1.0f
#define MIN_ANGLE_DIFFERENCE 0.1f

static float delta_angle = -90.0f;
static float target_rot_angle = 0.0f;
static float current_rot_angle = 0.0f;

static void update_face_animation(void *param)
{
    (void)param;

    float diff = target_rot_angle - current_rot_angle;

    if (fabsf(diff) < MIN_ANGLE_DIFFERENCE)
    {
        current_rot_angle = target_rot_angle;
    }
    else
    {
        if (diff > 0)
        {
            current_rot_angle += ANIMATION_SPEED;
        }
        else
        {
            current_rot_angle -= ANIMATION_SPEED;
        }
    }

}

static void face_global_cb(l3_model_base_t *this)
{
    l3_camera_UVN_initialize(&this->camera, l3_4d_point(0, 0, 0), l3_4d_point(0, 0, 65), 1,
                             32767,
                             90, this->viewPortWidth, this->viewPortHeight);

    l3_world_initialize(&this->world, 0, 22, 65, 0, current_rot_angle, 0, 5);

}

static void low_latency_switch_in(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("low_latency_view switch in\n");
    gui_view_switch_on_event(view, gui_view_descriptor_get("multiple_layer_animation_view"),
                             SWITCH_OUT_TO_RIGHT_USE_TRANSLATION,
                             SWITCH_IN_FROM_LEFT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_RIGHT);
    gui_view_switch_on_event(view, gui_view_descriptor_get("smooth_gradient_image_view"),
                             SWITCH_OUT_TO_LEFT_USE_TRANSLATION,
                             SWITCH_IN_FROM_RIGHT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_LEFT);

    l3_model_base_t *face_3d = l3_create_model((void *)_acdesc_face, L3_DRAW_FRONT_AND_SORT, 0, 0, 410,
                                               502);
    l3_set_global_transform(face_3d, (l3_global_transform_cb)face_global_cb);

    gui_lite3d_t *lite3d_face = gui_lite3d_create(view, "lite3d-widget", face_3d,
                                                  0, 0, 410, 502);

    target_rot_angle += delta_angle;
    gui_obj_create_timer(GUI_BASE(lite3d_face), 10, true, update_face_animation);
}

static void low_latency_switch_out(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("low_latency_view switch out\n");
}

GUI_VIEW_INSTANCE("low_latency_view", 1, low_latency_switch_in, low_latency_switch_out);

