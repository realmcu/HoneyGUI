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
#include "gui_components_init.h"
#include "root_image/ui_resource.h"
#include "tp_algo.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "koiclock_view"
#define DEG_TO_RAD(angle) ((angle) * M_PI_F / 180.0f)
#define RADIUS 55.0f
#define FISH_COUNT 3

/*============================================================================*
*                             Types
*============================================================================*/
typedef struct
{
    float x;
    float y;
    float angle;      // z angle in degrees
    float move_speed;
} Fish_Pos;

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void app_ui_koiclock_design(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/

static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;
static const gui_view_descriptor_t descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,

    .on_switch_in = app_ui_koiclock_design,
    .on_switch_out = NULL,

    .keep = false,
};

/* Fish Position*/
static Fish_Pos fish_pos[FISH_COUNT];
static gui_lite3d_t *lite3d_fish[FISH_COUNT] = {NULL};

/* Fish Movement */
static float swing_time = 0.0f;
static float swing_angle = 0.0f;
static uint8_t clicked_index = 0;

/* Wave Animation */
static bool wave_animating = false;
static float wave_scale = 1.0f;
static float wave_opa = 255.0f;

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


static void fish_pos_init(void)
{
    const float initial_angles[FISH_COUNT] = {0, 45, 240};

    for (int i = 0; i < FISH_COUNT; i++)
    {
        float rad = DEG_TO_RAD(initial_angles[i]);
        fish_pos[i] = (Fish_Pos)
        {
            .x = RADIUS * cosf(rad),
             .y = RADIUS * sinf(rad),
              .angle = initial_angles[i],
               .move_speed = 0.5f,
        };
    }
}

static void update_koi_move(void *param)
{
    (void)param;


    swing_time += 0.4f;
    swing_angle = 3.0f * sinf(swing_time);

    for (int i = 0; i < FISH_COUNT; i++)
    {
        fish_pos[i].angle += fish_pos[i].move_speed;
        fish_pos[i].move_speed *= 0.95f;
        // Ensure the speed does not drop below a threshold
        if (fish_pos[i].move_speed < 0.5f)
        {
            fish_pos[i].move_speed = 0.5f;
        }

        if (fish_pos[i].angle > 360.0f)
        {
            fish_pos[i].angle -= 360.0f;
        }
        float rad = DEG_TO_RAD(fish_pos[i].angle);

        fish_pos[i].x = RADIUS * cosf(rad);
        fish_pos[i].y = RADIUS * sinf(rad);
    }
}

static void fish_click_cb(void *obj, gui_event_t *e)
{
    (void)e;

    gui_lite3d_t *this = (gui_lite3d_t *)obj;

    for (int i = 0; i < FISH_COUNT; i++)
    {
        if (this == lite3d_fish[i])
        {
            clicked_index = i;
            break;
        }
    }
    fish_pos[clicked_index].move_speed = 2.0f;

}

static void wave_animate_cb(void *param)
{
    (void)param;

    gui_img_t *wave_img = (gui_img_t *)param;
    touch_info_t *tp = tp_get_info();

    if (tp->pressed && !wave_animating)
    {
        wave_scale = 1.0f;
        wave_opa = 255.0f;
        wave_animating = true;
        gui_img_translate(wave_img, tp->x, tp->y);
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

    l3_world_initialize(&this->world, fish_pos[0].x, fish_pos[0].y, 80, 0, 0, fish_pos[0].angle + 180,
                        2);

}
static void fish0_shadow_global_cb(l3_model_base_t *this)
{
    l3_camera_UVN_initialize(&this->camera, l3_4d_point(0, 0, 0), l3_4d_point(0, 0, 80), 1, 32767,
                             90, this->viewPortWidth, this->viewPortHeight);

    l3_world_initialize(&this->world, fish_pos[0].x + 3.0f, fish_pos[0].y + 3.0f, 80, 0, 0,
                        fish_pos[0].angle + 180, 2);

}
static void fish1_global_cb(l3_model_base_t *this)
{
    l3_camera_UVN_initialize(&this->camera, l3_4d_point(0, 0, 0), l3_4d_point(0, 0, 80), 1, 32767,
                             90, this->viewPortWidth, this->viewPortHeight);

    l3_world_initialize(&this->world, fish_pos[1].x, fish_pos[1].y, 80, 0, 0, fish_pos[1].angle + 180,
                        2);

}
static void fish1_shadow_global_cb(l3_model_base_t *this)
{
    l3_camera_UVN_initialize(&this->camera, l3_4d_point(0, 0, 0), l3_4d_point(0, 0, 80), 1, 32767,
                             90, this->viewPortWidth, this->viewPortHeight);

    l3_world_initialize(&this->world, fish_pos[1].x + 3.0f, fish_pos[1].y + 3.0f, 80, 0, 0,
                        fish_pos[1].angle + 180, 2);

}
static void fish2_global_cb(l3_model_base_t *this)
{
    l3_camera_UVN_initialize(&this->camera, l3_4d_point(0, 0, 0), l3_4d_point(0, 0, 80), 1, 32767,
                             90, this->viewPortWidth, this->viewPortHeight);

    l3_world_initialize(&this->world, fish_pos[2].x, fish_pos[2].y, 80, 0, 0, fish_pos[2].angle + 180,
                        2);

}
static void fish2_shadow_global_cb(l3_model_base_t *this)
{
    l3_camera_UVN_initialize(&this->camera, l3_4d_point(0, 0, 0), l3_4d_point(0, 0, 80), 1, 32767,
                             90, this->viewPortWidth, this->viewPortHeight);

    l3_world_initialize(&this->world, fish_pos[2].x + 3.0f, fish_pos[2].y + 3.0f, 80, 0, 0,
                        fish_pos[2].angle + 180, 2);

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

static void app_ui_koiclock_design(gui_view_t *view)
{
    srand((uint32_t)gui_ms_get());
    gui_obj_t *obj = GUI_BASE(view);
    gui_view_switch_on_event(view, menu_view->name, SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_PRESSED);

    gui_img_create_from_mem(obj, "koiclock_bg", LOTUS_BG_BIN, 0, 0, gui_get_screen_width(),
                            gui_get_screen_height());

    // Shadow
    l3_model_base_t *fish0_shadow = l3_create_model(DESC_KOI_SHADOW_BIN, L3_DRAW_FRONT_ONLY, 0, 0, 410,
                                                    502);
    l3_model_base_t *fish1_shadow = l3_create_model(DESC_KOI_SHADOW_BIN, L3_DRAW_FRONT_ONLY, 0, 0, 410,
                                                    502);
    l3_model_base_t *fish2_shadow = l3_create_model(DESC_KOI_SHADOW_BIN, L3_DRAW_FRONT_ONLY, 0, 0, 410,
                                                    502);
    // Fish
    l3_model_base_t *fish0 = l3_create_model(DESC_KOI0_BIN, L3_DRAW_FRONT_ONLY, 0, 0, 410, 502);
    l3_model_base_t *fish1 = l3_create_model(DESC_KOI1_BIN, L3_DRAW_FRONT_ONLY, 0, 0, 410, 502);
    l3_model_base_t *fish2 = l3_create_model(DESC_KOI2_BIN, L3_DRAW_FRONT_ONLY, 0, 0, 410, 502);

    fish_pos_init();

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

    gui_lite3d_create(obj, "lite3d_fish0_shadow", fish0_shadow, 0,
                      0, 0, 0);
    gui_lite3d_create(obj, "lite3d_fish1_shadow", fish1_shadow, 0,
                      0, 0, 0);
    gui_lite3d_create(obj, "lite3d_fish2_shadow", fish2_shadow, 0,
                      0, 0, 0);
    lite3d_fish[0] = gui_lite3d_create(obj, "lite3d_fish0", fish0, 0, 0, 0, 0);
    lite3d_fish[1] = gui_lite3d_create(obj, "lite3d_fish1", fish1, 0, 0, 0, 0);
    lite3d_fish[2] = gui_lite3d_create(obj, "lite3d_fish2", fish2, 0, 0, 0, 0);

    gui_img_t *clock_img = gui_img_create_from_mem(obj, "img_clock", CLOCK_BIN, 55, 90, 0, 0);
    gui_img_set_mode(clock_img, IMG_SRC_OVER_MODE);
    gui_img_t *wave_img = gui_img_create_from_mem(obj, "wave", WAVE_BIN, 0, 0, 0, 0);
    gui_img_set_mode(wave_img, IMG_SRC_OVER_MODE);
    gui_img_set_focus(wave_img, 32, 32);
    wave_img->base.not_show = true;


    gui_lite3d_on_click(lite3d_fish[0], fish_click_cb, NULL);
    gui_lite3d_on_click(lite3d_fish[1], fish_click_cb, NULL);
    gui_lite3d_on_click(lite3d_fish[2], fish_click_cb, NULL);

    gui_obj_create_timer(GUI_BASE(lite3d_fish[0]), 10, true, update_koi_move);
    gui_obj_create_timer(GUI_BASE(wave_img), 10, true, wave_animate_cb);
}
