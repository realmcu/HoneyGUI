/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file particle_effect.c
 * @brief Particle Effect Configuration Implementation
 */

#include "particle_effect.h"
#include <string.h>
#include <stdio.h>

#ifndef M_PI_F
#define M_PI_F 3.14159265358979323846f
#endif

/*============================================================================*
 *                           Helper Functions
 *============================================================================*/

float particle_config_clamp_gravity(float gravity)
{
    if (gravity < PARTICLE_GRAVITY_MIN)
    {
        return PARTICLE_GRAVITY_MIN;
    }
    if (gravity > PARTICLE_GRAVITY_MAX)
    {
        return PARTICLE_GRAVITY_MAX;
    }
    return gravity;
}

float particle_config_clamp_damping(float damping)
{
    if (damping < PARTICLE_DAMPING_MIN)
    {
        return PARTICLE_DAMPING_MIN;
    }
    if (damping > PARTICLE_DAMPING_MAX)
    {
        return PARTICLE_DAMPING_MAX;
    }
    return damping;
}

float particle_config_clamp_rate(float rate)
{
    if (rate < PARTICLE_RATE_MIN)
    {
        return PARTICLE_RATE_MIN;
    }
    if (rate > PARTICLE_RATE_MAX)
    {
        return PARTICLE_RATE_MAX;
    }
    return rate;
}

/*============================================================================*
 *                           Configuration Initialization
 *============================================================================*/

void particle_effect_config_init(particle_effect_config_t *config)
{
    if (config == NULL)
    {
        return;
    }

    /* Clear entire structure */
    memset(config, 0, sizeof(particle_effect_config_t));

    /* Version control */
    config->magic = PARTICLE_CONFIG_MAGIC;
    config->version = PARTICLE_CONFIG_VERSION;
    config->size = (uint16_t)sizeof(particle_effect_config_t);
    config->flags = 0;

    /* Shape: Point at origin */
    config->shape.type = PARTICLE_SHAPE_POINT;
    config->shape.point.x = 0.0f;
    config->shape.point.y = 0.0f;

    /* Trajectory: Linear with no gravity */
    config->trajectory.type = PARTICLE_TRAJECTORY_LINEAR;
    config->trajectory.gravity = 0.0f;
    config->trajectory.damping = 0.0f;
    config->trajectory.wind_x = 0.0f;
    config->trajectory.wind_y = 0.0f;
    config->trajectory.orbit_cx = 0.0f;
    config->trajectory.orbit_cy = 0.0f;
    config->trajectory.orbit_speed = 0.0f;

    /* Emission: 10 particles/second, full circle */
    config->emission.angle_min = 0.0f;
    config->emission.angle_max = 2.0f * M_PI_F;
    config->emission.speed_min = 50.0f;
    config->emission.speed_max = 100.0f;
    config->emission.rate = 10.0f;
    config->emission.burst_count = 10;
    config->emission.burst_interval = 0;
    config->emission.burst_enabled = 0;

    /* Lifecycle: 1000ms default */
    config->lifecycle.life_min = 800;
    config->lifecycle.life_max = 1200;
    config->lifecycle.effect_duration = 0;  /* Infinite */
    config->lifecycle.auto_cleanup = 1;
    config->lifecycle.loop = 1;

    /* Color: White solid */
    config->color.mode = PARTICLE_COLOR_SOLID;
    config->color.color_start = 0xFFFFFFFF;
    config->color.color_end = 0xFFFFFFFF;
    config->color.palette_size = 0;

    /* Opacity: Fade from full to transparent */
    config->opacity.start = 255;
    config->opacity.end = 0;
    config->opacity.easing = PARTICLE_EASING_LINEAR;

    /* Scale: 1.0 constant */
    config->scale.start = 1.0f;
    config->scale.end = 1.0f;
    config->scale.min = 0.8f;
    config->scale.max = 1.2f;

    /* Rotation: None */
    config->rotation.angle_min = 0.0f;
    config->rotation.angle_max = 0.0f;
    config->rotation.speed_min = 0.0f;
    config->rotation.speed_max = 0.0f;
    config->rotation.align_velocity = 0;

    /* Boundary: None */
    config->boundary.behavior = PARTICLE_BOUNDARY_NONE;
    config->boundary.left = 0.0f;
    config->boundary.top = 0.0f;
    config->boundary.right = 0.0f;
    config->boundary.bottom = 0.0f;
    config->boundary.reflect_damping = 0.8f;

    /* Behavior: None */
    config->behavior.mode = PARTICLE_BEHAVIOR_NONE;
    config->behavior.pulse_frequency = 1.0f;
    config->behavior.pulse_amplitude = 0.5f;
    config->behavior.breathe_frequency = 1.0f;

    /* Render: Normal blend */
    config->render.blend_mode = PARTICLE_BLEND_NORMAL;
    config->render.texture = NULL;
    config->render.base_size = 8.0f;

    /* Callbacks: None */
    config->callbacks.on_particle_init = NULL;
    config->callbacks.on_particle_update = NULL;
    config->callbacks.on_particle_render = NULL;
    config->callbacks.on_particle_death = NULL;
    config->callbacks.on_emitter_start = NULL;
    config->callbacks.on_emitter_stop = NULL;
    config->callbacks.user_data = NULL;
}

/*============================================================================*
 *                           Configuration Validation
 *============================================================================*/

particle_config_error_t particle_effect_config_validate(
    const particle_effect_config_t *config)
{
    if (config == NULL)
    {
        return PARTICLE_CONFIG_ERR_MAGIC;
    }

    /* Check magic number */
    if (config->magic != PARTICLE_CONFIG_MAGIC)
    {
        return PARTICLE_CONFIG_ERR_MAGIC;
    }

    /* Check version */
    if (config->version > PARTICLE_CONFIG_VERSION)
    {
        return PARTICLE_CONFIG_ERR_VERSION;
    }

    /* Validate shape */
    if (config->shape.type > PARTICLE_SHAPE_RING)
    {
        return PARTICLE_CONFIG_ERR_SHAPE;
    }

    /* Validate shape-specific parameters */
    switch (config->shape.type)
    {
    case PARTICLE_SHAPE_CIRCLE:
        if (config->shape.circle.radius < 0.0f)
        {
            return PARTICLE_CONFIG_ERR_SHAPE;
        }
        break;
    case PARTICLE_SHAPE_RECT:
        if (config->shape.rect.w < 0.0f || config->shape.rect.h < 0.0f)
        {
            return PARTICLE_CONFIG_ERR_SHAPE;
        }
        break;
    case PARTICLE_SHAPE_RING:
        if (config->shape.ring.inner_r < 0.0f ||
            config->shape.ring.outer_r < 0.0f ||
            config->shape.ring.inner_r > config->shape.ring.outer_r)
        {
            return PARTICLE_CONFIG_ERR_SHAPE;
        }
        break;
    default:
        break;
    }

    /* Validate trajectory */
    if (config->trajectory.type > PARTICLE_TRAJECTORY_ORBIT)
    {
        return PARTICLE_CONFIG_ERR_TRAJECTORY;
    }

    /* Validate emission */
    if (config->emission.speed_min < 0.0f ||
        config->emission.speed_max < config->emission.speed_min)
    {
        return PARTICLE_CONFIG_ERR_EMISSION;
    }

    /* Validate lifecycle */
    if (config->lifecycle.life_max < config->lifecycle.life_min)
    {
        return PARTICLE_CONFIG_ERR_LIFECYCLE;
    }

    /* Validate color */
    if (config->color.mode > PARTICLE_COLOR_RAINBOW)
    {
        return PARTICLE_CONFIG_ERR_COLOR;
    }

    if (config->color.mode == PARTICLE_COLOR_RANDOM &&
        config->color.palette_size == 0)
    {
        return PARTICLE_CONFIG_ERR_COLOR;
    }

    /* Validate scale */
    if (config->scale.min < 0.0f || config->scale.max < config->scale.min)
    {
        return PARTICLE_CONFIG_ERR_SCALE;
    }

    /* Validate boundary */
    if (config->boundary.behavior > PARTICLE_BOUNDARY_WRAP)
    {
        return PARTICLE_CONFIG_ERR_BOUNDARY;
    }

    /* Validate behavior */
    if (config->behavior.mode > PARTICLE_BEHAVIOR_BREATHE)
    {
        return PARTICLE_CONFIG_ERR_BEHAVIOR;
    }

    /* Validate render */
    if (config->render.blend_mode > PARTICLE_BLEND_MULTIPLY)
    {
        return PARTICLE_CONFIG_ERR_RENDER;
    }

    if (config->render.base_size <= 0.0f)
    {
        return PARTICLE_CONFIG_ERR_RENDER;
    }

    return PARTICLE_CONFIG_OK;
}

/*============================================================================*
 *                           Binary Format Loading
 *============================================================================*/

/**
 * @brief Convert binary format to runtime config
 */
static void binary_to_runtime(particle_effect_config_t *config,
                              const particle_effect_config_binary_t *binary)
{
    /* Copy version control */
    config->magic = binary->magic;
    config->version = binary->version;
    config->size = (uint16_t)sizeof(particle_effect_config_t);
    config->flags = binary->flags;

    /* Copy all pure-data sub-structures directly */
    config->shape = binary->shape;
    config->trajectory = binary->trajectory;
    config->emission = binary->emission;
    config->lifecycle = binary->lifecycle;
    config->color = binary->color;
    config->opacity = binary->opacity;
    config->scale = binary->scale;
    config->rotation = binary->rotation;
    config->boundary = binary->boundary;
    config->behavior = binary->behavior;

    /* Convert render config (binary has no pointer) */
    config->render.blend_mode = (particle_blend_mode_t)binary->render.blend_mode;
    config->render.base_size = binary->render.base_size;
    config->render.texture = NULL;  /* Runtime only */

    /* Clear callbacks (runtime only, not in binary) */
    config->callbacks.on_particle_init = NULL;
    config->callbacks.on_particle_update = NULL;
    config->callbacks.on_particle_render = NULL;
    config->callbacks.on_particle_death = NULL;
    config->callbacks.on_emitter_start = NULL;
    config->callbacks.on_emitter_stop = NULL;
    config->callbacks.user_data = NULL;

    /* Clear reserved */
    memset(config->reserved, 0, sizeof(config->reserved));
}

/**
 * @brief Convert runtime config to binary format
 */
static void runtime_to_binary(particle_effect_config_binary_t *binary,
                              const particle_effect_config_t *config)
{
    /* Clear entire structure first */
    memset(binary, 0, sizeof(particle_effect_config_binary_t));

    /* Copy version control */
    binary->magic = config->magic;
    binary->version = config->version;
    binary->size = PARTICLE_CONFIG_BINARY_SIZE;
    binary->flags = config->flags;

    /* Copy all pure-data sub-structures directly */
    binary->shape = config->shape;
    binary->trajectory = config->trajectory;
    binary->emission = config->emission;
    binary->lifecycle = config->lifecycle;
    binary->color = config->color;
    binary->opacity = config->opacity;
    binary->scale = config->scale;
    binary->rotation = config->rotation;
    binary->boundary = config->boundary;
    binary->behavior = config->behavior;

    /* Convert render config (no pointer in binary) */
    binary->render.blend_mode = (uint32_t)config->render.blend_mode;
    binary->render.base_size = config->render.base_size;
    binary->render.reserved = 0;

    /* Callbacks are NOT saved - runtime only */
}

int particle_effect_config_load_mem(particle_effect_config_t *config,
                                    const void *addr)
{
    if (config == NULL || addr == NULL)
    {
        return -1;
    }

    /* Cast to binary format structure */
    const particle_effect_config_binary_t *binary =
        (const particle_effect_config_binary_t *)addr;

    /* Validate magic number */
    if (binary->magic != PARTICLE_CONFIG_MAGIC)
    {
        return -1;
    }

    /* Validate version (must be compatible) */
    if (binary->version > PARTICLE_CONFIG_VERSION)
    {
        return -1;
    }

    /* Validate size matches binary format */
    if (binary->size != PARTICLE_CONFIG_BINARY_SIZE)
    {
        return -1;
    }

    /* Convert binary format to runtime config */
    binary_to_runtime(config, binary);

    return 0;
}

int particle_effect_config_save(const particle_effect_config_t *config,
                                void *buffer, size_t buffer_size)
{
    if (config == NULL || buffer == NULL)
    {
        return -1;
    }

    /* Check buffer size */
    if (buffer_size < PARTICLE_CONFIG_BINARY_SIZE)
    {
        return -1;
    }

    /* Convert runtime config to binary format */
    particle_effect_config_binary_t *binary =
        (particle_effect_config_binary_t *)buffer;
    runtime_to_binary(binary, config);

    return PARTICLE_CONFIG_BINARY_SIZE;
}

int particle_effect_config_load_fs(particle_effect_config_t *config,
                                   const char *filepath)
{
#if defined(_WIN32) || defined(__linux__)
    /* Simulator environment - use standard file I/O */
    if (config == NULL || filepath == NULL)
    {
        return -1;
    }

    FILE *fp = fopen(filepath, "rb");
    if (fp == NULL)
    {
        return -1;
    }

    /* Read binary format structure */
    particle_effect_config_binary_t binary;
    size_t read_size = fread(&binary, 1, sizeof(binary), fp);
    fclose(fp);

    if (read_size != sizeof(binary))
    {
        return -1;
    }

    /* Validate and convert */
    if (binary.magic != PARTICLE_CONFIG_MAGIC)
    {
        return -1;
    }

    if (binary.version > PARTICLE_CONFIG_VERSION)
    {
        return -1;
    }

    if (binary.size != PARTICLE_CONFIG_BINARY_SIZE)
    {
        return -1;
    }

    binary_to_runtime(config, &binary);

    return 0;
#else
    /* Embedded environment - use VFS API */
    (void)config;
    (void)filepath;
    return -1;
#endif
}

int particle_effect_config_load_ftl(particle_effect_config_t *config,
                                    uint32_t ftl_addr)
{
    /* FTL mode is reserved for future implementation */
    (void)config;
    (void)ftl_addr;
    return -1;
}
