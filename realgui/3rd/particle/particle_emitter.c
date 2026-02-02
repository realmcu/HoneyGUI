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

    /* Default color: white solid */
    emitter->particle_color = 0xFFFFFFFF;
    emitter->color_mode = PARTICLE_COLOR_SOLID;
    emitter->color_start = 0xFFFFFFFF;
    emitter->color_end = 0xFFFFFFFF;
    emitter->palette_size = 0;

    /* Default opacity easing */
    emitter->opacity_easing = PARTICLE_EASING_LINEAR;

    /* Default rotation alignment */
    emitter->align_velocity = 0;

    /* Default state */
    emitter->particle_image = NULL;
    emitter->enabled = 1;
    emitter->follow_x = 0.0f;
    emitter->follow_y = 0.0f;
    emitter->follow_enabled = 0;

    /* Default burst mode */
    emitter->burst_enabled = 0;
    emitter->burst_count = 10;
    emitter->burst_interval = 0;
    emitter->last_burst_time = 0;

    /* Default lifecycle */
    emitter->auto_cleanup = 1;
    emitter->loop = 1;
    emitter->effect_duration = 0;
    emitter->start_time = 0;

    /* Default boundary */
    emitter->boundary_behavior = PARTICLE_BOUNDARY_NONE;
    emitter->boundary_left = 0.0f;
    emitter->boundary_top = 0.0f;
    emitter->boundary_right = 0.0f;
    emitter->boundary_bottom = 0.0f;
    emitter->boundary_reflect_damping = 0.8f;

    /* Default behavior mode */
    emitter->behavior_mode = PARTICLE_BEHAVIOR_NONE;
    emitter->pulse_frequency = 1.0f;
    emitter->pulse_amplitude = 0.5f;
    emitter->breathe_frequency = 1.0f;
    emitter->prev_follow_x = 0.0f;
    emitter->prev_follow_y = 0.0f;
    emitter->has_prev_position = 0;

    /* Default render */
    emitter->blend_mode = PARTICLE_BLEND_NORMAL;
    emitter->base_size = 8.0f;

    /* Default callbacks (NULL = no callback) */
    emitter->on_particle_init = NULL;
    emitter->on_particle_update = NULL;
    emitter->on_particle_render = NULL;
    emitter->on_particle_death = NULL;
    emitter->on_emitter_start = NULL;
    emitter->on_emitter_stop = NULL;
    emitter->user_data = NULL;

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

    /* Call on_emitter_stop callback if set */
    if (emitter->on_emitter_stop != NULL)
    {
        emitter->on_emitter_stop(emitter, emitter->user_data);
    }

    config->free_fn(emitter);
}

/* ============================================================================
 * Create from Configuration
 * ============================================================================ */

/**
 * @brief Convert particle_shape_config_t to emitter_shape_config_t
 */
static void convert_shape_config(const particle_shape_config_t *src,
                                 emitter_shape_config_t *dst)
{
    switch (src->type)
    {
    case PARTICLE_SHAPE_POINT:
        dst->type = EMITTER_SHAPE_POINT;
        dst->point.x = src->point.x;
        dst->point.y = src->point.y;
        break;
    case PARTICLE_SHAPE_LINE:
        dst->type = EMITTER_SHAPE_LINE;
        dst->line.x1 = src->line.x1;
        dst->line.y1 = src->line.y1;
        dst->line.x2 = src->line.x2;
        dst->line.y2 = src->line.y2;
        break;
    case PARTICLE_SHAPE_CIRCLE:
        dst->type = EMITTER_SHAPE_CIRCLE;
        dst->circle.cx = src->circle.cx;
        dst->circle.cy = src->circle.cy;
        dst->circle.radius = src->circle.radius;
        break;
    case PARTICLE_SHAPE_RECT:
        dst->type = EMITTER_SHAPE_RECT;
        dst->rect.x = src->rect.x;
        dst->rect.y = src->rect.y;
        dst->rect.w = src->rect.w;
        dst->rect.h = src->rect.h;
        break;
    case PARTICLE_SHAPE_RING:
        dst->type = EMITTER_SHAPE_RING;
        dst->ring.cx = src->ring.cx;
        dst->ring.cy = src->ring.cy;
        dst->ring.inner_r = src->ring.inner_r;
        dst->ring.outer_r = src->ring.outer_r;
        break;
    default:
        dst->type = EMITTER_SHAPE_POINT;
        dst->point.x = 0.0f;
        dst->point.y = 0.0f;
        break;
    }
}

/**
 * @brief Convert particle_trajectory_config_t to trajectory_config_t
 */
static void convert_trajectory_config(const particle_trajectory_config_t *src,
                                      trajectory_config_t *dst)
{
    switch (src->type)
    {
    case PARTICLE_TRAJECTORY_LINEAR:
        dst->type = TRAJECTORY_LINEAR;
        break;
    case PARTICLE_TRAJECTORY_GRAVITY:
        dst->type = TRAJECTORY_GRAVITY;
        break;
    case PARTICLE_TRAJECTORY_ORBIT:
        dst->type = TRAJECTORY_SPIRAL;
        break;
    default:
        dst->type = TRAJECTORY_LINEAR;
        break;
    }

    dst->gravity = trajectory_clamp_gravity(src->gravity);
    dst->damping = trajectory_clamp_damping(src->damping);
    dst->wind_x = src->wind_x;
    dst->wind_y = src->wind_y;
    dst->spiral_cx = src->orbit_cx;
    dst->spiral_cy = src->orbit_cy;
    dst->spiral_speed = src->orbit_speed;
    dst->custom_cb = NULL;
    dst->custom_data = NULL;
}

particle_emitter_t *particle_emitter_create_from_config(
    const particle_effect_config_t *effect_config,
    particle_platform_config_t *platform_config)
{
    if (effect_config == NULL || platform_config == NULL ||
        platform_config->malloc_fn == NULL)
    {
        return NULL;
    }

    /* Validate configuration first */
    if (particle_effect_config_validate(effect_config) != PARTICLE_CONFIG_OK)
    {
        return NULL;
    }

    /* Allocate emitter */
    particle_emitter_t *emitter = (particle_emitter_t *)platform_config->malloc_fn(
                                      sizeof(particle_emitter_t));
    if (emitter == NULL)
    {
        return NULL;
    }

    /* Initialize to defaults first */
    particle_emitter_init(emitter);

    /* Use time function for random seed if available */
    if (platform_config->get_time_fn != NULL)
    {
        emitter->random_seed = platform_config->get_time_fn();
        emitter->start_time = platform_config->get_time_fn();
    }

    /* Apply shape configuration */
    convert_shape_config(&effect_config->shape, &emitter->shape);

    /* Apply trajectory configuration */
    convert_trajectory_config(&effect_config->trajectory, &emitter->trajectory);

    /* Apply emission parameters */
    emitter->angle_min = effect_config->emission.angle_min;
    emitter->angle_max = effect_config->emission.angle_max;
    emitter->speed_min = effect_config->emission.speed_min;
    emitter->speed_max = effect_config->emission.speed_max;
    emitter->emit_rate = particle_emitter_clamp_rate(effect_config->emission.rate);
    emitter->burst_count = effect_config->emission.burst_count;
    emitter->burst_interval = effect_config->emission.burst_interval;
    emitter->burst_enabled = effect_config->emission.burst_enabled;

    /* Apply lifecycle configuration */
    emitter->life_min = effect_config->lifecycle.life_min;
    emitter->life_max = effect_config->lifecycle.life_max;
    emitter->effect_duration = effect_config->lifecycle.effect_duration;
    emitter->auto_cleanup = effect_config->lifecycle.auto_cleanup;
    emitter->loop = effect_config->lifecycle.loop;

    /* Apply color configuration */
    emitter->color_mode = effect_config->color.mode;
    emitter->color_start = effect_config->color.color_start;
    emitter->color_end = effect_config->color.color_end;
    emitter->particle_color = effect_config->color.color_start;
    emitter->palette_size = effect_config->color.palette_size;
    for (int i = 0; i < 8 && i < effect_config->color.palette_size; i++)
    {
        emitter->color_palette[i] = effect_config->color.palette[i];
    }

    /* Apply opacity configuration */
    emitter->opacity_start = effect_config->opacity.start;
    emitter->opacity_end = effect_config->opacity.end;
    emitter->opacity_easing = effect_config->opacity.easing;

    /* Apply scale configuration */
    emitter->scale_start = effect_config->scale.start;
    emitter->scale_end = effect_config->scale.end;
    emitter->scale_min = effect_config->scale.min;
    emitter->scale_max = effect_config->scale.max;

    /* Apply rotation configuration */
    emitter->rotation_min = effect_config->rotation.angle_min;
    emitter->rotation_max = effect_config->rotation.angle_max;
    emitter->rotation_speed_min = effect_config->rotation.speed_min;
    emitter->rotation_speed_max = effect_config->rotation.speed_max;
    emitter->align_velocity = effect_config->rotation.align_velocity;

    /* Apply boundary configuration */
    emitter->boundary_behavior = effect_config->boundary.behavior;
    emitter->boundary_left = effect_config->boundary.left;
    emitter->boundary_top = effect_config->boundary.top;
    emitter->boundary_right = effect_config->boundary.right;
    emitter->boundary_bottom = effect_config->boundary.bottom;
    emitter->boundary_reflect_damping = effect_config->boundary.reflect_damping;

    /* Apply behavior mode configuration */
    emitter->behavior_mode = effect_config->behavior.mode;
    emitter->pulse_frequency = effect_config->behavior.pulse_frequency;
    emitter->pulse_amplitude = effect_config->behavior.pulse_amplitude;
    emitter->breathe_frequency = effect_config->behavior.breathe_frequency;

    /* Apply render configuration */
    emitter->blend_mode = effect_config->render.blend_mode;
    emitter->particle_image = effect_config->render.texture;
    emitter->base_size = effect_config->render.base_size;

    /* Apply callback configuration */
    emitter->on_particle_init = effect_config->callbacks.on_particle_init;
    emitter->on_particle_update = effect_config->callbacks.on_particle_update;
    emitter->on_particle_render = effect_config->callbacks.on_particle_render;
    emitter->on_particle_death = effect_config->callbacks.on_particle_death;
    emitter->on_emitter_start = effect_config->callbacks.on_emitter_start;
    emitter->on_emitter_stop = effect_config->callbacks.on_emitter_stop;
    emitter->user_data = effect_config->callbacks.user_data;

    /* Set follow mode based on behavior */
    if (emitter->behavior_mode == PARTICLE_BEHAVIOR_FOLLOW_TOUCH ||
        emitter->behavior_mode == PARTICLE_BEHAVIOR_TRAIL ||
        emitter->behavior_mode == PARTICLE_BEHAVIOR_TOUCH_FEEDBACK)
    {
        emitter->follow_enabled = 1;
    }

    /* Call on_emitter_start callback if set */
    if (emitter->on_emitter_start != NULL)
    {
        emitter->on_emitter_start(emitter, emitter->user_data);
    }

    return emitter;
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

    case EMITTER_SHAPE_RING:
        {
            /* Random point within ring (between inner and outer radius) */
            float angle = emitter_random_float(emitter) * 2.0f * M_PI_F;
            float inner_r = emitter->shape.ring.inner_r;
            float outer_r = emitter->shape.ring.outer_r;
            /* Uniform distribution in ring area */
            float r_sq = inner_r * inner_r +
                         emitter_random_float(emitter) * (outer_r * outer_r - inner_r * inner_r);
            float r = sqrtf(r_sq);
            base_x = emitter->shape.ring.cx + r * cosf(angle);
            base_y = emitter->shape.ring.cy + r * sinf(angle);
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

    /* Set color gradient properties */
    particle->color_start = emitter->color_start;
    particle->color_end = emitter->color_end;
    particle->color_mode = (uint8_t)emitter->color_mode;

    /* Set opacity easing */
    particle->opacity_easing = (uint8_t)emitter->opacity_easing;

    /* Set rotation alignment */
    particle->align_velocity = emitter->align_velocity;

    /* Set emitter reference for callbacks */
    particle->emitter_ref = emitter;

    /* Set color based on color mode */
    switch (emitter->color_mode)
    {
    case PARTICLE_COLOR_SOLID:
        particle->color = emitter->color_start;
        break;
    case PARTICLE_COLOR_RANDOM:
        if (emitter->palette_size > 0)
        {
            uint32_t idx = emitter_random(emitter) % emitter->palette_size;
            particle->color = emitter->color_palette[idx];
        }
        else
        {
            particle->color = emitter->particle_color;
        }
        break;
    case PARTICLE_COLOR_GRADIENT:
    case PARTICLE_COLOR_RAINBOW:
        /* Start with start color, will be interpolated during update */
        particle->color = emitter->color_start;
        break;
    default:
        particle->color = emitter->particle_color;
        break;
    }

    /* Set image */
    particle->image = emitter->particle_image;

    /* Call on_particle_init callback if set */
    if (emitter->on_particle_init != NULL)
    {
        emitter->on_particle_init(particle, emitter->user_data);
    }
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
            /* Pool is full, stop emitting */
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
            /* Pool is full, stop emitting */
            break;
        }

        /* Initialize the particle */
        emitter_init_particle(emitter, particle);
    }
}
