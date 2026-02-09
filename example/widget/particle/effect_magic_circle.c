/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_magic_circle.c
 * @brief Magic Circle Particle Effect - Doctor Strange Style
 *
 * This example demonstrates:
 * 1. Two rotating images (inner/outer) spinning in opposite directions
 * 2. Ring emission along circle edges with upward floating particles
 * 3. Orange-gold sparks with gravity-defying upward drift
 * 4. Additive blending for glowing magical effect
 */

#include "effect_magic_circle.h"
#include "gui_obj.h"
#include "gui_img.h"
#include "gui_api_os.h"
#include "gui_api_dc.h"
#include "def_type.h"
#include <math.h>
#include <stdlib.h>

#include "root_image/ui_resource.h"

#define PARTICLE_POOL_SIZE    400
#define OUTER_ROTATE_SPEED    1.0f    /* degrees per frame, clockwise */
#define INNER_ROTATE_SPEED    1.5f    /* degrees per frame, counter-clockwise */
#define CIRCLE_RADIUS         160.0f  /* Main circle radius for particle emission */
#define INNER_RADIUS          80.0f   /* Inner region emission radius */

static gui_particle_widget_t *s_mc_widget = NULL;
static gui_img_t *s_img_outer = NULL;
static gui_img_t *s_img_inner = NULL;
static float s_outer_angle = 0.0f;
static float s_inner_angle = 0.0f;
static float s_center_x = 0.0f;
static float s_center_y = 0.0f;
static int s_screen_w = 0;
static int s_screen_h = 0;

/**
 * @brief Custom particle init - emit from ring edge or center area
 */
static void mc_particle_init(particle_t *p, void *user_data)
{
    GUI_UNUSED(user_data);
    if (p == NULL)
    {
        return;
    }

    /* 70% from outer ring edge, 30% from inner center area */
    int zone = rand() % 100;
    float angle = (float)(rand() % 3600) / 3600.0f * 2.0f * M_PI_F;

    if (zone < 70)
    {
        /* Outer ring edge */
        float r = CIRCLE_RADIUS + ((float)(rand() % 40) - 20.0f);
        p->x = s_center_x + cosf(angle) * r;
        p->y = s_center_y + sinf(angle) * r;
    }
    else
    {
        /* Inner center area */
        float r = (float)(rand() % (int)INNER_RADIUS);
        p->x = s_center_x + cosf(angle) * r;
        p->y = s_center_y + sinf(angle) * r;
    }

    /* Upward velocity with slight horizontal spread */
    float spread = ((float)(rand() % 100) / 100.0f - 0.5f) * 30.0f;
    float up_speed = 20.0f + (float)(rand() % 40);
    p->vx = spread;
    p->vy = -up_speed;  /* Negative Y = upward */

    /* Orange-gold color palette */
    uint8_t palette_idx = rand() % 4;
    switch (palette_idx)
    {
    case 0:
        p->color = 0xFFFF8800; /* Orange */
        break;
    case 1:
        p->color = 0xFFFFAA22; /* Gold */
        break;
    case 2:
        p->color = 0xFFFFCC44; /* Light gold */
        break;
    default:
        p->color = 0xFFFF6600; /* Deep orange */
        break;
    }

    p->scale = 0.5f + (float)(rand() % 100) / 100.0f * 1.0f;
}

/**
 * @brief Custom particle update - upward float with fade
 */
static void mc_particle_update(particle_t *p, void *user_data)
{
    GUI_UNUSED(user_data);
    if (p == NULL || !p->active || p->max_life == 0)
    {
        return;
    }

    /* Gentle deceleration on upward motion */
    p->vy *= 0.995f;

    /* Slight horizontal wobble */
    p->vx += ((float)(rand() % 100) / 100.0f - 0.5f) * 2.0f;
    p->vx *= 0.98f;

    /* Fade based on remaining life */
    float life_ratio = (float)p->life / (float)p->max_life;
    p->opacity = (uint8_t)(255.0f * life_ratio * life_ratio);

    /* Shrink as fading */
    p->scale *= 0.998f;
}

/**
 * @brief Widget update callback - rotate images each frame
 */
static void mc_widget_update(void *user_data)
{
    GUI_UNUSED(user_data);

    /* Rotate outer image clockwise */
    s_outer_angle += OUTER_ROTATE_SPEED;
    if (s_outer_angle >= 360.0f)
    {
        s_outer_angle -= 360.0f;
    }

    /* Rotate inner image counter-clockwise */
    s_inner_angle -= INNER_ROTATE_SPEED;
    if (s_inner_angle <= -360.0f)
    {
        s_inner_angle += 360.0f;
    }

    if (s_img_outer != NULL)
    {
        gui_img_rotation(s_img_outer, s_outer_angle);
    }
    if (s_img_inner != NULL)
    {
        gui_img_rotation(s_img_inner, s_inner_angle);
    }
}

void effect_magic_circle_config(particle_effect_config_t *config)
{
    if (config == NULL)
    {
        return;
    }

    particle_effect_config_init(config);

    /* Ring emission - overridden by init callback */
    config->shape.type = PARTICLE_SHAPE_RING;
    config->shape.ring.cx = s_center_x;
    config->shape.ring.cy = s_center_y;
    config->shape.ring.inner_r = INNER_RADIUS;
    config->shape.ring.outer_r = CIRCLE_RADIUS;

    /* Linear trajectory - motion handled by callback */
    config->trajectory.type = PARTICLE_TRAJECTORY_LINEAR;
    config->trajectory.damping = 0.01f;

    /* Continuous emission */
    config->emission.angle_min = 0.0f;
    config->emission.angle_max = 2.0f * M_PI_F;
    config->emission.speed_min = 20.0f;
    config->emission.speed_max = 60.0f;
    config->emission.rate = 35.0f;
    config->emission.burst_enabled = 0;

    /* Particle lifetime */
    config->lifecycle.life_min = 1500;
    config->lifecycle.life_max = 3000;
    config->lifecycle.auto_cleanup = 0;
    config->lifecycle.loop = 1;

    /* Orange-gold color (overridden by init callback) */
    config->color.mode = PARTICLE_COLOR_SOLID;
    config->color.color_start = 0xFFFF8800;

    /* Opacity handled by update callback */
    config->opacity.start = 255;
    config->opacity.end = 0;
    config->opacity.easing = PARTICLE_EASING_EASE_OUT;

    /* Scale */
    config->scale.start = 1.0f;
    config->scale.end = 0.3f;
    config->scale.min = 0.5f;
    config->scale.max = 1.5f;

    /* No boundary check - particles fade out naturally */
    config->boundary.behavior = PARTICLE_BOUNDARY_NONE;

    /* Additive blending for glow */
    config->render.blend_mode = PARTICLE_BLEND_ADDITIVE;
    config->render.base_size = 4.0f;

    /* Register custom callbacks */
    config->callbacks.on_particle_init = mc_particle_init;
    config->callbacks.on_particle_update = mc_particle_update;
    config->callbacks.user_data = NULL;
}

gui_particle_widget_t *effect_magic_circle_demo_init(void)
{
    gui_obj_t *root = gui_obj_get_root();
    gui_dispdev_t *dc = gui_get_dc();
    s_screen_w = dc->screen_width;
    s_screen_h = dc->screen_height;
    s_center_x = (float)(s_screen_w / 2);
    s_center_y = (float)(s_screen_h / 2);

    /* Create particle widget (full screen) */
    s_mc_widget = gui_particle_widget_create(root, "magic_circle_demo",
                                             0, 0, s_screen_w, s_screen_h,
                                             PARTICLE_POOL_SIZE);
    if (s_mc_widget == NULL)
    {
        gui_log("MagicCircle: Failed to create widget\n");
        return NULL;
    }

    /* Configure and add particle effect */
    particle_effect_config_t config;
    effect_magic_circle_config(&config);
    config.shape.ring.cx = s_center_x;
    config.shape.ring.cy = s_center_y;
    gui_particle_widget_add_effect(s_mc_widget, &config);

    /*
     * Image matrix pipeline (from gui_img_prepare):
     *   Parent * T(obj.x, obj.y) * T(t_x, t_y) * R(degrees) * T(-f_x, -f_y) * P
     *
     * To rotate around image center and place at screen center:
     *   - pos = (0, 0): no extra offset from obj position
     *   - focus = (w/2, h/2): rotation pivot at image center
     *   - translate = (screen_cx, screen_cy): move pivot to screen center
     */

    /* Create outer circle image */
    s_img_outer = gui_img_create_from_mem(GUI_BASE(s_mc_widget), "mc_outer",
                                          MAGIC_OUTER_BIN, 0, 0, 0, 0);
    if (s_img_outer != NULL)
    {
        uint16_t ow = gui_img_get_width(s_img_outer);
        uint16_t oh = gui_img_get_height(s_img_outer);
        gui_img_set_focus(s_img_outer, (float)ow / 2.0f, (float)oh / 2.0f);
        gui_img_translate(s_img_outer, s_center_x, s_center_y);
        gui_img_set_mode(s_img_outer, IMG_SRC_OVER_MODE);
        gui_img_set_quality(s_img_outer, true);
        gui_log("MagicCircle: outer img %dx%d\n", ow, oh);
    }

    /* Create inner circle image */
    s_img_inner = gui_img_create_from_mem(GUI_BASE(s_mc_widget), "mc_inner",
                                          MAGIC_INNER_BIN, 0, 0, 0, 0);
    if (s_img_inner != NULL)
    {
        uint16_t iw = gui_img_get_width(s_img_inner);
        uint16_t ih = gui_img_get_height(s_img_inner);
        gui_img_set_focus(s_img_inner, (float)iw / 2.0f, (float)ih / 2.0f);
        gui_img_translate(s_img_inner, s_center_x, s_center_y);
        gui_img_set_mode(s_img_inner, IMG_SRC_OVER_MODE);
        gui_img_set_quality(s_img_inner, true);
        gui_log("MagicCircle: inner img %dx%d\n", iw, ih);
    }

    /* Register frame update callback for image rotation */
    gui_particle_widget_set_update_cb(s_mc_widget, mc_widget_update, NULL);

    s_outer_angle = 0.0f;
    s_inner_angle = 0.0f;

    gui_log("MagicCircle: Demo initialized - rotating magic circle\n");
    return s_mc_widget;
}
