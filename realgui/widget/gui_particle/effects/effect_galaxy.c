/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_galaxy.c
 * @brief Galaxy Spiral Particle Effect
 *
 * This example demonstrates:
 * 1. Spiral arm structure (logarithmic spiral)
 * 2. Particles placed along spiral arms
 * 3. Orbital motion with slow inward drift
 * 4. Color gradient from blue (outer) to white/pink (core)
 * 5. Rotating spiral pattern
 */

#include "effect_galaxy.h"
#include "gui_obj.h"
#include "gui_api_os.h"
#include "gui_api_dc.h"
#include "def_type.h"
#include <math.h>
#include <stdlib.h>

#define PARTICLE_POOL_SIZE   600
#define GALAXY_OUTER_RATIO   0.49f
#define GALAXY_INNER_RATIO   0.44f
#define GALAXY_CORE_RATIO    0.20f
#define GALAXY_PULL_SPEED    12.0f
#define GALAXY_SPIN_SPEED    2.0f
#define GALAXY_NUM_ARMS      3
#define GALAXY_WIDGET_MARGIN 0.10f

static gui_particle_widget_t *s_galaxy_widget = NULL;
static float s_galaxy_center_x = 0.0f;
static float s_galaxy_center_y = 0.0f;
static float s_galaxy_rotation = 0.0f;

static float s_galaxy_outer_r = 0.0f;
static float s_galaxy_inner_r = 0.0f;
static float s_galaxy_core_r = 0.0f;

/**
 * @brief Custom particle init - place on spiral arms
 */
static void galaxy_particle_init(particle_t *p, void *user_data)
{
    GUI_UNUSED(user_data);
    if (p == NULL)
    {
        return;
    }

    int arm = rand() % GALAXY_NUM_ARMS;
    float arm_angle = (float)arm * 2.0f * M_PI_F / GALAXY_NUM_ARMS;

    float dist = s_galaxy_inner_r + (float)(rand() % 100) / 100.0f *
                 (s_galaxy_outer_r - s_galaxy_inner_r);

    float spiral_angle = arm_angle + s_galaxy_rotation +
                         logf(dist / s_galaxy_core_r) * 0.8f;

    float spread = ((float)(rand() % 100) / 100.0f - 0.5f) * 0.3f;
    p->x = s_galaxy_center_x + cosf(spiral_angle + spread) * dist;
    p->y = s_galaxy_center_y + sinf(spiral_angle + spread) * dist;

    float tx = -sinf(spiral_angle);
    float ty = cosf(spiral_angle);
    p->vx = tx * GALAXY_SPIN_SPEED * dist * 0.2f;
    p->vy = ty * GALAXY_SPIN_SPEED * dist * 0.2f;
}

/**
 * @brief Custom particle update callback for spiral motion
 */
static void galaxy_particle_update(particle_t *p, void *user_data)
{
    GUI_UNUSED(user_data);
    if (p == NULL || !p->active)
    {
        return;
    }

    float dx = p->x - s_galaxy_center_x;
    float dy = p->y - s_galaxy_center_y;
    float dist = sqrtf(dx * dx + dy * dy);

    if (dist < s_galaxy_core_r)
    {
        p->life = 0;
        return;
    }

    float nx = dx / dist;
    float ny = dy / dist;
    float tx = -ny;
    float ty = nx;

    float spin_factor = 1.0f + (s_galaxy_outer_r - dist) / s_galaxy_outer_r * 3.0f;
    float spin_speed = GALAXY_SPIN_SPEED * spin_factor;

    float pull_factor = 1.0f + (s_galaxy_outer_r - dist) / s_galaxy_outer_r * 1.5f;
    float pull_speed = GALAXY_PULL_SPEED * pull_factor;

    p->vx = -nx * pull_speed + tx * spin_speed * dist * 0.3f;
    p->vy = -ny * pull_speed + ty * spin_speed * dist * 0.3f;

    float dist_ratio = dist / s_galaxy_outer_r;
    if (dist_ratio > 1.0f) { dist_ratio = 1.0f; }
    if (dist_ratio < 0.0f) { dist_ratio = 0.0f; }

    uint8_t r = (uint8_t)(0x44 + (0xFF - 0x44) * (1.0f - dist_ratio));
    uint8_t g = (uint8_t)(0x88 + (0xAA - 0x88) * (1.0f - dist_ratio));
    uint8_t b = 0xFF;
    p->color = 0xFF000000 | (r << 16) | (g << 8) | b;

    float scale_factor = 0.5f + (1.0f - dist_ratio) * 0.8f;
    p->scale = scale_factor;

    p->rotation = atan2f(p->vy, p->vx);
}

/*============================================================================*
 *                           Static Configuration
 *============================================================================*/

static void effect_galaxy_config(particle_effect_config_t *config)
{
    if (config == NULL)
    {
        return;
    }

    particle_effect_config_init(config);

    config->shape.type = PARTICLE_SHAPE_RING;
    config->shape.ring.cx = s_galaxy_center_x;
    config->shape.ring.cy = s_galaxy_center_y;
    config->shape.ring.inner_r = s_galaxy_inner_r;
    config->shape.ring.outer_r = s_galaxy_outer_r;

    config->trajectory.type = PARTICLE_TRAJECTORY_LINEAR;
    config->trajectory.damping = 0.0f;

    config->emission.angle_min = 0.0f;
    config->emission.angle_max = 2.0f * M_PI_F;
    config->emission.speed_min = 5.0f;
    config->emission.speed_max = 15.0f;
    config->emission.rate = 30.0f;
    config->emission.burst_enabled = 0;

    config->lifecycle.life_min = 8000;
    config->lifecycle.life_max = 12000;
    config->lifecycle.auto_cleanup = 0;
    config->lifecycle.loop = 1;

    config->color.mode = PARTICLE_COLOR_SOLID;
    config->color.color_start = 0xFF4488FF;

    config->opacity.start = 255;
    config->opacity.end = 180;
    config->opacity.easing = PARTICLE_EASING_LINEAR;

    config->scale.start = 1.0f;
    config->scale.end = 0.5f;
    config->scale.min = 0.8f;
    config->scale.max = 1.5f;

    config->rotation.align_velocity = 1;

    config->boundary.behavior = PARTICLE_BOUNDARY_NONE;

    config->render.blend_mode = PARTICLE_BLEND_ADDITIVE;
    config->render.base_size = 4.0f;

    config->callbacks.on_particle_init = galaxy_particle_init;
    config->callbacks.on_particle_update = galaxy_particle_update;
    config->callbacks.user_data = NULL;
}

/**
 * @brief Widget update callback - rotate spiral arms
 */
static void galaxy_widget_update(void *user_data)
{
    GUI_UNUSED(user_data);
    s_galaxy_rotation += 0.01f;
    if (s_galaxy_rotation > 2.0f * M_PI_F)
    {
        s_galaxy_rotation -= 2.0f * M_PI_F;
    }
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

gui_particle_widget_t *effect_galaxy_create(gui_obj_t *parent, const char *name,
                                            int16_t x, int16_t y, int16_t w, int16_t h)
{
    float min_dim = (float)(w < h ? w : h);
    s_galaxy_outer_r = min_dim * GALAXY_OUTER_RATIO;
    s_galaxy_inner_r = min_dim * GALAXY_INNER_RATIO;
    s_galaxy_core_r  = min_dim * GALAXY_CORE_RATIO;

    s_galaxy_center_x = (float)(w / 2);
    s_galaxy_center_y = (float)(h / 2);

    s_galaxy_widget = gui_particle_widget_create(parent, name,
                                                 x, y, w, h,
                                                 PARTICLE_POOL_SIZE);
    if (s_galaxy_widget == NULL)
    {
        return NULL;
    }

    particle_effect_config_t config;
    effect_galaxy_config(&config);

    config.shape.ring.cx = s_galaxy_center_x;
    config.shape.ring.cy = s_galaxy_center_y;

    gui_particle_widget_add_effect(s_galaxy_widget, &config);
    gui_particle_widget_set_update_cb(s_galaxy_widget, galaxy_widget_update, NULL);

    return s_galaxy_widget;
}
