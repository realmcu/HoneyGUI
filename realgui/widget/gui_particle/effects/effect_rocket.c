/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_rocket.c
 * @brief Rocket Thruster Exhaust Particle Effect
 *
 * Creates a realistic rocket exhaust/thruster effect with:
 * - Main flame: bright yellow-orange particles shooting downward
 * - Core glow: intense white-yellow center
 * - Smoke trail: darker particles that linger longer
 */

#include "effect_rocket.h"
#include "gui_obj.h"
#include "gui_api_os.h"
#include "gui_api_dc.h"
#include "def_type.h"

#define PARTICLE_POOL_SIZE 512

/* Exhaust direction: lower-left, opposite to rocket flight direction */
/* In this coordinate system: 0°=right, 90°=down, 180°=left, 270°=up */
/* Rocket flies upper-right, so exhaust goes lower-left (around 135°) */
#define EXHAUST_DIRECTION  (135.0f * M_PI_F / 180.0f)
/* Cone spread angle: +/- 25 degrees for wider flame */
#define EXHAUST_SPREAD     (M_PI_F / 7.2f)

/*============================================================================*
 *                           Static Configuration
 *============================================================================*/

static void effect_rocket_flame_config(particle_effect_config_t *config,
                                       float nozzle_x, float nozzle_y,
                                       int16_t w, int16_t h)
{
    if (config == NULL)
    {
        return;
    }

    particle_effect_config_init(config);

    /* Point emission from rocket nozzle (local coordinates relative to widget) */
    config->shape.type = PARTICLE_SHAPE_POINT;
    config->shape.point.x = nozzle_x;
    config->shape.point.y = nozzle_y;

    /* Linear trajectory with slight gravity pull */
    config->trajectory.type = PARTICLE_TRAJECTORY_LINEAR;
    config->trajectory.gravity = 50.0f;
    config->trajectory.damping = 0.02f;

    /* Conical emission: mostly downward with spread */
    config->emission.angle_min = EXHAUST_DIRECTION - EXHAUST_SPREAD;
    config->emission.angle_max = EXHAUST_DIRECTION + EXHAUST_SPREAD;
    config->emission.speed_min = 180.0f;
    config->emission.speed_max = 320.0f;
    config->emission.rate = 120.0f;
    config->emission.burst_enabled = 0;

    /* Short lifetime for sharp flame effect */
    config->lifecycle.life_min = 300;
    config->lifecycle.life_max = 500;
    config->lifecycle.auto_cleanup = 0;
    config->lifecycle.loop = 1;

    /* Gradient: bright yellow -> orange -> red */
    config->color.mode = PARTICLE_COLOR_GRADIENT;
    config->color.color_start = 0xFFFFFF00;
    config->color.color_end = 0xFFFF4400;

    /* Fade out quickly */
    config->opacity.start = 255;
    config->opacity.end = 0;
    config->opacity.easing = PARTICLE_EASING_EASE_OUT;

    /* Scale: start larger, shrink as they cool */
    config->scale.start = 1.5f;
    config->scale.end = 0.4f;
    config->scale.min = 1.0f;
    config->scale.max = 1.5f;

    /* Align particles to velocity direction */
    config->rotation.align_velocity = 1;

    /* Kill at widget boundary */
    config->boundary.behavior = PARTICLE_BOUNDARY_KILL;
    config->boundary.left = 0.0f;
    config->boundary.top = 0.0f;
    config->boundary.right = (float)w;
    config->boundary.bottom = (float)h;

    /* Additive blending for bright glow */
    config->render.blend_mode = PARTICLE_BLEND_ADDITIVE;
    config->render.base_size = 10.0f;
}

/**
 * @brief Configure smoke trail effect (secondary layer)
 */
static void effect_rocket_smoke_config(particle_effect_config_t *config,
                                       float nozzle_x, float nozzle_y,
                                       int16_t w, int16_t h)
{
    if (config == NULL)
    {
        return;
    }

    particle_effect_config_init(config);

    /* Point emission slightly behind the flame */
    config->shape.type = PARTICLE_SHAPE_POINT;
    config->shape.point.x = nozzle_x;
    config->shape.point.y = nozzle_y + 15.0f;

    /* Linear with more spread and slower speed */
    config->trajectory.type = PARTICLE_TRAJECTORY_LINEAR;
    config->trajectory.gravity = -30.0f;
    config->trajectory.damping = 0.05f;

    /* Wider spread for smoke dispersion */
    config->emission.angle_min = EXHAUST_DIRECTION - (EXHAUST_SPREAD * 1.5f);
    config->emission.angle_max = EXHAUST_DIRECTION + (EXHAUST_SPREAD * 1.5f);
    config->emission.speed_min = 40.0f;
    config->emission.speed_max = 80.0f;
    config->emission.rate = 35.0f;
    config->emission.burst_enabled = 0;

    /* Longer lifetime for lingering smoke */
    config->lifecycle.life_min = 600;
    config->lifecycle.life_max = 1000;
    config->lifecycle.auto_cleanup = 0;
    config->lifecycle.loop = 1;

    /* Gray smoke color */
    config->color.mode = PARTICLE_COLOR_GRADIENT;
    config->color.color_start = 0xFF888888;
    config->color.color_end = 0xFF444444;

    /* Slow fade */
    config->opacity.start = 150;
    config->opacity.end = 0;
    config->opacity.easing = PARTICLE_EASING_LINEAR;

    /* Smoke expands as it rises */
    config->scale.start = 0.8f;
    config->scale.end = 2.0f;
    config->scale.min = 0.6f;
    config->scale.max = 1.0f;

    /* Kill at widget boundary */
    config->boundary.behavior = PARTICLE_BOUNDARY_KILL;
    config->boundary.left = 0.0f;
    config->boundary.top = 0.0f;
    config->boundary.right = (float)w;
    config->boundary.bottom = (float)h;

    /* Normal blending for smoke */
    config->render.blend_mode = PARTICLE_BLEND_NORMAL;
    config->render.base_size = 10.0f;
}

/*============================================================================*
 *                           User Creation API
 *============================================================================*/

gui_particle_widget_t *effect_rocket_create(gui_obj_t *parent, const char *name,
                                            int16_t x, int16_t y, int16_t w, int16_t h)
{
    /* Default nozzle at upper-right area (90% w, 10% h) */
    float nozzle_x = (float)w * 0.9f;
    float nozzle_y = (float)h * 0.1f;

    gui_particle_widget_t *widget = gui_particle_widget_create(parent, name,
                                                               x, y, w, h,
                                                               PARTICLE_POOL_SIZE);
    if (widget == NULL)
    {
        return NULL;
    }

    /* Add smoke layer first (renders behind flame) */
    particle_effect_config_t smoke_config;
    effect_rocket_smoke_config(&smoke_config, nozzle_x, nozzle_y, w, h);
    gui_particle_widget_add_effect(widget, &smoke_config);

    /* Add main flame effect */
    particle_effect_config_t flame_config;
    effect_rocket_flame_config(&flame_config, nozzle_x, nozzle_y, w, h);
    gui_particle_widget_add_effect(widget, &flame_config);

    return widget;
}


