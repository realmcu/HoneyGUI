/* 3d prism mirror demo start*/
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
#include "math.h"
#include <gui_view.h>

// #include "gui_prism_mirror3d.h"
#include <tp_algo.h>
#include "prism3d/desc.txt"

#include "prism3d/root/homelist_dog.c"
#include "prism3d/root/homelist_line_black.c"
#include "prism3d/root/homelist_line_orange.c"
#include "prism3d/root/homelist_number.c"
#include "prism3d/root/homelist_watch_black.c"
#include "prism3d/root/homelist_watch_white.c"


typedef struct
{
    float pos_x;
    float pos_y;
    float pos_z;
} Position_pos_t;

typedef struct
{
    float rot_x;
    float rot_y;
    float rot_z;
} Position_rot_t;

typedef struct
{
    gui_3d_t *render_object;               /**< The 3D render object */
    gui_animate_t animate;
} gui_prism_mirror3d_t;

static Position_pos_t prism_world_pos_temp = {0.0f, 0.0f, 0.0f};
static Position_pos_t prism_camera_pos_temp = {0.0f, 0.0f, 0.0f};
static Position_rot_t prism_world_rot_temp = {0.0f, 0.0f, 0.0f};



static Position_pos_t prism_world_pos_raw = {0.0f, 10.0f, 100.0f};
static Position_pos_t prism_camera_pos_raw = {0.0f, 3.0f, 55.0f};
static Position_rot_t prism_world_rot_raw = {0.0f, 0.0f, 0.0f};

static Position_pos_t prism_world_pos_target = {0.0f, 10.5f, 109.0f};
static Position_pos_t prism_camera_pos_target = {0.0f, 0.0f, 80.0f};
static Position_rot_t prism_world_rot_target = {0.0f, 0.0f, 0.0f};

static gui_prism_mirror3d_t *prism_mirror3d = NULL;
static gui_3d_t *prism_3d = NULL;
static uint8_t face_nums = 6;
int16_t face_flags_rotation = 1;
static float progress_percent = 0.0f;
static int16_t enter_face_flags = 1;
bool enter_prism_view_flag = true;

static void gui_prism_mirror3d_swap_states();
static void prism_mirror3d_render_animate_cb();
static void gui_prism_mirror3d_enter_animate();
static void prism_mirror3d_on_face_click_cb(void *obj, gui_event_t e, void *param);
static void prism_mirror3d_update_angle_cb();
static void prism_view_switch_to_other_view();

#define CURRENT_VIEW_NAME "prism_mirror_view"

static gui_view_t *current_view = NULL;
static const gui_view_descriptor_t *image_view = NULL;
static void app_ui_prism_mirror_design(gui_view_t *view);

/*
    * @brief  Initialize the 3D prism mirror view
    * @param  view: The view to initialize
*/


static void prism_mirror3d_update_angle_cb()
{

    touch_info_t *tp = tp_get_info();

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
            int16_t smoothed_delta_x = (int16_t)(delta_x * 0.05f);
            prism_world_rot_temp.rot_y += smoothed_delta_x;

            while (prism_world_rot_temp.rot_y > 360)
            {
                prism_world_rot_temp.rot_y -= 360;
            }
            while (prism_world_rot_temp.rot_y < -360)
            {
                prism_world_rot_temp.rot_y += 360;
            }
        }
    }
    {
        prism_world_rot_temp.rot_y += last_direction * prism_angle_increase;
        while (prism_world_rot_temp.rot_y > 360)
        {
            prism_world_rot_temp.rot_y -= 360;
        }
        while (prism_world_rot_temp.rot_y < -360)
        {
            prism_world_rot_temp.rot_y += 360;
        }
    }
}

static void prism_global_cb(gui_3d_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(prism_camera_pos_temp.pos_x,
                                                             prism_camera_pos_temp.pos_y, prism_camera_pos_temp.pos_z),
                                 gui_point_4d(0, 0, 0), 0, 0,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, prism_world_pos_temp.pos_x, prism_world_pos_temp.pos_y,
                             prism_world_pos_temp.pos_z,
                             prism_world_rot_temp.rot_x, prism_world_rot_temp.rot_y, prism_world_rot_temp.rot_z,
                             19);
}

static void gui_prism_mirror3d_update_face_flags()
{
    if (face_flags_rotation < 0)
    {
        face_flags_rotation += 360;
    }

    // Calculate which face is at front using mid-point threshold
    int section_size = 360 / face_nums;
    int half_section = section_size / 2;
    int effective_flag = (face_flags_rotation + half_section) % 360 / section_size;

    // Adjust 1-based index if needed
    face_flags_rotation = effective_flag + 1;

    // Ensure the flags are within bounds
    if (face_flags_rotation < 1 ||
        face_flags_rotation > face_nums)
    {
        face_flags_rotation = 1; // Assuming default to first face if out of bounds
    }
}
static void prism_mirror3d_render_animate_cb()
{
    if (progress_percent < 1.0f)
    {
        progress_percent += 0.05f;
    }
    else
    {
        progress_percent = 1.0f;
    }
    prism_world_pos_temp.pos_x = prism_world_pos_raw.pos_x +
                                 (prism_world_pos_target.pos_x - prism_world_pos_raw.pos_x) * progress_percent;
    prism_world_pos_temp.pos_y = prism_world_pos_raw.pos_y +
                                 (prism_world_pos_target.pos_y - prism_world_pos_raw.pos_y) * progress_percent;
    prism_world_pos_temp.pos_z = prism_world_pos_raw.pos_z +
                                 (prism_world_pos_target.pos_z - prism_world_pos_raw.pos_z) * progress_percent;
    prism_world_rot_temp.rot_x = prism_world_rot_raw.rot_x +
                                 (prism_world_rot_target.rot_x - prism_world_rot_raw.rot_x) * progress_percent;
    // prism_world_rot_temp.rot_y = prism_world_rot_raw.rot_y +
    //     (prism_world_rot_target.rot_y - prism_world_rot_raw.rot_y) * progress_percent;
    prism_world_rot_temp.rot_z = prism_world_rot_raw.rot_z +
                                 (prism_world_rot_target.rot_z - prism_world_rot_raw.rot_z) * progress_percent;
    prism_camera_pos_temp.pos_x = prism_camera_pos_raw.pos_x +
                                  (prism_camera_pos_target.pos_x - prism_camera_pos_raw.pos_x) * progress_percent;
    prism_camera_pos_temp.pos_y = prism_camera_pos_raw.pos_y +
                                  (prism_camera_pos_target.pos_y - prism_camera_pos_raw.pos_y) * progress_percent;
    prism_camera_pos_temp.pos_z = prism_camera_pos_raw.pos_z +
                                  (prism_camera_pos_target.pos_z - prism_camera_pos_raw.pos_z) * progress_percent;

    gui_3d_set_global_transform_cb(prism_3d, (gui_3d_global_transform_cb)prism_global_cb);
    if (progress_percent == 1)
    {
        if (enter_prism_view_flag)
        {
            gui_prism_mirror3d_swap_states();
            gui_obj_delete_timer(&(prism_3d->base));
            gui_obj_create_timer(&(prism_3d->base), 17, true, prism_mirror3d_update_angle_cb);
            gui_obj_start_timer(&(prism_3d->base));
            enter_prism_view_flag = false;
        }
        else
        {
            gui_obj_delete_timer(&(prism_3d->base));
            prism_view_switch_to_other_view();
            enter_face_flags = face_flags_rotation;
            // gui_log("enter_face_flags: %d\n", enter_face_flags);
            enter_prism_view_flag = true;
        }
    }
}

static void prism_mirror3d_on_face_click_cb(void *obj, gui_event_t e, void *param)
{
    GUI_ASSERT(obj != NULL);
    gui_3d_t *prism_mirror3d = (gui_3d_t *)obj;

    face_flags_rotation = (int32_t)(prism_world_rot_temp.rot_y + 0.5f);

    gui_prism_mirror3d_update_face_flags(prism_mirror3d);
    // gui_log("face: %d\n",face_flags_rotation);

    if (face_flags_rotation < 0 ||
        face_flags_rotation > (face_nums - 1))
    {
        face_flags_rotation = 0;
    }
    for (int i = 0; i < face_nums; i++)
    {
        if ((face_flags_rotation == 0) && (i == 0))
        {
            prism_world_pos_temp.pos_x = 0;
            prism_world_rot_temp.rot_y = (360.0 / face_nums) *
                                         (face_nums - 1);
            // gui_log("%f, %d\n", prism_world_rot_temp.rot_y, face_flags_rotation);
            break;
        }
        else if (i == face_flags_rotation)
        {
            prism_world_pos_temp.pos_x = 0;
            prism_world_rot_temp.rot_y = (360.0 / face_nums) *
                                         (face_flags_rotation -
                                          1);
            // gui_log("%f, %d\n", prism_world_rot_temp.rot_y, face_flags_rotation);
            break;
        }
    }
    gui_obj_create_timer(&(prism_3d->base), 17, true, prism_mirror3d_render_animate_cb);
    gui_obj_start_timer(&(prism_3d->base));

}
static void gui_prism_mirror3d_swap_states()
{
    Position_pos_t temp = prism_world_pos_raw;
    prism_world_pos_raw = prism_world_pos_target;
    prism_world_pos_target = temp;

    Position_pos_t temp1 = prism_camera_pos_raw;
    prism_camera_pos_raw = prism_camera_pos_target;
    prism_camera_pos_target = temp1;
    progress_percent = 0.0f;
}
static void gui_prism_mirror3d_enter_animate()
{
    gui_prism_mirror3d_swap_states();
    if (enter_prism_view_flag)
    {
        gui_obj_create_timer(&(prism_3d->base), 17, true, prism_mirror3d_render_animate_cb);
        gui_obj_start_timer(&(prism_3d->base));

    }
    prism_world_rot_temp.rot_y = (360 / face_nums) * (enter_face_flags -
                                                      1);// Set the initial rotation based on the face flags
}
static const gui_view_descriptor_t descriptor =
{
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = app_ui_prism_mirror_design,
    .on_switch_out = NULL,
    .keep = false,
};

static int gui_view_descriptor_register_init(void)
{
    gui_view_descriptor_register(&descriptor);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_REGISTER(gui_view_descriptor_register_init);

static int gui_view_get_other_view_descriptor_init(void)
{
    image_view = gui_view_descriptor_get("image_view");
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void prism_view_switch_to_other_view()
{
    if (image_view && face_flags_rotation >= 0 && face_flags_rotation < face_nums)
    {
        gui_view_switch_direct(current_view, image_view, SWITCH_OUT_NONE_ANIMATION,
                               SWITCH_OUT_NONE_ANIMATION);
    }
}

static void app_ui_prism_mirror_design(gui_view_t *view)
{
    prism_3d = gui_3d_create(view, "3d-prism", (void *)_acdesc, GUI_3D_DRAW_FRONT_AND_BACK, 0, 0,
                             480, 480);
    gui_prism_mirror3d_enter_animate();

    gui_3d_on_click(prism_3d, prism_mirror3d_on_face_click_cb, NULL);


    gui_view_set_animate_step(view, 1000);

}
static int app_init(void)
{
    gui_view_create(gui_obj_get_root(), &descriptor, 0, 0, 0, 0);
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);
/* 3d prism mirror demo end*/

