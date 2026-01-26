/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file particle_trajectory.h
 * @brief Particle Trajectory Module - Motion path calculations
 *
 * This module implements various trajectory types for particle motion:
 * - Linear: Constant velocity motion
 * - Gravity: Acceleration-based motion with configurable gravity
 * - Spiral: Circular motion around a center point
 * - Custom: User-defined trajectory callback
 */

#ifndef PARTICLE_TRAJECTORY_H
#define PARTICLE_TRAJECTORY_H

#include "particle_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Trajectory type enumeration
 */
typedef enum
{
    TRAJECTORY_LINEAR,        /**< Linear motion (constant velocity) */
    TRAJECTORY_GRAVITY,       /**< Gravity-affected motion */
    TRAJECTORY_SPIRAL,        /**< Spiral motion around center */
    TRAJECTORY_CUSTOM,        /**< Custom callback-based motion */
} trajectory_type_t;

/**
 * @brief Custom trajectory callback function type
 * @param particle Pointer to the particle to update
 * @param dt Delta time in seconds
 * @param user_data User-provided data pointer
 */
typedef void (*trajectory_callback_t)(particle_t *particle, float dt, void *user_data);

/**
 * @brief Trajectory configuration structure
 *
 * Contains all parameters needed to define particle motion behavior.
 */
typedef struct trajectory_config
{
    trajectory_type_t type;     /**< Trajectory type */
    float gravity;              /**< Gravity acceleration (pixels/s²), clamped to [-1000, 1000] */
    float damping;              /**< Damping coefficient (0.0-1.0), reduces velocity each frame */
    float spiral_speed;         /**< Spiral angular velocity (rad/s) */
    float spiral_cx;            /**< Spiral center X coordinate */
    float spiral_cy;            /**< Spiral center Y coordinate */
    float wind_x;               /**< Wind force X component (pixels/s²) */
    float wind_y;               /**< Wind force Y component (pixels/s²) */
    trajectory_callback_t custom_cb;  /**< Custom trajectory callback */
    void *custom_data;          /**< User data for custom callback */
} trajectory_config_t;

/* Gravity limits as per Requirements 3.2 */
#define TRAJECTORY_GRAVITY_MIN (-1000.0f)
#define TRAJECTORY_GRAVITY_MAX (1000.0f)

/* Damping limits as per Requirements 3.3 */
#define TRAJECTORY_DAMPING_MIN (0.0f)
#define TRAJECTORY_DAMPING_MAX (1.0f)

/**
 * @brief Initialize trajectory configuration to default values
 * @param config Pointer to configuration to initialize
 */
void trajectory_config_init(trajectory_config_t *config);

/**
 * @brief Apply trajectory to a particle
 *
 * Updates particle position and velocity based on the trajectory configuration.
 *
 * @param particle Pointer to the particle to update
 * @param config Pointer to trajectory configuration
 * @param dt Delta time in seconds
 */
void trajectory_apply(particle_t *particle, trajectory_config_t *config, float dt);

/**
 * @brief Clamp gravity value to valid range
 * @param gravity Input gravity value
 * @return Clamped gravity value within [-1000, 1000]
 */
float trajectory_clamp_gravity(float gravity);

/**
 * @brief Clamp damping value to valid range
 * @param damping Input damping value
 * @return Clamped damping value within [0.0, 1.0]
 */
float trajectory_clamp_damping(float damping);

#ifdef __cplusplus
}
#endif

#endif /* PARTICLE_TRAJECTORY_H */
