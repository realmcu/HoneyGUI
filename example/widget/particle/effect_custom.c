/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_custom.c
 * @brief Custom Particle Effect with Callbacks Demo
 */

#include <math.h>
#include "effect_custom.h"
#include "gui_obj.h"
#include "gui_api_dc.h"
#include "def_type.h"

#define PARTICLE_POOL_SIZE 256

/*============================================================================*
 *                           Private Functions
 *============================================================================*/

static uint32_t custom_rand(void)
{
    static uint32_t seed = 12345;
    seed = seed * 1103515245 + 12345;
    return (seed >> 16) & 0x7FFF;
}

static void custom_particle_init_cb(particle_t *particle, void *user_data)
{
    GUI_UNUSED(user_data);
    if (particle == NULL)
    {
        return;
    }
    float scale_variation = 0.5f + (float)(custom_rand() % 100) / 100.0f;
    particle->scale *= scale_variation;
}

static void custom_particle_update_cb(particle_t *particle, void *user_data)
{
    GUI_UNUSED(user_data);
    if (particle == NULL)
    {
        return;
    }
    float life_ratio = (float)particle->life / 2000.0f;
    particle->vx += 0.5f * sinf(life_ratio * 10.0f);
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void effect_custom_config(particle_effect_config_t *config)
{
    if (config == NULL)
    {
        return;
    }

    particle_effect_config_init(config);

    /* Shape: Circle at center (caller must set cx/cy) */
    config->shape.type = PARTICLE_SHAPE_CIRCLE;
    config->shape.circle.cx = 0.0f;
    config->shape.circle.cy = 0.0f;
    config->shape.circle.radius = 50.0f;

    /* Trajectory: Upward gravity with wind */
    config->trajectory.type = PARTICLE_TRAJECTORY_GRAVITY;
    config->trajectory.gravity = -30.0f;
    config->trajectory.damping = 0.02f;
    config->trajectory.wind_x = 10.0f;

    /* Emission: 360 degrees, continuous */
    config->emission.angle_min = 0.0f;
    config->emission.angle_max = 2.0f * M_PI_F;
    config->emission.speed_min = 20.0f;
    config->emission.speed_max = 60.0f;
    config->emission.rate = 15.0f;
    config->emission.burst_enabled = 0;

    /* Lifecycle: Medium duration, looping */
    config->lifecycle.life_min = 2000;
    config->lifecycle.life_max = 4000;
    config->lifecycle.auto_cleanup = 0;
    config->lifecycle.loop = 1;

    /* Color: Rainbow mode */
    config->color.mode = PARTICLE_COLOR_RAINBOW;

    /* Opacity: Fade out */
    config->opacity.start = 255;
    config->opacity.end = 0;
    config->opacity.easing = PARTICLE_EASING_EASE_OUT;

    /* Scale: Shrink over time */
    config->scale.start = 1.5f;
    config->scale.end = 0.3f;
    config->scale.min = 0.8f;
    config->scale.max = 1.2f;

    /* Rotation: Random spin */
    config->rotation.angle_min = 0.0f;
    config->rotation.angle_max = 2.0f * M_PI_F;
    config->rotation.speed_min = -1.0f;
    config->rotation.speed_max = 1.0f;
    config->rotation.align_velocity = 0;

    /* Boundary: Wrap around (caller must set right/bottom) */
    config->boundary.behavior = PARTICLE_BOUNDARY_WRAP;
    config->boundary.left = 0.0f;
    config->boundary.top = 0.0f;
    config->boundary.right = 0.0f;
    config->boundary.bottom = 0.0f;

    /* Render: Additive blending */
    config->render.blend_mode = PARTICLE_BLEND_ADDITIVE;
    config->render.base_size = 8.0f;

    /* Callbacks: Custom init and update */
    config->callbacks.on_particle_init = custom_particle_init_cb;
    config->callbacks.on_particle_update = custom_particle_update_cb;
    config->callbacks.user_data = NULL;
}

gui_particle_widget_t *effect_custom_demo_init(void)
{
    gui_obj_t *root = gui_obj_get_root();
    gui_dispdev_t *dc = gui_get_dc();
    int screen_w = dc->screen_width;
    int screen_h = dc->screen_height;

    gui_particle_widget_t *widget = gui_particle_widget_create(root, "custom_demo",
                                                               0, 0, screen_w, screen_h,
                                                               PARTICLE_POOL_SIZE);
    if (widget == NULL)
    {
        return NULL;
    }

    particle_effect_config_t config;
    effect_custom_config(&config);

    /* Override shape to match widget size */
    config.shape.circle.cx = (float)(screen_w / 2);
    config.shape.circle.cy = (float)(screen_h / 2);

    /* Update boundary to match widget */
    gui_obj_t *base = GUI_BASE(widget);
    config.boundary.right = (float)base->w;
    config.boundary.bottom = (float)base->h;

    gui_particle_widget_add_effect(widget, &config);

    return widget;
}
