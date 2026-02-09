/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_rain.c
 * @brief Rain Particle Effect - Falling Raindrops with Splash
 *
 * This example demonstrates:
 * 1. PARTICLE_SHAPE_LINE - top edge emission
 * 2. PARTICLE_BOUNDARY_KILL - kill at bottom
 * 3. on_particle_death callback - spawn splash particles
 * 4. Two-layer particle system (raindrops + splashes)
 */

#include "effect_rain.h"
#include "gui_obj.h"
#include "gui_api_os.h"
#include "gui_api_dc.h"
#include "def_type.h"
#include <math.h>

#define PARTICLE_POOL_SIZE 500
#define RAIN_SPEED_MIN 300.0f
#define RAIN_SPEED_MAX 450.0f
#define RAIN_ANGLE_DEG 15.0f
#define RAIN_ANGLE_RAD ((90.0f + RAIN_ANGLE_DEG) * M_PI_F / 180.0f)
#define SPLASH_PARTICLES 5
#define SPLASH_SPEED 80.0f

static gui_particle_widget_t *s_rain_widget = NULL;
static int s_screen_w = 0;
static int s_screen_h = 0;
static float s_ground_y = 0.0f;

static uint32_t rain_rand(void)
{
    static uint32_t seed = 12321;
    seed = seed * 1103515245 + 12345;
    return (seed >> 16) & 0x7FFF;
}

static float rain_rand_float(float min, float max)
{
    return min + (float)(rain_rand() % 1000) / 1000.0f * (max - min);
}

/**
 * @brief Emit splash particles at impact point
 */
static void emit_splash(float x, float y)
{
    if (s_rain_widget == NULL || s_rain_widget->ps == NULL ||
        s_rain_widget->ps->pool == NULL)
    {
        return;
    }

    particle_pool_t *pool = s_rain_widget->ps->pool;

    for (int i = 0; i < SPLASH_PARTICLES; i++)
    {
        particle_t *p = particle_pool_alloc(pool);
        if (p == NULL)
        {
            break;
        }

        /* Position at impact */
        p->x = x + rain_rand_float(-3.0f, 3.0f);
        p->y = y;

        /* Upward arc velocity */
        float angle = M_PI_F * (0.3f + rain_rand_float(0.0f, 0.4f));
        float speed = SPLASH_SPEED * rain_rand_float(0.5f, 1.0f);
        p->vx = cosf(angle) * speed * (rain_rand() % 2 ? 1.0f : -1.0f);
        p->vy = -sinf(angle) * speed;

        /* Gravity for arc */
        p->ax = 0;
        p->ay = 400.0f;

        /* Light blue splash */
        p->color = 0xFFAADDFF;
        p->opacity = (uint8_t)(150 + rain_rand() % 80);
        p->scale = rain_rand_float(0.3f, 0.6f);

        /* Short life - mark as splash with specific max_life */
        p->life = 300 + rain_rand() % 200;
        p->max_life = 999;  /* Special marker for splash */
        p->active = 1;
        p->rotation = 0;
        p->rotation_speed = 0;
    }
}

/**
 * @brief Particle death callback - spawn splash for raindrops
 */
static void rain_particle_death(particle_t *p, void *user_data)
{
    GUI_UNUSED(user_data);
    if (p == NULL)
    {
        return;
    }

    /* Only raindrops (max_life > 1000) spawn splash, not splash particles */
    if (p->max_life > 1000 && p->y >= s_ground_y - 10.0f)
    {
        emit_splash(p->x, s_ground_y);
    }
}

/**
 * @brief Particle init - set raindrop properties
 */
static void rain_particle_init(particle_t *p, void *user_data)
{
    GUI_UNUSED(user_data);
    if (p == NULL)
    {
        return;
    }

    /* Diagonal downward velocity */
    float speed = rain_rand_float(RAIN_SPEED_MIN, RAIN_SPEED_MAX);
    p->vx = cosf(RAIN_ANGLE_RAD) * speed;
    p->vy = sinf(RAIN_ANGLE_RAD) * speed;

    /* Slight blue tint variation */
    uint8_t b = 255;
    uint8_t g = (uint8_t)(200 + rain_rand() % 55);
    uint8_t r = (uint8_t)(150 + rain_rand() % 50);
    p->color = (0xFFU << 24) | (r << 16) | (g << 8) | b;

    /* Elongated raindrop */
    p->scale = rain_rand_float(0.8f, 1.2f);

    /* Align rotation to fall direction */
    p->rotation = RAIN_ANGLE_RAD - M_PI_F / 2.0f;
}

/**
 * @brief Particle update - check ground collision
 */
static void rain_particle_update(particle_t *p, void *user_data)
{
    GUI_UNUSED(user_data);
    if (p == NULL || !p->active)
    {
        return;
    }

    /* Splash particles have gravity applied */
    if (p->max_life == 999)
    {
        p->vy += p->ay * 0.016f;  /* ~60fps */

        /* Fade out */
        float life_ratio = (float)p->life / 500.0f;
        if (life_ratio > 1.0f)
        {
            life_ratio = 1.0f;
        }
        p->opacity = (uint8_t)(200 * life_ratio);

        /* Kill if below ground */
        if (p->y > s_ground_y + 20.0f)
        {
            p->life = 0;
        }
        return;
    }

    /* Raindrop - check ground collision */
    if (p->y >= s_ground_y)
    {
        p->life = 0;  /* Will trigger death callback */
    }

    /* Slight opacity variation for depth */
    float depth = p->x / (float)s_screen_w;
    p->opacity = (uint8_t)(180 + depth * 75);
}

void effect_rain_config(particle_effect_config_t *config)
{
    if (config == NULL)
    {
        return;
    }

    particle_effect_config_init(config);

    /* Line emission from top edge (caller must set x1/x2 with drift margin) */
    config->shape.type = PARTICLE_SHAPE_LINE;
    config->shape.line.x1 = 0.0f;
    config->shape.line.y1 = -20.0f;
    config->shape.line.x2 = 0.0f;
    config->shape.line.y2 = -20.0f;

    /* Gravity trajectory */
    config->trajectory.type = PARTICLE_TRAJECTORY_GRAVITY;
    config->trajectory.gravity = 100.0f;
    config->trajectory.wind_x = 30.0f;
    config->trajectory.wind_y = 0.0f;
    config->trajectory.damping = 0.0f;

    /* Emission settings */
    config->emission.angle_min = RAIN_ANGLE_RAD - 0.1f;
    config->emission.angle_max = RAIN_ANGLE_RAD + 0.1f;
    config->emission.speed_min = RAIN_SPEED_MIN;
    config->emission.speed_max = RAIN_SPEED_MAX;
    config->emission.rate = 50.0f;
    config->emission.burst_enabled = 0;

    /* Lifecycle */
    config->lifecycle.life_min = 2000;
    config->lifecycle.life_max = 3000;
    config->lifecycle.auto_cleanup = 0;
    config->lifecycle.loop = 1;

    /* Blue-white color */
    config->color.mode = PARTICLE_COLOR_SOLID;
    config->color.color_start = 0xFFCCEEFF;

    /* Opacity */
    config->opacity.start = 220;
    config->opacity.end = 180;
    config->opacity.easing = PARTICLE_EASING_LINEAR;

    /* Elongated scale for raindrop shape */
    config->scale.start = 1.0f;
    config->scale.end = 1.0f;
    config->scale.min = 0.8f;
    config->scale.max = 1.2f;

    /* Rotation aligned to velocity */
    config->rotation.align_velocity = 1;

    /* Kill at boundary (caller must set with drift margin) */
    config->boundary.behavior = PARTICLE_BOUNDARY_KILL;
    config->boundary.left = 0.0f;
    config->boundary.top = -20.0f;
    config->boundary.right = 0.0f;
    config->boundary.bottom = 0.0f;

    /* Normal blending */
    config->render.blend_mode = PARTICLE_BLEND_ADDITIVE;
    config->render.base_size = 3.0f;

    /* Register callbacks */
    config->callbacks.on_particle_init = rain_particle_init;
    config->callbacks.on_particle_update = rain_particle_update;
    config->callbacks.on_particle_death = rain_particle_death;
    config->callbacks.user_data = NULL;
}

gui_particle_widget_t *effect_rain_demo_init(void)
{
    gui_obj_t *root = gui_obj_get_root();
    gui_dispdev_t *dc = gui_get_dc();
    s_screen_w = dc->screen_width;
    s_screen_h = dc->screen_height;
    s_ground_y = (float)(s_screen_h - 30);

    s_rain_widget = gui_particle_widget_create(root, "rain_demo",
                                               0, 0, s_screen_w, s_screen_h,
                                               PARTICLE_POOL_SIZE);
    if (s_rain_widget == NULL)
    {
        gui_log("Rain: Failed to create widget\n");
        return NULL;
    }

    particle_effect_config_t config;
    effect_rain_config(&config);

    /* Horizontal drift = tan(angle) * screen_height */
    float drift = tanf(RAIN_ANGLE_DEG * M_PI_F / 180.0f)
                  * (float)s_screen_h;
    float splash_margin = 10.0f;

    /* Emission line covers screen + drift overshoot */
    config.shape.line.x1 = -drift;
    config.shape.line.x2 = (float)s_screen_w + drift;

    /* Boundary with drift margin */
    config.boundary.left = -drift;
    config.boundary.right = (float)s_screen_w + drift;
    config.boundary.bottom = s_ground_y + splash_margin;

    gui_particle_widget_add_effect(s_rain_widget, &config);

    gui_log("Rain: Demo initialized - raindrops with splash\n");
    return s_rain_widget;
}
