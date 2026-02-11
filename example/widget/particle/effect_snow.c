/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_snow.c
 * @brief Snow Particle Effect Configuration and Demo
 */

#include "effect_snow.h"
#include "gui_obj.h"
#include "gui_api_dc.h"
#include "def_type.h"

#define PARTICLE_POOL_SIZE 256

/*============================================================================*
 *                           Static Configuration
 *============================================================================*/

static void effect_snow_config(particle_effect_config_t *config)
{
    if (config == NULL)
    {
        return;
    }

    particle_effect_config_init(config);

    /* Shape: Line at top of screen (caller must set x2 to screen width) */
    config->shape.type = PARTICLE_SHAPE_LINE;
    config->shape.line.x1 = 0.0f;
    config->shape.line.y1 = 0.0f;
    config->shape.line.x2 = 0.0f;
    config->shape.line.y2 = 0.0f;

    /* Trajectory: Light gravity with wind */
    config->trajectory.type = PARTICLE_TRAJECTORY_GRAVITY;
    config->trajectory.gravity = 30.0f;
    config->trajectory.wind_x = 10.0f;

    /* Emission: Continuous, downward angle */
    config->emission.angle_min = M_PI_F * 0.4f;
    config->emission.angle_max = M_PI_F * 0.6f;
    config->emission.speed_min = 20.0f;
    config->emission.speed_max = 40.0f;
    config->emission.rate = 15.0f;
    config->emission.burst_count = 1;
    config->emission.burst_enabled = 0;

    /* Lifecycle: Long-lived, continuous */
    config->lifecycle.life_min = 3000;
    config->lifecycle.life_max = 5000;
    config->lifecycle.auto_cleanup = 0;
    config->lifecycle.loop = 1;

    /* Color: White */
    config->color.mode = PARTICLE_COLOR_SOLID;
    config->color.color_start = 0xFFFFFFFF;
    config->color.color_end = 0xFFFFFFFF;

    /* Opacity: Slight fade */
    config->opacity.start = 230;
    config->opacity.end = 100;
    config->opacity.easing = PARTICLE_EASING_LINEAR;

    /* Scale: Constant size, random initial */
    config->scale.start = 1.0f;
    config->scale.end = 1.0f;
    config->scale.min = 0.3f;
    config->scale.max = 1.0f;

    /* Rotation: Slow random spin */
    config->rotation.angle_min = 0.0f;
    config->rotation.angle_max = 2.0f * M_PI_F;
    config->rotation.speed_min = -0.5f;
    config->rotation.speed_max = 0.5f;

    /* Boundary: Kill outside extended area (caller must set right/bottom) */
    config->boundary.behavior = PARTICLE_BOUNDARY_KILL;
    config->boundary.left = -8.0f;
    config->boundary.top = -8.0f;
    config->boundary.right = 0.0f;
    config->boundary.bottom = 0.0f;
    config->boundary.reflect_damping = 0.8f;

    /* Render: Normal blending */
    config->render.blend_mode = PARTICLE_BLEND_NORMAL;
    config->render.base_size = 4.0f;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

gui_particle_widget_t *effect_snow_create(gui_obj_t *parent, const char *name,
                                          int16_t x, int16_t y, int16_t w, int16_t h)
{
    gui_particle_widget_t *widget = gui_particle_widget_create(parent, name,
                                                               x, y, w, h,
                                                               PARTICLE_POOL_SIZE);
    if (widget == NULL)
    {
        return NULL;
    }

    particle_effect_config_t config;
    effect_snow_config(&config);

    /* Adapt to widget size */
    config.shape.line.x1 = 0.0f;
    config.shape.line.y1 = 0.0f;
    config.shape.line.x2 = (float)w;
    config.shape.line.y2 = 0.0f;

    config.emission.rate = 25.0f;
    config.color.color_start = 0xFFCCEEFF;
    config.scale.min = 0.3f;
    config.scale.max = 0.8f;

    config.boundary.left = -8.0f;
    config.boundary.top = -8.0f;
    config.boundary.right = (float)w + 8.0f;
    config.boundary.bottom = (float)h + 8.0f;

    gui_particle_widget_add_effect(widget, &config);

    return widget;
}


