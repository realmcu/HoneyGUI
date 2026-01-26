/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file particle_emitter.c
 * @brief Particle Emitter Implementation
 */

#include "particle_emitter.h"
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* Float version of PI to avoid double-promotion warnings */
#define M_PI_F 3.14159265358979323846f

/* ============================================================================
 * Internal Random Number Generator
 * ============================================================================ */

/**
 * @brief Simple LCG random number generator
 */
static uint32_t emitter_random(particle_emitter_t *emitter)
{
    emitter->random_seed = emitter->random_seed * 1103515245 + 12345;
    return (emitter->random_seed >> 16) & 0x7FFF;
}

/**
 * @brief Generate random float in range [0, 1]
 */
static float emitter_random_float(particle_emitter_t *emitter)
{
    return (float)emitter_random(emitter) / 32767.0f;
}

/**
 * @brief Generate random float in range [min, max]
 */
static float emitter_random_range(particle_emitter_t *emitter, float min, float max)
{
    return min + emitter_random_float(emitter) * (max - min);
}

/* ============================================================================
 * Emitter Creation and Destruction
 * ============================================================================ */

void particle_emitter_init(particle_emitter_t *emitter)
{
    if (emitter == NULL)
    {
        return;
    }

    memset(emitter, 0, sizeof(particle_emitter_t));

    /* Default shape: point at origin */
    emitter->shape.type = EMITTER_SHAPE_POINT;
    emitter->shape.point.x = 0.0f;
    emitter->shape.point.y = 0.0f;

    /* Default trajectory */
    trajectory_config_init(&emitter->trajectory);

    /* Default emission rate (clamped to valid range) */
    emitter->emit_rate = 10.0f;
    emitter->emit_accumulator = 0.0f;

    /* Default particle property ranges */
    emitter->speed_min = 50.0f;
    emitter->speed_max = 100.0f;
    emitter->angle_min = 0.0f;
    emitter->angle_max = 2.0f * M_PI_F;
    emitter->scale_min = 1.0f;
    emitter->scale_max = 1.0f;
    emitter->rotation_min = 0.0f;
    emitter->rotation_max = 0.0f;
    emitter->rotation_speed_min = 0.0f;
    emitter->rotation_speed_max = 0.0f;
    emitter->life_min = 1000;
    emitter->life_max = 2000;

    /* Default gradient */
    emitter->opacity_start = 255;
    emitter->opacity_end = 0;
    emitter->scale_start = 1.0f;
    emitter->scale_end = 0.0f;

    /* Default color: white */
    emitter->particle_color = 0xFFFFFFFF;

    /* Default state */
    emitter->particle_image = NULL;
    emitter->enabled = 1;
    emitter->follow_x = 0.0f;
    emitter->follow_y = 0.0f;
    emitter->follow_enabled = 0;

    /* Initialize random seed */
    emitter->random_seed = 12345;
}

particle_emitter_t *particle_emitter_create(particle_platform_config_t *config)
{
    if (config == NULL || config->malloc_fn == NULL)
    {
        return NULL;
    }

    particle_emitter_t *emitter = (particle_emitter_t *)config->malloc_fn(sizeof(particle_emitter_t));
    if (emitter == NULL)
    {
        return NULL;
    }

    particle_emitter_init(emitter);

    /* Use time function for random seed if available */
    if (config->get_time_fn != NULL)
    {
        emitter->random_seed = config->get_time_fn();
    }

    return emitter;
}

void particle_emitter_destroy(particle_emitter_t *emitter, particle_platform_config_t *config)
{
    if (emitter == NULL || config == NULL || config->free_fn == NULL)
    {
        return;
    }

    config->free_fn(emitter);
}

/* ============================================================================
 * Configuration Functions
 * ============================================================================ */

void particle_emitter_set_shape(particle_emitter_t *emitter, emitter_shape_config_t *shape)
{
    if (emitter == NULL || shape == NULL)
    {
        return;
    }

    memcpy(&emitter->shape, shape, sizeof(emitter_shape_config_t));
}

void particle_emitter_set_trajectory(particle_emitter_t *emitter, trajectory_config_t *trajectory)
{
    if (emitter == NULL || trajectory == NULL)
    {
        return;
    }

    memcpy(&emitter->trajectory, trajectory, sizeof(trajectory_config_t));
}

float particle_emitter_clamp_rate(float rate)
{
    if (rate < EMITTER_RATE_MIN)
    {
        return EMITTER_RATE_MIN;
    }
    if (rate > EMITTER_RATE_MAX)
    {
        return EMITTER_RATE_MAX;
    }
    return rate;
}

void particle_emitter_set_rate(particle_emitter_t *emitter, float rate)
{
    if (emitter == NULL)
    {
        return;
    }

    emitter->emit_rate = particle_emitter_clamp_rate(rate);
}

void particle_emitter_set_color(particle_emitter_t *emitter, uint32_t color)
{
    if (emitter == NULL)
    {
        return;
    }

    emitter->particle_color = color;
}

void particle_emitter_set_image(particle_emitter_t *emitter, void *image)
{
    if (emitter == NULL)
    {
        return;
    }

    emitter->particle_image = image;
}

void particle_emitter_set_follow_position(particle_emitter_t *emitter, float x, float y)
{
    if (emitter == NULL)
    {
        return;
    }

    emitter->follow_x = x;
    emitter->follow_y = y;
}

void particle_emitter_enable_follow(particle_emitter_t *emitter, uint8_t enable)
{
    if (emitter == NULL)
    {
        return;
    }

    emitter->follow_enabled = enable ? 1 : 0;
}

void particle_emitter_enable(particle_emitter_t *emitter, uint8_t enable)
{
    if (emitter == NULL)
    {
        return;
    }

    emitter->enabled = enable ? 1 : 0;
}


/* ============================================================================
 * Position Generation
 * ============================================================================ */

void particle_emitter_get_emit_position(particle_emitter_t *emitter, float *out_x, float *out_y)
{
    if (emitter == NULL || out_x == NULL || out_y == NULL)
    {
        return;
    }

    float base_x = 0.0f;
    float base_y = 0.0f;

    switch (emitter->shape.type)
    {
    case EMITTER_SHAPE_POINT:
        base_x = emitter->shape.point.x;
        base_y = emitter->shape.point.y;
        break;

    case EMITTER_SHAPE_LINE:
        {
            /* Random point along line segment */
            float t = emitter_random_float(emitter);
            base_x = emitter->shape.line.x1 + t * (emitter->shape.line.x2 - emitter->shape.line.x1);
            base_y = emitter->shape.line.y1 + t * (emitter->shape.line.y2 - emitter->shape.line.y1);
            break;
        }

    case EMITTER_SHAPE_CIRCLE:
        {
            /* Random point within circle (uniform distribution) */
            float angle = emitter_random_float(emitter) * 2.0f * M_PI_F;
            float r = emitter->shape.circle.radius * sqrtf(emitter_random_float(emitter));
            base_x = emitter->shape.circle.cx + r * cosf(angle);
            base_y = emitter->shape.circle.cy + r * sinf(angle);
            break;
        }

    case EMITTER_SHAPE_RECT:
        {
            /* Random point within rectangle */
            base_x = emitter->shape.rect.x + emitter_random_float(emitter) * emitter->shape.rect.w;
            base_y = emitter->shape.rect.y + emitter_random_float(emitter) * emitter->shape.rect.h;
            break;
        }

    default:
        base_x = 0.0f;
        base_y = 0.0f;
        break;
    }

    /* Apply follow offset if enabled */
    if (emitter->follow_enabled)
    {
        base_x += emitter->follow_x;
        base_y += emitter->follow_y;
    }

    *out_x = base_x;
    *out_y = base_y;
}

/* ============================================================================
 * Particle Initialization
 * ============================================================================ */

/**
 * @brief Initialize a newly allocated particle with emitter settings
 */
static void emitter_init_particle(particle_emitter_t *emitter, particle_t *particle)
{
    if (emitter == NULL || particle == NULL)
    {
        return;
    }

    /* Set position */
    particle_emitter_get_emit_position(emitter, &particle->x, &particle->y);

    /* Set velocity based on speed and angle ranges */
    float speed = emitter_random_range(emitter, emitter->speed_min, emitter->speed_max);
    float angle = emitter_random_range(emitter, emitter->angle_min, emitter->angle_max);
    particle->vx = speed * cosf(angle);
    particle->vy = speed * sinf(angle);

    /* Set acceleration from trajectory */
    particle->ax = emitter->trajectory.wind_x;
    particle->ay = emitter->trajectory.wind_y;
    if (emitter->trajectory.type == TRAJECTORY_GRAVITY)
    {
        particle->ay += emitter->trajectory.gravity;
    }

    /* Set visual properties */
    particle->scale = emitter_random_range(emitter, emitter->scale_min, emitter->scale_max);
    particle->rotation = emitter_random_range(emitter, emitter->rotation_min, emitter->rotation_max);
    particle->rotation_speed = emitter_random_range(emitter,
                                                    emitter->rotation_speed_min,
                                                    emitter->rotation_speed_max);

    /* Set lifecycle */
    uint16_t life = (uint16_t)emitter_random_range(emitter,
                                                   (float)emitter->life_min,
                                                   (float)emitter->life_max);
    particle->life = life;
    particle->max_life = life;

    /* Set gradient properties */
    particle->opacity_start = emitter->opacity_start;
    particle->opacity_end = emitter->opacity_end;
    particle->opacity = emitter->opacity_start;
    particle->scale_start = emitter->scale_start;
    particle->scale_end = emitter->scale_end;

    /* Set color */
    particle->color = emitter->particle_color;

    /* Set image */
    particle->image = emitter->particle_image;
}


/* ============================================================================
 * Emission Functions
 * ============================================================================ */

void particle_emitter_emit(particle_emitter_t *emitter, particle_pool_t *pool, float dt)
{
    if (emitter == NULL || pool == NULL || dt <= 0.0f)
    {
        return;
    }

    if (!emitter->enabled)
    {
        return;
    }

    /* Accumulate particles to emit based on rate and delta time */
    emitter->emit_accumulator += emitter->emit_rate * dt;

    /* Emit whole particles */
    while (emitter->emit_accumulator >= 1.0f)
    {
        /* Try to allocate a particle from the pool */
        particle_t *particle = particle_pool_alloc(pool);
        if (particle == NULL)
        {
            /* Pool is full, stop emitting (Requirements 2.5) */
            emitter->emit_accumulator = 0.0f;
            break;
        }

        /* Initialize the particle */
        emitter_init_particle(emitter, particle);

        emitter->emit_accumulator -= 1.0f;
    }
}

void particle_emitter_burst(particle_emitter_t *emitter, particle_pool_t *pool, uint16_t count)
{
    if (emitter == NULL || pool == NULL || count == 0)
    {
        return;
    }

    /* Emit specified number of particles immediately */
    for (uint16_t i = 0; i < count; i++)
    {
        /* Try to allocate a particle from the pool */
        particle_t *particle = particle_pool_alloc(pool);
        if (particle == NULL)
        {
            /* Pool is full, stop emitting (Requirements 2.5) */
            break;
        }

        /* Initialize the particle */
        emitter_init_particle(emitter, particle);
    }
}
