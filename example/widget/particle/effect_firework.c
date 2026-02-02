/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_firework.c
 * @brief Firework Particle Effect Configuration and Demo
 */

#include "effect_firework.h"
#include "gui_obj.h"
#include "gui_api_os.h"

#ifndef M_PI_F
#define M_PI_F 3.14159265358979323846f
#endif

#ifndef DRV_LCD_WIDTH
#define DRV_LCD_WIDTH  480
#endif

#ifndef DRV_LCD_HEIGHT
#define DRV_LCD_HEIGHT 480
#endif

#define PARTICLE_POOL_SIZE 256

static gui_particle_widget_t *s_firework_widget = NULL;
static uint32_t s_firework_timer = 0;

static uint32_t firework_rand(void)
{
    static uint32_t seed = 12345;
    seed = seed * 1103515245 + 12345;
    return (seed >> 16) & 0x7FFF;
}

static float firework_rand_float(float min, float max)
{
    return min + (float)(firework_rand() % 1000) / 1000.0f * (max - min);
}

static void firework_update_cb(void *user_data)
{
    GUI_UNUSED(user_data);
    if (s_firework_widget == NULL || s_firework_widget->ps == NULL)
    {
        return;
    }

    particle_system_t *ps = s_firework_widget->ps;
    uint32_t current_time = gui_ms_get();

    /* Clean up effect entries for emitters that have been auto-cleaned */
    for (int j = 0; j < PARTICLE_WIDGET_MAX_EFFECTS; j++)
    {
        if (s_firework_widget->effects[j].active)
        {
            particle_emitter_t *emitter = s_firework_widget->effects[j].emitter;
            uint8_t emitter_exists = 0;
            for (int k = 0; k < ps->emitter_count; k++)
            {
                if (ps->emitters[k] == emitter)
                {
                    emitter_exists = 1;
                    break;
                }
            }
            if (!emitter_exists)
            {
                s_firework_widget->effects[j].handle = PARTICLE_INVALID_HANDLE;
                s_firework_widget->effects[j].emitter = NULL;
                s_firework_widget->effects[j].active = 0;
            }
        }
    }

    /* Trigger new firework every 1.5 seconds */
    if (current_time - s_firework_timer > 1500)
    {
        particle_effect_config_t config;
        effect_firework_config(&config);

        config.shape.point.x = firework_rand_float(100.0f, (float)(DRV_LCD_WIDTH - 100));
        config.shape.point.y = firework_rand_float(100.0f, (float)(DRV_LCD_HEIGHT / 2));

        uint32_t colors[] =
        {
            0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFF00,
            0xFFFF00FF, 0xFF00FFFF, 0xFFFF8800, 0xFFFFFFFF
        };
        config.color.mode = PARTICLE_COLOR_SOLID;
        config.color.color_start = colors[firework_rand() % 8];
        config.emission.burst_count = 40 + (firework_rand() % 40);
        config.lifecycle.life_min = 1500;
        config.lifecycle.life_max = 2500;
        config.boundary.right = (float)DRV_LCD_WIDTH;
        config.boundary.bottom = (float)DRV_LCD_HEIGHT;

        gui_particle_widget_add_effect(s_firework_widget, &config);
        s_firework_timer = current_time;
    }
}

void effect_firework_config(particle_effect_config_t *config)
{
    if (config == NULL)
    {
        return;
    }

    particle_effect_config_init(config);

    config->shape.type = PARTICLE_SHAPE_POINT;
    config->shape.point.x = 200.0f;
    config->shape.point.y = 200.0f;

    config->trajectory.type = PARTICLE_TRAJECTORY_GRAVITY;
    config->trajectory.gravity = 200.0f;
    config->trajectory.damping = 0.02f;

    config->emission.angle_min = 0.0f;
    config->emission.angle_max = 2.0f * M_PI_F;
    config->emission.speed_min = 100.0f;
    config->emission.speed_max = 200.0f;
    config->emission.rate = 50.0f;
    config->emission.burst_count = 30;
    config->emission.burst_interval = 0;
    config->emission.burst_enabled = 1;

    config->lifecycle.life_min = 800;
    config->lifecycle.life_max = 1500;
    config->lifecycle.auto_cleanup = 1;
    config->lifecycle.loop = 0;

    config->color.mode = PARTICLE_COLOR_RANDOM;
    config->color.palette[0] = 0xFFFF0000;
    config->color.palette[1] = 0xFFFFFF00;
    config->color.palette[2] = 0xFF00FF00;
    config->color.palette[3] = 0xFF00FFFF;
    config->color.palette[4] = 0xFF0000FF;
    config->color.palette[5] = 0xFFFF00FF;
    config->color.palette[6] = 0xFFFFAA00;
    config->color.palette[7] = 0xFFFFFFFF;
    config->color.palette_size = 8;

    config->opacity.start = 255;
    config->opacity.end = 0;
    config->opacity.easing = PARTICLE_EASING_EASE_OUT;

    config->scale.start = 1.0f;
    config->scale.end = 0.3f;
    config->scale.min = 0.8f;
    config->scale.max = 1.2f;

    config->rotation.angle_min = 0.0f;
    config->rotation.angle_max = 2.0f * M_PI_F;

    config->boundary.behavior = PARTICLE_BOUNDARY_KILL;
    config->boundary.left = 0.0f;
    config->boundary.top = 0.0f;
    config->boundary.right = 400.0f;
    config->boundary.bottom = 400.0f;
    config->boundary.reflect_damping = 0.8f;

    config->render.blend_mode = PARTICLE_BLEND_ADDITIVE;
    config->render.base_size = 6.0f;
}

gui_particle_widget_t *effect_firework_demo_init(void)
{
    gui_obj_t *root = gui_obj_get_root();

    s_firework_widget = gui_particle_widget_create(root, "firework_demo",
                                                   0, 0, DRV_LCD_WIDTH, DRV_LCD_HEIGHT,
                                                   PARTICLE_POOL_SIZE);
    if (s_firework_widget == NULL)
    {
        return NULL;
    }

    gui_particle_widget_set_update_cb(s_firework_widget, firework_update_cb, NULL);

    particle_effect_config_t config;
    effect_firework_config(&config);

    config.shape.point.x = (float)(DRV_LCD_WIDTH / 2);
    config.shape.point.y = (float)(DRV_LCD_HEIGHT / 3);
    config.color.color_start = 0xFFFF0000;
    config.emission.burst_count = 60;
    config.lifecycle.life_min = 1500;
    config.lifecycle.life_max = 2500;
    config.boundary.right = (float)DRV_LCD_WIDTH;
    config.boundary.bottom = (float)DRV_LCD_HEIGHT;

    gui_particle_widget_add_effect(s_firework_widget, &config);
    s_firework_timer = gui_ms_get();

    return s_firework_widget;
}
