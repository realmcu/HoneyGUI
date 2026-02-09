/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_fireflies.c
 * @brief Fireflies Particle Effect - Floating Glowing Dots
 *
 * This example demonstrates:
 * 1. PARTICLE_SHAPE_RECT - full screen random emission
 * 2. PARTICLE_BEHAVIOR_BREATHE - breathing opacity effect
 * 3. Slow random drift motion
 * 4. Warm yellow-green glow colors
 */

#include "effect_fireflies.h"
#include "gui_obj.h"
#include "gui_api_os.h"
#include "gui_api_dc.h"
#include "def_type.h"
#include <math.h>

#define PARTICLE_POOL_SIZE 80
#define FIREFLY_DRIFT_SPEED 15.0f

static gui_particle_widget_t *s_fireflies_widget = NULL;
static int s_screen_w = 0;
static int s_screen_h = 0;

static uint32_t firefly_rand(void)
{
    static uint32_t seed = 98765;
    seed = seed * 1103515245 + 12345;
    return (seed >> 16) & 0x7FFF;
}

static float firefly_rand_float(float min, float max)
{
    return min + (float)(firefly_rand() % 1000) / 1000.0f * (max - min);
}

/**
 * @brief Custom particle init - random slow drift direction
 */
static void firefly_particle_init(particle_t *p, void *user_data)
{
    GUI_UNUSED(user_data);
    if (p == NULL)
    {
        return;
    }

    /* Random drift direction */
    float angle = firefly_rand_float(0.0f, 2.0f * M_PI_F);
    float speed = firefly_rand_float(5.0f, FIREFLY_DRIFT_SPEED);
    p->vx = cosf(angle) * speed;
    p->vy = sinf(angle) * speed;

    /* Random initial phase for breathing (store in rotation as phase offset) */
    p->rotation = firefly_rand_float(0.0f, 2.0f * M_PI_F);

    /* Warm yellow-green color with variation */
    uint8_t g = (uint8_t)(200 + firefly_rand() % 55);
    uint8_t r = (uint8_t)(180 + firefly_rand() % 75);
    uint8_t b = (uint8_t)(50 + firefly_rand() % 50);
    p->color = (0xFFU << 24) | (r << 16) | (g << 8) | b;

    /* Random size */
    p->scale = firefly_rand_float(0.8f, 1.5f);
}

/**
 * @brief Custom particle update - breathing glow + boundary wrap
 */
static void firefly_particle_update(particle_t *p, void *user_data)
{
    GUI_UNUSED(user_data);
    if (p == NULL || !p->active)
    {
        return;
    }

    /* Breathing effect using sine wave */
    /* Use rotation field as phase offset for varied timing */
    float time_factor = (float)(p->max_life - p->life) / 500.0f;
    float breath = 0.5f + 0.5f * sinf(time_factor * 3.0f + p->rotation);
    p->opacity = (uint8_t)(80 + breath * 175);

    /* Slight random direction change for organic movement */
    if (firefly_rand() % 60 == 0)
    {
        float angle_change = firefly_rand_float(-0.5f, 0.5f);
        float speed = sqrtf(p->vx * p->vx + p->vy * p->vy);
        float angle = atan2f(p->vy, p->vx) + angle_change;
        p->vx = cosf(angle) * speed;
        p->vy = sinf(angle) * speed;
    }

    /* Manual boundary wrap */
    float margin = 20.0f;
    if (p->x < -margin)
    {
        p->x = (float)s_screen_w + margin;
    }
    else if (p->x > (float)s_screen_w + margin)
    {
        p->x = -margin;
    }
    if (p->y < -margin)
    {
        p->y = (float)s_screen_h + margin;
    }
    else if (p->y > (float)s_screen_h + margin)
    {
        p->y = -margin;
    }
}

void effect_fireflies_config(particle_effect_config_t *config)
{
    if (config == NULL)
    {
        return;
    }

    particle_effect_config_init(config);

    /* Rectangle emission - full screen random spawn */
    config->shape.type = PARTICLE_SHAPE_RECT;
    config->shape.rect.x = 0.0f;
    config->shape.rect.y = 0.0f;
    config->shape.rect.w = (float)s_screen_w;
    config->shape.rect.h = (float)s_screen_h;

    /* Linear trajectory - motion handled by callback */
    config->trajectory.type = PARTICLE_TRAJECTORY_LINEAR;
    config->trajectory.damping = 0.0f;

    /* Slow emission to maintain steady count */
    config->emission.angle_min = 0.0f;
    config->emission.angle_max = 2.0f * M_PI_F;
    config->emission.speed_min = 5.0f;
    config->emission.speed_max = FIREFLY_DRIFT_SPEED;
    config->emission.rate = 3.0f;
    config->emission.burst_enabled = 1;
    config->emission.burst_count = 30;
    config->emission.burst_interval = 0;

    /* Long lifetime for persistent fireflies */
    config->lifecycle.life_min = 8000;
    config->lifecycle.life_max = 15000;
    config->lifecycle.auto_cleanup = 0;
    config->lifecycle.loop = 1;

    /* Breathing behavior mode */
    config->behavior.mode = PARTICLE_BEHAVIOR_BREATHE;
    config->behavior.breathe_frequency = 1.5f;

    /* Warm yellow-green base color */
    config->color.mode = PARTICLE_COLOR_SOLID;
    config->color.color_start = 0xFFDDFF66;

    /* Opacity controlled by callback */
    config->opacity.start = 200;
    config->opacity.end = 100;
    config->opacity.easing = PARTICLE_EASING_LINEAR;

    /* Small glowing dots */
    config->scale.start = 1.0f;
    config->scale.end = 1.0f;
    config->scale.min = 0.8f;
    config->scale.max = 1.5f;

    /* No boundary - handled by callback for wrap effect */
    config->boundary.behavior = PARTICLE_BOUNDARY_NONE;

    /* Additive blending for glow */
    config->render.blend_mode = PARTICLE_BLEND_ADDITIVE;
    config->render.base_size = 6.0f;

    /* Register callbacks */
    config->callbacks.on_particle_init = firefly_particle_init;
    config->callbacks.on_particle_update = firefly_particle_update;
    config->callbacks.user_data = NULL;
}

gui_particle_widget_t *effect_fireflies_demo_init(void)
{
    gui_obj_t *root = gui_obj_get_root();
    gui_dispdev_t *dc = gui_get_dc();
    s_screen_w = dc->screen_width;
    s_screen_h = dc->screen_height;

    s_fireflies_widget = gui_particle_widget_create(root, "fireflies_demo",
                                                    0, 0, s_screen_w, s_screen_h,
                                                    PARTICLE_POOL_SIZE);
    if (s_fireflies_widget == NULL)
    {
        gui_log("Fireflies: Failed to create widget\n");
        return NULL;
    }

    particle_effect_config_t config;
    effect_fireflies_config(&config);

    /* Update rect for actual screen size */
    config.shape.rect.w = (float)s_screen_w;
    config.shape.rect.h = (float)s_screen_h;

    gui_particle_widget_add_effect(s_fireflies_widget, &config);

    gui_log("Fireflies: Demo initialized - breathing glow effect\n");
    return s_fireflies_widget;
}
