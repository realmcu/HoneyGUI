/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_bubble.c
 * @brief Bubble Particle Effect Configuration and Demo
 */

#include "effect_bubble.h"
#include "gui_obj.h"
#include "gui_api_dc.h"
#include "def_type.h"

#define PARTICLE_POOL_SIZE 256

/*============================================================================*
 *                           Static Configuration
 *============================================================================*/

static void effect_bubble_config(particle_effect_config_t *config)
{
    if (config == NULL)
    {
        return;
    }

    particle_effect_config_init(config);

    /* Shape: Line at bottom of screen (caller must set y1/y2 to screen height, x2 to screen width) */
    config->shape.type = PARTICLE_SHAPE_LINE;
    config->shape.line.x1 = 0.0f;
    config->shape.line.y1 = 0.0f;
    config->shape.line.x2 = 0.0f;
    config->shape.line.y2 = 0.0f;

    config->trajectory.type = PARTICLE_TRAJECTORY_GRAVITY;
    config->trajectory.gravity = -50.0f;
    config->trajectory.wind_x = 5.0f;

    config->emission.angle_min = -M_PI_F * 0.6f;
    config->emission.angle_max = -M_PI_F * 0.4f;
    config->emission.speed_min = 10.0f;
    config->emission.speed_max = 30.0f;
    config->emission.rate = 8.0f;
    config->emission.burst_count = 1;
    config->emission.burst_enabled = 0;

    config->lifecycle.life_min = 4000;
    config->lifecycle.life_max = 6000;
    config->lifecycle.auto_cleanup = 0;
    config->lifecycle.loop = 1;

    config->color.mode = PARTICLE_COLOR_SOLID;
    config->color.color_start = 0xAADDEEFF;
    config->color.color_end = 0xAADDEEFF;

    config->opacity.start = 180;
    config->opacity.end = 100;
    config->opacity.easing = PARTICLE_EASING_LINEAR;

    config->scale.start = 1.0f;
    config->scale.end = 1.2f;
    config->scale.min = 0.5f;
    config->scale.max = 1.5f;

    /* Boundary: Reflect (caller must set right/bottom) */
    config->boundary.behavior = PARTICLE_BOUNDARY_REFLECT;
    config->boundary.left = 0.0f;
    config->boundary.top = -40.0f;
    config->boundary.right = 0.0f;
    config->boundary.bottom = 0.0f;
    config->boundary.reflect_damping = 0.9f;

    config->render.blend_mode = PARTICLE_BLEND_NORMAL;
    config->render.base_size = 12.0f;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

gui_particle_widget_t *effect_bubble_create(gui_obj_t *parent, const char *name,
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
    effect_bubble_config(&config);

    /* Adapt to widget size */
    config.shape.line.x1 = 0.0f;
    config.shape.line.y1 = (float)h;
    config.shape.line.x2 = (float)w;
    config.shape.line.y2 = (float)h;

    config.boundary.right = (float)w;
    config.boundary.bottom = (float)h + 40.0f;

    gui_particle_widget_add_effect(widget, &config);

    return widget;
}
