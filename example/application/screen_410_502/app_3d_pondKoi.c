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
#include <math.h>
#include "guidef.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "gui_view.h"
#include "gui_lite3d.h"
#include "tp_algo.h"
#include "gui_components_init.h"
#include "root_image/ui_resource.h"
#include "app_main_watch.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "pandkoi_view"
#define DEG_TO_RAD(angle) ((angle) * M_PI_F / 180.0f)
#define KOI_COUNT 4
#define LOTUS_COUNT 5
#define LOTUS_MAX_OFFSET 8.0f
/*============================================================================*
*                             Types
*============================================================================*/
typedef struct
{
    float x;
    float y;
    float angle;      // z angle in degrees
    float move_speed;
    float target_x;
    float target_y;
    bool moving_to_target;
    bool clicked;
} Koi_Pos;

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void app_ui_pond_koi_design(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;
static const gui_view_descriptor_t descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,

    .on_switch_in = app_ui_pond_koi_design,
    .on_switch_out = NULL,
    .keep = false,
};


/* Fish Position*/
static Koi_Pos koi_pos[KOI_COUNT];
static gui_lite3d_t *lite3d_koi[KOI_COUNT] = {NULL};

/* Lotus Parameter*/
static gui_img_t *lotus_img[LOTUS_COUNT] = {NULL};
static const float LOTUS_SWING_SPEED = 2.0f;
static float lotus_swing_angle = 0.0f;

/* Fish Movement */
static float swing_time = 0.0f;
static float swing_angle = 0.0f;
static int clicked_index = -1;

/* Wave Animation */
static float wave_scale = 1.0f;
static float wave_opa = 255.0f;
static bool wave_animating = false;

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

static void koi_pos_init(void)
{
    for (int i = 0; i < KOI_COUNT; i++)
    {
        float x = (float)(xorshift16() % 161 - 80);
        float y = (float)(xorshift16() % 161 - 80);
        float target_x = (float)(xorshift16() % 201 - 100);
        float target_y = (float)(xorshift16() % 201 - 100);

        float angle = atan2f(target_y - y, target_x - x) * (180.0f / M_PI_F) + 90;

        koi_pos[i] = (Koi_Pos)
        {
            .x = x,
             .y = y,
              .target_x = target_x,
               .target_y = target_y,
                .angle = angle,
                 .move_speed = 0.01f,
                  .moving_to_target = true,
        };
    }
}

static void koi_animate_cb(void *param)
{
    (void)param;

    swing_time += 0.4f;
    swing_angle = 3.0f * sinf(swing_time);


    for (int i = 0; i < KOI_COUNT; i++)
    {
        if (koi_pos[i].moving_to_target)
        {
            float dx = koi_pos[i].target_x - koi_pos[i].x;
            float dy = koi_pos[i].target_y - koi_pos[i].y;
            float distance = sqrtf(dx * dx + dy * dy);

            if (distance > 10.0f)
            {
                // Acceleration and deceleration
                float speed_factor = fminf(distance / 20.0f, 1.0f);
                koi_pos[i].x += dx * koi_pos[i].move_speed * speed_factor;
                koi_pos[i].y += dy * koi_pos[i].move_speed * speed_factor;

                // Caculate new rotate angle
                float desired_angle = atan2f(dy, dx) * (180.0f / M_PI_F) + 90;
                float angle_difference = desired_angle - koi_pos[i].angle;

                if (angle_difference > 180.0f)
                {
                    angle_difference -= 360.0f;
                }
                if (angle_difference < -180.0f)
                {
                    angle_difference += 360.0f;
                }
                koi_pos[i].angle += angle_difference * 0.2f;
            }
            else
            {
                koi_pos[i].moving_to_target = false;
            }
        }
        else
        {
            koi_pos[i].target_x = (float)(xorshift16() % 201 - 100);
            koi_pos[i].target_y = (float)(xorshift16() % 201 - 100);
            koi_pos[i].moving_to_target = true;
            koi_pos[i].move_speed = 0.01f;
        }
    }

}

static void koi_click_cb(void *obj, gui_event_t *e)
{
    (void)e;

    gui_lite3d_t *this = (gui_lite3d_t *)obj;

    for (int i = 0; i < KOI_COUNT; i++)
    {
        if (this == lite3d_koi[i])
        {
            clicked_index = i;
            break;
        }
    }

    if (clicked_index != -1)
    {
        koi_pos[clicked_index].move_speed = 0.05f;
        koi_pos[clicked_index].moving_to_target = true;
        koi_pos[clicked_index].target_x *= 2.0f;
        koi_pos[clicked_index].target_y *= 2.0f;
    }
}

static void lotus_leaf_animate_cb(void *param)
{
    (void)param;
    lotus_swing_angle -= LOTUS_SWING_SPEED;
    if (lotus_swing_angle < 360.0f)
    {
        lotus_swing_angle += 360.0f;
    }
    float rad = DEG_TO_RAD(lotus_swing_angle);

    float offset_x = sinf(rad) * LOTUS_MAX_OFFSET;
    float offset_y = cosf(rad) * LOTUS_MAX_OFFSET * 0.5f;
    float rotate_angle = sinf(rad) * 3.0f;

    for (int i = 0; i < LOTUS_COUNT; i++)
    {
        if (lotus_img[i] != NULL)
        {
            gui_img_translate(lotus_img[i], offset_x, offset_y);
            gui_img_rotation(lotus_img[i], rotate_angle);
        }
    }
}

static void wave_animate_cb(void *param)
{
    (void)param;

    touch_info_t *tp = tp_get_info();
    gui_img_t *wave_img = (gui_img_t *)param;

    if (tp->pressed)
    {
        if (!wave_animating)
        {
            wave_scale = 1.0f;
            wave_opa = 255.0f;
            wave_animating = true;
            gui_img_translate(wave_img, tp->x, tp->y);
        }
    }
    if (wave_animating)
    {
        wave_img->base.not_show = false;
        wave_scale += 0.1f;
        wave_opa -= 5.0f;
        if (wave_opa <= 0)
        {
            wave_opa = 0;
            wave_img->base.not_show = true;
            wave_animating = false;
        }

        gui_img_scale(wave_img, wave_scale, wave_scale);
        gui_img_set_opacity(wave_img, wave_opa);
    }
    else
    {
        wave_img->base.not_show = true;
    }
}

static void fish0_global_cb(l3_model_base_t *this)
{
    l3_camera_UVN_initialize(&this->camera, l3_4d_point(0, 0, 0), l3_4d_point(0, 0, 80), 1, 32767,
                             90, this->viewPortWidth, this->viewPortHeight);

    l3_world_initialize(&this->world, koi_pos[0].x, koi_pos[0].y, 80, 0, 0, koi_pos[0].angle, 5);
}

static void fish0_shadow_global_cb(l3_model_base_t *this)
{
    l3_camera_UVN_initialize(&this->camera, l3_4d_point(0, 0, 0), l3_4d_point(0, 0, 80), 1, 32767,
                             90, this->viewPortWidth, this->viewPortHeight);

    l3_world_initialize(&this->world, koi_pos[0].x - 8, koi_pos[0].y - 8, 80, 0, 0, koi_pos[0].angle,
                        5);
}

static void fish1_global_cb(l3_model_base_t *this)
{
    l3_camera_UVN_initialize(&this->camera, l3_4d_point(0, 0, 0), l3_4d_point(0, 0, 80), 1, 32767,
                             90, this->viewPortWidth, this->viewPortHeight);

    l3_world_initialize(&this->world, koi_pos[1].x, koi_pos[1].y, 80, 0, 0, koi_pos[1].angle, 4);
}

static void fish1_shadow_global_cb(l3_model_base_t *this)
{
    l3_camera_UVN_initialize(&this->camera, l3_4d_point(0, 0, 0), l3_4d_point(0, 0, 80), 1, 32767,
                             90, this->viewPortWidth, this->viewPortHeight);

    l3_world_initialize(&this->world, koi_pos[1].x - 8, koi_pos[1].y - 8, 80, 0, 0, koi_pos[1].angle,
                        4);
}

static void fish2_global_cb(l3_model_base_t *this)
{
    l3_camera_UVN_initialize(&this->camera, l3_4d_point(0, 0, 0), l3_4d_point(0, 0, 80), 1, 32767,
                             90, this->viewPortWidth, this->viewPortHeight);

    l3_world_initialize(&this->world, koi_pos[2].x, koi_pos[2].y, 80, 0, 0, koi_pos[2].angle, 4);
}

static void fish2_shadow_global_cb(l3_model_base_t *this)
{
    l3_camera_UVN_initialize(&this->camera, l3_4d_point(0, 0, 0), l3_4d_point(0, 0, 80), 1, 32767,
                             90, this->viewPortWidth, this->viewPortHeight);

    l3_world_initialize(&this->world, koi_pos[2].x - 8, koi_pos[2].y - 8, 80, 0, 0, koi_pos[2].angle,
                        4);
}

static void fish3_global_cb(l3_model_base_t *this)
{
    l3_camera_UVN_initialize(&this->camera, l3_4d_point(0, 0, 0), l3_4d_point(0, 0, 80), 1, 32767,
                             90, this->viewPortWidth, this->viewPortHeight);

    l3_world_initialize(&this->world, koi_pos[3].x, koi_pos[3].y, 80, 0, 0, koi_pos[3].angle, 4);
}

static void fish3_shadow_global_cb(l3_model_base_t *this)
{
    l3_camera_UVN_initialize(&this->camera, l3_4d_point(0, 0, 0), l3_4d_point(0, 0, 80), 1, 32767,
                             90, this->viewPortWidth, this->viewPortHeight);

    l3_world_initialize(&this->world, koi_pos[3].x - 8, koi_pos[3].y - 8, 80, 0, 0, koi_pos[3].angle,
                        4);
}

static l3_4x4_matrix_t fish_face_cb(l3_model_base_t *this, size_t face_index/*face offset*/)
{
    l3_4x4_matrix_t face_matrix;
    l3_4x4_matrix_t transform_matrix;

    if (face_index == 0 || face_index == 1)// head && body1
    {
        l3_calculator_4x4_matrix(&face_matrix, 0, 0, 0, l3_4d_point(0, 0, 0), l3_4d_vector(0, 0, 1),
                                 swing_angle, 1);
    }
    else if (face_index == 2)// body2
    {
        l3_calculator_4x4_matrix(&face_matrix, 0, 0, 0, l3_4d_point(0, 0, 0), l3_4d_vector(0, 0, 1),
                                 0, 1);
    }
    else if (face_index == 3 || face_index == 4)// body3 && tail
    {
        l3_calculator_4x4_matrix(&face_matrix, 0, 0, 0, l3_4d_point(0, 0, 0), l3_4d_vector(0, 0, 1),
                                 -swing_angle, 1);
    }

    l3_4x4_matrix_mul(&this->world, &face_matrix, &transform_matrix);

    return transform_matrix;
}

static void app_ui_pond_koi_design(gui_view_t *view)
{
    wave_animating = false;
    srand((unsigned int)gui_ms_get());
    gui_obj_t *obj = GUI_BASE(view);
    gui_view_switch_on_event(view, menu_view->name, SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_PRESSED);

    gui_img_create_from_mem(obj, "img_1_test", PONDGROUND_BIN, 0, 0, 0, 0);

    // Shadow
    l3_model_base_t *fish0_shadow = l3_create_model(DESC_KOI_SHADOW_BIN, L3_DRAW_FRONT_ONLY, 0, 0, 410,
                                                    502);
    l3_model_base_t *fish1_shadow = l3_create_model(DESC_KOI_SHADOW_BIN, L3_DRAW_FRONT_ONLY, 0, 0, 410,
                                                    502);
    l3_model_base_t *fish2_shadow = l3_create_model(DESC_KOI_SHADOW_BIN, L3_DRAW_FRONT_ONLY, 0, 0, 410,
                                                    502);
    l3_model_base_t *fish3_shadow = l3_create_model(DESC_KOI_SHADOW_BIN, L3_DRAW_FRONT_ONLY, 0, 0, 410,
                                                    502);
    // Fish
    l3_model_base_t *fish0 = l3_create_model(DESC_KOI0_BIN, L3_DRAW_FRONT_ONLY, 0, 0, 410, 502);
    l3_model_base_t *fish1 = l3_create_model(DESC_KOI1_BIN, L3_DRAW_FRONT_ONLY, 0, 0, 410, 502);
    l3_model_base_t *fish2 = l3_create_model(DESC_KOI2_BIN, L3_DRAW_FRONT_ONLY, 0, 0, 410, 502);
    l3_model_base_t *fish3 = l3_create_model(DESC_KOI0_BIN, L3_DRAW_FRONT_ONLY, 0, 0, 410, 502);

    koi_pos_init();

    l3_set_global_transform(fish0, (l3_global_transform_cb)fish0_global_cb);
    l3_set_face_transform(fish0, (l3_face_transform_cb)fish_face_cb);
    l3_set_global_transform(fish0_shadow, (l3_global_transform_cb)fish0_shadow_global_cb);
    l3_set_face_transform(fish0_shadow, (l3_face_transform_cb)fish_face_cb);

    l3_set_global_transform(fish1, (l3_global_transform_cb)fish1_global_cb);
    l3_set_face_transform(fish1, (l3_face_transform_cb)fish_face_cb);
    l3_set_global_transform(fish1_shadow, (l3_global_transform_cb)fish1_shadow_global_cb);
    l3_set_face_transform(fish1_shadow, (l3_face_transform_cb)fish_face_cb);

    l3_set_global_transform(fish2, (l3_global_transform_cb)fish2_global_cb);
    l3_set_face_transform(fish2, (l3_face_transform_cb)fish_face_cb);
    l3_set_global_transform(fish2_shadow, (l3_global_transform_cb)fish2_shadow_global_cb);
    l3_set_face_transform(fish2_shadow, (l3_face_transform_cb)fish_face_cb);

    l3_set_global_transform(fish3, (l3_global_transform_cb)fish3_global_cb);
    l3_set_face_transform(fish3, (l3_face_transform_cb)fish_face_cb);
    l3_set_global_transform(fish3_shadow, (l3_global_transform_cb)fish3_shadow_global_cb);
    l3_set_face_transform(fish3_shadow, (l3_face_transform_cb)fish_face_cb);

    gui_lite3d_create(obj, "lite3d_fish0_shadow", fish0_shadow, 0,
                      0, 0, 0);
    gui_lite3d_create(obj, "lite3d_fish1_shadow", fish1_shadow, 0,
                      0, 0, 0);
    gui_lite3d_create(obj, "lite3d_fish2_shadow", fish2_shadow, 0,
                      0, 0, 0);
    gui_lite3d_create(obj, "lite3d_fish3_shadow", fish3_shadow, 0,
                      0, 0, 0);
    lite3d_koi[0] = gui_lite3d_create(obj, "lite3d_koi0", fish0, 0, 0, 0, 0);
    lite3d_koi[1] = gui_lite3d_create(obj, "lite3d_koi1", fish1, 0, 0, 0, 0);
    lite3d_koi[2] = gui_lite3d_create(obj, "lite3d_koi2", fish2, 0, 0, 0, 0);
    lite3d_koi[3] = gui_lite3d_create(obj, "lite3d_koi3", fish3, 0, 0, 0, 0);

    // Lotus
    lotus_img[0] = gui_img_create_from_mem(obj, "lotus", LOTUS00_BIN, 240, 340, 80, 80);
    gui_img_set_mode(lotus_img[0], IMG_SRC_OVER_MODE);
    gui_img_set_focus(lotus_img[0], 40, 40);
    lotus_img[1] = gui_img_create_from_mem(obj, "leaf0", LOTUS01_BIN, 70, 90, 80, 80);
    gui_img_set_mode(lotus_img[1], IMG_SRC_OVER_MODE);
    gui_img_set_focus(lotus_img[1], 40, 40);
    lotus_img[2] = gui_img_create_from_mem(obj, "leaf1", LOTUS02_BIN, 66, 251, 112, 122);
    gui_img_set_mode(lotus_img[2], IMG_SRC_OVER_MODE);
    gui_img_set_focus(lotus_img[2], 56, 61);
    lotus_img[3] = gui_img_create_from_mem(obj, "leaf2", LOTUS03_BIN, 272, 92, 144, 144);
    gui_img_set_mode(lotus_img[3], IMG_SRC_OVER_MODE);
    gui_img_set_focus(lotus_img[3], 72, 72);
    lotus_img[4] = gui_img_create_from_mem(obj, "lotus_leaf", LOTUS04_BIN, 372, 452, 144, 144);
    gui_img_set_mode(lotus_img[4], IMG_SRC_OVER_MODE);
    gui_img_set_focus(lotus_img[4], 72, 72);
    // Wave
    gui_img_t *wave_img = gui_img_create_from_mem(obj, "wave", WAVE_BIN, 0, 0, 0, 0);
    gui_img_set_mode(wave_img, IMG_SRC_OVER_MODE);
    gui_img_set_focus(wave_img, 32, 32);
    wave_img->base.not_show = true;

    gui_lite3d_on_click(lite3d_koi[0], koi_click_cb, NULL);
    gui_lite3d_on_click(lite3d_koi[1], koi_click_cb, NULL);
    gui_lite3d_on_click(lite3d_koi[2], koi_click_cb, NULL);
    gui_lite3d_on_click(lite3d_koi[3], koi_click_cb, NULL);

    gui_obj_create_timer(GUI_BASE(lite3d_koi[0]), 17, true, koi_animate_cb);
    gui_obj_create_timer(GUI_BASE(lotus_img[0]), 10, true, lotus_leaf_animate_cb);
    gui_obj_create_timer(GUI_BASE(wave_img), 10, true, wave_animate_cb);

}



