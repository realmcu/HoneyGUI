/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_ripple.c
 * @brief Ripple Particle Effect - Concentric Circle Expansion
 *
 * This example demonstrates:
 * 1. Touch-triggered concentric ring expansion
 * 2. on_particle_death callback to spawn secondary ripples
 * 3. PARTICLE_SHAPE_RING for ring emission
 * 4. Fade-out opacity with expansion
 */

#include "effect_ripple.h"
#include "gui_obj.h"
#include "gui_api_os.h"
#include "gui_api_dc.h"
#include "tp_algo.h"
#include "def_type.h"
#include <math.h>

#define PARTICLE_POOL_SIZE 400
#define RIPPLE_EXPAND_SPEED 120.0f
#define RIPPLE_MAX_RADIUS 200.0f
#define RIPPLE_RING_PARTICLES 36
#define RIPPLE_SECONDARY_WAVES 2

static gui_particle_widget_t *s_ripple_widget = NULL;
static float s_ripple_center_x = 0.0f;
static float s_ripple_center_y = 0.0f;
static int s_screen_w = 0;
static int s_screen_h = 0;
static uint8_t s_touch_active = 0;
static uint32_t s_last_ripple_time = 0;
static int s_ripple_wave_count = 0;

static void emit_ripple_ring(float cx, float cy, float radius, uint8_t opacity, float scale)
{
    if (s_ripple_widget == NULL || s_ripple_widget->ps == NULL ||
        s_ripple_widget->ps->pool == NULL)
    {
        return;
    }

    particle_pool_t *pool = s_ripple_widget->ps->pool;

    for (int i = 0; i < RIPPLE_RING_PARTICLES; i++)
    {
        particle_t *p = particle_pool_alloc(pool);
        if (p == NULL)
        {
            break;
        }

        float angle = (float)i / RIPPLE_RING_PARTICLES * 2.0f * M_PI_F;

        p->x = cx + cosf(angle) * radius;
        p->y = cy + sinf(angle) * radius;
        p->vx = cosf(angle) * RIPPLE_EXPAND_SPEED;
        p->vy = sinf(angle) * RIPPLE_EXPAND_SPEED;

        p->color = 0xFF44DDFF;
        p->opacity = opacity;
        p->scale = scale;
        p->life = 1500;
        p->max_life = 1500;
        p->active = 1;
        p->ax = 0;
        p->ay = 0;
        p->rotation = angle;
        p->rotation_speed = 0;
    }
}

static void ripple_particle_death(particle_t *p, void *user_data)
{
    GUI_UNUSED(user_data);
    if (p == NULL)
    {
        return;
    }

    if (p->max_life == 1500 && s_ripple_wave_count < RIPPLE_SECONDARY_WAVES)
    {
        float dx = p->x - s_ripple_center_x;
        float dy = p->y - s_ripple_center_y;
        float dist = sqrtf(dx * dx + dy * dy);

        if (dist > RIPPLE_MAX_RADIUS * 0.6f && dist < RIPPLE_MAX_RADIUS * 0.8f)
        {
            float angle = atan2f(dy, dx);
            if (angle > -0.1f && angle < 0.1f)
            {
                s_ripple_wave_count++;
                emit_ripple_ring(s_ripple_center_x, s_ripple_center_y,
                                 dist * 0.3f, 150, 0.6f);
            }
        }
    }
}

static void ripple_particle_update(particle_t *p, void *user_data)
{
    GUI_UNUSED(user_data);
    if (p == NULL || !p->active || p->max_life == 0)
    {
        return;
    }

    float dx = p->x - s_ripple_center_x;
    float dy = p->y - s_ripple_center_y;
    float dist = sqrtf(dx * dx + dy * dy);

    float fade = 1.0f - dist / RIPPLE_MAX_RADIUS;
    if (fade < 0.0f)
    {
        fade = 0.0f;
    }
    p->opacity = (uint8_t)(255 * fade * fade);

    p->scale = 0.8f + dist / RIPPLE_MAX_RADIUS * 0.5f;

    if (dist > RIPPLE_MAX_RADIUS)
    {
        p->life = 0;
    }
}

static void ripple_update_cb(void *user_data)
{
    GUI_UNUSED(user_data);

    if (s_ripple_widget == NULL)
    {
        return;
    }

    touch_info_t *tp = tp_get_info();
    uint32_t now = gui_ms_get();

    if (tp != NULL && tp->pressed)
    {
        s_touch_active = 1;
        s_ripple_center_x = (float)tp->x;
        s_ripple_center_y = (float)tp->y;
        s_ripple_wave_count = 0;

        emit_ripple_ring(s_ripple_center_x, s_ripple_center_y, 5.0f, 255, 1.0f);
        s_last_ripple_time = now;
    }
    else if (tp != NULL && tp->released)
    {
        s_touch_active = 0;
    }

    /* Auto-trigger ripple for demo if no touch for 3 seconds */
    if (!s_touch_active && now - s_last_ripple_time > 3000)
    {
        s_ripple_center_x = (float)(s_screen_w / 2);
        s_ripple_center_y = (float)(s_screen_h / 2);
        s_ripple_wave_count = 0;
        emit_ripple_ring(s_ripple_center_x, s_ripple_center_y, 5.0f, 255, 1.0f);
        s_last_ripple_time = now;
    }
}

/*============================================================================*
 *                           Static Configuration
 *============================================================================*/

static void effect_ripple_config(particle_effect_config_t *config)
{
    if (config == NULL)
    {
        return;
    }

    particle_effect_config_init(config);

    config->shape.type = PARTICLE_SHAPE_RING;
    config->shape.ring.cx = s_ripple_center_x;
    config->shape.ring.cy = s_ripple_center_y;
    config->shape.ring.inner_r = 0.0f;
    config->shape.ring.outer_r = 5.0f;

    config->trajectory.type = PARTICLE_TRAJECTORY_LINEAR;
    config->trajectory.damping = 0.02f;

    config->emission.rate = 0.0f;
    config->emission.burst_enabled = 0;

    config->lifecycle.life_min = 1500;
    config->lifecycle.life_max = 1500;
    config->lifecycle.auto_cleanup = 1;
    config->lifecycle.loop = 0;

    config->color.mode = PARTICLE_COLOR_SOLID;
    config->color.color_start = 0xFF44DDFF;

    config->opacity.start = 255;
    config->opacity.end = 0;
    config->opacity.easing = PARTICLE_EASING_EASE_OUT;

    config->scale.start = 1.0f;
    config->scale.end = 1.5f;

    config->boundary.behavior = PARTICLE_BOUNDARY_NONE;

    config->behavior.mode = PARTICLE_BEHAVIOR_TOUCH_FEEDBACK;

    config->render.blend_mode = PARTICLE_BLEND_ADDITIVE;
    config->render.base_size = 4.0f;

    config->callbacks.on_particle_update = ripple_particle_update;
    config->callbacks.on_particle_death = ripple_particle_death;
    config->callbacks.user_data = NULL;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

gui_particle_widget_t *effect_ripple_create(gui_obj_t *parent, const char *name,
                                            int16_t x, int16_t y, int16_t w, int16_t h)
{
    s_screen_w = w;
    s_screen_h = h;
    s_ripple_center_x = (float)(w / 2);
    s_ripple_center_y = (float)(h / 2);

    s_ripple_widget = gui_particle_widget_create(parent, name,
                                                 x, y, w, h,
                                                 PARTICLE_POOL_SIZE);
    if (s_ripple_widget == NULL)
    {
        return NULL;
    }

    gui_particle_widget_set_update_cb(s_ripple_widget, ripple_update_cb, NULL);

    /* Initial ripple */
    s_ripple_wave_count = 0;
    emit_ripple_ring(s_ripple_center_x, s_ripple_center_y, 5.0f, 255, 1.0f);
    s_last_ripple_time = gui_ms_get();

    return s_ripple_widget;
}
