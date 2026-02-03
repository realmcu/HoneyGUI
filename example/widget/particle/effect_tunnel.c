/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_tunnel.c
 * @brief Tunnel Particle Effect - Warp Speed / Hyperspace Effect
 *
 * This example demonstrates:
 * 1. Ring emission shape (particles emit from a small ring at center)
 * 2. Linear trajectory with no gravity (straight line movement)
 * 3. Fast particle speed for "warp" effect
 * 4. Binary format loading using load_mem() from embedded data array
 */

#include "effect_tunnel.h"
#include "gui_obj.h"
#include "gui_api_os.h"
#include "gui_api_dc.h"
#include <string.h>
#include "def_type.h"

#define PARTICLE_POOL_SIZE 512

static gui_particle_widget_t *s_tunnel_widget = NULL;

const void *effect_tunnel_get_binary_config(size_t *size)
{
    if (size)
    {
        *size = TUNNEL_EFFECT_BINARY_SIZE;
    }
    return tunnel_effect_binary_data;
}

void effect_tunnel_config(particle_effect_config_t *config)
{
    if (config == NULL)
    {
        return;
    }

    gui_dispdev_t *dc = gui_get_dc();
    int screen_w = dc->screen_width;
    int screen_h = dc->screen_height;

    particle_effect_config_init(config);

    /* Ring emission from center - very small for concentrated start point */
    config->shape.type = PARTICLE_SHAPE_RING;
    config->shape.ring.cx = (float)(screen_w / 2);
    config->shape.ring.cy = (float)(screen_h / 2);
    config->shape.ring.inner_r = 3.0f;   /* Tiny center */
    config->shape.ring.outer_r = 10.0f;  /* Small variation */

    /* Linear trajectory - no gravity, straight lines outward */
    config->trajectory.type = PARTICLE_TRAJECTORY_LINEAR;
    config->trajectory.gravity = 0.0f;
    config->trajectory.damping = 0.0f;  /* No slowdown - constant speed */

    /* Full 360 degree emission, VERY fast speed for warp effect */
    config->emission.angle_min = 0.0f;
    config->emission.angle_max = 2.0f * M_PI_F;
    config->emission.speed_min = 300.0f;  /* Fast */
    config->emission.speed_max = 500.0f;  /* Even faster */
    config->emission.rate = 40.0f;        /* Moderate rate */
    config->emission.burst_enabled = 0;   /* Continuous emission */

    /* Short lifetime - particles die quickly for sharp streaks */
    config->lifecycle.life_min = 500;
    config->lifecycle.life_max = 800;
    config->lifecycle.auto_cleanup = 0;
    config->lifecycle.loop = 1;  /* Continuous effect */

    /* White color for bright streaks */
    config->color.mode = PARTICLE_COLOR_SOLID;
    config->color.color_start = 0xFFFFFFFF;  /* White */

    /* Start bright, fade to transparent */
    config->opacity.start = 255;
    config->opacity.end = 0;
    config->opacity.easing = PARTICLE_EASING_LINEAR;

    /* Scale: start small near center, grow as they move outward */
    config->scale.start = 0.5f;
    config->scale.end = 2.5f;
    config->scale.min = 0.8f;
    config->scale.max = 1.2f;

    /* Align rotation to velocity - particles point outward */
    config->rotation.align_velocity = 1;

    /* Kill at screen boundary */
    config->boundary.behavior = PARTICLE_BOUNDARY_KILL;
    config->boundary.left = 0.0f;
    config->boundary.top = 0.0f;
    config->boundary.right = (float)screen_w;
    config->boundary.bottom = (float)screen_h;

    /* Additive blending for bright glow effect */
    config->render.blend_mode = PARTICLE_BLEND_ADDITIVE;
    config->render.base_size = 3.0f;
}

gui_particle_widget_t *effect_tunnel_demo_init(void)
{
    gui_obj_t *root = gui_obj_get_root();
    gui_dispdev_t *dc = gui_get_dc();
    int screen_w = dc->screen_width;
    int screen_h = dc->screen_height;

    s_tunnel_widget = gui_particle_widget_create(root, "tunnel_demo",
                                                 0, 0, screen_w, screen_h,
                                                 PARTICLE_POOL_SIZE);
    if (s_tunnel_widget == NULL)
    {
        gui_log("Tunnel: Failed to create widget\n");
        return NULL;
    }

    particle_effect_config_t config;

    /* Load from embedded binary data array using load_mem */
    if (particle_effect_config_load_mem(&config, tunnel_effect_binary_data) == 0)
    {
        gui_log("Tunnel: Loaded config from binary array (load_mem)\n");
    }
    else
    {
        /* Fallback to direct config if binary load fails */
        gui_log("Tunnel: Binary load failed, using direct config\n");
        effect_tunnel_config(&config);
    }

    gui_particle_widget_add_effect(s_tunnel_widget, &config);

    gui_log("Tunnel: Demo initialized - warp speed effect\n");
    return s_tunnel_widget;
}
