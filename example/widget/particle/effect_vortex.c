/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_vortex.c
 * @brief Energy Vortex Particle Effect - Spiral Inward Effect
 *
 * This example demonstrates:
 * 1. Ring emission shape (particles emit from outer ring)
 * 2. Orbital trajectory (particles spiral toward center)
 * 3. Rainbow color mode (hue cycles over lifetime)
 * 4. Custom update callback for inward spiral motion
 */

#include "effect_vortex.h"
#include "gui_obj.h"
#include "gui_api_os.h"
#include "gui_api_dc.h"
#include "def_type.h"
#include <math.h>

#define PARTICLE_POOL_SIZE 300
#define VORTEX_OUTER_RATIO 0.475f  /* 95% diameter / 2 */
#define VORTEX_INNER_RATIO 0.425f  /* 85% diameter / 2 */
#define VORTEX_PULL_SPEED  60.0f   /* Inward pull speed */
#define VORTEX_SPIN_SPEED  2.5f    /* Angular velocity (rad/s) */

static gui_particle_widget_t *s_vortex_widget = NULL;
static float s_vortex_center_x = 0.0f;
static float s_vortex_center_y = 0.0f;
static float s_vortex_outer_r = 0.0f;
static float s_vortex_inner_r = 0.0f;

/**
 * @brief Custom particle update callback for spiral motion
 *
 * Overrides default linear/gravity motion with orbital + inward pull
 */
static void vortex_particle_update(particle_t *p, void *user_data)
{
    GUI_UNUSED(user_data);
    if (p == NULL || !p->active)
    {
        return;
    }

    /* Calculate vector from center to particle */
    float dx = p->x - s_vortex_center_x;
    float dy = p->y - s_vortex_center_y;
    float dist = sqrtf(dx * dx + dy * dy);

    if (dist < 5.0f)
    {
        /* Kill particle when it reaches center */
        p->life = 0;
        return;
    }

    /* Normalize direction */
    float nx = dx / dist;
    float ny = dy / dist;

    /* Tangent direction (perpendicular, for spin) */
    float tx = -ny;
    float ty = nx;

    /* Combine inward pull + tangential spin */
    /* Spin faster as particle gets closer to center */
    float spin_factor = 1.0f + (s_vortex_outer_r - dist) / s_vortex_outer_r * 2.0f;
    float spin_speed = VORTEX_SPIN_SPEED * spin_factor;

    p->vx = -nx * VORTEX_PULL_SPEED + tx * spin_speed * dist * 0.5f;
    p->vy = -ny * VORTEX_PULL_SPEED + ty * spin_speed * dist * 0.5f;

    /* Rotate particle to face movement direction */
    p->rotation = atan2f(p->vy, p->vx);
}

void effect_vortex_config(particle_effect_config_t *config)
{
    if (config == NULL)
    {
        return;
    }

    particle_effect_config_init(config);

    /* Ring emission from outer edge */
    config->shape.type = PARTICLE_SHAPE_RING;
    config->shape.ring.cx = s_vortex_center_x;
    config->shape.ring.cy = s_vortex_center_y;
    config->shape.ring.inner_r = s_vortex_inner_r;
    config->shape.ring.outer_r = s_vortex_outer_r;

    /* Orbital trajectory - we'll override with custom callback */
    config->trajectory.type = PARTICLE_TRAJECTORY_ORBIT;
    config->trajectory.orbit_cx = s_vortex_center_x;
    config->trajectory.orbit_cy = s_vortex_center_y;
    config->trajectory.orbit_speed = VORTEX_SPIN_SPEED;
    config->trajectory.gravity = 0.0f;
    config->trajectory.damping = 0.0f;

    /* Initial velocity toward center (will be overridden by callback) */
    config->emission.angle_min = 0.0f;
    config->emission.angle_max = 2.0f * M_PI_F;
    config->emission.speed_min = 10.0f;
    config->emission.speed_max = 30.0f;
    config->emission.rate = 25.0f;
    config->emission.burst_enabled = 0;

    /* Long lifetime for full spiral journey */
    config->lifecycle.life_min = 4000;
    config->lifecycle.life_max = 6000;
    config->lifecycle.auto_cleanup = 0;
    config->lifecycle.loop = 1;

    /* Rainbow color - hue cycles over lifetime */
    config->color.mode = PARTICLE_COLOR_RAINBOW;
    config->color.color_start = 0xFFFF0000;
    config->color.color_end = 0xFF0000FF;

    /* Opacity: start visible, fade as approaching center */
    config->opacity.start = 255;
    config->opacity.end = 100;
    config->opacity.easing = PARTICLE_EASING_EASE_IN;

    /* Scale: shrink as particles spiral inward */
    config->scale.start = 1.5f;
    config->scale.end = 0.2f;
    config->scale.min = 1.0f;
    config->scale.max = 1.5f;

    /* Rotation aligned to velocity */
    config->rotation.align_velocity = 1;
    config->rotation.speed_min = 0.0f;
    config->rotation.speed_max = 0.0f;

    /* No boundary - particles die at center via callback */
    config->boundary.behavior = PARTICLE_BOUNDARY_NONE;

    /* Additive blending for glowing effect */
    config->render.blend_mode = PARTICLE_BLEND_ADDITIVE;
    config->render.base_size = 5.0f;

    /* Register custom update callback */
    config->callbacks.on_particle_update = vortex_particle_update;
    config->callbacks.user_data = NULL;
}

gui_particle_widget_t *effect_vortex_demo_init(void)
{
    gui_obj_t *root = gui_obj_get_root();
    gui_dispdev_t *dc = gui_get_dc();
    int screen_w = dc->screen_width;
    int screen_h = dc->screen_height;

    /* Update center coordinates */
    s_vortex_center_x = (float)(screen_w / 2);
    s_vortex_center_y = (float)(screen_h / 2);

    /* Compute radii from shorter screen dimension */
    float min_dim = (float)(screen_w < screen_h ? screen_w : screen_h);
    s_vortex_outer_r = min_dim * VORTEX_OUTER_RATIO;
    s_vortex_inner_r = min_dim * VORTEX_INNER_RATIO;

    s_vortex_widget = gui_particle_widget_create(root, "vortex_demo",
                                                 0, 0, screen_w, screen_h,
                                                 PARTICLE_POOL_SIZE);
    if (s_vortex_widget == NULL)
    {
        gui_log("Vortex: Failed to create widget\n");
        return NULL;
    }

    particle_effect_config_t config;
    effect_vortex_config(&config);

    gui_particle_widget_add_effect(s_vortex_widget, &config);

    gui_log("Vortex: Demo initialized - energy spiral effect\n");
    return s_vortex_widget;
}
