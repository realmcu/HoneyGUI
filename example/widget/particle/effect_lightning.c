/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_lightning.c
 * @brief Lightning Arc Particle Effect - Realistic Electric Discharge
 *
 * This example demonstrates:
 * 1. Jagged zigzag path generation for realistic lightning bolts
 * 2. Branching structure with recursive sub-bolts
 * 3. Instantaneous flash with fade-out
 * 4. Bright core with glow effect using multiple particle layers
 * 5. Periodic discharge with random variation
 */

#include "effect_lightning.h"
#include "gui_obj.h"
#include "gui_api_os.h"
#include "gui_api_dc.h"
#include "def_type.h"
#include <math.h>

#define PARTICLE_POOL_SIZE      800
#define LIGHTNING_SEGMENTS      12
#define LIGHTNING_JITTER        35.0f
#define LIGHTNING_BRANCH_DEPTH  3
#define LIGHTNING_DISCHARGE_MS  1200
#define LIGHTNING_FLASH_MS      150
#define LIGHTNING_FADE_MS       200

typedef struct
{
    float x1, y1;
    float x2, y2;
} lightning_segment_t;

typedef struct
{
    lightning_segment_t segments[64];
    int segment_count;
    uint32_t birth_time;
    uint8_t is_branch;
    float brightness;
} lightning_bolt_t;

#define MAX_BOLTS 8
static lightning_bolt_t s_bolts[MAX_BOLTS];
static int s_bolt_count = 0;

static gui_particle_widget_t *s_lightning_widget = NULL;
static float s_lightning_start_x = 100.0f;
static float s_lightning_start_y = 80.0f;
static float s_lightning_end_x = 300.0f;
static float s_lightning_end_y = 350.0f;
static uint32_t s_lightning_timer = 0;
static uint32_t s_last_bolt_time = 0;

static uint32_t lightning_rand(void)
{
    static uint32_t seed = 54321;
    seed = seed * 1103515245 + 12345;
    return (seed >> 16) & 0x7FFF;
}

static float lightning_rand_float(float min, float max)
{
    return min + (float)(lightning_rand() % 1000) / 1000.0f * (max - min);
}

static void generate_bolt_path(float x1, float y1, float x2, float y2,
                               lightning_bolt_t *bolt, int segments, float jitter)
{
    if (bolt == NULL || segments < 2)
    {
        return;
    }

    float dx = x2 - x1;
    float dy = y2 - y1;
    float length = sqrtf(dx * dx + dy * dy);

    if (length < 1.0f)
    {
        return;
    }

    float px = -dy / length;
    float py = dx / length;

    float points_x[32];
    float points_y[32];
    int num_points = (segments < 30) ? segments + 1 : 31;

    points_x[0] = x1;
    points_y[0] = y1;
    points_x[num_points - 1] = x2;
    points_y[num_points - 1] = y2;

    for (int i = 1; i < num_points - 1; i++)
    {
        float t = (float)i / (float)(num_points - 1);
        float base_x = x1 + dx * t;
        float base_y = y1 + dy * t;
        float envelope = 4.0f * t * (1.0f - t);
        float displacement = lightning_rand_float(-jitter, jitter) * envelope;

        points_x[i] = base_x + px * displacement;
        points_y[i] = base_y + py * displacement;
    }

    bolt->segment_count = 0;
    for (int i = 0; i < num_points - 1 && bolt->segment_count < 64; i++)
    {
        bolt->segments[bolt->segment_count].x1 = points_x[i];
        bolt->segments[bolt->segment_count].y1 = points_y[i];
        bolt->segments[bolt->segment_count].x2 = points_x[i + 1];
        bolt->segments[bolt->segment_count].y2 = points_y[i + 1];
        bolt->segment_count++;
    }
}

static void emit_segment_particles(gui_particle_widget_t *widget,
                                   float x1, float y1, float x2, float y2,
                                   float brightness, uint8_t is_core)
{
    if (widget == NULL || widget->ps == NULL || widget->ps->pool == NULL)
    {
        return;
    }

    float dx = x2 - x1;
    float dy = y2 - y1;
    float length = sqrtf(dx * dx + dy * dy);

    if (length < 2.0f)
    {
        return;
    }

    int particle_count = (int)(length / 3.0f);
    if (particle_count < 2)
    {
        particle_count = 2;
    }
    if (particle_count > 20)
    {
        particle_count = 20;
    }

    particle_pool_t *pool = widget->ps->pool;

    for (int i = 0; i < particle_count; i++)
    {
        particle_t *p = particle_pool_alloc(pool);
        if (p == NULL)
        {
            break;
        }

        float t = (float)i / (float)(particle_count - 1);
        t += lightning_rand_float(-0.1f, 0.1f);
        if (t < 0.0f)
        {
            t = 0.0f;
        }
        if (t > 1.0f)
        {
            t = 1.0f;
        }

        p->x = x1 + dx * t;
        p->y = y1 + dy * t;

        float px = -dy / length;
        float py = dx / length;
        if (!is_core)
        {
            float offset = lightning_rand_float(-4.0f, 4.0f);
            p->x += px * offset;
            p->y += py * offset;
        }

        p->vx = lightning_rand_float(-5.0f, 5.0f);
        p->vy = lightning_rand_float(-5.0f, 5.0f);

        if (is_core)
        {
            p->color = 0xFFFFFFFF;
            p->scale = lightning_rand_float(1.5f, 2.5f) * brightness;
            p->opacity = (uint8_t)(255 * brightness);
            p->life = LIGHTNING_FLASH_MS + lightning_rand() % 50;
        }
        else
        {
            uint8_t b = 255;
            uint8_t g = (uint8_t)(180 + lightning_rand() % 75);
            uint8_t r = (uint8_t)(100 + lightning_rand() % 100);
            p->color = (0xFFU << 24) | (b << 16) | (g << 8) | r;
            p->scale = lightning_rand_float(2.0f, 4.0f) * brightness;
            p->opacity = (uint8_t)((150 + lightning_rand() % 50) * brightness);
            p->life = LIGHTNING_FLASH_MS + LIGHTNING_FADE_MS + lightning_rand() % 100;
        }

        p->max_life = p->life;
        p->active = 1;
        p->ax = 0;
        p->ay = 0;
        p->rotation = 0;
        p->rotation_speed = 0;
    }
}

static void emit_bolt_particles(gui_particle_widget_t *widget, lightning_bolt_t *bolt)
{
    if (widget == NULL || bolt == NULL || bolt->segment_count == 0)
    {
        return;
    }

    for (int i = 0; i < bolt->segment_count; i++)
    {
        emit_segment_particles(widget,
                               bolt->segments[i].x1, bolt->segments[i].y1,
                               bolt->segments[i].x2, bolt->segments[i].y2,
                               bolt->brightness * (bolt->is_branch ? 0.6f : 1.0f), 0);
    }

    for (int i = 0; i < bolt->segment_count; i++)
    {
        emit_segment_particles(widget,
                               bolt->segments[i].x1, bolt->segments[i].y1,
                               bolt->segments[i].x2, bolt->segments[i].y2,
                               bolt->brightness * (bolt->is_branch ? 0.7f : 1.0f), 1);
    }
}

static void generate_lightning_strike(float start_x, float start_y,
                                      float end_x, float end_y, int depth)
{
    if (s_bolt_count >= MAX_BOLTS || depth > LIGHTNING_BRANCH_DEPTH)
    {
        return;
    }

    lightning_bolt_t *bolt = &s_bolts[s_bolt_count];
    s_bolt_count++;

    int segments = LIGHTNING_SEGMENTS - depth * 2;
    if (segments < 4)
    {
        segments = 4;
    }

    float jitter = LIGHTNING_JITTER * (1.0f - depth * 0.2f);
    generate_bolt_path(start_x, start_y, end_x, end_y, bolt, segments, jitter);

    bolt->birth_time = gui_ms_get();
    bolt->is_branch = (depth > 0) ? 1 : 0;
    bolt->brightness = 1.0f - depth * 0.25f;

    if (depth < LIGHTNING_BRANCH_DEPTH && bolt->segment_count > 3)
    {
        int branch_count = (depth == 0) ? (2 + lightning_rand() % 3) : (lightning_rand() % 2);

        for (int b = 0; b < branch_count && s_bolt_count < MAX_BOLTS; b++)
        {
            int seg_idx = 1 + lightning_rand() % (bolt->segment_count - 2);
            lightning_segment_t *seg = &bolt->segments[seg_idx];

            float branch_x = (seg->x1 + seg->x2) * 0.5f;
            float branch_y = (seg->y1 + seg->y2) * 0.5f;

            float main_dx = end_x - start_x;
            float main_dy = end_y - start_y;
            float main_len = sqrtf(main_dx * main_dx + main_dy * main_dy);

            float angle = atan2f(main_dy, main_dx);
            float branch_angle = angle + lightning_rand_float(-1.2f, 1.2f);

            float remaining = main_len * (1.0f - (float)seg_idx / bolt->segment_count);
            float branch_len = remaining * lightning_rand_float(0.3f, 0.6f);

            float branch_end_x = branch_x + cosf(branch_angle) * branch_len;
            float branch_end_y = branch_y + sinf(branch_angle) * branch_len;

            generate_lightning_strike(branch_x, branch_y, branch_end_x, branch_end_y, depth + 1);
        }
    }
}

static void lightning_particle_update(particle_t *p, void *user_data)
{
    GUI_UNUSED(user_data);
    if (p == NULL || !p->active || p->max_life == 0)
    {
        return;
    }

    float life_ratio = (float)p->life / (float)p->max_life;

    if (life_ratio > 0.7f)
    {
        /* Bright flash phase - keep opacity */
    }
    else
    {
        p->opacity = (uint8_t)(255 * life_ratio / 0.7f);
    }

    if (lightning_rand() % 8 == 0)
    {
        p->opacity = (uint8_t)(p->opacity * lightning_rand_float(0.7f, 1.0f));
    }
}

static void lightning_update_cb(void *user_data)
{
    GUI_UNUSED(user_data);
    if (s_lightning_widget == NULL || s_lightning_widget->ps == NULL ||
        s_lightning_widget->ps->pool == NULL)
    {
        return;
    }

    uint32_t current_time = gui_ms_get();

    /* Let particle system handle updates - just trigger new strikes */

    if (current_time - s_lightning_timer > LIGHTNING_DISCHARGE_MS)
    {
        s_bolt_count = 0;

        gui_dispdev_t *dc = gui_get_dc();
        s_lightning_end_x = s_lightning_start_x + lightning_rand_float(-100.0f, 100.0f);
        s_lightning_end_y = (float)(dc->screen_height - 50);

        generate_lightning_strike(s_lightning_start_x, s_lightning_start_y,
                                  s_lightning_end_x, s_lightning_end_y, 0);

        for (int i = 0; i < s_bolt_count; i++)
        {
            emit_bolt_particles(s_lightning_widget, &s_bolts[i]);
        }

        s_lightning_timer = current_time;
        s_last_bolt_time = current_time;

        gui_log("Lightning: Strike! %d bolts\n", s_bolt_count);
    }

    uint32_t since_strike = current_time - s_last_bolt_time;
    GUI_UNUSED(since_strike);
    /* Re-flash effect removed to avoid direct pool access */
}

void effect_lightning_config(particle_effect_config_t *config)
{
    if (config == NULL)
    {
        return;
    }

    particle_effect_config_init(config);

    config->shape.type = PARTICLE_SHAPE_POINT;
    config->shape.point.x = s_lightning_start_x;
    config->shape.point.y = s_lightning_start_y;

    config->emission.burst_enabled = 0;
    config->emission.rate = 0;

    config->lifecycle.auto_cleanup = 1;
    config->lifecycle.loop = 0;

    config->color.mode = PARTICLE_COLOR_GRADIENT;
    config->color.color_start = 0xFFFFFFFF;
    config->color.color_end = 0xFF88CCFF;

    config->render.blend_mode = PARTICLE_BLEND_ADDITIVE;
    config->render.base_size = 3.0f;

    config->callbacks.on_particle_update = lightning_particle_update;
}

gui_particle_widget_t *effect_lightning_demo_init(void)
{
    gui_obj_t *root = gui_obj_get_root();
    gui_dispdev_t *dc = gui_get_dc();
    int screen_w = dc->screen_width;
    int screen_h = dc->screen_height;

    s_lightning_start_x = (float)(screen_w / 2);
    s_lightning_start_y = 30.0f;
    s_lightning_end_x = (float)(screen_w / 2);
    s_lightning_end_y = (float)(screen_h - 50);

    s_lightning_widget = gui_particle_widget_create(root, "lightning_demo",
                                                    0, 0, screen_w, screen_h,
                                                    PARTICLE_POOL_SIZE);
    if (s_lightning_widget == NULL)
    {
        gui_log("Lightning: Failed to create widget\n");
        return NULL;
    }

    gui_particle_widget_set_update_cb(s_lightning_widget, lightning_update_cb, NULL);

    s_bolt_count = 0;
    generate_lightning_strike(s_lightning_start_x, s_lightning_start_y,
                              s_lightning_end_x, s_lightning_end_y, 0);

    for (int i = 0; i < s_bolt_count; i++)
    {
        emit_bolt_particles(s_lightning_widget, &s_bolts[i]);
    }

    s_lightning_timer = gui_ms_get();
    s_last_bolt_time = s_lightning_timer;

    gui_log("Lightning: Demo initialized - realistic electric arc\n");
    return s_lightning_widget;
}
