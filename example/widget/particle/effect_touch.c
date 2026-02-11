/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_touch.c
 * @brief Touch Feedback Particle Effect Configuration and Demo
 */

#include "effect_touch.h"
#include "gui_obj.h"
#include "gui_api_dc.h"
#include "tp_algo.h"
#include "def_type.h"

#define PARTICLE_POOL_SIZE 256

/*============================================================================*
 *                           Local Variables
 *============================================================================*/

static gui_particle_widget_t *s_touch_widget = NULL;
static particle_effect_handle_t s_touch_handle = PARTICLE_INVALID_HANDLE;
static uint8_t s_touch_was_pressing = 0;

/*============================================================================*
 *                           Local Functions
 *============================================================================*/

static void touch_update_cb(void *user_data)
{
    GUI_UNUSED(user_data);

    if (s_touch_widget == NULL || s_touch_handle == PARTICLE_INVALID_HANDLE)
    {
        return;
    }

    touch_info_t *tp = tp_get_info();
    if (tp == NULL)
    {
        return;
    }

    float x = (float)(tp->x + tp->deltaX);
    float y = (float)(tp->y + tp->deltaY);

    /* Handle touch pressed event - trigger burst */
    if (tp->pressed && !s_touch_was_pressing)
    {
        gui_particle_widget_update_position(s_touch_widget, s_touch_handle, x, y);
        gui_particle_widget_trigger_burst(s_touch_widget, s_touch_handle);
        s_touch_was_pressing = 1;
    }
    /* Handle touch dragging - continuous emission */
    else if (tp->pressing && s_touch_was_pressing)
    {
        gui_particle_widget_update_position(s_touch_widget, s_touch_handle, x, y);
    }
    /* Handle touch released event - final burst and stop */
    else if (tp->released && s_touch_was_pressing)
    {
        gui_particle_widget_update_position(s_touch_widget, s_touch_handle, x, y);
        gui_particle_widget_trigger_burst(s_touch_widget, s_touch_handle);
        gui_particle_widget_stop_effect(s_touch_widget, s_touch_handle);
        s_touch_was_pressing = 0;
    }
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void effect_touch_config(particle_effect_config_t *config)
{
    if (config == NULL)
    {
        return;
    }

    particle_effect_config_init(config);

    /* Shape: Point at touch position */
    config->shape.type = PARTICLE_SHAPE_POINT;

    /* Trajectory: Linear with slight damping */
    config->trajectory.type = PARTICLE_TRAJECTORY_LINEAR;
    config->trajectory.damping = 0.1f;

    /* Emission: Burst mode on touch */
    config->emission.angle_min = 0.0f;
    config->emission.angle_max = 2.0f * M_PI_F;
    config->emission.speed_min = 50.0f;
    config->emission.speed_max = 100.0f;
    config->emission.rate = 20.0f;
    config->emission.burst_count = 15;
    config->emission.burst_enabled = 1;

    /* Lifecycle: Short-lived, auto cleanup */
    config->lifecycle.life_min = 400;
    config->lifecycle.life_max = 800;
    config->lifecycle.auto_cleanup = 1;
    config->lifecycle.loop = 0;

    /* Color: Solid golden yellow */
    config->color.mode = PARTICLE_COLOR_SOLID;
    config->color.color_start = 0xFFFFDD44;
    config->color.color_end = 0xFFFFDD44;

    /* Opacity: Quick fade out */
    config->opacity.start = 255;
    config->opacity.end = 0;
    config->opacity.easing = PARTICLE_EASING_EASE_OUT;

    /* Scale: Shrink to nothing */
    config->scale.start = 0.5f;
    config->scale.end = 0.0f;
    config->scale.min = 0.3f;
    config->scale.max = 0.8f;

    /* Rotation: Random with spin */
    config->rotation.angle_min = 0.0f;
    config->rotation.angle_max = 2.0f * M_PI_F;
    config->rotation.speed_min = -1.0f;
    config->rotation.speed_max = 1.0f;

    /* Boundary: Defaults to 0, should be overridden with actual widget size */
    config->boundary.behavior = PARTICLE_BOUNDARY_KILL;
    config->boundary.left = 0.0f;
    config->boundary.top = 0.0f;
    config->boundary.right = 0.0f;
    config->boundary.bottom = 0.0f;
    config->boundary.reflect_damping = 0.8f;

    /* Behavior: Touch feedback mode */
    config->behavior.mode = PARTICLE_BEHAVIOR_TOUCH_FEEDBACK;

    /* Render: Additive blending for glow */
    config->render.blend_mode = PARTICLE_BLEND_ADDITIVE;
    config->render.base_size = 10.0f;
}

gui_particle_widget_t *effect_touch_demo_init(void)
{
    gui_obj_t *root = gui_obj_get_root();
    gui_dispdev_t *dc = gui_get_dc();
    int screen_w = dc->screen_width;
    int screen_h = dc->screen_height;

    s_touch_widget = gui_particle_widget_create(root, "touch_demo",
                                                0, 0, screen_w, screen_h,
                                                PARTICLE_POOL_SIZE);
    if (s_touch_widget == NULL)
    {
        return NULL;
    }

    particle_effect_config_t config;
    effect_touch_config(&config);

    /* Customize touch appearance */
    config.color.mode = PARTICLE_COLOR_GRADIENT;
    config.color.color_start = 0xFFFF00FF;
    config.color.color_end = 0xFFFF88FF;
    config.render.base_size = 5.0f;

    /* Adjust emission for touch feedback */
    config.emission.burst_count = 15;
    config.emission.rate = 30.0f;
    config.emission.burst_enabled = 0;

    /* Disable auto_cleanup so emitter persists */
    config.lifecycle.auto_cleanup = 0;

    /* Update boundary to match actual widget size */
    gui_obj_t *base = GUI_BASE(s_touch_widget);
    config.boundary.right = (float)base->w;
    config.boundary.bottom = (float)base->h;

    s_touch_handle = gui_particle_widget_add_effect(s_touch_widget, &config);

    /* Start with emitter disabled */
    gui_particle_widget_stop_effect(s_touch_widget, s_touch_handle);

    gui_particle_widget_set_update_cb(s_touch_widget, touch_update_cb, NULL);

    return s_touch_widget;
}
