/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_vortex.c
 * @brief Energy Vortex Particle Effect - Spiral Orbit Effect
 *
 * This example demonstrates:
 * 1. Ring emission shape (particles emit from outer ring)
 * 2. Orbital trajectory (particles spiral around center)
 * 3. Rainbow color mode (hue cycles over lifetime)
 * 4. Custom update callback for spiral motion
 */

#include "effect_vortex.h"
#include "gui_obj.h"
#include "gui_api_os.h"
#include "gui_api_dc.h"
#include "def_type.h"
#include <math.h>

#define PARTICLE_POOL_SIZE 300
#define VORTEX_OUTER_RATIO 0.475f
#define VORTEX_INNER_RATIO 0.425f
#define VORTEX_PULL_SPEED  60.0f
#define VORTEX_SPIN_SPEED  2.5f

static gui_particle_widget_t *s_vortex_widget = NULL;
static float s_vortex_center_x = 0.0f;
static float s_vortex_center_y = 0.0f;
static float s_vortex_outer_r = 0.0f;
static float s_vortex_inner_r = 0.0f;

/**
 * @brief Custom particle update callback for spiral motion
 */
static void vortex_particle_update(particle_t *p, void *user_data)
{
    GUI_UNUSED(user_data);
    if (p == NULL || !p->active)
    {
        return;
    }

    float dx = p->x - s_vortex_center_x;
    float dy = p->y - s_vortex_center_y;
    float dist = sqrtf(dx * dx + dy * dy);

    if (dist < 5.0f)
    {
        p->life = 0;
        return;
    }

    float nx = dx / dist;
    float ny = dy / dist;
    float tx = -ny;
    float ty = nx;

    float spin_factor = 1.0f + (s_vortex_outer_r - dist) / s_vortex_outer_r * 2.0f;
    float spin_speed = VORTEX_SPIN_SPEED * spin_factor;

    p->vx = -nx * VORTEX_PULL_SPEED + tx * spin_speed * dist * 0.5f;
    p->vy = -ny * VORTEX_PULL_SPEED + ty * spin_speed * dist * 0.5f;

    p->rotation = atan2f(p->vy, p->vx);
}

/*============================================================================*
 *                           Static Configuration
 *============================================================================*/

static void effect_vortex_config(particle_effect_config_t *config,
                                 float cx, float cy, float inner_r, float outer_r)
{
    if (config == NULL)
    {
        return;
    }

    particle_effect_config_init(config);

    config->shape.type = PARTICLE_SHAPE_RING;
    config->shape.ring.cx = cx;
    config->shape.ring.cy = cy;
    config->shape.ring.inner_r = inner_r;
    config->shape.ring.outer_r = outer_r;

    config->trajectory.type = PARTICLE_TRAJECTORY_ORBIT;
    config->trajectory.orbit_cx = cx;
    config->trajectory.orbit_cy = cy;
    config->trajectory.orbit_speed = VORTEX_SPIN_SPEED;
    config->trajectory.gravity = 0.0f;
    config->trajectory.damping = 0.0f;

    config->emission.angle_min = 0.0f;
    config->emission.angle_max = 2.0f * M_PI_F;
    config->emission.speed_min = 10.0f;
    config->emission.speed_max = 30.0f;
    config->emission.rate = 25.0f;
    config->emission.burst_enabled = 0;

    config->lifecycle.life_min = 4000;
    config->lifecycle.life_max = 6000;
    config->lifecycle.auto_cleanup = 0;
    config->lifecycle.loop = 1;

    config->color.mode = PARTICLE_COLOR_RAINBOW;
    config->color.color_start = 0xFFFF0000;
    config->color.color_end = 0xFF0000FF;

    config->opacity.start = 255;
    config->opacity.end = 100;
    config->opacity.easing = PARTICLE_EASING_EASE_IN;

    config->scale.start = 1.5f;
    config->scale.end = 0.2f;
    config->scale.min = 1.0f;
    config->scale.max = 1.5f;

    config->rotation.align_velocity = 1;
    config->rotation.speed_min = 0.0f;
    config->rotation.speed_max = 0.0f;

    config->boundary.behavior = PARTICLE_BOUNDARY_NONE;

    config->render.blend_mode = PARTICLE_BLEND_ADDITIVE;
    config->render.base_size = 5.0f;

    config->callbacks.on_particle_update = vortex_particle_update;
    config->callbacks.user_data = NULL;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

gui_particle_widget_t *effect_vortex_create(gui_obj_t *parent, const char *name,
                                            int16_t x, int16_t y, int16_t w, int16_t h)
{
    /* Compute center and radii from widget size */
    s_vortex_center_x = (float)(w / 2);
    s_vortex_center_y = (float)(h / 2);

    float min_dim = (float)(w < h ? w : h);
    s_vortex_outer_r = min_dim * VORTEX_OUTER_RATIO;
    s_vortex_inner_r = min_dim * VORTEX_INNER_RATIO;

    s_vortex_widget = gui_particle_widget_create(parent, name,
                                                 x, y, w, h,
                                                 PARTICLE_POOL_SIZE);
    if (s_vortex_widget == NULL)
    {
        return NULL;
    }

    particle_effect_config_t config;
    effect_vortex_config(&config, s_vortex_center_x, s_vortex_center_y,
                         s_vortex_inner_r, s_vortex_outer_r);

    gui_particle_widget_add_effect(s_vortex_widget, &config);

    return s_vortex_widget;
}
