/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file particle_pool.h
 * @brief Particle Pool Management - Object pool for efficient particle reuse
 *
 * This module implements an object pool pattern for managing particle lifecycle.
 * It uses a free index stack for O(1) allocation and deallocation.
 */

#ifndef PARTICLE_POOL_H
#define PARTICLE_POOL_H

#include "particle_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Particle pool structure
 *
 * Manages a fixed-size pool of particles using a free index stack.
 * Provides O(1) allocation and deallocation operations.
 */
typedef struct particle_pool
{
    particle_t *particles;      /**< Array of particles */
    uint16_t capacity;          /**< Pool capacity (max particles) */
    uint16_t active_count;      /**< Number of active particles */
    uint16_t *free_indices;     /**< Stack of free particle indices */
    uint16_t free_top;          /**< Top of free index stack */
} particle_pool_t;

/**
 * @brief Create a particle pool
 * @param capacity Maximum number of particles in the pool
 * @param config Platform configuration with memory functions
 * @return Pointer to created pool, or NULL on failure
 */
particle_pool_t *particle_pool_create(uint16_t capacity, particle_platform_config_t *config);

/**
 * @brief Destroy a particle pool and free all resources
 * @param pool Pool to destroy
 * @param config Platform configuration with memory functions
 */
void particle_pool_destroy(particle_pool_t *pool, particle_platform_config_t *config);

/**
 * @brief Allocate a particle from the pool
 * @param pool Pool to allocate from
 * @return Pointer to allocated particle, or NULL if pool is full
 */
particle_t *particle_pool_alloc(particle_pool_t *pool);

/**
 * @brief Free a particle back to the pool
 * @param pool Pool to return particle to
 * @param particle Particle to free
 */
void particle_pool_free(particle_pool_t *pool, particle_t *particle);

/**
 * @brief Get the number of active particles
 * @param pool Pool to query
 * @return Number of active particles
 */
uint16_t particle_pool_get_active_count(particle_pool_t *pool);

/**
 * @brief Get the pool capacity
 * @param pool Pool to query
 * @return Pool capacity
 */
uint16_t particle_pool_get_capacity(particle_pool_t *pool);

/**
 * @brief Clear all particles in the pool (mark all as inactive)
 * @param pool Pool to clear
 */
void particle_pool_clear(particle_pool_t *pool);

#ifdef __cplusplus
}
#endif

#endif /* PARTICLE_POOL_H */
