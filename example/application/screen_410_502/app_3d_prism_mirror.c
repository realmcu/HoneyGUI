/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "guidef.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "gui_canvas.h"
#include "gui_lite3d.h"
#include "tp_algo.h"
#include "gui_components_init.h"
#include "root_image/ui_resource.h"
#include "app_main_watch.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "prism3d_mirror_view"

/*============================================================================*
*                             Types
*============================================================================*/
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

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void gui_prism_mirror3d_swap_states(void);
static void prism_mirror3d_render_animate_cb(void *param);
static void gui_prism_mirror3d_enter_animate(gui_lite3d_t *lite3d_prism_mirror);
static void prism_mirror3d_on_face_click_cb(void *obj, gui_event_t e, void *param);
static void prism_mirror3d_update_angle_cb(void *param);
static void prism_view_switch_to_other_view(void);
/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
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

/* Prism Animate */
static Position_pos_t prism_world_pos_temp = {0.0f, 0.0f, 0.0f};
static Position_rot_t prism_world_rot_temp = {0.0f, 0.0f, 0.0f};
static Position_pos_t prism_world_pos_raw = {0.0f, 0.0f, 0.0f};
static Position_pos_t prism_world_pos_target = {0.0f, 0.0f, 0.0f};

static uint8_t face_nums = 6;
int16_t face_flags_rotation = 1;
static float progress_percent = 0.0f;
static int16_t enter_face_flags = 1;
static bool enter_prism_view_flag = true;

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
    image_view = gui_view_descriptor_get("image_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);


static void prism_mirror3d_update_angle_cb(void *param)
{
    (void)param;
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

static void prism_global_cb(l3_model_base_t *this)
{
    l3_camera_UVN_initialize(&this->camera, l3_4d_point(0, 0, 0),
                             l3_4d_point(0, 0, prism_world_pos_temp.pos_z), 0, 0,
                             90, this->viewPortWidth, this->viewPortHeight);

    l3_world_initialize(&this->world, prism_world_pos_temp.pos_x, prism_world_pos_temp.pos_y,
                        prism_world_pos_temp.pos_z,
                        0, prism_world_rot_temp.rot_y, 0, 18);
}

static void gui_prism_mirror3d_update_face_flags(void)
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
static void prism_mirror3d_render_animate_cb(void *param)
{
    gui_lite3d_t *lite3d_prism_mirror = (gui_lite3d_t *)param;

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

    if (progress_percent == 1)
    {
        if (enter_prism_view_flag)
        {
            gui_prism_mirror3d_swap_states();
            gui_obj_delete_timer(&(lite3d_prism_mirror->base));
            gui_obj_create_timer(&(lite3d_prism_mirror->base), 17, true, prism_mirror3d_update_angle_cb);
            gui_obj_start_timer(&(lite3d_prism_mirror->base));
            enter_prism_view_flag = false;
        }
        else
        {
            gui_obj_delete_timer(&(lite3d_prism_mirror->base));
            prism_view_switch_to_other_view();
            enter_face_flags = face_flags_rotation;
            // gui_log("enter_face_flags: %d\n", enter_face_flags);
            enter_prism_view_flag = true;
        }
    }
}

static void prism_mirror3d_on_face_click_cb(void *obj, gui_event_t e, void *param)
{
    (void)e;
    (void)param;
    GUI_ASSERT(obj != NULL);
    gui_lite3d_t *lite3d_prism_mirror = (gui_lite3d_t *)obj;

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
    gui_obj_create_timer(&(lite3d_prism_mirror->base), 17, true, prism_mirror3d_render_animate_cb);
    gui_obj_start_timer(&(lite3d_prism_mirror->base));

}

static void gui_prism_mirror3d_swap_states()
{
    Position_pos_t temp = prism_world_pos_raw;
    prism_world_pos_raw = prism_world_pos_target;
    prism_world_pos_target = temp;

    progress_percent = 0.0f;
}

static void gui_prism_mirror3d_enter_animate(gui_lite3d_t *lite3d_prism_mirror)
{
    gui_prism_mirror3d_swap_states();
    if (enter_prism_view_flag)
    {
        gui_obj_create_timer(&(lite3d_prism_mirror->base), 17, true, prism_mirror3d_render_animate_cb);
        gui_obj_start_timer(&(lite3d_prism_mirror->base));

    }
    prism_world_rot_temp.rot_y = (360 / face_nums) * (enter_face_flags -
                                                      1);// Set the initial rotation based on the face flags
}

static void prism_view_switch_to_other_view()
{
    if (image_view && face_flags_rotation >= 0 && face_flags_rotation < face_nums)
    {
        gui_view_switch_direct(current_view, image_view->name, SWITCH_OUT_NONE_ANIMATION,
                               SWITCH_IN_NONE_ANIMATION);
    }
}

static void prism_position_init(void)
{
    prism_world_pos_raw.pos_x = 0.0f;
    prism_world_pos_raw.pos_y = 10.0f;
    prism_world_pos_raw.pos_z = 40.0f;

    prism_world_pos_target.pos_x = 0.0f;
    prism_world_pos_target.pos_y = 12.3f;
    prism_world_pos_target.pos_z = 28.3f;
}

static void app_ui_prism_mirror_design(gui_view_t *view)
{

    gui_obj_t *obj = GUI_BASE(view);
    // gui_obj_create_timer(obj, 10, true, return_timer_cb);
    gui_view_switch_on_event(view, menu_view->name, SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);

    l3_model_base_t *prism_3d = l3_create_model(DESC_PRISM_BIN, L3_DRAW_FRONT_AND_BACK, 0, 0, 410, 502);

    l3_set_face_image(prism_3d, 0, PRISM_FACE0_BIN);
    l3_set_face_image(prism_3d, 1, PRISM_FACE1_BIN);
    l3_set_face_image(prism_3d, 2, PRISM_FACE2_BIN);
    l3_set_face_image(prism_3d, 3, PRISM_FACE3_BIN);
    l3_set_face_image(prism_3d, 4, PRISM_FACE4_BIN);
    l3_set_face_image(prism_3d, 5, PRISM_FACE5_BIN);
    l3_set_face_image(prism_3d, 6, PRISM_FACE1_BIN);
    l3_set_face_image(prism_3d, 7, PRISM_FACE2_BIN);
    l3_set_face_image(prism_3d, 8, PRISM_FACE3_BIN);
    l3_set_face_image(prism_3d, 9, PRISM_FACE4_BIN);
    l3_set_face_image(prism_3d, 10, PRISM_FACE0_BIN);
    l3_set_face_image(prism_3d, 11, PRISM_FACE5_BIN);
    l3_set_global_transform(prism_3d, (l3_global_transform_cb)prism_global_cb);
    gui_lite3d_t *lite3d_prism_mirror = gui_lite3d_create(obj, "lite3d_prism_mirror", prism_3d, 0, 0, 0,
                                                          0);

    enter_prism_view_flag = true;
    prism_position_init();
    gui_prism_mirror3d_enter_animate(lite3d_prism_mirror);

    gui_lite3d_on_click(lite3d_prism_mirror, prism_mirror3d_on_face_click_cb, NULL);

    gui_view_set_animate_step(view, 1000);
}

