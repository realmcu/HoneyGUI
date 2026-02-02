/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file particle_effect.h
 * @brief Unified Particle Effect Configuration Structure
 *
 * This file defines the configuration-driven architecture for particle effects.
 * All particle effect parameters are organized into modular sub-structures.
 */

#ifndef PARTICLE_EFFECT_H
#define PARTICLE_EFFECT_H

#include <stdint.h>
#include <stddef.h>
#include "particle_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                           Constants
 *============================================================================*/

/** Configuration magic number "PART" */
#define PARTICLE_CONFIG_MAGIC   0x50415254
/** Configuration version */
#define PARTICLE_CONFIG_VERSION 1

/*============================================================================*
 *                           Enumerations
 *============================================================================*/

/**
 * @brief Emission shape type
 */
typedef enum
{
    PARTICLE_SHAPE_POINT = 0,   /**< Point emission (single location) */
    PARTICLE_SHAPE_LINE,        /**< Line emission (along a segment) */
    PARTICLE_SHAPE_CIRCLE,      /**< Circle emission (within radius) */
    PARTICLE_SHAPE_RECT,        /**< Rectangle emission (within bounds) */
    PARTICLE_SHAPE_RING,        /**< Ring emission (between inner/outer radius) */
} particle_shape_type_t;

/**
 * @brief Trajectory type
 */
typedef enum
{
    PARTICLE_TRAJECTORY_LINEAR = 0, /**< Linear motion (constant velocity) */
    PARTICLE_TRAJECTORY_GRAVITY,    /**< Gravity-affected motion */
    PARTICLE_TRAJECTORY_ORBIT,      /**< Orbital motion around center */
} particle_trajectory_type_t;

/**
 * @brief Color mode
 */
typedef enum
{
    PARTICLE_COLOR_SOLID = 0,   /**< Single solid color */
    PARTICLE_COLOR_RANDOM,      /**< Random color from palette */
    PARTICLE_COLOR_GRADIENT,    /**< Gradient from start to end color */
    PARTICLE_COLOR_RAINBOW,     /**< Rainbow hue cycle over lifetime */
} particle_color_mode_t;

/**
 * @brief Easing function type
 */
typedef enum
{
    PARTICLE_EASING_LINEAR = 0,     /**< Linear interpolation */
    PARTICLE_EASING_EASE_IN,        /**< Quadratic ease in */
    PARTICLE_EASING_EASE_OUT,       /**< Quadratic ease out */
    PARTICLE_EASING_EASE_IN_OUT,    /**< Smooth step ease in/out */
} particle_easing_t;

/**
 * @brief Boundary behavior
 */
typedef enum
{
    PARTICLE_BOUNDARY_NONE = 0,     /**< No boundary checking */
    PARTICLE_BOUNDARY_KILL,         /**< Kill particle at boundary */
    PARTICLE_BOUNDARY_REFLECT,      /**< Reflect at boundary */
    PARTICLE_BOUNDARY_WRAP,         /**< Wrap to opposite side */
} particle_boundary_behavior_t;

/**
 * @brief Behavior mode
 */
typedef enum
{
    PARTICLE_BEHAVIOR_NONE = 0,         /**< Static effect, no interaction */
    PARTICLE_BEHAVIOR_FOLLOW_TOUCH,     /**< Emitter follows touch position */
    PARTICLE_BEHAVIOR_TRAIL,            /**< Trail mode, emit opposite to movement */
    PARTICLE_BEHAVIOR_TOUCH_FEEDBACK,   /**< Touch feedback (tap/drag/release) */
    PARTICLE_BEHAVIOR_PULSE,            /**< Pulsing emission rate */
    PARTICLE_BEHAVIOR_BREATHE,          /**< Breathing opacity effect */
} particle_behavior_mode_t;

/**
 * @brief Blend mode for rendering
 */
typedef enum
{
    PARTICLE_BLEND_NORMAL = 0,  /**< Normal alpha blending */
    PARTICLE_BLEND_ADDITIVE,    /**< Additive blending */
    PARTICLE_BLEND_MULTIPLY,    /**< Multiply blending */
} particle_blend_mode_t;

/**
 * @brief Configuration validation error codes
 */
typedef enum
{
    PARTICLE_CONFIG_OK = 0,             /**< Configuration is valid */
    PARTICLE_CONFIG_ERR_MAGIC,          /**< Invalid magic number */
    PARTICLE_CONFIG_ERR_VERSION,        /**< Unsupported version */
    PARTICLE_CONFIG_ERR_SHAPE,          /**< Invalid shape parameters */
    PARTICLE_CONFIG_ERR_TRAJECTORY,     /**< Invalid trajectory parameters */
    PARTICLE_CONFIG_ERR_EMISSION,       /**< Invalid emission parameters */
    PARTICLE_CONFIG_ERR_LIFECYCLE,      /**< Invalid lifecycle parameters */
    PARTICLE_CONFIG_ERR_COLOR,          /**< Invalid color parameters */
    PARTICLE_CONFIG_ERR_OPACITY,        /**< Invalid opacity parameters */
    PARTICLE_CONFIG_ERR_SCALE,          /**< Invalid scale parameters */
    PARTICLE_CONFIG_ERR_ROTATION,       /**< Invalid rotation parameters */
    PARTICLE_CONFIG_ERR_BOUNDARY,       /**< Invalid boundary parameters */
    PARTICLE_CONFIG_ERR_BEHAVIOR,       /**< Invalid behavior parameters */
    PARTICLE_CONFIG_ERR_RENDER,         /**< Invalid render parameters */
} particle_config_error_t;

/*============================================================================*
 *                           Forward Declarations
 *============================================================================*/

struct particle_emitter;

/*============================================================================*
 *                           Callback Types
 *============================================================================*/

/**
 * @brief Particle callback function type
 * @param particle Pointer to the particle
 * @param user_data User-provided context
 */
typedef void (*particle_callback_fn)(particle_t *particle, void *user_data);

/**
 * @brief Emitter callback function type
 * @param emitter Pointer to the emitter
 * @param user_data User-provided context
 */
typedef void (*emitter_callback_fn)(struct particle_emitter *emitter, void *user_data);


/*============================================================================*
 *                           Sub-Configuration Structures
 *============================================================================*/

/**
 * @brief Shape configuration (32 bytes)
 *
 * Defines the geometric shape from which particles are emitted.
 */
typedef struct particle_shape_config
{
    particle_shape_type_t type;     /**< Shape type */
    union
    {
        struct { float x, y; } point;                       /**< Point: single position */
        struct { float x1, y1, x2, y2; } line;              /**< Line: start and end points */
        struct { float cx, cy, radius; } circle;            /**< Circle: center and radius */
        struct { float x, y, w, h; } rect;                  /**< Rectangle: position and size */
        struct { float cx, cy, inner_r, outer_r; } ring;    /**< Ring: center and radii */
    };
    uint8_t reserved[8];            /**< Reserved for future use */
} particle_shape_config_t;

/**
 * @brief Trajectory configuration (40 bytes)
 *
 * Defines particle motion behavior.
 */
typedef struct particle_trajectory_config
{
    particle_trajectory_type_t type;    /**< Trajectory type */
    float gravity;                      /**< Gravity: [-1000, 1000] px/s² */
    float damping;                      /**< Damping: [0.0, 1.0] */
    float wind_x;                       /**< Wind X force: px/s² */
    float wind_y;                       /**< Wind Y force: px/s² */
    float orbit_cx;                     /**< Orbit center X */
    float orbit_cy;                     /**< Orbit center Y */
    float orbit_speed;                  /**< Orbit angular speed: rad/s */
    uint8_t reserved[8];                /**< Reserved for future use */
} particle_trajectory_config_t;

/**
 * @brief Emission configuration (32 bytes)
 *
 * Defines particle emission parameters.
 */
typedef struct particle_emission_config
{
    float angle_min;                /**< Minimum emission angle (radians) */
    float angle_max;                /**< Maximum emission angle (radians) */
    float speed_min;                /**< Minimum emission speed (px/s) */
    float speed_max;                /**< Maximum emission speed (px/s) */
    float rate;                     /**< Emission rate: [1, 100] particles/s */
    uint16_t burst_count;           /**< Number of particles per burst */
    uint16_t burst_interval;        /**< Burst interval (ms), 0 = single burst */
    uint8_t burst_enabled;          /**< Whether burst mode is enabled */
    uint8_t reserved[7];            /**< Reserved for future use */
} particle_emission_config_t;

/**
 * @brief Lifecycle configuration (16 bytes)
 *
 * Defines particle and effect lifecycle.
 */
typedef struct particle_lifecycle_config
{
    uint16_t life_min;              /**< Minimum particle life (ms) */
    uint16_t life_max;              /**< Maximum particle life (ms) */
    uint32_t effect_duration;       /**< Effect duration (ms), 0 = infinite */
    uint8_t auto_cleanup;           /**< Auto-remove emitter when done */
    uint8_t loop;                   /**< Loop emission continuously */
    uint8_t reserved[6];            /**< Reserved for future use */
} particle_lifecycle_config_t;

/**
 * @brief Color configuration (48 bytes)
 *
 * Defines particle color behavior.
 */
typedef struct particle_color_config
{
    particle_color_mode_t mode;     /**< Color mode */
    uint32_t color_start;           /**< Start color (ARGB8888) */
    uint32_t color_end;             /**< End color (ARGB8888) */
    uint32_t palette[8];            /**< Color palette for RANDOM mode */
    uint8_t palette_size;           /**< Number of colors in palette */
    uint8_t reserved[7];            /**< Reserved for future use */
} particle_color_config_t;

/**
 * @brief Opacity configuration (8 bytes)
 *
 * Defines particle opacity gradient.
 */
typedef struct particle_opacity_config
{
    uint8_t start;                  /**< Start opacity [0-255] */
    uint8_t end;                    /**< End opacity [0-255] */
    uint8_t easing;                 /**< Easing function (particle_easing_t) */
    uint8_t reserved[5];            /**< Reserved for future use */
} particle_opacity_config_t;

/**
 * @brief Scale configuration (20 bytes)
 *
 * Defines particle scale behavior.
 */
typedef struct particle_scale_config
{
    float start;                    /**< Start scale factor */
    float end;                      /**< End scale factor */
    float min;                      /**< Minimum initial scale */
    float max;                      /**< Maximum initial scale */
    uint8_t reserved[4];            /**< Reserved for future use */
} particle_scale_config_t;

/**
 * @brief Rotation configuration (20 bytes)
 *
 * Defines particle rotation behavior.
 */
typedef struct particle_rotation_config
{
    float angle_min;                /**< Minimum initial angle (radians) */
    float angle_max;                /**< Maximum initial angle (radians) */
    float speed_min;                /**< Minimum rotation speed (rad/s) */
    float speed_max;                /**< Maximum rotation speed (rad/s) */
    uint8_t align_velocity;         /**< Align rotation to velocity direction */
    uint8_t reserved[3];            /**< Reserved for future use */
} particle_rotation_config_t;

/**
 * @brief Boundary configuration (24 bytes)
 *
 * Defines boundary behavior for particles.
 */
typedef struct particle_boundary_config
{
    particle_boundary_behavior_t behavior;  /**< Boundary behavior */
    float left;                     /**< Left boundary */
    float top;                      /**< Top boundary */
    float right;                    /**< Right boundary */
    float bottom;                   /**< Bottom boundary */
    float reflect_damping;          /**< Reflection damping [0.0, 1.0] */
} particle_boundary_config_t;

/**
 * @brief Behavior configuration (20 bytes)
 *
 * Defines interactive behavior mode.
 */
typedef struct particle_behavior_config
{
    particle_behavior_mode_t mode;  /**< Behavior mode (4 bytes) */
    float pulse_frequency;          /**< Pulse frequency (Hz) */
    float pulse_amplitude;          /**< Pulse amplitude */
    float breathe_frequency;        /**< Breathe frequency (Hz) */
    uint8_t reserved[4];            /**< Reserved for future use */
} particle_behavior_config_t;

/**
 * @brief Render configuration (16 bytes)
 *
 * Defines rendering options.
 * Note: texture pointer is runtime-only, not serialized to binary format.
 */
typedef struct particle_render_config
{
    particle_blend_mode_t blend_mode;   /**< Blend mode */
    void *texture;                      /**< Particle texture/image pointer (runtime only) */
    float base_size;                    /**< Base particle size (px) */
    uint8_t reserved[4];                /**< Reserved for future use */
} particle_render_config_t;

/**
 * @brief Callback configuration
 *
 * Defines callback functions for particle lifecycle events.
 * Note: Callbacks are runtime-only, not serialized to binary format.
 */
typedef struct particle_callback_config
{
    particle_callback_fn on_particle_init;      /**< Called when particle is initialized */
    particle_callback_fn on_particle_update;    /**< Called each frame update */
    particle_callback_fn on_particle_render;    /**< Called before rendering */
    particle_callback_fn on_particle_death;     /**< Called when particle dies */
    emitter_callback_fn on_emitter_start;       /**< Called when emitter starts */
    emitter_callback_fn on_emitter_stop;        /**< Called when emitter stops */
    void *user_data;                            /**< User data passed to callbacks */
} particle_callback_config_t;


/*============================================================================*
 *                           Binary Format Structure
 *============================================================================*/

/**
 * @brief Binary format render configuration (12 bytes, fixed size)
 *
 * Used for serialization - no pointers, platform-independent.
 */
typedef struct particle_render_config_binary
{
    uint32_t blend_mode;                /**< Blend mode (as uint32_t) */
    float base_size;                    /**< Base particle size (px) */
    uint32_t reserved;                  /**< Reserved for future use */
} particle_render_config_binary_t;

/**
 * @brief Binary format configuration structure (fixed 320 bytes)
 *
 * This structure is used for serialization/deserialization.
 * It contains NO pointers and has a fixed size across all platforms.
 * Callbacks and texture pointer are runtime-only and not serialized.
 *
 * Layout (verified offsets):
 *   Offset   0: Header (12 bytes) - magic, version, size, flags
 *   Offset  12: Shape (32 bytes)
 *   Offset  44: Trajectory (40 bytes)
 *   Offset  84: Emission (32 bytes)
 *   Offset 116: Lifecycle (16 bytes)
 *   Offset 132: Color (48 bytes)
 *   Offset 180: Opacity (8 bytes)
 *   Offset 188: Scale (20 bytes)
 *   Offset 208: Rotation (20 bytes)
 *   Offset 228: Boundary (24 bytes)
 *   Offset 252: Behavior (20 bytes)
 *   Offset 272: Render (12 bytes)
 *   Offset 284: Reserved (36 bytes)
 *   Total: 320 bytes
 */
#define PARTICLE_CONFIG_BINARY_SIZE 320

typedef struct particle_effect_config_binary
{
    /* Version control (12 bytes) */
    uint32_t magic;                             /**< Magic number: PARTICLE_CONFIG_MAGIC */
    uint16_t version;                           /**< Version number */
    uint16_t size;                              /**< Binary structure size (320) */
    uint32_t flags;                             /**< Configuration flags */

    /* Sub-configuration structures (pure data, no pointers) */
    particle_shape_config_t shape;              /**< 32 bytes */
    particle_trajectory_config_t trajectory;    /**< 40 bytes */
    particle_emission_config_t emission;        /**< 32 bytes */
    particle_lifecycle_config_t lifecycle;      /**< 16 bytes */
    particle_color_config_t color;              /**< 48 bytes */
    particle_opacity_config_t opacity;          /**< 8 bytes */
    particle_scale_config_t scale;              /**< 20 bytes */
    particle_rotation_config_t rotation;        /**< 20 bytes */
    particle_boundary_config_t boundary;        /**< 24 bytes */
    particle_behavior_config_t behavior;        /**< 20 bytes */
    particle_render_config_binary_t render;     /**< 12 bytes (no pointer) */

    /* Reserved for future expansion (36 bytes) */
    uint8_t reserved[36];
} particle_effect_config_binary_t;

/*============================================================================*
 *                           Main Configuration Structure
 *============================================================================*/

/**
 * @brief Main particle effect configuration structure (runtime)
 *
 * This is the unified configuration structure for particle effects.
 * Contains pointers (texture, callbacks) that are platform-dependent.
 * For serialization, use particle_effect_config_binary_t instead.
 *
 * Memory layout (platform-dependent due to pointers):
 * - Version control: 12 bytes
 * - shape: 32 bytes
 * - trajectory: 40 bytes
 * - emission: 32 bytes
 * - lifecycle: 16 bytes
 * - color: 48 bytes
 * - opacity: 8 bytes
 * - scale: 20 bytes
 * - rotation: 20 bytes
 * - boundary: 24 bytes
 * - behavior: 16 bytes
 * - render: 16+ bytes (contains pointer)
 * - callbacks: platform-dependent (contains pointers)
 * - reserved: 64 bytes
 */
typedef struct particle_effect_config
{
    /* Version control (12 bytes) */
    uint32_t magic;                             /**< Magic number: PARTICLE_CONFIG_MAGIC */
    uint16_t version;                           /**< Version number */
    uint16_t size;                              /**< Structure size */
    uint32_t flags;                             /**< Configuration flags */

    /* Sub-configuration structures */
    particle_shape_config_t shape;              /**< Emission shape configuration */
    particle_trajectory_config_t trajectory;    /**< Trajectory configuration */
    particle_emission_config_t emission;        /**< Emission parameters */
    particle_lifecycle_config_t lifecycle;      /**< Lifecycle configuration */
    particle_color_config_t color;              /**< Color configuration */
    particle_opacity_config_t opacity;          /**< Opacity configuration */
    particle_scale_config_t scale;              /**< Scale configuration */
    particle_rotation_config_t rotation;        /**< Rotation configuration */
    particle_boundary_config_t boundary;        /**< Boundary configuration */
    particle_behavior_config_t behavior;        /**< Behavior mode configuration */
    particle_render_config_t render;            /**< Render configuration (runtime, has pointer) */
    particle_callback_config_t callbacks;       /**< Callback functions (runtime only) */

    /* Reserved for future expansion (64 bytes) */
    uint8_t reserved[64];
} particle_effect_config_t;

/*============================================================================*
 *                           Parameter Limits
 *============================================================================*/

/** Gravity limits (px/s²) */
#define PARTICLE_GRAVITY_MIN    (-1000.0f)
#define PARTICLE_GRAVITY_MAX    (1000.0f)

/** Damping limits */
#define PARTICLE_DAMPING_MIN    (0.0f)
#define PARTICLE_DAMPING_MAX    (1.0f)

/** Emission rate limits (particles/s) */
#define PARTICLE_RATE_MIN       (1.0f)
#define PARTICLE_RATE_MAX       (100.0f)

/** Reflect damping limits */
#define PARTICLE_REFLECT_DAMPING_MIN    (0.0f)
#define PARTICLE_REFLECT_DAMPING_MAX    (1.0f)

/*============================================================================*
 *                           API Functions
 *============================================================================*/

/**
 * @brief Initialize configuration to default values
 *
 * Sets all fields to reasonable defaults:
 * - Point shape at (0, 0)
 * - Linear trajectory with no gravity
 * - 10 particles/second emission rate
 * - 1000ms particle lifetime
 * - White solid color
 * - Full opacity fading to transparent
 * - Scale 1.0
 * - No rotation
 * - No boundary checking
 * - No behavior mode
 * - Normal blend mode
 *
 * @param config Pointer to configuration to initialize
 */
void particle_effect_config_init(particle_effect_config_t *config);

/**
 * @brief Validate configuration values
 *
 * Checks all configuration values are within valid ranges.
 * Returns specific error code indicating which field is invalid.
 *
 * @param config Pointer to configuration to validate
 * @return PARTICLE_CONFIG_OK if valid, error code otherwise
 */
particle_config_error_t particle_effect_config_validate(
    const particle_effect_config_t *config);

/**
 * @brief Load configuration from memory (MEM mode)
 *
 * Loads configuration from binary format data in memory.
 * The binary format is platform-independent (fixed 320 bytes).
 * Validates magic and version fields before loading.
 * Pointers (texture, callbacks) are set to NULL after loading.
 *
 * @param config Pointer to runtime configuration to fill
 * @param addr Memory address containing binary format data
 * @return 0 on success, -1 on error (invalid magic/version/size)
 */
int particle_effect_config_load_mem(particle_effect_config_t *config,
                                    const void *addr);

/**
 * @brief Load configuration from file system (FS mode)
 *
 * Uses VFS API to load binary format configuration from file system.
 * Supports both embedded and simulator environments.
 *
 * @param config Pointer to runtime configuration to fill
 * @param filepath Path to .pec file
 * @return 0 on success, -1 on error
 */
int particle_effect_config_load_fs(particle_effect_config_t *config,
                                   const char *filepath);

/**
 * @brief Load configuration from FTL (FTL mode) - Reserved
 *
 * Reserved interface for loading from Flash Translation Layer.
 * Currently not implemented.
 *
 * @param config Pointer to configuration to fill
 * @param ftl_addr FTL address
 * @return -1 (not implemented)
 */
int particle_effect_config_load_ftl(particle_effect_config_t *config,
                                    uint32_t ftl_addr);

/**
 * @brief Save configuration to binary format
 *
 * Saves configuration to platform-independent binary format (320 bytes).
 * Pointers (texture, callbacks) are NOT saved - they are runtime-only.
 *
 * @param config Pointer to runtime configuration to save
 * @param buffer Output buffer for binary data (must be >= 320 bytes)
 * @param buffer_size Size of output buffer
 * @return Number of bytes written (320), or -1 on error
 */
int particle_effect_config_save(const particle_effect_config_t *config,
                                void *buffer, size_t buffer_size);

/**
 * @brief Get binary format size
 *
 * Returns the fixed size of the binary format structure.
 * Use this to allocate buffer for save() function.
 *
 * @return Binary format size (320 bytes)
 */
static inline size_t particle_effect_config_binary_size(void)
{
    return PARTICLE_CONFIG_BINARY_SIZE;
}

/*============================================================================*
 *                           Helper Functions
 *============================================================================*/

/**
 * @brief Clamp gravity value to valid range
 * @param gravity Input gravity value
 * @return Clamped value within [-1000, 1000]
 */
float particle_config_clamp_gravity(float gravity);

/**
 * @brief Clamp damping value to valid range
 * @param damping Input damping value
 * @return Clamped value within [0.0, 1.0]
 */
float particle_config_clamp_damping(float damping);

/**
 * @brief Clamp emission rate to valid range
 * @param rate Input rate value
 * @return Clamped value within [1, 100]
 */
float particle_config_clamp_rate(float rate);

#ifdef __cplusplus
}
#endif

#endif /* PARTICLE_EFFECT_H */
