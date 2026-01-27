/**
 * @file particle_system.c
 * @brief Particle System Core Controller Implementation
 *
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "particle_system.h"
#include "particle_trajectory.h"
#include <string.h>

/* ============================================================================
 * Helper Functions
 * ============================================================================ */

/**
 * @brief Clamp a float value to a range
 */
static float clamp_float(float value, float min, float max)
{
    if (value < min) { return min; }
    if (value > max) { return max; }
    return value;
}

/**
 * @brief Linear interpolation between two values
 */
static float lerp(float start, float end, float t)
{
    return start + (end - start) * t;
}

/**
 * @brief Linear interpolation for uint8_t values
 */
static uint8_t lerp_u8(uint8_t start, uint8_t end, float t)
{
    float result = (float)start + ((float)end - (float)start) * t;
    if (result < 0.0f) { return 0; }
    if (result > 255.0f) { return 255; }
    return (uint8_t)result;
}

/* ============================================================================
 * System Creation and Destruction
 * ============================================================================ */

particle_system_t *particle_system_create(uint16_t pool_capacity,
                                          particle_platform_config_t *config)
{
    if (config == NULL || config->malloc_fn == NULL || config->free_fn == NULL)
    {
        return NULL;
    }

    if (pool_capacity == 0)
    {
        return NULL;
    }

    /* Allocate particle system structure */
    particle_system_t *ps = (particle_system_t *)config->malloc_fn(sizeof(particle_system_t));
    if (ps == NULL)
    {
        return NULL;
    }

    /* Initialize to zero */
    memset(ps, 0, sizeof(particle_system_t));

    /* Store platform configuration */
    memcpy(&ps->platform, config, sizeof(particle_platform_config_t));

    /* Create particle pool */
    ps->pool = particle_pool_create(pool_capacity, config);
    if (ps->pool == NULL)
    {
        config->free_fn(ps);
        return NULL;
    }

    /* Initialize emitter array */
    ps->emitter_count = 0;
    for (int i = 0; i < PARTICLE_SYSTEM_MAX_EMITTERS; i++)
    {
        ps->emitters[i] = NULL;
    }

    /* Initialize timing */
    ps->last_update_time = 0;
    if (config->get_time_fn != NULL)
    {
        ps->last_update_time = config->get_time_fn();
    }

    /* Initialize auto-throttle (default: enabled at 80% threshold) */
    ps->auto_throttle = 1;
    ps->throttle_threshold = PARTICLE_SYSTEM_DEFAULT_THROTTLE_THRESHOLD;
    ps->throttle_factor = 1.0f;

    /* Initialize boundaries (disabled by default) */
    ps->bound_left = 0.0f;
    ps->bound_top = 0.0f;
    ps->bound_right = 0.0f;
    ps->bound_bottom = 0.0f;
    ps->bound_enabled = 0;
    ps->bound_behavior = BOUNDARY_BEHAVIOR_KILL;

    /* Initialize render callback */
    ps->render_cb = NULL;
    ps->render_ctx = NULL;

    return ps;
}

void particle_system_destroy(particle_system_t *ps)
{
    if (ps == NULL)
    {
        return;
    }

    /* Destroy particle pool */
    if (ps->pool != NULL)
    {
        particle_pool_destroy(ps->pool, &ps->platform);
        ps->pool = NULL;
    }

    /* Note: Emitters are not destroyed here - they are managed externally */
    /* Clear emitter references */
    for (int i = 0; i < PARTICLE_SYSTEM_MAX_EMITTERS; i++)
    {
        ps->emitters[i] = NULL;
    }
    ps->emitter_count = 0;

    /* Free the system structure */
    if (ps->platform.free_fn != NULL)
    {
        ps->platform.free_fn(ps);
    }
}

/* ============================================================================
 * Emitter Management
 * ============================================================================ */

uint8_t particle_system_add_emitter(particle_system_t *ps, particle_emitter_t *emitter)
{
    if (ps == NULL || emitter == NULL)
    {
        return 0;
    }

    /* Check if already at max emitters */
    if (ps->emitter_count >= PARTICLE_SYSTEM_MAX_EMITTERS)
    {
        return 0;
    }

    /* Check if emitter is already added */
    for (int i = 0; i < ps->emitter_count; i++)
    {
        if (ps->emitters[i] == emitter)
        {
            return 0; /* Already added */
        }
    }

    /* Add emitter */
    ps->emitters[ps->emitter_count] = emitter;
    ps->emitter_count++;

    return 1;
}

void particle_system_remove_emitter(particle_system_t *ps, particle_emitter_t *emitter)
{
    if (ps == NULL || emitter == NULL)
    {
        return;
    }

    /* Find and remove emitter */
    for (int i = 0; i < ps->emitter_count; i++)
    {
        if (ps->emitters[i] == emitter)
        {
            /* Shift remaining emitters down */
            for (int j = i; j < ps->emitter_count - 1; j++)
            {
                ps->emitters[j] = ps->emitters[j + 1];
            }
            ps->emitters[ps->emitter_count - 1] = NULL;
            ps->emitter_count--;
            return;
        }
    }
}


/* ============================================================================
 * Particle Update Functions
 * ============================================================================ */

/**
 * @brief Update particle lifecycle and gradients
 *
 * Updates particle life, opacity gradient, and scale gradient based on
 * remaining life ratio.
 */
static void particle_update_lifecycle(particle_t *particle, float dt_ms)
{
    if (particle == NULL || !particle->active)
    {
        return;
    }

    /* Update life (in milliseconds) */
    if (particle->life > 0)
    {
        uint16_t dt_life = (uint16_t)dt_ms;
        if (dt_life > particle->life)
        {
            particle->life = 0;
        }
        else
        {
            particle->life -= dt_life;
        }
    }

    /* Calculate life ratio (0.0 = dead, 1.0 = just born) */
    float life_ratio = 0.0f;
    if (particle->max_life > 0)
    {
        life_ratio = (float)particle->life / (float)particle->max_life;
    }

    /* Apply opacity gradient (linear interpolation) */
    /* At life_ratio=1.0 (start), opacity = opacity_start */
    /* At life_ratio=0.0 (end), opacity = opacity_end */
    particle->opacity = lerp_u8(particle->opacity_end, particle->opacity_start, life_ratio);

    /* Apply scale gradient (linear interpolation) */
    /* At life_ratio=1.0 (start), scale = scale_start */
    /* At life_ratio=0.0 (end), scale = scale_end */
    particle->scale = lerp(particle->scale_end, particle->scale_start, life_ratio);
}

/**
 * @brief Check and handle particle boundary collision
 *
 * Returns 1 if particle should be killed, 0 otherwise.
 */
static uint8_t particle_check_boundary(particle_system_t *ps, particle_t *particle)
{
    if (ps == NULL || particle == NULL || !ps->bound_enabled)
    {
        return 0;
    }

    uint8_t out_of_bounds = 0;
    uint8_t hit_left = particle->x < ps->bound_left;
    uint8_t hit_right = particle->x > ps->bound_right;
    uint8_t hit_top = particle->y < ps->bound_top;
    uint8_t hit_bottom = particle->y > ps->bound_bottom;

    if (hit_left || hit_right || hit_top || hit_bottom)
    {
        out_of_bounds = 1;
    }

    if (!out_of_bounds)
    {
        return 0;
    }

    switch (ps->bound_behavior)
    {
    case BOUNDARY_BEHAVIOR_KILL:
        /* Kill particle when crossing boundary */
        return 1;

    case BOUNDARY_BEHAVIOR_REFLECT:
        /* Reflect velocity on boundary collision */
        if (hit_left)
        {
            particle->x = ps->bound_left;
            particle->vx = -particle->vx;
        }
        else if (hit_right)
        {
            particle->x = ps->bound_right;
            particle->vx = -particle->vx;
        }

        if (hit_top)
        {
            particle->y = ps->bound_top;
            particle->vy = -particle->vy;
        }
        else if (hit_bottom)
        {
            particle->y = ps->bound_bottom;
            particle->vy = -particle->vy;
        }
        return 0;

    case BOUNDARY_BEHAVIOR_WRAP:
        /* Wrap to opposite side */
        if (hit_left)
        {
            particle->x = ps->bound_right;
        }
        else if (hit_right)
        {
            particle->x = ps->bound_left;
        }

        if (hit_top)
        {
            particle->y = ps->bound_bottom;
        }
        else if (hit_bottom)
        {
            particle->y = ps->bound_top;
        }
        return 0;

    default:
        return 1;
    }
}

/**
 * @brief Calculate auto-throttle factor based on pool usage
 */
static void particle_system_update_throttle(particle_system_t *ps)
{
    if (ps == NULL)
    {
        return;
    }

    if (!ps->auto_throttle)
    {
        ps->throttle_factor = 1.0f;
        return;
    }

    uint16_t active = particle_pool_get_active_count(ps->pool);
    uint16_t capacity = particle_pool_get_capacity(ps->pool);

    if (capacity == 0)
    {
        ps->throttle_factor = 1.0f;
        return;
    }

    float usage_ratio = (float)active / (float)capacity;

    if (usage_ratio >= ps->throttle_threshold)
    {
        /* Calculate throttle factor: linearly decrease from 1.0 to 0.0 */
        /* as usage goes from threshold to 1.0 */
        float excess = usage_ratio - ps->throttle_threshold;
        float range = 1.0f - ps->throttle_threshold;

        if (range > 0.0f)
        {
            ps->throttle_factor = 1.0f - (excess / range);
            ps->throttle_factor = clamp_float(ps->throttle_factor, 0.0f, 1.0f);
        }
        else
        {
            ps->throttle_factor = 0.0f;
        }
    }
    else
    {
        ps->throttle_factor = 1.0f;
    }
}


/* ============================================================================
 * Main Update and Render Functions
 * ============================================================================ */

void particle_system_update(particle_system_t *ps, float dt)
{
    if (ps == NULL || ps->pool == NULL || dt <= 0.0f)
    {
        return;
    }

    float dt_ms = dt * 1000.0f; /* Convert to milliseconds for lifecycle */

    /* Update auto-throttle factor */
    particle_system_update_throttle(ps);

    /* Emit particles from all emitters (with throttling) */
    float throttled_dt = dt * ps->throttle_factor;
    for (int i = 0; i < ps->emitter_count; i++)
    {
        particle_emitter_t *emitter = ps->emitters[i];
        if (emitter != NULL && emitter->enabled)
        {
            particle_emitter_emit(emitter, ps->pool, throttled_dt);
        }
    }

    /* Update all particles in the pool */
    uint16_t capacity = particle_pool_get_capacity(ps->pool);

    for (uint16_t i = 0; i < capacity; i++)
    {
        particle_t *particle = &ps->pool->particles[i];

        if (!particle->active)
        {
            continue;
        }

        /* Apply trajectory (position, velocity, rotation) */
        /* Find the emitter's trajectory config for this particle */
        /* For simplicity, we use a default linear trajectory if no emitter match */
        trajectory_config_t default_traj;
        trajectory_config_init(&default_traj);

        /* Try to find matching emitter trajectory */
        trajectory_config_t *traj = &default_traj;
        for (int e = 0; e < ps->emitter_count; e++)
        {
            if (ps->emitters[e] != NULL)
            {
                /* Use first emitter's trajectory as default */
                /* In a more complex system, particles would store their emitter reference */
                traj = &ps->emitters[e]->trajectory;
                break;
            }
        }

        trajectory_apply(particle, traj, dt);

        /* Update lifecycle and gradients */
        particle_update_lifecycle(particle, dt_ms);

        /* Check if particle should be recycled (life ended) */
        if (particle->life == 0)
        {
            particle_pool_free(ps->pool, particle);
            continue;
        }

        /* Check boundary collision */
        if (particle_check_boundary(ps, particle))
        {
            /* Particle crossed boundary and should be killed */
            particle_pool_free(ps->pool, particle);
            continue;
        }
    }
}

void particle_system_render(particle_system_t *ps)
{
    if (ps == NULL || ps->pool == NULL || ps->render_cb == NULL)
    {
        return;
    }

    uint16_t capacity = particle_pool_get_capacity(ps->pool);

    for (uint16_t i = 0; i < capacity; i++)
    {
        particle_t *particle = &ps->pool->particles[i];

        if (particle->active)
        {
            ps->render_cb(particle, ps->render_ctx);
        }
    }
}

void particle_system_clear(particle_system_t *ps)
{
    if (ps == NULL || ps->pool == NULL)
    {
        return;
    }

    particle_pool_clear(ps->pool);
}

/* ============================================================================
 * Configuration Functions
 * ============================================================================ */

void particle_system_set_bounds(particle_system_t *ps, float left, float top,
                                float right, float bottom)
{
    if (ps == NULL)
    {
        return;
    }

    ps->bound_left = left;
    ps->bound_top = top;
    ps->bound_right = right;
    ps->bound_bottom = bottom;
}

void particle_system_enable_bounds(particle_system_t *ps, uint8_t enable)
{
    if (ps == NULL)
    {
        return;
    }

    ps->bound_enabled = enable ? 1 : 0;
}

void particle_system_set_bound_behavior(particle_system_t *ps, boundary_behavior_t behavior)
{
    if (ps == NULL)
    {
        return;
    }

    ps->bound_behavior = behavior;
}

void particle_system_set_auto_throttle(particle_system_t *ps, uint8_t enable, float threshold)
{
    if (ps == NULL)
    {
        return;
    }

    ps->auto_throttle = enable ? 1 : 0;
    ps->throttle_threshold = clamp_float(threshold, 0.0f, 1.0f);

    /* Reset throttle factor when disabled */
    if (!enable)
    {
        ps->throttle_factor = 1.0f;
    }
}

void particle_system_set_render_callback(particle_system_t *ps,
                                         particle_render_callback_t cb, void *ctx)
{
    if (ps == NULL)
    {
        return;
    }

    ps->render_cb = cb;
    ps->render_ctx = ctx;
}

/* ============================================================================
 * Query Functions
 * ============================================================================ */

uint16_t particle_system_get_active_count(particle_system_t *ps)
{
    if (ps == NULL || ps->pool == NULL)
    {
        return 0;
    }

    return particle_pool_get_active_count(ps->pool);
}

float particle_system_get_throttle_factor(particle_system_t *ps)
{
    if (ps == NULL)
    {
        return 1.0f;
    }

    return ps->throttle_factor;
}

particle_pool_t *particle_system_get_pool(particle_system_t *ps)
{
    if (ps == NULL)
    {
        return NULL;
    }

    return ps->pool;
}
