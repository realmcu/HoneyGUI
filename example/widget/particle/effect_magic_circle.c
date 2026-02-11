/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_magic_circle.c
 * @brief Magic Circle Particle Effect - Doctor Strange Style
 *
 * This example demonstrates:
 * 1. Ring emission along circle edges with upward floating particles
 * 2. Orange-gold sparks with gravity-defying upward drift
 * 3. Additive blending for glowing magical effect
 */

#include "effect_magic_circle.h"
#include "gui_obj.h"
#include "gui_api_os.h"
#include "gui_api_dc.h"
#include "def_type.h"
#include <math.h>
#include <stdlib.h>

#define PARTICLE_POOL_SIZE    400
#define CIRCLE_RADIUS         160.0f
#define INNER_RADIUS          80.0f

static gui_particle_widget_t *s_mc_widget = NULL;
static float s_center_x = 0.0f;
static float s_center_y = 0.0f;

static void mc_particle_init(particle_t *p, void *user_data)
{
    GUI_UNUSED(user_data);
    if (p == NULL)
    {
        return;
    }

    int zone = rand() % 100;
    float angle = (float)(rand() % 3600) / 3600.0f * 2.0f * M_PI_F;

    if (zone < 70)
    {
        float r = CIRCLE_RADIUS + ((float)(rand() % 40) - 20.0f);
        p->x = s_center_x + cosf(angle) * r;
        p->y = s_center_y + sinf(angle) * r;
    }
    else
    {
        float r = (float)(rand() % (int)INNER_RADIUS);
        p->x = s_center_x + cosf(angle) * r;
        p->y = s_center_y + sinf(angle) * r;
    }

    float spread = ((float)(rand() % 100) / 100.0f - 0.5f) * 30.0f;
    float up_speed = 20.0f + (float)(rand() % 40);
    p->vx = spread;
    p->vy = -up_speed;

    uint8_t palette_idx = rand() % 4;
    switch (palette_idx)
    {
    case 0:
        p->color = 0xFFFF8800;
        break;
    case 1:
        p->color = 0xFFFFAA22;
        break;
    case 2:
        p->color = 0xFFFFCC44;
        break;
    default:
        p->color = 0xFFFF6600;
        break;
    }

    p->scale = 0.5f + (float)(rand() % 100) / 100.0f * 1.0f;
}

static void mc_particle_update(particle_t *p, void *user_data)
{
    GUI_UNUSED(user_data);
    if (p == NULL || !p->active || p->max_life == 0)
    {
        return;
    }

    p->vy *= 0.995f;

    p->vx += ((float)(rand() % 100) / 100.0f - 0.5f) * 2.0f;
    p->vx *= 0.98f;

    float life_ratio = (float)p->life / (float)p->max_life;
    p->opacity = (uint8_t)(255.0f * life_ratio * life_ratio);

    p->scale *= 0.998f;
}

/*============================================================================*
 *                           Static Configuration
 *============================================================================*/

static void effect_magic_circle_config(particle_effect_config_t *config)
{
    if (config == NULL)
    {
        return;
    }

    particle_effect_config_init(config);

    config->shape.type = PARTICLE_SHAPE_RING;
    config->shape.ring.cx = s_center_x;
    config->shape.ring.cy = s_center_y;
    config->shape.ring.inner_r = INNER_RADIUS;
    config->shape.ring.outer_r = CIRCLE_RADIUS;

    config->trajectory.type = PARTICLE_TRAJECTORY_LINEAR;
    config->trajectory.damping = 0.01f;

    config->emission.angle_min = 0.0f;
    config->emission.angle_max = 2.0f * M_PI_F;
    config->emission.speed_min = 20.0f;
    config->emission.speed_max = 60.0f;
    config->emission.rate = 35.0f;
    config->emission.burst_enabled = 0;

    config->lifecycle.life_min = 1500;
    config->lifecycle.life_max = 3000;
    config->lifecycle.auto_cleanup = 0;
    config->lifecycle.loop = 1;

    config->color.mode = PARTICLE_COLOR_SOLID;
    config->color.color_start = 0xFFFF8800;

    config->opacity.start = 255;
    config->opacity.end = 0;
    config->opacity.easing = PARTICLE_EASING_EASE_OUT;

    config->scale.start = 1.0f;
    config->scale.end = 0.3f;
    config->scale.min = 0.5f;
    config->scale.max = 1.5f;

    config->boundary.behavior = PARTICLE_BOUNDARY_NONE;

    config->render.blend_mode = PARTICLE_BLEND_ADDITIVE;
    config->render.base_size = 4.0f;

    config->callbacks.on_particle_init = mc_particle_init;
    config->callbacks.on_particle_update = mc_particle_update;
    config->callbacks.user_data = NULL;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

gui_particle_widget_t *effect_magic_circle_create(gui_obj_t *parent, const char *name,
                                                  int16_t x, int16_t y, int16_t w, int16_t h)
{
    s_center_x = (float)(w / 2);
    s_center_y = (float)(h / 2);

    s_mc_widget = gui_particle_widget_create(parent, name,
                                             x, y, w, h,
                                             PARTICLE_POOL_SIZE);
    if (s_mc_widget == NULL)
    {
        return NULL;
    }

    particle_effect_config_t config;
    effect_magic_circle_config(&config);
    config.shape.ring.cx = s_center_x;
    config.shape.ring.cy = s_center_y;
    gui_particle_widget_add_effect(s_mc_widget, &config);

    return s_mc_widget;
}
