/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file particle_system.h
 * @brief Particle System Core Controller - Main system management
 *
 * This module implements the core particle system controller that manages:
 * - Particle pool lifecycle
 * - Multiple emitters
 * - Particle update loop (position, lifecycle, gradients)
 * - Boundary detection and recycling
 * - Auto-throttling mechanism
 * - Render callback invocation
 */

#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "particle_types.h"
#include "particle_pool.h"
#include "particle_emitter.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Maximum number of emitters per particle system */
#define PARTICLE_SYSTEM_MAX_EMITTERS 16

/** Default auto-throttle threshold (80% of pool capacity) */
#define PARTICLE_SYSTEM_DEFAULT_THROTTLE_THRESHOLD (0.8f)

/** Boundary behavior flags */
typedef enum
{
    BOUNDARY_BEHAVIOR_KILL,      /**< Kill particle when it crosses boundary */
    BOUNDARY_BEHAVIOR_REFLECT,   /**< Reflect velocity when hitting boundary */
    BOUNDARY_BEHAVIOR_WRAP,      /**< Wrap particle to opposite side */
} boundary_behavior_t;

/**
 * @brief Particle system structure
 *
 * Main controller for the particle system. Manages particle pool,
 * emitters, updates, and rendering.
 */
typedef struct particle_system
{
    particle_pool_t *pool;                              /**< Particle pool */
    particle_emitter_t *emitters[PARTICLE_SYSTEM_MAX_EMITTERS]; /**< Emitter array */
    uint8_t emitter_count;                              /**< Number of active emitters */
    particle_platform_config_t platform;                /**< Platform configuration */
    uint32_t last_update_time;                          /**< Last update timestamp (ms) */

    /* Auto-throttle settings */
    uint8_t auto_throttle;                              /**< Auto-throttle enable flag */
    float throttle_threshold;                           /**< Throttle threshold (0.0-1.0) */
    float throttle_factor;                              /**< Current throttle factor (0.0-1.0) */

    /* Boundary configuration */
    float bound_left;                                   /**< Left boundary */
    float bound_top;                                    /**< Top boundary */
    float bound_right;                                  /**< Right boundary */
    float bound_bottom;                                 /**< Bottom boundary */
    uint8_t bound_enabled;                              /**< Boundary checking enabled */
    boundary_behavior_t bound_behavior;                 /**< Boundary behavior mode */

    /* Render callback */
    particle_render_callback_t render_cb;               /**< Render callback function */
    void *render_ctx;                                   /**< Render context */
} particle_system_t;

/**
 * @brief Create a particle system
 * @param pool_capacity Maximum number of particles
 * @param config Platform configuration with memory functions
 * @return Pointer to created system, or NULL on failure
 */
particle_system_t *particle_system_create(uint16_t pool_capacity,
                                          particle_platform_config_t *config);

/**
 * @brief Destroy a particle system and free all resources
 * @param ps Particle system to destroy
 */
void particle_system_destroy(particle_system_t *ps);

/**
 * @brief Add an emitter to the particle system
 * @param ps Particle system
 * @param emitter Emitter to add
 * @return 1 on success, 0 on failure (max emitters reached)
 */
uint8_t particle_system_add_emitter(particle_system_t *ps, particle_emitter_t *emitter);

/**
 * @brief Remove an emitter from the particle system
 * @param ps Particle system
 * @param emitter Emitter to remove
 */
void particle_system_remove_emitter(particle_system_t *ps, particle_emitter_t *emitter);

/**
 * @brief Update the particle system
 *
 * Updates all particles: applies trajectories, updates lifecycle,
 * applies gradients, checks boundaries, and handles auto-throttling.
 *
 * @param ps Particle system
 * @param dt Delta time in seconds
 */
void particle_system_update(particle_system_t *ps, float dt);

/**
 * @brief Render all active particles
 *
 * Calls the render callback for each active particle.
 *
 * @param ps Particle system
 */
void particle_system_render(particle_system_t *ps);

/**
 * @brief Clear all particles in the system
 * @param ps Particle system
 */
void particle_system_clear(particle_system_t *ps);

/**
 * @brief Set boundary configuration
 * @param ps Particle system
 * @param left Left boundary
 * @param top Top boundary
 * @param right Right boundary
 * @param bottom Bottom boundary
 */
void particle_system_set_bounds(particle_system_t *ps, float left, float top,
                                float right, float bottom);

/**
 * @brief Enable or disable boundary checking
 * @param ps Particle system
 * @param enable 1 to enable, 0 to disable
 */
void particle_system_enable_bounds(particle_system_t *ps, uint8_t enable);

/**
 * @brief Set boundary behavior
 * @param ps Particle system
 * @param behavior Boundary behavior mode
 */
void particle_system_set_bound_behavior(particle_system_t *ps, boundary_behavior_t behavior);

/**
 * @brief Configure auto-throttling
 * @param ps Particle system
 * @param enable 1 to enable, 0 to disable
 * @param threshold Throttle threshold (0.0-1.0), triggers when active/capacity exceeds this
 */
void particle_system_set_auto_throttle(particle_system_t *ps, uint8_t enable, float threshold);

/**
 * @brief Set render callback
 * @param ps Particle system
 * @param cb Render callback function
 * @param ctx Render context passed to callback
 */
void particle_system_set_render_callback(particle_system_t *ps,
                                         particle_render_callback_t cb, void *ctx);

/**
 * @brief Get number of active particles
 * @param ps Particle system
 * @return Number of active particles
 */
uint16_t particle_system_get_active_count(particle_system_t *ps);

/**
 * @brief Get current throttle factor
 * @param ps Particle system
 * @return Current throttle factor (0.0-1.0, 1.0 = no throttling)
 */
float particle_system_get_throttle_factor(particle_system_t *ps);

/**
 * @brief Get the particle pool
 * @param ps Particle system
 * @return Pointer to particle pool
 */
particle_pool_t *particle_system_get_pool(particle_system_t *ps);

#ifdef __cplusplus
}
#endif

#endif /* PARTICLE_SYSTEM_H */
