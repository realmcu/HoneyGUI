/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
* @file particle_effect.c
* @brief Preset Particle Effects Implementation
*/

#include "particle_effect.h"
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* Float version of PI to avoid double-promotion warnings */
#define M_PI_F 3.14159265358979323846f

/* ============================================================================
 * Firework Effect Implementation
 * ============================================================================ */

particle_emitter_t *effect_firework_create(particle_platform_config_t *config,
                                           float x, float y, uint8_t particle_count)
{
    if (config == NULL)
    {
        return NULL;
    }

    /* Clamp particle count to valid range (20-100) */
    if (particle_count < 20)
    {
        particle_count = 20;
    }
    else if (particle_count > 100)
    {
        particle_count = 100;
    }

    particle_emitter_t *emitter = particle_emitter_create(config);
    if (emitter == NULL)
    {
        return NULL;
    }

    /* Configure point emission at explosion center */
    emitter_shape_config_t shape;
    shape.type = EMITTER_SHAPE_POINT;
    shape.point.x = x;
    shape.point.y = y;
    particle_emitter_set_shape(emitter, &shape);

    /* Configure gravity trajectory */
    trajectory_config_t trajectory;
    trajectory_config_init(&trajectory);
    trajectory.type = TRAJECTORY_GRAVITY;
    trajectory.gravity = 200.0f;  /* Moderate gravity for falling effect */
    trajectory.damping = 0.02f;   /* Light damping for air resistance */
    particle_emitter_set_trajectory(emitter, &trajectory);

    /* Spherical burst pattern */
    emitter->angle_min = 0.0f;
    emitter->angle_max = 2.0f * M_PI_F;  /* Full 360 degrees */

    /* Random velocity variation */
    emitter->speed_min = 100.0f;
    emitter->speed_max = 250.0f;

    /* Visual properties */
    emitter->scale_min = 0.5f;
    emitter->scale_max = 1.5f;
    emitter->rotation_min = 0.0f;
    emitter->rotation_max = 2.0f * M_PI_F;
    emitter->rotation_speed_min = -1.0f;
    emitter->rotation_speed_max = 1.0f;

    /* Lifecycle */
    emitter->life_min = 800;
    emitter->life_max = 1500;

    /* Opacity fade from 100% to 0% */
    emitter->opacity_start = 255;
    emitter->opacity_end = 0;

    /* Scale remains constant for firework */
    emitter->scale_start = 1.0f;
    emitter->scale_end = 0.3f;

    /* Disable continuous emission - firework is burst only */
    emitter->emit_rate = 1.0f;
    emitter->enabled = 0;

    return emitter;
}

/* ============================================================================
 * Trail Effect Implementation
 * ============================================================================ */

particle_emitter_t *effect_trail_create(particle_platform_config_t *config)
{
    if (config == NULL)
    {
        return NULL;
    }

    particle_emitter_t *emitter = particle_emitter_create(config);
    if (emitter == NULL)
    {
        return NULL;
    }

    /* Configure point emission (will follow pointer) */
    emitter_shape_config_t shape;
    shape.type = EMITTER_SHAPE_POINT;
    shape.point.x = 0.0f;
    shape.point.y = 0.0f;
    particle_emitter_set_shape(emitter, &shape);

    /* Enable follow mode */
    particle_emitter_enable_follow(emitter, 1);

    /* Configure damping trajectory */
    trajectory_config_t trajectory;
    trajectory_config_init(&trajectory);
    trajectory.type = TRAJECTORY_LINEAR;
    trajectory.damping = 0.15f;  /* Smooth deceleration */
    particle_emitter_set_trajectory(emitter, &trajectory);

    /* Initial velocity will be set by effect_trail_update */
    emitter->speed_min = 30.0f;
    emitter->speed_max = 80.0f;

    /* Angle will be set dynamically based on movement direction */
    emitter->angle_min = 0.0f;
    emitter->angle_max = 2.0f * M_PI_F;

    /* Visual properties - larger particles for better visibility */
    emitter->scale_min = 1.0f;
    emitter->scale_max = 1.8f;
    emitter->rotation_min = 0.0f;
    emitter->rotation_max = 0.0f;
    emitter->rotation_speed_min = 0.0f;
    emitter->rotation_speed_max = 0.0f;

    /* Longer lifecycle for longer trail */
    emitter->life_min = 400;
    emitter->life_max = 800;

    /* Scale fade from 100% to 0% */
    emitter->opacity_start = 255;
    emitter->opacity_end = 0;
    emitter->scale_start = 1.2f;
    emitter->scale_end = 0.2f;

    /* Higher emission rate for denser trail */
    emitter->emit_rate = 60.0f;
    emitter->enabled = 0;  /* Start disabled, enable when moving */

    return emitter;
}

void effect_trail_update(particle_emitter_t *emitter, float x, float y,
                         float prev_x, float prev_y)
{
    if (emitter == NULL)
    {
        return;
    }

    /* Calculate movement vector */
    float dx = x - prev_x;
    float dy = y - prev_y;
    float distance = sqrtf(dx * dx + dy * dy);

    /* Update follow position */
    particle_emitter_set_follow_position(emitter, x, y);

    /* Always enable when called - let the caller decide when to stop */
    particle_emitter_enable(emitter, 1);

    if (distance < 0.5f)
    {
        /* Pointer barely moving - emit particles in all directions */
        emitter->angle_min = 0.0f;
        emitter->angle_max = 2.0f * 3.14159f;
        particle_emitter_set_rate(emitter, 40.0f);
        return;
    }

    /* Calculate angle opposite to movement direction */
    float move_angle = atan2f(dy, dx);
    float opposite_angle = move_angle + 3.14159f;  /* 180 degrees opposite */

    /* Set emission angle range around opposite direction */
    float spread = 3.14159f / 3.0f;  /* 60 degree spread */
    emitter->angle_min = opposite_angle - spread;
    emitter->angle_max = opposite_angle + spread;

    /* Adjust emission rate based on movement speed */
    float speed_factor = distance / 5.0f;  /* Normalize speed */
    if (speed_factor < 1.0f) { speed_factor = 1.0f; }
    if (speed_factor > 4.0f) { speed_factor = 4.0f; }

    particle_emitter_set_rate(emitter, 40.0f * speed_factor);
}

/* ============================================================================
 * Touch Effect Implementation
 * ============================================================================ */

particle_emitter_t *effect_touch_create(particle_platform_config_t *config)
{
    if (config == NULL)
    {
        return NULL;
    }

    particle_emitter_t *emitter = particle_emitter_create(config);
    if (emitter == NULL)
    {
        return NULL;
    }

    /* Configure point emission at touch location */
    emitter_shape_config_t shape;
    shape.type = EMITTER_SHAPE_POINT;
    shape.point.x = 0.0f;
    shape.point.y = 0.0f;
    particle_emitter_set_shape(emitter, &shape);

    /* Enable follow mode for drag */
    particle_emitter_enable_follow(emitter, 1);

    /* Configure linear trajectory with light damping */
    trajectory_config_t trajectory;
    trajectory_config_init(&trajectory);
    trajectory.type = TRAJECTORY_LINEAR;
    trajectory.damping = 0.05f;
    particle_emitter_set_trajectory(emitter, &trajectory);

    /* Circular pattern around touch point */
    emitter->angle_min = 0.0f;
    emitter->angle_max = 2.0f * M_PI_F;

    /* Configurable expansion speed */
    emitter->speed_min = 50.0f;
    emitter->speed_max = 150.0f;

    /* Visual properties */
    emitter->scale_min = 0.5f;
    emitter->scale_max = 1.0f;
    emitter->rotation_min = 0.0f;
    emitter->rotation_max = 2.0f * M_PI_F;
    emitter->rotation_speed_min = -2.0f;
    emitter->rotation_speed_max = 2.0f;

    /* Lifecycle */
    emitter->life_min = 300;
    emitter->life_max = 600;

    /* Fade out */
    emitter->opacity_start = 255;
    emitter->opacity_end = 0;
    emitter->scale_start = 1.0f;
    emitter->scale_end = 0.5f;

    /* Emission rate for drag */
    emitter->emit_rate = 40.0f;
    emitter->enabled = 0;  /* Start disabled */

    return emitter;
}

void effect_touch_tap(particle_emitter_t *emitter, particle_pool_t *pool,
                      float x, float y)
{
    if (emitter == NULL || pool == NULL)
    {
        return;
    }

    /* Set touch position */
    particle_emitter_set_follow_position(emitter, x, y);

    /* Burst 5-20 particles */
    /* Use a random-ish count based on position */
    uint16_t count = 10 + ((uint16_t)(x + y) % 11);  /* 10-20 particles */
    if (count < 5) { count = 5; }
    if (count > 20) { count = 20; }

    particle_emitter_burst(emitter, pool, count);
}

void effect_touch_drag(particle_emitter_t *emitter, float x, float y)
{
    if (emitter == NULL)
    {
        return;
    }

    /* Update position and enable continuous emission */
    particle_emitter_set_follow_position(emitter, x, y);
    particle_emitter_enable(emitter, 1);
}

void effect_touch_release(particle_emitter_t *emitter, particle_pool_t *pool,
                          float x, float y)
{
    if (emitter == NULL || pool == NULL)
    {
        return;
    }

    /* Disable continuous emission */
    particle_emitter_enable(emitter, 0);

    /* Set release position */
    particle_emitter_set_follow_position(emitter, x, y);

    /* Store original speed range */
    float orig_speed_min = emitter->speed_min;
    float orig_speed_max = emitter->speed_max;

    /* Increase initial velocity for final burst */
    emitter->speed_min = orig_speed_min * 1.5f;
    emitter->speed_max = orig_speed_max * 1.5f;

    /* Final burst */
    particle_emitter_burst(emitter, pool, 15);

    /* Restore original speed range */
    emitter->speed_min = orig_speed_min;
    emitter->speed_max = orig_speed_max;
}

/* ============================================================================
 * Snow Effect Implementation
 * ============================================================================ */

particle_emitter_t *effect_snow_create(particle_platform_config_t *config,
                                       float screen_width)
{
    if (config == NULL)
    {
        return NULL;
    }

    particle_emitter_t *emitter = particle_emitter_create(config);
    if (emitter == NULL)
    {
        return NULL;
    }

    /* Configure line emission from top edge */
    emitter_shape_config_t shape;
    shape.type = EMITTER_SHAPE_LINE;
    shape.line.x1 = 0.0f;
    shape.line.y1 = 0.0f;
    shape.line.x2 = screen_width;
    shape.line.y2 = 0.0f;
    particle_emitter_set_shape(emitter, &shape);

    /* Configure gravity with horizontal sway */
    trajectory_config_t trajectory;
    trajectory_config_init(&trajectory);
    trajectory.type = TRAJECTORY_GRAVITY;
    trajectory.gravity = 30.0f;   /* Light gravity for slow falling */
    trajectory.wind_x = 10.0f;    /* Horizontal sway */
    trajectory.damping = 0.0f;    /* No damping for constant fall */
    particle_emitter_set_trajectory(emitter, &trajectory);

    /* Downward emission with slight variation */
    emitter->angle_min = M_PI_F / 2.0f - 0.3f;  /* Mostly downward */
    emitter->angle_max = M_PI_F / 2.0f + 0.3f;

    /* Slow initial speed */
    emitter->speed_min = 10.0f;
    emitter->speed_max = 30.0f;

    /* Visual properties - slow rotation for tumbling */
    emitter->scale_min = 0.3f;
    emitter->scale_max = 1.0f;
    emitter->rotation_min = 0.0f;
    emitter->rotation_max = 2.0f * M_PI_F;
    emitter->rotation_speed_min = -0.5f;
    emitter->rotation_speed_max = 0.5f;

    /* Long lifecycle for slow falling */
    emitter->life_min = 5000;
    emitter->life_max = 10000;

    /* Slight fade */
    emitter->opacity_start = 230;
    emitter->opacity_end = 100;
    emitter->scale_start = 1.0f;
    emitter->scale_end = 0.8f;

    /* Configurable emission rate */
    emitter->emit_rate = 10.0f;
    emitter->enabled = 1;

    return emitter;
}

void effect_snow_set_intensity(particle_emitter_t *emitter, float intensity)
{
    if (emitter == NULL)
    {
        return;
    }

    /* Clamp intensity to valid range */
    if (intensity < 0.0f) { intensity = 0.0f; }
    if (intensity > 1.0f) { intensity = 1.0f; }

    /* Adjust emission rate based on intensity */
    /* Range: 5-50 particles per second */
    float rate = 5.0f + intensity * 45.0f;
    particle_emitter_set_rate(emitter, rate);

    /* Adjust particle size based on intensity */
    emitter->scale_min = 0.2f + intensity * 0.3f;
    emitter->scale_max = 0.5f + intensity * 0.8f;
}

/* ============================================================================
 * Bubble Effect Implementation
 * ============================================================================ */

particle_emitter_t *effect_bubble_create(particle_platform_config_t *config,
                                         float screen_width, float screen_height)
{
    if (config == NULL)
    {
        return NULL;
    }

    (void)screen_height;  /* Used for boundary setup in particle system */

    particle_emitter_t *emitter = particle_emitter_create(config);
    if (emitter == NULL)
    {
        return NULL;
    }

    /* Configure line emission from bottom edge */
    emitter_shape_config_t shape;
    shape.type = EMITTER_SHAPE_LINE;
    shape.line.x1 = 0.0f;
    shape.line.y1 = screen_height;
    shape.line.x2 = screen_width;
    shape.line.y2 = screen_height;
    particle_emitter_set_shape(emitter, &shape);

    /* Configure negative gravity and horizontal sway */
    trajectory_config_t trajectory;
    trajectory_config_init(&trajectory);
    trajectory.type = TRAJECTORY_GRAVITY;
    trajectory.gravity = -50.0f;  /* Negative gravity for upward motion */
    trajectory.wind_x = 15.0f;    /* Horizontal sway */
    trajectory.damping = 0.01f;   /* Very light damping */
    particle_emitter_set_trajectory(emitter, &trajectory);

    /* Upward emission with variation */
    emitter->angle_min = -M_PI_F / 2.0f - 0.4f;  /* Mostly upward */
    emitter->angle_max = -M_PI_F / 2.0f + 0.4f;

    /* Slow initial speed */
    emitter->speed_min = 20.0f;
    emitter->speed_max = 60.0f;

    /* Visual properties - subtle size pulsation */
    emitter->scale_min = 0.5f;
    emitter->scale_max = 1.5f;
    emitter->rotation_min = 0.0f;
    emitter->rotation_max = 0.0f;  /* Bubbles don't rotate */
    emitter->rotation_speed_min = 0.0f;
    emitter->rotation_speed_max = 0.0f;

    /* Long lifecycle for slow rising */
    emitter->life_min = 4000;
    emitter->life_max = 8000;

    /* Slight transparency */
    emitter->opacity_start = 180;
    emitter->opacity_end = 100;

    /* Scale pulsation effect (simulated through start/end) */
    emitter->scale_start = 1.0f;
    emitter->scale_end = 1.2f;  /* Slight growth */

    /* Moderate emission rate */
    emitter->emit_rate = 8.0f;
    emitter->enabled = 1;

    return emitter;
}
