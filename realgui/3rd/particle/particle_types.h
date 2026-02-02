/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file particle_types.h
 * @brief Particle System Core Types - Independent, no HoneyGUI dependencies
 *
 * This file defines the fundamental types for the particle system including:
 * - Platform abstraction interfaces (malloc/free/get_time)
 * - Particle structure definition
 * - Render callback type
 */

#ifndef PARTICLE_TYPES_H
#define PARTICLE_TYPES_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Platform abstraction: memory allocation function type
 * @param size Number of bytes to allocate
 * @return Pointer to allocated memory, or NULL on failure
 */
typedef void *(*particle_malloc_t)(size_t size);

/**
 * @brief Platform abstraction: memory free function type
 * @param ptr Pointer to memory to free
 */
typedef void (*particle_free_t)(void *ptr);

/**
 * @brief Platform abstraction: get current time in milliseconds
 * @return Current time in milliseconds
 */
typedef uint32_t (*particle_get_time_ms_t)(void);

/**
 * @brief Platform configuration structure
 *
 * Contains function pointers for platform-specific operations.
 * Must be provided during particle system initialization.
 */
typedef struct particle_platform_config
{
    particle_malloc_t malloc_fn;        /**< Memory allocation function */
    particle_free_t free_fn;            /**< Memory free function */
    particle_get_time_ms_t get_time_fn; /**< Time retrieval function */
} particle_platform_config_t;


/**
 * @brief Particle structure
 *
 * Represents a single particle entity with all its properties.
 * Particles are managed by the particle pool for efficient reuse.
 */
typedef struct particle
{
    /* Position */
    float x;                /**< X position in pixels */
    float y;                /**< Y position in pixels */

    /* Velocity */
    float vx;               /**< X velocity in pixels/second */
    float vy;               /**< Y velocity in pixels/second */

    /* Acceleration */
    float ax;               /**< X acceleration in pixels/second² */
    float ay;               /**< Y acceleration in pixels/second² */

    /* Visual properties */
    float scale;            /**< Scale factor (1.0 = original size) */
    float rotation;         /**< Rotation angle in radians */
    float rotation_speed;   /**< Rotation speed in radians/second */
    uint8_t opacity;        /**< Opacity (0-255, 255 = fully opaque) */
    uint32_t color;         /**< Color in ARGB8888 format (0xAARRGGBB) */

    /* Lifecycle */
    uint16_t life;          /**< Remaining life in milliseconds */
    uint16_t max_life;      /**< Maximum life in milliseconds */
    uint8_t active;         /**< Whether particle is active (1) or inactive (0) */

    /* Resources */
    void *image;            /**< Particle image resource (externally managed) */
    void *user_data;        /**< User-defined data pointer */

    /* Gradient properties (for interpolation) */
    uint8_t opacity_start;  /**< Starting opacity for gradient */
    uint8_t opacity_end;    /**< Ending opacity for gradient */
    float scale_start;      /**< Starting scale for gradient */
    float scale_end;        /**< Ending scale for gradient */

    /* Color gradient properties (Requirements 6.3, 6.5) */
    uint32_t color_start;   /**< Starting color for gradient (ARGB8888) */
    uint32_t color_end;     /**< Ending color for gradient (ARGB8888) */
    uint8_t color_mode;     /**< Color mode (0=SOLID, 1=RANDOM, 2=GRADIENT, 3=RAINBOW) */

    /* Opacity easing (Requirements 7.2, 7.5) */
    uint8_t opacity_easing; /**< Easing function (0=LINEAR, 1=EASE_IN, 2=EASE_OUT, 3=EASE_IN_OUT) */

    /* Rotation alignment (Requirements 8.3, 8.4) */
    uint8_t align_velocity; /**< Align rotation to velocity direction */

    /* Emitter reference for callbacks */
    void *emitter_ref;      /**< Reference to parent emitter for callbacks */
} particle_t;

/**
 * @brief Render callback function type
 *
 * Called by the particle system for each active particle during rendering.
 * The external renderer implements this callback to draw particles.
 *
 * @param particle Pointer to the particle to render
 * @param render_ctx User-provided render context
 */
typedef void (*particle_render_callback_t)(particle_t *particle, void *render_ctx);

/**
 * @brief Initialize a particle to default values
 * @param particle Pointer to particle to initialize
 */
static inline void particle_init(particle_t *particle)
{
    if (particle == NULL)
    {
        return;
    }

    particle->x = 0.0f;
    particle->y = 0.0f;
    particle->vx = 0.0f;
    particle->vy = 0.0f;
    particle->ax = 0.0f;
    particle->ay = 0.0f;
    particle->scale = 1.0f;
    particle->rotation = 0.0f;
    particle->rotation_speed = 0.0f;
    particle->opacity = 255;
    particle->color = 0xFFFFFFFF;  /* Default: white */
    particle->life = 0;
    particle->max_life = 0;
    particle->active = 0;
    particle->image = NULL;
    particle->user_data = NULL;
    particle->opacity_start = 255;
    particle->opacity_end = 0;
    particle->scale_start = 1.0f;
    particle->scale_end = 0.0f;
    particle->color_start = 0xFFFFFFFF;
    particle->color_end = 0xFFFFFFFF;
    particle->color_mode = 0;  /* SOLID */
    particle->opacity_easing = 0;  /* LINEAR */
    particle->align_velocity = 0;
    particle->emitter_ref = NULL;
}

#ifdef __cplusplus
}
#endif

#endif /* PARTICLE_TYPES_H */
