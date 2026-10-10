/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <math.h>
#include <stdint.h>
#include <string.h>
#include "guidef.h"
#include "gui_canvas.h"
#include "gui_components_init.h"
#include "gui_img.h"
#include "gui_lite3d.h"
#include "gui_matrix.h"
#include "gui_obj.h"
#include "gui_server.h"
#include "gui_win.h"
#include "tp_algo.h"
#include "seawater/desc_seafish0.txt"
#include "seawater/desc_seafish1.txt"
#include "seawater/desc_seafish2.txt"
#include "seawater/desc_seafish3.txt"
#include "seawater/CanSkin_green.txt"
#include "seawater/CanSkin_purple.txt"
#include "seawater/CanSkin_red.txt"
#include "seawater/CanSkin_yellow.txt"
#include "seawater/Droplet.txt"
#include "seawater/WaterFactor_bg.txt"
#include "seawater/seaBackGround.txt"
#include "seawater/UI_TEXT_0.txt"
#include "seawater/UI_TEXT_2.txt"
#include "seawater/UI_TEXT_5.txt"

#define SEAWATER_WIDTH 410
#define SEAWATER_HEIGHT 502
#define ROTATION_INCREMENT 0.5f
#define POSITION_INCREMENT 2.0f
#define CAN_COUNT 4
#define WAVE_DURATION_MS 5000
#define MAX_BUBBLES 20

typedef struct
{
    gui_img_t *img;
    float scale;
    int start_x;
    int start_y;
    uint32_t create_time;
} Bubble;

static gui_canvas_t *canvas_wave;
static gui_win_t *scene;
static gui_win_t *can_windows[CAN_COUNT];
static gui_win_t *bubble_window;
static Bubble *bubbles;
static int active_bubble_count;

static float fish_angle;
static float fish_time;
static float fish_x[CAN_COUNT] = {-70, 70, -50, -45};
static int16_t fish_y[CAN_COUNT] = {50, 40, 60, 45};
static uint32_t fish_last_bubble_time[CAN_COUNT];
static uint8_t fish_transform_index;
static uint16_t random_seed = 12345;

static bool wave_active;
static uint32_t wave_start_time;
static float touch_x = -1;
static float touch_time;
static float wave_alpha;
static float can_positions[CAN_COUNT] = {55, 140, 225, 315};
static float can_wave_y[CAN_COUNT];
static float can_current_pos[CAN_COUNT];
static float can_rotation_targets[CAN_COUNT] = {-10, 4, -5, 5};
static float can_current_rotation[CAN_COUNT];
static float scene_scale;
static float scene_x;
static float scene_y;

static void scene_prepare(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (cb_type == OBJ_PRE_PROCESS)
    {
        gui_win_prepare(obj);
        matrix_translate(scene_x, scene_y, obj->matrix);
        matrix_scale(scene_scale, scene_scale, obj->matrix);
    }
}

static float scene_touch_x(void)
{
    return (tp_get_info()->x - scene_x) / scene_scale;
}

static uint16_t seawater_random(void)
{
    random_seed ^= random_seed << 6;
    random_seed ^= random_seed >> 9;
    random_seed ^= random_seed << 2;
    return random_seed;
}

static void gui_wave_draw_graph(gui_canvas_t *canvas)
{
    NVGcontext *vg = canvas->vg;
    float y_base = 245;
    float w = SEAWATER_WIDTH;
    float h = 30;
    float t = gui_ms_get() * 0.001f;

    touch_info_t *tp = tp_get_info();
    if (tp->pressed)
    {
        touch_x = scene_touch_x();
        touch_time = t;
        wave_alpha = 1.0f;
    }

    if (wave_alpha > 0)
    {
        float elapsed = t - touch_time;
        wave_alpha = expf(-elapsed * 0.5f);
        if (wave_alpha < 0.01f)
        {
            wave_alpha = 0.0f;
            touch_x = -1;
        }
    }

    for (int i = 0; i < CAN_COUNT; i++)
    {
        float xi = can_positions[i];
        float base_wave = 0.4f * sinf(xi * 0.05f + t * 1.5f)
                          + 0.2f * sinf(xi * 0.1f + t * 2.0f);
        float touch_effect = 0;
        if (touch_x >= 0)
        {
            float distance = fabsf(xi - touch_x);
            if (distance < 50)
            {
                float decay_factor = expf(-distance * 0.02f);
                float time_decay = expf(-(t - touch_time) * 2.0f);
                touch_effect = decay_factor * 0.8f * time_decay;
            }
        }

        can_wave_y[i] = y_base - h * fmaxf(0, fminf(1, base_wave * 0.4f + 0.5f + touch_effect));
    }

    if (wave_alpha > 0)
    {
        NVGpaint bg = nvgLinearGradient(vg, 0, y_base - h, 0, y_base,
                                        nvgRGBA(0, 160, 192, (int)(50 * wave_alpha)),
                                        nvgRGBA(0, 160, 192, (int)(200 * wave_alpha)));
        nvgBeginPath(vg);
        for (int i = 0; i < CAN_COUNT; i++)
        {
            if (i == 0) { nvgMoveTo(vg, can_positions[i], can_wave_y[i]); }
            else { nvgLineTo(vg, can_positions[i], can_wave_y[i]); }
        }
        nvgLineTo(vg, w, y_base);
        nvgLineTo(vg, 0, y_base);
        nvgFillPaint(vg, bg);
        nvgFill(vg);
    }
}

static void wave_animate_cb(void)
{
    touch_info_t *tp = tp_get_info();
    uint32_t current_time = gui_ms_get();

    if (tp->pressed)
    {
        wave_active = true;
        wave_start_time = current_time;
        gui_canvas_set_canvas_cb(canvas_wave, gui_wave_draw_graph);
    }

    if (wave_active && current_time - wave_start_time > WAVE_DURATION_MS)
    {
        wave_active = false;
        gui_canvas_set_canvas_cb(canvas_wave, NULL);
    }
}

static void animate_cans_based_on_wave(void)
{
    float y_base = 230;

    for (int j = 0; j < CAN_COUNT; j++)
    {
        if (can_wave_y[j] == 0 || wave_alpha == 0)
        {
            continue;
        }

        float target_pos = (y_base - can_wave_y[j]) * 3.0f;
        if (can_current_pos[j] < target_pos)
        {
            can_current_pos[j] += POSITION_INCREMENT;
            if (can_current_pos[j] > target_pos) { can_current_pos[j] = target_pos; }
        }
        else if (can_current_pos[j] > target_pos)
        {
            can_current_pos[j] -= POSITION_INCREMENT;
            if (can_current_pos[j] < target_pos) { can_current_pos[j] = target_pos; }
        }
    }
}

static void can_update_param_cb(void)
{
    touch_info_t *tp = tp_get_info();
    if (tp->pressed)
    {
        for (int i = 0; i < CAN_COUNT; i++)
        {
            can_rotation_targets[i] = ((seawater_random() % 11) - 5) / 1.0f;
        }
    }

    animate_cans_based_on_wave();

    for (int i = 0; i < CAN_COUNT; i++)
    {
        if (can_current_rotation[i] < can_rotation_targets[i])
        {
            can_current_rotation[i] += ROTATION_INCREMENT;
            if (can_current_rotation[i] > can_rotation_targets[i])
            {
                can_current_rotation[i] = can_rotation_targets[i];
            }
        }
        else if (can_current_rotation[i] > can_rotation_targets[i])
        {
            can_current_rotation[i] -= ROTATION_INCREMENT;
            if (can_current_rotation[i] < can_rotation_targets[i])
            {
                can_current_rotation[i] = can_rotation_targets[i];
            }
        }
    }
}

static void create_bubble(int x, int y)
{
    if (active_bubble_count >= MAX_BUBBLES) { return; }

    for (int i = 0; i < MAX_BUBBLES; i++)
    {
        Bubble *bubble = &bubbles[i];
        if (!bubble->img || gui_obj_is_hidden(GUI_BASE(bubble->img)))
        {
            if (!bubble->img)
            {
                bubble->img = gui_img_create_from_mem(bubble_window, "bubble", (void *)_acDroplet,
                                                      x, y, 0, 0);
                if (!bubble->img) { return; }
            }
            else
            {
                bubble->img->base.x = x;
                bubble->img->base.y = y;
                gui_obj_hidden(&bubble->img->base, false);
            }

            bubble->scale = 0.3f + (seawater_random() % 7) / 20.0f;
            bubble->img->opacity_value = 150 + seawater_random() % 105;
            bubble->start_x = x;
            bubble->start_y = y;
            bubble->create_time = gui_ms_get();
            gui_img_scale(bubble->img, bubble->scale, bubble->scale);
            gui_img_set_opacity(bubble->img, bubble->img->opacity_value);
            active_bubble_count++;
            break;
        }
    }
}

static void update_all_bubbles(void)
{
    uint32_t current_time = gui_ms_get();

    for (int i = 0; i < MAX_BUBBLES; i++)
    {
        Bubble *bubble = &bubbles[i];
        if (bubble->img && !gui_obj_is_hidden(GUI_BASE(bubble->img)))
        {
            uint32_t elapsed = current_time - bubble->create_time;
            int y_offset = (int)(elapsed / 20.0f);
            bubble->img->base.y = bubble->start_y - y_offset;
            bubble->img->base.x = bubble->start_x + 5 * sinf(elapsed / 200.0f);
            bubble->scale += 0.002f;
            bubble->img->opacity_value = 255 - (elapsed / 10);
            gui_img_scale(bubble->img, bubble->scale, bubble->scale);
            gui_img_set_opacity(bubble->img,
                                bubble->img->opacity_value > 0 ? bubble->img->opacity_value : 0);

            if (elapsed > 2500 || bubble->img->opacity_value <= 0)
            {
                gui_obj_hidden(&bubble->img->base, true);
                active_bubble_count--;
            }
        }
    }
}

static int fish_x_to_screen(float x)
{
    float screen_x = ((x - 80.0f) / (-90.0f - 80.0f)) * SEAWATER_WIDTH;
    if (screen_x < 0) { screen_x = 0; }
    if (screen_x > SEAWATER_WIDTH) { screen_x = SEAWATER_WIDTH; }
    return (int)screen_x;
}

static void fish_animate_cb(void)
{
    touch_info_t *tp = tp_get_info();
    float fish_pos_x[CAN_COUNT];

    if (tp->released)
    {
        for (int i = 0; i < CAN_COUNT; i++)
        {
            fish_pos_x[i] = seawater_random() % 161;
        }
        if (fish_x[0] > 90) { fish_x[0] = 70 - fish_pos_x[0]; }
        if (fish_x[1] < -80) { fish_x[1] = fish_pos_x[1] - 70; }
        if (fish_x[2] > 90) { fish_x[2] = 70 - fish_pos_x[2]; }
        if (fish_x[3] > 90) { fish_x[3] = 70 - fish_pos_x[3]; }
    }
    else
    {
        uint32_t current_time = gui_ms_get();
        const uint32_t bubble_intervals[CAN_COUNT] = {200, 400, 600, 800};
        for (int i = 0; i < CAN_COUNT; i++)
        {
            if (current_time - fish_last_bubble_time[i] > bubble_intervals[i] &&
                (i != 0 || fish_x[i] < 70))
            {
                create_bubble(fish_x_to_screen(fish_x[i]), fish_y[i] + 80);
                fish_last_bubble_time[i] = current_time;
            }
        }

        fish_x[0]++;
        fish_x[1]--;
        fish_x[2] += 1.2f;
        fish_x[3]++;
        fish_time += 0.4f;
        fish_angle = 10.0f * sinf(fish_time);
    }
    update_all_bubbles();
}

static void can_animate_cb(void *p)
{
    gui_obj_t *obj = (gui_obj_t *)p;
    uint8_t index = 0;
    for (uint8_t i = 1; i < CAN_COUNT; i++)
    {
        if (obj == GUI_BASE(can_windows[i])) { index = i; break; }
    }
    matrix_translate(0, can_current_pos[index], obj->matrix);
    matrix_translate(-obj->w / 2, -obj->h / 2, obj->matrix);
    matrix_rotate(can_current_rotation[index], obj->matrix);
    matrix_translate(obj->w / 2, obj->h / 2, obj->matrix);
}

static void app_animate_cb(void *p)
{
    (void)p;
    fish_animate_cb();
    wave_animate_cb();
    can_update_param_cb();
}

static void fish_global_cb(l3_model_base_t *this)
{
    l3_camera_UVN_initialize(&this->camera, l3_4d_point(0, 0, 0), l3_4d_point(0, 0, 80), 1, 32767,
                             90, this->viewPortWidth, this->viewPortHeight);
    l3_world_initialize(&this->world, -fish_x[fish_transform_index], fish_y[fish_transform_index],
                        80, 0, 0, 0, 5);
    fish_transform_index = (fish_transform_index + 1) % CAN_COUNT;
}

static l3_4x4_matrix_t fish_face_cb(l3_model_base_t *this, size_t face_index)
{
    l3_4x4_matrix_t face_matrix;
    l3_4x4_matrix_t transform_matrix;
    l3_calculator_4x4_matrix(&face_matrix, 0, 0, 0, l3_4d_point(0, 0, 0),
                             l3_4d_vector(0, 1, 0), face_index == 0 ? fish_angle : 0, 1);
    l3_4x4_matrix_mul(&this->world, &face_matrix, &transform_matrix);
    return transform_matrix;
}

static int app_init(void)
{
    static const unsigned char *const fish_desc[CAN_COUNT] =
    {
        _acdesc_seafish0, _acdesc_seafish1, _acdesc_seafish2, _acdesc_seafish3
    };
    static const unsigned char *const can_skins[CAN_COUNT] =
    {
        _acCanSkin_purple, _acCanSkin_yellow, _acCanSkin_green, _acCanSkin_red
    };
    static const unsigned char *const can_numbers[CAN_COUNT] =
    {
        _acUI_TEXT_2, _acUI_TEXT_0, _acUI_TEXT_2, _acUI_TEXT_5
    };
    gui_dispdev_t *dc = gui_get_dc();
    float scale_x = (float)dc->screen_width / SEAWATER_WIDTH;
    float scale_y = (float)dc->screen_height / SEAWATER_HEIGHT;
    scene_scale = fminf(scale_x, scale_y);
    scene_x = (dc->screen_width - SEAWATER_WIDTH * scene_scale) / 2.0f;
    scene_y = (dc->screen_height - SEAWATER_HEIGHT * scene_scale) / 2.0f;

    scene = gui_win_create(gui_obj_get_root(), "seawater_scene", 0, 0,
                           SEAWATER_WIDTH, SEAWATER_HEIGHT);
    GUI_BASE(scene)->obj_cb = scene_prepare;
    gui_img_create_from_mem(scene, "background", (void *)_acseaBackGround, 0, 0, 0, 0);

    for (int i = 0; i < CAN_COUNT; i++)
    {
        l3_model_base_t *fish = l3_create_model((void *)fish_desc[i], L3_DRAW_FRONT_ONLY,
                                                0, 0, SEAWATER_WIDTH, SEAWATER_HEIGHT);
        l3_set_global_transform(fish, (l3_global_transform_cb)fish_global_cb);
        l3_set_face_transform(fish, (l3_face_transform_cb)fish_face_cb);
        gui_lite3d_create(scene, "lite3d_fish", fish, 0, 0, SEAWATER_WIDTH, SEAWATER_HEIGHT);
    }

    for (int i = 0; i < CAN_COUNT; i++)
    {
        can_windows[i] = gui_win_create(scene, "can_window", i == 3 ? 330 : 70 + i * 85,
                                        190, 65, 129);
        gui_img_create_from_mem(can_windows[i], "can", (void *)can_skins[i], 0, 0, 0, 0);
        gui_img_create_from_mem(can_windows[i], "time", (void *)can_numbers[i], 16, 35, 0, 0);
    }

    gui_img_t *waterface_bg = gui_img_create_from_mem(scene, "waterface",
                                                      (void *)_acWaterFactor_bg, 0, 240, 0, 244);
    gui_img_set_mode(waterface_bg, IMG_SRC_OVER_MODE);
    gui_img_set_opacity(waterface_bg, 100);

    canvas_wave = gui_canvas_create(scene, "canvas", 0, 0, 0,
                                    SEAWATER_WIDTH, SEAWATER_HEIGHT);
    bubble_window = gui_win_create(scene, "bubble_window", 0, 240, SEAWATER_WIDTH, 240);
    bubbles = gui_malloc(sizeof(Bubble) * MAX_BUBBLES);
    if (!bubbles) { return -1; }
    memset(bubbles, 0, sizeof(Bubble) * MAX_BUBBLES);

    gui_obj_create_timer(GUI_BASE(scene), 10, true, app_animate_cb);
    gui_obj_start_timer(GUI_BASE(scene));
    for (int i = 0; i < CAN_COUNT; i++)
    {
        gui_obj_create_timer(GUI_BASE(can_windows[i]), 10, true, can_animate_cb);
        gui_obj_start_timer(GUI_BASE(can_windows[i]));
    }
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);
