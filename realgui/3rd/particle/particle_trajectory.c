/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file particle_trajectory.c
 * @brief Particle Trajectory Implementation
 *
 * Implements various trajectory types for particle motion including
 * linear, gravity, spiral, and custom callback-based trajectories.
 */

#include "particle_trajectory.h"
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

float trajectory_clamp_gravity(float gravity)
{
    return clamp_float(gravity, TRAJECTORY_GRAVITY_MIN, TRAJECTORY_GRAVITY_MAX);
}

float trajectory_clamp_damping(float damping)
{
    return clamp_float(damping, TRAJECTORY_DAMPING_MIN, TRAJECTORY_DAMPING_MAX);
}

/* ============================================================================
 * Trajectory Configuration
 * ============================================================================ */

void trajectory_config_init(trajectory_config_t *config)
{
    if (config == NULL)
    {
        return;
    }

    config->type = TRAJECTORY_LINEAR;
    config->gravity = 0.0f;
    config->damping = 0.0f;
    config->spiral_speed = 0.0f;
    config->spiral_cx = 0.0f;
    config->spiral_cy = 0.0f;
    config->wind_x = 0.0f;
    config->wind_y = 0.0f;
    config->custom_cb = NULL;
    config->custom_data = NULL;
}

/* ============================================================================
 * Trajectory Application Functions
 * ============================================================================ */

/**
 * @brief Apply linear trajectory (constant velocity)
 */
static void trajectory_apply_linear(particle_t *particle, trajectory_config_t *config, float dt)
{
    (void)config; /* Linear doesn't use config parameters beyond damping/wind */

    /* Update position based on velocity */
    particle->x += particle->vx * dt;
    particle->y += particle->vy * dt;
}

/**
 * @brief Apply gravity trajectory
 *
 * Gravity is applied as acceleration in the Y direction.
 * Positive gravity pulls downward (increases Y).
 */
static void trajectory_apply_gravity(particle_t *particle, trajectory_config_t *config, float dt)
{
    /* Clamp gravity to valid range */
    float gravity = trajectory_clamp_gravity(config->gravity);

    /* Apply gravity acceleration to velocity */
    particle->vy += gravity * dt;

    /* Update position based on velocity */
    particle->x += particle->vx * dt;
    particle->y += particle->vy * dt;
}

/**
 * @brief Apply spiral trajectory
 *
 * Particle rotates around a center point while maintaining its distance.
 */
static void trajectory_apply_spiral(particle_t *particle, trajectory_config_t *config, float dt)
{
    /* Calculate current offset from spiral center */
    float dx = particle->x - config->spiral_cx;
    float dy = particle->y - config->spiral_cy;

    /* Calculate current radius and angle */
    float radius = sqrtf(dx * dx + dy * dy);

    if (radius < 0.001f)
    {
        /* Particle is at center, just apply linear motion */
        particle->x += particle->vx * dt;
        particle->y += particle->vy * dt;
        return;
    }

    float angle = atan2f(dy, dx);

    /* Update angle based on spiral speed */
    angle += config->spiral_speed * dt;

    /* Calculate new position */
    particle->x = config->spiral_cx + radius * cosf(angle);
    particle->y = config->spiral_cy + radius * sinf(angle);

    /* Update velocity to be tangent to the spiral */
    float tangent_speed = radius * config->spiral_speed;
    particle->vx = -tangent_speed * sinf(angle);
    particle->vy = tangent_speed * cosf(angle);
}

/**
 * @brief Apply custom trajectory callback
 */
static void trajectory_apply_custom(particle_t *particle, trajectory_config_t *config, float dt)
{
    if (config->custom_cb != NULL)
    {
        config->custom_cb(particle, dt, config->custom_data);
    }
}

/**
 * @brief Apply damping to particle velocity
 *
 * Damping reduces velocity each frame by a factor of (1 - damping * dt).
 * This simulates air resistance or friction.
 */
static void trajectory_apply_damping(particle_t *particle, float damping, float dt)
{
    if (damping <= 0.0f)
    {
        return;
    }

    /* Clamp damping to valid range */
    damping = trajectory_clamp_damping(damping);

    /* Calculate damping factor */
    float factor = 1.0f - damping * dt;

    /* Ensure factor doesn't go negative for large dt */
    if (factor < 0.0f)
    {
        factor = 0.0f;
    }

    /* Apply damping to velocity */
    particle->vx *= factor;
    particle->vy *= factor;
}

/**
 * @brief Apply wind force to particle
 */
static void trajectory_apply_wind(particle_t *particle, float wind_x, float wind_y, float dt)
{
    /* Wind is applied as acceleration */
    particle->vx += wind_x * dt;
    particle->vy += wind_y * dt;
}

/* ============================================================================
 * Main Trajectory Application
 * ============================================================================ */

void trajectory_apply(particle_t *particle, trajectory_config_t *config, float dt)
{
    if (particle == NULL || config == NULL || dt <= 0.0f)
    {
        return;
    }

    /* Apply wind force first (affects all trajectory types) */
    if (config->wind_x != 0.0f || config->wind_y != 0.0f)
    {
        trajectory_apply_wind(particle, config->wind_x, config->wind_y, dt);
    }

    /* Apply trajectory-specific motion */
    switch (config->type)
    {
    case TRAJECTORY_LINEAR:
        trajectory_apply_linear(particle, config, dt);
        break;

    case TRAJECTORY_GRAVITY:
        trajectory_apply_gravity(particle, config, dt);
        break;

    case TRAJECTORY_SPIRAL:
        trajectory_apply_spiral(particle, config, dt);
        break;

    case TRAJECTORY_CUSTOM:
        trajectory_apply_custom(particle, config, dt);
        break;

    default:
        /* Unknown trajectory type, fall back to linear */
        trajectory_apply_linear(particle, config, dt);
        break;
    }

    /* Apply damping after motion (affects all trajectory types) */
    if (config->damping > 0.0f)
    {
        trajectory_apply_damping(particle, config->damping, dt);
    }

    /* Update rotation */
    particle->rotation += particle->rotation_speed * dt;
}
