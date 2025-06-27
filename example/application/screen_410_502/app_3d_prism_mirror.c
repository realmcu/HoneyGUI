
#include "guidef.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "gui_server.h"
#include "gui_canvas.h"
#include "gui_3d.h"
#include "math.h"
#include <gui_view.h>
#include <tp_algo.h>
#include "gui_components_init.h"
#include "root_image_hongkong/ui_resource.h"
#include "app_hongkong.h"


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
static Position_rot_t prism_world_rot_temp = {0.0f, 0.0f, 0.0f};

static Position_pos_t prism_world_pos_raw = {0.0f, 0.0f, 0.0f};
static Position_pos_t prism_world_pos_target = {0.0f, 0.0f, 0.0f};

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

#define CURRENT_VIEW_NAME "prism3d_mirror_view"

static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;
const static gui_view_descriptor_t *image_view = NULL;
static void app_ui_prism_mirror_design(gui_view_t *view);
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
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_REGISTER(gui_view_descriptor_register_init);

static int gui_view_get_other_view_descriptor_init(void)
{
    /* you can get other view descriptor point here */
    menu_view = gui_view_descriptor_get("menu_view");
    image_view = gui_view_descriptor_get("image_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);
static void return_to_menu()
{
    gui_view_switch_direct(current_view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

// static void return_timer_cb()
// {
//     touch_info_t *tp = tp_get_info();
//     GUI_RETURN_HELPER(tp, gui_get_dc()->screen_width, return_to_menu)
// }

/*
    * @brief  Initialize the 3D prism mirror view
    * @param  view: The view to initialize
*/


static void prism_mirror3d_update_angle_cb()
{

    touch_info_t *tp = tp_get_info();

    static float prism_angle_increase = 0.0f;
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

    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 0),
                                 gui_point_4d(0, 0, prism_world_pos_temp.pos_z), 0, 0,
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

    gui_prism_mirror3d_update_face_flags();
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

static void prism_view_switch_to_other_view()
{
    if (image_view && face_flags_rotation >= 0 && face_flags_rotation < face_nums)
    {
        gui_view_switch_direct(current_view, image_view, SWITCH_OUT_NONE_ANIMATION,
                               SWITCH_OUT_NONE_ANIMATION);
    }
}
static void prism_position_init()
{
    prism_world_pos_raw.pos_x = 0.0f;
    prism_world_pos_raw.pos_y = 7.0f;
    prism_world_pos_raw.pos_z = 40.0f;

    prism_world_pos_target.pos_x = 0.0f;
    prism_world_pos_target.pos_y = 11.3f;
    prism_world_pos_target.pos_z = 29.0f;
}
static void app_ui_prism_mirror_design(gui_view_t *view)
{

    gui_obj_t *obj = GUI_BASE(view);
    // gui_obj_create_timer(obj, 10, true, return_timer_cb);
    gui_view_switch_on_event(view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);

    prism_3d = gui_3d_create(view, "3d-prism", DESC_PRISM_BIN, GUI_3D_DRAW_FRONT_AND_BACK, 0, 0,
                             410, 502);

    gui_3d_set_face_image(prism_3d, 0, PRISM_FACE0_BIN);
    gui_3d_set_face_image(prism_3d, 1, PRISM_FACE1_BIN);
    gui_3d_set_face_image(prism_3d, 2, PRISM_FACE2_BIN);
    gui_3d_set_face_image(prism_3d, 3, PRISM_FACE3_BIN);
    gui_3d_set_face_image(prism_3d, 4, PRISM_FACE4_BIN);
    gui_3d_set_face_image(prism_3d, 5, PRISM_FACE5_BIN);
    gui_3d_set_face_image(prism_3d, 6, PRISM_FACE1_BIN);
    gui_3d_set_face_image(prism_3d, 7, PRISM_FACE2_BIN);
    gui_3d_set_face_image(prism_3d, 8, PRISM_FACE3_BIN);
    gui_3d_set_face_image(prism_3d, 9, PRISM_FACE4_BIN);
    gui_3d_set_face_image(prism_3d, 10, PRISM_FACE0_BIN);
    gui_3d_set_face_image(prism_3d, 11, PRISM_FACE5_BIN);
    enter_prism_view_flag = true;
    prism_position_init();
    gui_prism_mirror3d_enter_animate();

    gui_3d_on_click(prism_3d, prism_mirror3d_on_face_click_cb, NULL);

    gui_view_set_animate_step(view, 1000);

}

