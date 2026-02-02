/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file particle_emitter.h
 * @brief Particle Emitter Module - Particle generation and emission control
 *
 * This module implements particle emitters with various shapes:
 * - Point: Single point emission
 * - Line: Emission along a line segment
 * - Circle: Emission within a circular area
 * - Rectangle: Emission within a rectangular area
 */

#ifndef PARTICLE_EMITTER_H
#define PARTICLE_EMITTER_H

#include "particle_types.h"
#include "particle_pool.h"
#include "particle_trajectory.h"
#include "particle_effect.h"

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
    EMITTER_SHAPE_RING,       /**< Ring emission (between inner/outer radius) */
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
        struct { float cx, cy, inner_r, outer_r; } ring;/**< Ring: center and radii */
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
    particle_color_mode_t color_mode;   /**< Color mode (SOLID, RANDOM, GRADIENT, RAINBOW) */
    uint32_t color_start;               /**< Start color for gradient mode */
    uint32_t color_end;                 /**< End color for gradient mode */
    uint32_t color_palette[8];          /**< Color palette for random mode */
    uint8_t palette_size;               /**< Number of colors in palette */

    /* Opacity easing */
    uint8_t opacity_easing;             /**< Opacity easing function (particle_easing_t) */

    /* Rotation alignment */
    uint8_t align_velocity;             /**< Align rotation to velocity direction */

    /* Image resource */
    void *particle_image;               /**< Particle image (externally managed) */

    /* State */
    uint8_t enabled;                    /**< Whether emitter is active */
    float follow_x, follow_y;           /**< Follow position */
    uint8_t follow_enabled;             /**< Whether position following is enabled */

    /* Burst mode */
    uint8_t burst_enabled;              /**< Whether burst mode is enabled */
    uint16_t burst_count;               /**< Number of particles per burst */
    uint16_t burst_interval;            /**< Burst interval (ms), 0 = single burst */
    uint32_t last_burst_time;           /**< Last burst time (ms) */

    /* Lifecycle */
    uint8_t auto_cleanup;               /**< Auto-remove emitter when done */
    uint8_t loop;                       /**< Loop emission continuously */
    uint32_t effect_duration;           /**< Effect duration (ms), 0 = infinite */
    uint32_t start_time;                /**< Effect start time (ms) */

    /* Boundary configuration */
    particle_boundary_behavior_t boundary_behavior; /**< Boundary behavior */
    float boundary_left;                /**< Left boundary */
    float boundary_top;                 /**< Top boundary */
    float boundary_right;               /**< Right boundary */
    float boundary_bottom;              /**< Bottom boundary */
    float boundary_reflect_damping;     /**< Reflection damping [0.0, 1.0] */

    /* Behavior mode configuration */
    particle_behavior_mode_t behavior_mode; /**< Behavior mode */
    float pulse_frequency;              /**< Pulse frequency (Hz) */
    float pulse_amplitude;              /**< Pulse amplitude */
    float breathe_frequency;            /**< Breathe frequency (Hz) */
    float prev_follow_x, prev_follow_y; /**< Previous follow position for trail mode */
    uint8_t has_prev_position;          /**< Whether previous position is valid */

    /* Render configuration */
    particle_blend_mode_t blend_mode;   /**< Blend mode */
    float base_size;                    /**< Base particle size (px) */

    /* Callback functions */
    particle_callback_fn on_particle_init;      /**< Called when particle is initialized */
    particle_callback_fn on_particle_update;    /**< Called each frame update */
    particle_callback_fn on_particle_render;    /**< Called before rendering */
    particle_callback_fn on_particle_death;     /**< Called when particle dies */
    emitter_callback_fn on_emitter_start;       /**< Called when emitter starts */
    emitter_callback_fn on_emitter_stop;        /**< Called when emitter stops */
    void *user_data;                            /**< User data passed to callbacks */

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
 * @brief Create a particle emitter from effect configuration
 *
 * Creates and initializes an emitter with all parameters from the
 * particle_effect_config_t structure.
 *
 * @param effect_config Effect configuration structure
 * @param platform_config Platform configuration with memory functions
 * @return Pointer to created emitter, or NULL on failure
 */
particle_emitter_t *particle_emitter_create_from_config(
    const particle_effect_config_t *effect_config,
    particle_platform_config_t *platform_config);

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
