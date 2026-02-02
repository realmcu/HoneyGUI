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

#ifndef M_PI_F
#define M_PI_F 3.14159265358979323846f
#endif

#ifndef DRV_LCD_WIDTH
#define DRV_LCD_WIDTH  480
#endif

#ifndef DRV_LCD_HEIGHT
#define DRV_LCD_HEIGHT 480
#endif

#define PARTICLE_POOL_SIZE 256

void effect_bubble_config(particle_effect_config_t *config)
{
    if (config == NULL)
    {
        return;
    }

    particle_effect_config_init(config);

    config->shape.type = PARTICLE_SHAPE_LINE;
    config->shape.line.x1 = 50.0f;
    config->shape.line.y1 = 400.0f;
    config->shape.line.x2 = 350.0f;
    config->shape.line.y2 = 400.0f;

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

    config->boundary.behavior = PARTICLE_BOUNDARY_REFLECT;
    config->boundary.left = 0.0f;
    config->boundary.top = -50.0f;
    config->boundary.right = 400.0f;
    config->boundary.bottom = 450.0f;
    config->boundary.reflect_damping = 0.9f;

    config->render.blend_mode = PARTICLE_BLEND_NORMAL;
    config->render.base_size = 12.0f;
}

gui_particle_widget_t *effect_bubble_demo_init(void)
{
    gui_obj_t *root = gui_obj_get_root();

    gui_particle_widget_t *widget = gui_particle_widget_create(root, "bubble_demo",
                                                               0, 0, DRV_LCD_WIDTH, DRV_LCD_HEIGHT,
                                                               PARTICLE_POOL_SIZE);
    if (widget == NULL)
    {
        return NULL;
    }

    particle_effect_config_t config;
    effect_bubble_config(&config);

    config.color.mode = PARTICLE_COLOR_GRADIENT;
    config.color.color_start = 0xFF88DDFF;
    config.color.color_end = 0xFF44AAFF;

    config.shape.line.x1 = 0.0f;
    config.shape.line.y1 = (float)DRV_LCD_HEIGHT;
    config.shape.line.x2 = (float)DRV_LCD_WIDTH;
    config.shape.line.y2 = (float)DRV_LCD_HEIGHT;

    config.boundary.right = (float)DRV_LCD_WIDTH;
    config.boundary.bottom = (float)DRV_LCD_HEIGHT + 50.0f;
    config.emission.rate = 8.0f;
    config.scale.min = 0.5f;
    config.scale.max = 1.5f;
    config.render.base_size = 10.0f;

    gui_particle_widget_add_effect(widget, &config);

    return widget;
}
