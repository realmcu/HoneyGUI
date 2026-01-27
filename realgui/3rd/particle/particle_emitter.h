/**
 * @file particle_emitter.h
 * @brief Particle Emitter Module - Particle generation and emission control
 *
 * This module implements particle emitters with various shapes:
 * - Point: Single point emission
 * - Line: Emission along a line segment
 * - Circle: Emission within a circular area
 * - Rectangle: Emission within a rectangular area
 *
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PARTICLE_EMITTER_H
#define PARTICLE_EMITTER_H

#include "particle_types.h"
#include "particle_pool.h"
#include "particle_trajectory.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Emitter shape type enumeration
 */
typedef enum
{
    EMITTER_SHAPE_POINT,      /**< Point emission (single location) */
    EMITTER_SHAPE_LINE,       /**< Line emission (along a segment) */
    EMITTER_SHAPE_CIRCLE,     /**< Circle emission (within radius) */
    EMITTER_SHAPE_RECT,       /**< Rectangle emission (within bounds) */
} emitter_shape_type_t;

/**
 * @brief Emitter shape configuration
 *
 * Defines the geometric shape from which particles are emitted.
 */
typedef struct emitter_shape_config
{
    emitter_shape_type_t type;  /**< Shape type */
    union
    {
        struct { float x, y; } point;                   /**< Point: single position */
        struct { float x1, y1, x2, y2; } line;          /**< Line: start and end points */
        struct { float cx, cy, radius; } circle;        /**< Circle: center and radius */
        struct { float x, y, w, h; } rect;              /**< Rectangle: position and size */
    };
} emitter_shape_config_t;

/* Emission rate limits as per Requirements 2.2 */
#define EMITTER_RATE_MIN (1.0f)
#define EMITTER_RATE_MAX (100.0f)


/**
 * @brief Particle emitter structure
 *
 * Controls particle generation including shape, rate, and initial properties.
 */
typedef struct particle_emitter
{
    /* Shape configuration */
    emitter_shape_config_t shape;       /**< Emission shape */

    /* Trajectory configuration */
    trajectory_config_t trajectory;     /**< Particle trajectory settings */

    /* Emission rate control */
    float emit_rate;                    /**< Emission rate (particles/second), clamped to [1, 100] */
    float emit_accumulator;             /**< Fractional particle accumulator */

    /* Particle initial property ranges */
    float speed_min, speed_max;         /**< Speed range (pixels/second) */
    float angle_min, angle_max;         /**< Emission angle range (radians) */
    float scale_min, scale_max;         /**< Scale range */
    float rotation_min, rotation_max;   /**< Initial rotation range (radians) */
    float rotation_speed_min, rotation_speed_max;  /**< Rotation speed range (rad/s) */
    uint16_t life_min, life_max;        /**< Life range (milliseconds) */

    /* Gradient properties */
    uint8_t opacity_start, opacity_end; /**< Opacity gradient (0-255) */
    float scale_start, scale_end;       /**< Scale gradient */

    /* Color configuration */
    uint32_t particle_color;            /**< Particle color in ARGB8888 format (0xAARRGGBB) */

    /* Image resource */
    void *particle_image;               /**< Particle image (externally managed) */

    /* State */
    uint8_t enabled;                    /**< Whether emitter is active */
    float follow_x, follow_y;           /**< Follow position */
    uint8_t follow_enabled;             /**< Whether position following is enabled */

    /* Random number generator state */
    uint32_t random_seed;               /**< RNG seed for reproducibility */
} particle_emitter_t;

/**
 * @brief Create a particle emitter
 * @param config Platform configuration with memory functions
 * @return Pointer to created emitter, or NULL on failure
 */
particle_emitter_t *particle_emitter_create(particle_platform_config_t *config);

/**
 * @brief Destroy a particle emitter
 * @param emitter Emitter to destroy
 * @param config Platform configuration with memory functions
 */
void particle_emitter_destroy(particle_emitter_t *emitter, particle_platform_config_t *config);

/**
 * @brief Set emitter shape configuration
 * @param emitter Emitter to configure
 * @param shape Shape configuration
 */
void particle_emitter_set_shape(particle_emitter_t *emitter, emitter_shape_config_t *shape);

/**
 * @brief Set emitter trajectory configuration
 * @param emitter Emitter to configure
 * @param trajectory Trajectory configuration
 */
void particle_emitter_set_trajectory(particle_emitter_t *emitter, trajectory_config_t *trajectory);

/**
 * @brief Set emission rate (clamped to [1, 100] particles/second)
 * @param emitter Emitter to configure
 * @param rate Emission rate in particles per second
 */
void particle_emitter_set_rate(particle_emitter_t *emitter, float rate);

/**
 * @brief Set particle color
 * @param emitter Emitter to configure
 * @param color Color in ARGB8888 format (0xAARRGGBB)
 */
void particle_emitter_set_color(particle_emitter_t *emitter, uint32_t color);

/**
 * @brief Set particle image resource
 * @param emitter Emitter to configure
 * @param image Image resource pointer (externally managed)
 */
void particle_emitter_set_image(particle_emitter_t *emitter, void *image);

/**
 * @brief Set follow position
 * @param emitter Emitter to configure
 * @param x X coordinate
 * @param y Y coordinate
 */
void particle_emitter_set_follow_position(particle_emitter_t *emitter, float x, float y);

/**
 * @brief Enable or disable position following
 * @param emitter Emitter to configure
 * @param enable 1 to enable, 0 to disable
 */
void particle_emitter_enable_follow(particle_emitter_t *emitter, uint8_t enable);

/**
 * @brief Enable or disable the emitter
 * @param emitter Emitter to configure
 * @param enable 1 to enable, 0 to disable
 */
void particle_emitter_enable(particle_emitter_t *emitter, uint8_t enable);

/**
 * @brief Emit particles based on elapsed time
 * @param emitter Emitter to use
 * @param pool Particle pool to allocate from
 * @param dt Delta time in seconds
 */
void particle_emitter_emit(particle_emitter_t *emitter, particle_pool_t *pool, float dt);

/**
 * @brief Emit a burst of particles immediately
 * @param emitter Emitter to use
 * @param pool Particle pool to allocate from
 * @param count Number of particles to emit
 */
void particle_emitter_burst(particle_emitter_t *emitter, particle_pool_t *pool, uint16_t count);

/**
 * @brief Initialize emitter to default values
 * @param emitter Emitter to initialize
 */
void particle_emitter_init(particle_emitter_t *emitter);

/**
 * @brief Clamp emission rate to valid range [1, 100]
 * @param rate Input rate value
 * @return Clamped rate value
 */
float particle_emitter_clamp_rate(float rate);

/**
 * @brief Get a random position within the emitter shape
 * @param emitter Emitter with shape configuration
 * @param out_x Output X coordinate
 * @param out_y Output Y coordinate
 */
void particle_emitter_get_emit_position(particle_emitter_t *emitter, float *out_x, float *out_y);

#ifdef __cplusplus
}
#endif

#endif /* PARTICLE_EMITTER_H */
