/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file particle_pool.c
 * @brief Particle Pool Implementation
 */

#include "particle_pool.h"

particle_pool_t *particle_pool_create(uint16_t capacity, particle_platform_config_t *config)
{
    if (config == NULL || config->malloc_fn == NULL || capacity == 0)
    {
        return NULL;
    }

    particle_pool_t *pool = (particle_pool_t *)config->malloc_fn(sizeof(particle_pool_t));
    if (pool == NULL)
    {
        return NULL;
    }

    /* Allocate particle array */
    pool->particles = (particle_t *)config->malloc_fn(sizeof(particle_t) * capacity);
    if (pool->particles == NULL)
    {
        config->free_fn(pool);
        return NULL;
    }

    /* Allocate free index stack */
    pool->free_indices = (uint16_t *)config->malloc_fn(sizeof(uint16_t) * capacity);
    if (pool->free_indices == NULL)
    {
        config->free_fn(pool->particles);
        config->free_fn(pool);
        return NULL;
    }

    pool->capacity = capacity;
    pool->active_count = 0;
    pool->free_top = capacity;

    /* Initialize all particles and free index stack */
    for (uint16_t i = 0; i < capacity; i++)
    {
        particle_init(&pool->particles[i]);
        pool->free_indices[i] = i;
    }

    return pool;
}

void particle_pool_destroy(particle_pool_t *pool, particle_platform_config_t *config)
{
    if (pool == NULL || config == NULL || config->free_fn == NULL)
    {
        return;
    }

    if (pool->free_indices != NULL)
    {
        config->free_fn(pool->free_indices);
    }

    if (pool->particles != NULL)
    {
        config->free_fn(pool->particles);
    }

    config->free_fn(pool);
}

particle_t *particle_pool_alloc(particle_pool_t *pool)
{
    if (pool == NULL || pool->free_top == 0)
    {
        return NULL;
    }

    /* Pop index from free stack */
    pool->free_top--;
    uint16_t index = pool->free_indices[pool->free_top];

    particle_t *particle = &pool->particles[index];
    particle_init(particle);
    particle->active = 1;
    pool->active_count++;

    return particle;
}

void particle_pool_free(particle_pool_t *pool, particle_t *particle)
{
    if (pool == NULL || particle == NULL || !particle->active)
    {
        return;
    }

    /* Calculate particle index */
    ptrdiff_t index = particle - pool->particles;
    if (index < 0 || index >= pool->capacity)
    {
        return;
    }

    particle->active = 0;
    pool->active_count--;

    /* Push index back to free stack */
    pool->free_indices[pool->free_top] = (uint16_t)index;
    pool->free_top++;
}

uint16_t particle_pool_get_active_count(particle_pool_t *pool)
{
    if (pool == NULL)
    {
        return 0;
    }
    return pool->active_count;
}

uint16_t particle_pool_get_capacity(particle_pool_t *pool)
{
    if (pool == NULL)
    {
        return 0;
    }
    return pool->capacity;
}

void particle_pool_clear(particle_pool_t *pool)
{
    if (pool == NULL)
    {
        return;
    }

    /* Reset all particles and rebuild free stack */
    for (uint16_t i = 0; i < pool->capacity; i++)
    {
        particle_init(&pool->particles[i]);
        pool->free_indices[i] = i;
    }

    pool->active_count = 0;
    pool->free_top = pool->capacity;
}
