/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_trail.c
 * @brief Trail Particle Effect Configuration and Demo
 */

#include "effect_trail.h"
#include "gui_obj.h"
#include "gui_api_dc.h"
#include "tp_algo.h"
#include "def_type.h"

#define PARTICLE_POOL_SIZE 256

/*============================================================================*
 *                           Local Variables
 *============================================================================*/

static gui_particle_widget_t *s_trail_widget = NULL;
static particle_effect_handle_t s_trail_handle = PARTICLE_INVALID_HANDLE;
static uint8_t s_trail_was_pressing = 0;

/*============================================================================*
 *                           Local Functions
 *============================================================================*/

static void trail_update_cb(void *user_data)
{
    GUI_UNUSED(user_data);

    if (s_trail_widget == NULL || s_trail_handle == PARTICLE_INVALID_HANDLE)
    {
        return;
    }

    touch_info_t *tp = tp_get_info();
    if (tp == NULL)
    {
        return;
    }

    uint8_t is_pressing = tp->pressing;
    if (tp->type == TOUCH_HOLD_X || tp->type == TOUCH_HOLD_Y ||
        tp->type == TOUCH_SHORT || tp->type == TOUCH_LONG)
    {
        is_pressing = 1;
    }

    if (is_pressing)
    {
        float x = (float)(tp->x + tp->deltaX);
        float y = (float)(tp->y + tp->deltaY);

        gui_particle_widget_update_position(s_trail_widget, s_trail_handle, x, y);
        s_trail_was_pressing = 1;
    }
    else if (s_trail_was_pressing)
    {
        gui_particle_widget_stop_effect(s_trail_widget, s_trail_handle);
        s_trail_was_pressing = 0;
    }
}

/*============================================================================*
 *                           Static Configuration
 *============================================================================*/

static void effect_trail_config(particle_effect_config_t *config)
{
    if (config == NULL)
    {
        return;
    }

    particle_effect_config_init(config);

    /* Shape: Point (position updated externally) */
    config->shape.type = PARTICLE_SHAPE_POINT;

    /* Trajectory: Linear with high damping */
    config->trajectory.type = PARTICLE_TRAJECTORY_LINEAR;
    config->trajectory.damping = 0.95f;

    /* Emission: Continuous, full circle spread */
    config->emission.angle_min = 0.0f;
    config->emission.angle_max = 2.0f * M_PI_F;
    config->emission.speed_min = 20.0f;
    config->emission.speed_max = 50.0f;
    config->emission.rate = 30.0f;
    config->emission.burst_count = 1;
    config->emission.burst_enabled = 0;

    /* Lifecycle: Short-lived, continuous loop */
    config->lifecycle.life_min = 300;
    config->lifecycle.life_max = 600;
    config->lifecycle.auto_cleanup = 0;
    config->lifecycle.loop = 1;

    /* Color: Solid light blue */
    config->color.mode = PARTICLE_COLOR_SOLID;
    config->color.color_start = 0xFFAADDFF;
    config->color.color_end = 0xFFAADDFF;

    /* Opacity: Fade out linearly */
    config->opacity.start = 200;
    config->opacity.end = 0;
    config->opacity.easing = PARTICLE_EASING_LINEAR;

    /* Scale: Shrink significantly */
    config->scale.start = 1.0f;
    config->scale.end = 0.1f;
    config->scale.min = 0.5f;
    config->scale.max = 1.0f;

    /* Boundary: Defaults to 0, should be overridden with actual widget size */
    config->boundary.behavior = PARTICLE_BOUNDARY_KILL;
    config->boundary.left = 0.0f;
    config->boundary.top = 0.0f;
    config->boundary.right = 0.0f;
    config->boundary.bottom = 0.0f;
    config->boundary.reflect_damping = 0.8f;

    /* Behavior: Trail mode */
    config->behavior.mode = PARTICLE_BEHAVIOR_TRAIL;

    /* Render: Normal blending */
    config->render.blend_mode = PARTICLE_BLEND_NORMAL;
    config->render.base_size = 8.0f;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

gui_particle_widget_t *effect_trail_create(gui_obj_t *parent, const char *name,
                                           int16_t x, int16_t y, int16_t w, int16_t h)
{
    s_trail_widget = gui_particle_widget_create(parent, name,
                                                x, y, w, h,
                                                PARTICLE_POOL_SIZE);
    if (s_trail_widget == NULL)
    {
        return NULL;
    }

    particle_effect_config_t config;
    effect_trail_config(&config);

    /* Customize trail appearance */
    config.color.mode = PARTICLE_COLOR_GRADIENT;
    config.color.color_start = 0xFF00FFFF;
    config.color.color_end = 0xFF0088FF;
    config.render.base_size = 6.0f;

    /* Set boundary to match actual widget size */
    config.boundary.right = (float)w;
    config.boundary.bottom = (float)h;

    s_trail_handle = gui_particle_widget_add_effect(s_trail_widget, &config);

    /* Start with emitter disabled */
    gui_particle_widget_stop_effect(s_trail_widget, s_trail_handle);

    gui_particle_widget_set_update_cb(s_trail_widget, trail_update_cb, NULL);

    return s_trail_widget;
}
