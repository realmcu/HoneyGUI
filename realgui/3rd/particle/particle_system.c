/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file particle_system.c
 * @brief Particle System Core Controller Implementation
 */

#include "particle_system.h"
#include "particle_trajectory.h"
#include <string.h>
#include <math.h>

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

/**
 * @brief Apply easing function to a normalized time value
 * @param t Normalized time [0, 1]
 * @param easing Easing type (0=LINEAR, 1=EASE_IN, 2=EASE_OUT, 3=EASE_IN_OUT)
 * @return Eased time value [0, 1]
 */
static float apply_easing(float t, uint8_t easing)
{
    switch (easing)
    {
    case 0: /* LINEAR */
        return t;
    case 1: /* EASE_IN - quadratic */
        return t * t;
    case 2: /* EASE_OUT - quadratic */
        return 1.0f - (1.0f - t) * (1.0f - t);
    case 3: /* EASE_IN_OUT - smooth step */
        return t * t * (3.0f - 2.0f * t);
    default:
        return t;
    }
}

/**
 * @brief Interpolate between two ARGB8888 colors
 * @param color_start Start color
 * @param color_end End color
 * @param t Interpolation factor [0, 1]
 * @return Interpolated color
 */
static uint32_t lerp_color(uint32_t color_start, uint32_t color_end, float t)
{
    /* Extract ARGB components */
    uint8_t a1 = (color_start >> 24) & 0xFF;
    uint8_t r1 = (color_start >> 16) & 0xFF;
    uint8_t g1 = (color_start >> 8) & 0xFF;
    uint8_t b1 = color_start & 0xFF;

    uint8_t a2 = (color_end >> 24) & 0xFF;
    uint8_t r2 = (color_end >> 16) & 0xFF;
    uint8_t g2 = (color_end >> 8) & 0xFF;
    uint8_t b2 = color_end & 0xFF;

    /* Interpolate each component */
    uint8_t a = lerp_u8(a1, a2, t);
    uint8_t r = lerp_u8(r1, r2, t);
    uint8_t g = lerp_u8(g1, g2, t);
    uint8_t b = lerp_u8(b1, b2, t);

    /* Combine back to ARGB8888 */
    return ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

/**
 * @brief Convert HSV to RGB color
 * @param h Hue [0, 360)
 * @param s Saturation [0, 1]
 * @param v Value [0, 1]
 * @return RGB color in ARGB8888 format (alpha = 255)
 */
static uint32_t hsv_to_rgb(float h, float s, float v)
{
    float c = v * s;
    float x = c * (1.0f - fabsf(fmodf(h / 60.0f, 2.0f) - 1.0f));
    float m = v - c;

    float r_prime, g_prime, b_prime;

    if (h < 60.0f)
    {
        r_prime = c; g_prime = x; b_prime = 0.0f;
    }
    else if (h < 120.0f)
    {
        r_prime = x; g_prime = c; b_prime = 0.0f;
    }
    else if (h < 180.0f)
    {
        r_prime = 0.0f; g_prime = c; b_prime = x;
    }
    else if (h < 240.0f)
    {
        r_prime = 0.0f; g_prime = x; b_prime = c;
    }
    else if (h < 300.0f)
    {
        r_prime = x; g_prime = 0.0f; b_prime = c;
    }
    else
    {
        r_prime = c; g_prime = 0.0f; b_prime = x;
    }

    uint8_t r = (uint8_t)((r_prime + m) * 255.0f);
    uint8_t g = (uint8_t)((g_prime + m) * 255.0f);
    uint8_t b = (uint8_t)((b_prime + m) * 255.0f);

    return 0xFF000000 | ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
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
 * @brief Call on_particle_update callback if set
 *
 * Safely invokes the on_particle_update callback for a particle,
 * passing the user_data from the emitter.
 *
 * @param particle Particle to update
 */
static void particle_call_update_callback(particle_t *particle)
{
    if (particle == NULL || particle->emitter_ref == NULL)
    {
        return;
    }

    particle_emitter_t *emitter = (particle_emitter_t *)particle->emitter_ref;

    /* Call on_particle_update callback if set */
    if (emitter->on_particle_update != NULL)
    {
        emitter->on_particle_update(particle, emitter->user_data);
    }
}

/**
 * @brief Call on_particle_death callback if set
 *
 * Safely invokes the on_particle_death callback for a particle,
 * passing the user_data from the emitter.
 *
 * @param particle Particle that is dying
 */
static void particle_call_death_callback(particle_t *particle)
{
    if (particle == NULL || particle->emitter_ref == NULL)
    {
        return;
    }

    particle_emitter_t *emitter = (particle_emitter_t *)particle->emitter_ref;

    /* Call on_particle_death callback if set */
    if (emitter->on_particle_death != NULL)
    {
        emitter->on_particle_death(particle, emitter->user_data);
    }
}

/**
 * @brief Call on_particle_render callback if set
 *
 * Safely invokes the on_particle_render callback for a particle,
 * passing the user_data from the emitter.
 *
 * @param particle Particle to render
 */
static void particle_call_render_callback(particle_t *particle)
{
    if (particle == NULL || particle->emitter_ref == NULL)
    {
        return;
    }

    particle_emitter_t *emitter = (particle_emitter_t *)particle->emitter_ref;

    /* Call on_particle_render callback if set */
    if (emitter->on_particle_render != NULL)
    {
        emitter->on_particle_render(particle, emitter->user_data);
    }
}

/**
 * @brief Update particle lifecycle and gradients
 *
 * Updates particle life, opacity gradient (with easing), scale gradient,
 * color gradient, and velocity-aligned rotation based on remaining life ratio.
 *
 * Requirements: 6.5 (color gradient), 7.5 (opacity easing), 8.4 (velocity alignment)
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

    /* Calculate progress (0.0 = just born, 1.0 = dead) for easing */
    float progress = 1.0f - life_ratio;

    /* Apply opacity gradient with easing */
    /* At life_ratio=1.0 (start), opacity = opacity_start */
    /* At life_ratio=0.0 (end), opacity = opacity_end */
    float eased_progress = apply_easing(progress, particle->opacity_easing);
    particle->opacity = lerp_u8(particle->opacity_start, particle->opacity_end, eased_progress);

    /* Apply scale gradient (linear interpolation) */
    /* At life_ratio=1.0 (start), scale = scale_start */
    /* At life_ratio=0.0 (end), scale = scale_end */
    particle->scale = lerp(particle->scale_start, particle->scale_end, progress);

    /* Apply color gradient based on color mode */
    switch (particle->color_mode)
    {
    case 0: /* SOLID - no change */
        break;
    case 1: /* RANDOM - color already set at init, no change */
        break;
    case 2: /* GRADIENT - interpolate from start to end color */
        /* At life_ratio=1.0 (start), color = color_start */
        /* At life_ratio=0.0 (end), color = color_end */
        particle->color = lerp_color(particle->color_start, particle->color_end, progress);
        break;
    case 3: /* RAINBOW - cycle through hue over lifetime */
        {
            /* Hue cycles from 0 to 360 over particle lifetime */
            float hue = progress * 360.0f;
            particle->color = hsv_to_rgb(hue, 1.0f, 1.0f);
        }
        break;
    default:
        break;
    }
}

/**
 * @brief Check and handle particle boundary collision
 *
 * Returns 1 if particle should be killed, 0 otherwise.
 */
static uint8_t particle_check_boundary(particle_system_t *ps, particle_t *particle)
{
    if (ps == NULL || particle == NULL)
    {
        return 0;
    }

    /* Get boundary config from particle's emitter */
    particle_emitter_t *emitter = (particle_emitter_t *)particle->emitter_ref;
    if (emitter == NULL || emitter->boundary_behavior == PARTICLE_BOUNDARY_NONE)
    {
        /* Fall back to system-level boundary if no emitter or NONE behavior */
        if (!ps->bound_enabled)
        {
            return 0;
        }
        /* Use system boundary with default KILL behavior */
        if (particle->x < ps->bound_left || particle->x > ps->bound_right ||
            particle->y < ps->bound_top || particle->y > ps->bound_bottom)
        {
            return 1;
        }
        return 0;
    }

    /* Use emitter's boundary configuration */
    float bound_left = emitter->boundary_left;
    float bound_top = emitter->boundary_top;
    float bound_right = emitter->boundary_right;
    float bound_bottom = emitter->boundary_bottom;

    uint8_t hit_left = particle->x < bound_left;
    uint8_t hit_right = particle->x > bound_right;
    uint8_t hit_top = particle->y < bound_top;
    uint8_t hit_bottom = particle->y > bound_bottom;

    if (!hit_left && !hit_right && !hit_top && !hit_bottom)
    {
        return 0;
    }

    switch (emitter->boundary_behavior)
    {
    case PARTICLE_BOUNDARY_KILL:
        return 1;

    case PARTICLE_BOUNDARY_REFLECT:
        if (hit_left)
        {
            particle->x = bound_left;
            particle->vx = -particle->vx * emitter->boundary_reflect_damping;
        }
        else if (hit_right)
        {
            particle->x = bound_right;
            particle->vx = -particle->vx * emitter->boundary_reflect_damping;
        }

        if (hit_top)
        {
            particle->y = bound_top;
            particle->vy = -particle->vy * emitter->boundary_reflect_damping;
        }
        else if (hit_bottom)
        {
            particle->y = bound_bottom;
            particle->vy = -particle->vy * emitter->boundary_reflect_damping;
        }
        return 0;

    case PARTICLE_BOUNDARY_WRAP:
        if (hit_left)
        {
            particle->x = bound_right;
        }
        else if (hit_right)
        {
            particle->x = bound_left;
        }

        if (hit_top)
        {
            particle->y = bound_bottom;
        }
        else if (hit_bottom)
        {
            particle->y = bound_top;
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
 * Behavior Mode Helper Functions
 * ============================================================================ */

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define M_PI_F 3.14159265358979323846f

/**
 * @brief Update emitter behavior based on behavior mode
 *
 * Implements various behavior modes:
 * - FOLLOW_TOUCH: Emitter position follows touch/pointer
 * - TRAIL: Emission angle is opposite to movement direction
 * - PULSE: Emission rate varies periodically
 * - BREATHE: Opacity varies periodically
 *
 * Requirements: 10.1-10.7
 *
 * @param emitter Emitter to update
 * @param dt Delta time in seconds
 * @param current_time Current time in milliseconds
 */
static void update_emitter_behavior(particle_emitter_t *emitter, float dt, uint32_t current_time)
{
    if (emitter == NULL)
    {
        return;
    }

    (void)dt; /* May be used in future implementations */

    switch (emitter->behavior_mode)
    {
    case PARTICLE_BEHAVIOR_NONE:
        /* No special behavior */
        break;

    case PARTICLE_BEHAVIOR_FOLLOW_TOUCH:
        /* Position following is handled externally via particle_emitter_set_follow_position */
        /* The emitter shape position is updated based on follow_x, follow_y in emit function */
        break;

    case PARTICLE_BEHAVIOR_TRAIL:
        /* Calculate emission angle opposite to movement direction */
        if (emitter->has_prev_position)
        {
            float dx = emitter->follow_x - emitter->prev_follow_x;
            float dy = emitter->follow_y - emitter->prev_follow_y;
            float dist_sq = dx * dx + dy * dy;

            if (dist_sq > 0.01f)  /* Only update if there's significant movement */
            {
                /* Calculate movement direction */
                float move_angle = atan2f(dy, dx);

                /* Emission angle is opposite to movement (180 degrees offset) */
                float trail_angle = move_angle + M_PI_F;

                /* Set emission angle range centered on trail angle with some spread */
                float spread = 0.3f;  /* ~17 degrees spread */
                emitter->angle_min = trail_angle - spread;
                emitter->angle_max = trail_angle + spread;
            }
        }

        /* Update previous position for next frame */
        emitter->prev_follow_x = emitter->follow_x;
        emitter->prev_follow_y = emitter->follow_y;
        emitter->has_prev_position = 1;
        break;

    case PARTICLE_BEHAVIOR_TOUCH_FEEDBACK:
        /* Touch feedback is handled externally via burst/continuous emission */
        /* Tap: burst, Drag: continuous, Release: burst */
        break;

    case PARTICLE_BEHAVIOR_PULSE:
        /* Periodic emission rate variation */
        if (emitter->pulse_frequency > 0.0f)
        {
            float time_sec = (float)current_time / 1000.0f;
            float phase = time_sec * emitter->pulse_frequency * 2.0f * M_PI_F;
            float pulse_factor = 0.5f + 0.5f * sinf(phase);  /* Range [0, 1] */

            /* Apply pulse amplitude to emission rate */
            /* Base rate is modified by pulse_amplitude * pulse_factor */
            /* Store original rate if not already stored */
            float base_rate = emitter->emit_rate / (1.0f + emitter->pulse_amplitude * 0.5f);
            emitter->emit_rate = base_rate * (1.0f + emitter->pulse_amplitude * pulse_factor);

            /* Clamp to valid range */
            emitter->emit_rate = particle_emitter_clamp_rate(emitter->emit_rate);
        }
        break;

    case PARTICLE_BEHAVIOR_BREATHE:
        /* Periodic opacity variation */
        if (emitter->breathe_frequency > 0.0f)
        {
            float time_sec = (float)current_time / 1000.0f;
            float phase = time_sec * emitter->breathe_frequency * 2.0f * M_PI_F;
            float breathe_factor = 0.5f + 0.5f * sinf(phase);  /* Range [0, 1] */

            /* Modify opacity start/end based on breathe factor */
            /* This creates a breathing effect where particles fade in and out */
            uint8_t base_opacity = 255;
            emitter->opacity_start = (uint8_t)(base_opacity * breathe_factor);
        }
        break;

    default:
        break;
    }
}

/* ============================================================================
 * Auto-Cleanup Helper Functions
 * ============================================================================ */

/**
 * @brief Count active particles belonging to a specific emitter
 * @param ps Particle system
 * @param emitter Emitter to count particles for
 * @return Number of active particles belonging to the emitter
 */
static uint16_t count_emitter_particles(particle_system_t *ps, particle_emitter_t *emitter)
{
    if (ps == NULL || ps->pool == NULL || emitter == NULL)
    {
        return 0;
    }

    uint16_t count = 0;
    uint16_t capacity = particle_pool_get_capacity(ps->pool);

    for (uint16_t i = 0; i < capacity; i++)
    {
        particle_t *particle = &ps->pool->particles[i];
        if (particle->active && particle->emitter_ref == emitter)
        {
            count++;
        }
    }

    return count;
}

/**
 * @brief Check and perform auto-cleanup for emitters
 *
 * Removes and destroys emitters that have auto_cleanup enabled and
 * have no remaining active particles.
 *
 * Requirements: 5.2, 5.5
 *
 * @param ps Particle system
 */
static void particle_system_auto_cleanup(particle_system_t *ps)
{
    if (ps == NULL)
    {
        return;
    }

    /* Iterate backwards to safely remove emitters */
    for (int i = ps->emitter_count - 1; i >= 0; i--)
    {
        particle_emitter_t *emitter = ps->emitters[i];
        if (emitter == NULL)
        {
            continue;
        }

        /* Check if auto-cleanup is enabled for this emitter */
        if (!emitter->auto_cleanup)
        {
            continue;
        }

        /* Check if emitter is disabled (stopped emitting) */
        if (emitter->enabled)
        {
            /* Emitter is still active, check if it should be disabled */
            /* based on effect_duration or loop settings */
            if (emitter->effect_duration > 0 && !emitter->loop)
            {
                /* Check if effect duration has elapsed */
                uint32_t current_time = 0;
                if (ps->platform.get_time_fn != NULL)
                {
                    current_time = ps->platform.get_time_fn();
                }
                uint32_t elapsed = current_time - emitter->start_time;
                if (elapsed >= emitter->effect_duration)
                {
                    /* Disable emitter - it will be cleaned up when particles expire */
                    emitter->enabled = 0;
                }
            }
            continue;
        }

        /* Emitter is disabled, check if all its particles have expired */
        uint16_t particle_count = count_emitter_particles(ps, emitter);
        if (particle_count == 0)
        {
            /* All particles expired, remove and destroy emitter */
            /* Call on_emitter_stop callback if set */
            if (emitter->on_emitter_stop != NULL)
            {
                emitter->on_emitter_stop(emitter, emitter->user_data);
            }

            /* Remove from emitter array */
            for (int j = i; j < ps->emitter_count - 1; j++)
            {
                ps->emitters[j] = ps->emitters[j + 1];
            }
            ps->emitters[ps->emitter_count - 1] = NULL;
            ps->emitter_count--;

            /* Destroy the emitter */
            if (ps->platform.free_fn != NULL)
            {
                ps->platform.free_fn(emitter);
            }
        }
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

    /* Get current time for behavior modes */
    uint32_t current_time = 0;
    if (ps->platform.get_time_fn != NULL)
    {
        current_time = ps->platform.get_time_fn();
    }

    /* Update emitter behaviors and emit particles (with throttling) */
    float throttled_dt = dt * ps->throttle_factor;
    for (int i = 0; i < ps->emitter_count; i++)
    {
        particle_emitter_t *emitter = ps->emitters[i];
        if (emitter != NULL && emitter->enabled)
        {
            /* Update behavior mode */
            update_emitter_behavior(emitter, dt, current_time);

            /* Emit particles */
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
        /* Use the particle's own emitter trajectory, fall back to default */
        trajectory_config_t default_traj;
        trajectory_config_init(&default_traj);

        trajectory_config_t *traj = &default_traj;
        if (particle->emitter_ref != NULL)
        {
            particle_emitter_t *p_emitter = (particle_emitter_t *)particle->emitter_ref;
            traj = &p_emitter->trajectory;
        }

        trajectory_apply(particle, traj, dt);

        /* Apply velocity-aligned rotation if enabled */
        if (particle->align_velocity)
        {
            /* Only align if particle has non-zero velocity */
            float speed_sq = particle->vx * particle->vx + particle->vy * particle->vy;
            if (speed_sq > 0.0001f)  /* Small threshold to avoid division by zero */
            {
                particle->rotation = atan2f(particle->vy, particle->vx);
            }
        }

        /* Update lifecycle and gradients */
        particle_update_lifecycle(particle, dt_ms);

        /* Call on_particle_update callback */
        particle_call_update_callback(particle);

        /* Check if particle should be recycled (life ended) */
        if (particle->life == 0)
        {
            /* Call on_particle_death callback before freeing */
            particle_call_death_callback(particle);
            particle_pool_free(ps->pool, particle);
            continue;
        }

        /* Check boundary collision */
        if (particle_check_boundary(ps, particle))
        {
            /* Particle crossed boundary and should be killed */
            /* Call on_particle_death callback before freeing */
            particle_call_death_callback(particle);
            particle_pool_free(ps->pool, particle);
            continue;
        }
    }

    /* Perform auto-cleanup for emitters */
    particle_system_auto_cleanup(ps);
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
            /* Call on_particle_render callback before rendering */
            particle_call_render_callback(particle);

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
