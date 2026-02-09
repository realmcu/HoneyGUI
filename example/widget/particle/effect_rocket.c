/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_rocket.c
 * @brief Rocket Thruster Exhaust Particle Effect
 *
 * Creates a realistic rocket exhaust/thruster effect with:
 * - Main flame: bright yellow-orange particles shooting downward
 * - Core glow: intense white-yellow center
 * - Smoke trail: darker particles that linger longer
 */

#include "effect_rocket.h"
#include "gui_obj.h"
#include "gui_api_os.h"
#include "gui_api_dc.h"
#include "gui_img.h"
#include "def_type.h"
#include "root_image/ui_resource.h"

#define PARTICLE_POOL_SIZE 512

/* Rocket image: nozzle offset within the image (fixed, image-relative) */
#define ROCKET_NOZZLE_OFFSET_X   75
#define ROCKET_NOZZLE_OFFSET_Y   215

/* Exhaust direction: lower-left, opposite to rocket flight direction */
/* In this coordinate system: 0°=right, 90°=down, 180°=left, 270°=up */
/* Rocket flies upper-right, so exhaust goes lower-left (around 135°) */
#define EXHAUST_DIRECTION  (135.0f * M_PI_F / 180.0f)
/* Cone spread angle: +/- 25 degrees for wider flame */
#define EXHAUST_SPREAD     (M_PI_F / 7.2f)

static gui_particle_widget_t *s_rocket_widget = NULL;
static particle_effect_handle_t s_flame_handle = PARTICLE_INVALID_HANDLE;
static particle_effect_handle_t s_smoke_handle = PARTICLE_INVALID_HANDLE;

/* Computed at runtime from screen size */
static int s_widget_x = 0;
static int s_widget_y = 0;
static int s_widget_w = 0;
static int s_widget_h = 0;
static float s_nozzle_local_x = 0.0f;  /* Nozzle position in widget-local coords */
static float s_nozzle_local_y = 0.0f;

void effect_rocket_config(particle_effect_config_t *config)
{
    if (config == NULL)
    {
        return;
    }

    particle_effect_config_init(config);

    /* Point emission from rocket nozzle (local coordinates relative to widget) */
    config->shape.type = PARTICLE_SHAPE_POINT;
    config->shape.point.x = s_nozzle_local_x;
    config->shape.point.y = s_nozzle_local_y;

    /* Linear trajectory with slight gravity pull */
    config->trajectory.type = PARTICLE_TRAJECTORY_LINEAR;
    config->trajectory.gravity = 50.0f;  /* Light downward pull */
    config->trajectory.damping = 0.02f;  /* Minimal air resistance */

    /* Conical emission: mostly downward with spread */
    config->emission.angle_min = EXHAUST_DIRECTION - EXHAUST_SPREAD;
    config->emission.angle_max = EXHAUST_DIRECTION + EXHAUST_SPREAD;
    config->emission.speed_min = 180.0f;
    config->emission.speed_max = 320.0f;
    config->emission.rate = 120.0f;  /* Higher emission rate for denser flame */
    config->emission.burst_enabled = 0;

    /* Short lifetime for sharp flame effect */
    config->lifecycle.life_min = 300;
    config->lifecycle.life_max = 500;
    config->lifecycle.auto_cleanup = 0;
    config->lifecycle.loop = 1;

    /* Gradient: bright yellow -> orange -> red */
    config->color.mode = PARTICLE_COLOR_GRADIENT;
    config->color.color_start = 0xFFFFFF00;  /* Bright yellow */
    config->color.color_end = 0xFFFF4400;    /* Orange-red */

    /* Fade out quickly */
    config->opacity.start = 255;
    config->opacity.end = 0;
    config->opacity.easing = PARTICLE_EASING_EASE_OUT;

    /* Scale: start larger, shrink as they cool */
    config->scale.start = 1.5f;
    config->scale.end = 0.4f;
    config->scale.min = 1.0f;
    config->scale.max = 1.5f;

    /* Align particles to velocity direction */
    config->rotation.align_velocity = 1;

    /* Kill at widget boundary (local coordinates) */
    config->boundary.behavior = PARTICLE_BOUNDARY_KILL;
    config->boundary.left = 0.0f;
    config->boundary.top = 0.0f;
    config->boundary.right = (float)s_widget_w;
    config->boundary.bottom = (float)s_widget_h;

    /* Additive blending for bright glow */
    config->render.blend_mode = PARTICLE_BLEND_ADDITIVE;
    config->render.base_size = 10.0f;
}

/**
 * @brief Configure smoke trail effect (secondary layer)
 */
static void effect_rocket_smoke_config(particle_effect_config_t *config)
{
    if (config == NULL)
    {
        return;
    }

    particle_effect_config_init(config);

    /* Point emission slightly behind the flame (local coordinates) */
    config->shape.type = PARTICLE_SHAPE_POINT;
    config->shape.point.x = s_nozzle_local_x;
    config->shape.point.y = s_nozzle_local_y + 15.0f;

    /* Linear with more spread and slower speed */
    config->trajectory.type = PARTICLE_TRAJECTORY_LINEAR;
    config->trajectory.gravity = -30.0f;  /* Smoke drifts upward (opposite to exhaust) */
    config->trajectory.damping = 0.05f;

    /* Wider spread for smoke dispersion */
    config->emission.angle_min = EXHAUST_DIRECTION - (EXHAUST_SPREAD * 1.5f);
    config->emission.angle_max = EXHAUST_DIRECTION + (EXHAUST_SPREAD * 1.5f);
    config->emission.speed_min = 40.0f;
    config->emission.speed_max = 80.0f;
    config->emission.rate = 35.0f;
    config->emission.burst_enabled = 0;

    /* Longer lifetime for lingering smoke */
    config->lifecycle.life_min = 600;
    config->lifecycle.life_max = 1000;
    config->lifecycle.auto_cleanup = 0;
    config->lifecycle.loop = 1;

    /* Gray smoke color */
    config->color.mode = PARTICLE_COLOR_GRADIENT;
    config->color.color_start = 0xFF888888;  /* Light gray */
    config->color.color_end = 0xFF444444;    /* Dark gray */

    /* Slow fade */
    config->opacity.start = 150;
    config->opacity.end = 0;
    config->opacity.easing = PARTICLE_EASING_LINEAR;

    /* Smoke expands as it rises */
    config->scale.start = 0.8f;
    config->scale.end = 2.0f;
    config->scale.min = 0.6f;
    config->scale.max = 1.0f;

    /* Kill at widget boundary (local coordinates) */
    config->boundary.behavior = PARTICLE_BOUNDARY_KILL;
    config->boundary.left = 0.0f;
    config->boundary.top = 0.0f;
    config->boundary.right = (float)s_widget_w;
    config->boundary.bottom = (float)s_widget_h;

    /* Normal blending for smoke */
    config->render.blend_mode = PARTICLE_BLEND_NORMAL;
    config->render.base_size = 10.0f;
}

gui_particle_widget_t *effect_rocket_demo_init(void)
{
    gui_obj_t *root = gui_obj_get_root();
    gui_dispdev_t *dc = gui_get_dc();
    int screen_w = dc->screen_width;
    int screen_h = dc->screen_height;

    /* Get rocket image dimensions */
    gui_img_t *tmp_img = gui_img_create_from_mem(root, "tmp", ROCKET_BIN, 0, 0, 0, 0);
    int img_w = gui_img_get_width(tmp_img);
    int img_h = gui_img_get_height(tmp_img);
    gui_obj_tree_free(GUI_BASE(tmp_img));

    /* Center rocket image on screen */
    int img_x = (screen_w - img_w) / 2;
    int img_y = (screen_h - img_h) / 2;

    /* Nozzle position in screen coordinates */
    int nozzle_x = img_x + ROCKET_NOZZLE_OFFSET_X;
    int nozzle_y = img_y + ROCKET_NOZZLE_OFFSET_Y;

    /* Widget covers the exhaust area below-left of nozzle */
    s_widget_x = nozzle_x - 80;
    s_widget_y = nozzle_y;
    s_widget_w = screen_w - s_widget_x;
    s_widget_h = screen_h - s_widget_y;
    if (s_widget_x < 0) { s_widget_x = 0; }
    if (s_widget_w > screen_w) { s_widget_w = screen_w; }

    /* Nozzle in widget-local coordinates */
    s_nozzle_local_x = (float)(nozzle_x - s_widget_x);
    s_nozzle_local_y = 0.0f;

    s_rocket_widget = gui_particle_widget_create(root, "rocket_demo",
                                                 s_widget_x, s_widget_y,
                                                 s_widget_w, s_widget_h,
                                                 PARTICLE_POOL_SIZE);
    if (s_rocket_widget == NULL)
    {
        gui_log("Rocket: Failed to create widget\n");
        return NULL;
    }

    /* Add smoke layer first (renders behind flame) */
    particle_effect_config_t smoke_config;
    effect_rocket_smoke_config(&smoke_config);
    s_smoke_handle = gui_particle_widget_add_effect(s_rocket_widget, &smoke_config);

    /* Add main flame effect */
    particle_effect_config_t flame_config;
    effect_rocket_config(&flame_config);
    s_flame_handle = gui_particle_widget_add_effect(s_rocket_widget, &flame_config);

    /* Add rocket image as child (position relative to widget) */
    gui_img_t *img = gui_img_create_from_mem(GUI_BASE(s_rocket_widget), "img", ROCKET_BIN,
                                             img_x - s_widget_x,
                                             img_y - s_widget_y, 0, 0);
    gui_img_set_mode(img, IMG_FILTER_BLACK);

    gui_log("Rocket: Demo initialized\n");
    return s_rocket_widget;
}
