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
#include "gui_img.h"
#include "gui_api_os.h"
#include "gui_api_dc.h"
#include "def_type.h"
#include <math.h>
#include <stdlib.h>
#include "gui_text.h"

#include "galaxy.h"
#include "../../assets/font/NotoSans_Regular_vector.h"

#define PARTICLE_POOL_SIZE   600
#define GALAXY_OUTER_R       200.0f
#define GALAXY_INNER_R       180.0f
#define GALAXY_CORE_R        80.0f     /* Larger core to match halo image */
#define GALAXY_PULL_SPEED    12.0f     /* Increased inward pull */
#define GALAXY_SPIN_SPEED    2.0f
#define GALAXY_NUM_ARMS      3

#define TEXT_VALUE_MIN       60
#define TEXT_VALUE_MAX       89
#define TEXT_UPDATE_INTERVAL 1000  /* 1 second in ms */

static gui_particle_widget_t *s_galaxy_widget = NULL;
static float s_galaxy_center_x = 240.0f;
static float s_galaxy_center_y = 240.0f;
static float s_galaxy_rotation = 0.0f;

static gui_text_t *s_galaxy_text = NULL;
static int s_text_value = TEXT_VALUE_MIN;
static char s_text_buffer[16];

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

    /* Pick a random spiral arm */
    int arm = rand() % GALAXY_NUM_ARMS;
    float arm_angle = (float)arm * 2.0f * M_PI_F / GALAXY_NUM_ARMS;

    /* Random distance from center */
    float dist = GALAXY_INNER_R + (float)(rand() % 100) / 100.0f *
                 (GALAXY_OUTER_R - GALAXY_INNER_R);

    /* Spiral offset: angle increases as distance decreases (logarithmic spiral) */
    float spiral_angle = arm_angle + s_galaxy_rotation +
                         logf(dist / GALAXY_CORE_R) * 0.8f;

    /* Position on spiral arm with some spread */
    float spread = ((float)(rand() % 100) / 100.0f - 0.5f) * 0.3f;
    p->x = s_galaxy_center_x + cosf(spiral_angle + spread) * dist;
    p->y = s_galaxy_center_y + sinf(spiral_angle + spread) * dist;

    /* Initial velocity tangent to spiral */
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

    /* Calculate vector from particle to center */
    float dx = p->x - s_galaxy_center_x;
    float dy = p->y - s_galaxy_center_y;
    float dist = sqrtf(dx * dx + dy * dy);

    /* Core - particle absorbed */
    if (dist < GALAXY_CORE_R)
    {
        p->life = 0;
        return;
    }

    /* Normalize direction (outward from center) */
    float nx = dx / dist;
    float ny = dy / dist;

    /* Tangent direction (perpendicular, for orbital spin) */
    float tx = -ny;
    float ty = nx;

    /* Spin faster as particle gets closer to center */
    float spin_factor = 1.0f + (GALAXY_OUTER_R - dist) / GALAXY_OUTER_R * 3.0f;
    float spin_speed = GALAXY_SPIN_SPEED * spin_factor;

    /* Pull gradually stronger as particle gets closer */
    float pull_factor = 1.0f + (GALAXY_OUTER_R - dist) / GALAXY_OUTER_R * 1.5f;
    float pull_speed = GALAXY_PULL_SPEED * pull_factor;

    /* Combine inward pull + tangential spin */
    p->vx = -nx * pull_speed + tx * spin_speed * dist * 0.3f;
    p->vy = -ny * pull_speed + ty * spin_speed * dist * 0.3f;

    /* Color based on distance: blue/purple (outer) -> white/pink (inner) */
    float dist_ratio = dist / GALAXY_OUTER_R;
    if (dist_ratio > 1.0f) { dist_ratio = 1.0f; }
    if (dist_ratio < 0.0f) { dist_ratio = 0.0f; }

    /* Interpolate: outer blue 0x4488FF -> inner white/pink 0xFFAAFF */
    uint8_t r = (uint8_t)(0x44 + (0xFF - 0x44) * (1.0f - dist_ratio));
    uint8_t g = (uint8_t)(0x88 + (0xAA - 0x88) * (1.0f - dist_ratio));
    uint8_t b = 0xFF;
    p->color = 0xFF000000 | (r << 16) | (g << 8) | b;

    /* Scale: larger near center (brighter core) */
    float scale_factor = 0.5f + (1.0f - dist_ratio) * 0.8f;
    p->scale = scale_factor;

    /* Rotate particle to face movement direction */
    p->rotation = atan2f(p->vy, p->vx);
}

void effect_galaxy_config(particle_effect_config_t *config)
{
    if (config == NULL)
    {
        return;
    }

    particle_effect_config_init(config);

    /* Ring emission - will be overridden by init callback */
    config->shape.type = PARTICLE_SHAPE_RING;
    config->shape.ring.cx = s_galaxy_center_x;
    config->shape.ring.cy = s_galaxy_center_y;
    config->shape.ring.inner_r = GALAXY_INNER_R;
    config->shape.ring.outer_r = GALAXY_OUTER_R;

    /* Linear trajectory - motion handled by callback */
    config->trajectory.type = PARTICLE_TRAJECTORY_LINEAR;
    config->trajectory.damping = 0.0f;

    /* Emission settings */
    config->emission.angle_min = 0.0f;
    config->emission.angle_max = 2.0f * M_PI_F;
    config->emission.speed_min = 5.0f;
    config->emission.speed_max = 15.0f;
    config->emission.rate = 30.0f;
    config->emission.burst_enabled = 0;

    /* Longer lifetime to ensure particles reach core */
    config->lifecycle.life_min = 8000;
    config->lifecycle.life_max = 12000;
    config->lifecycle.auto_cleanup = 0;
    config->lifecycle.loop = 1;

    /* Color set by callback */
    config->color.mode = PARTICLE_COLOR_SOLID;
    config->color.color_start = 0xFF4488FF;

    /* Opacity */
    config->opacity.start = 255;
    config->opacity.end = 180;
    config->opacity.easing = PARTICLE_EASING_LINEAR;

    /* Scale set by callback */
    config->scale.start = 1.0f;
    config->scale.end = 0.5f;
    config->scale.min = 0.8f;
    config->scale.max = 1.5f;

    /* Rotation aligned to velocity */
    config->rotation.align_velocity = 1;

    /* No boundary - particles die at core via callback */
    config->boundary.behavior = PARTICLE_BOUNDARY_NONE;

    /* Additive blending for glowing effect */
    config->render.blend_mode = PARTICLE_BLEND_ADDITIVE;
    config->render.base_size = 4.0f;

    /* Register custom callbacks */
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
    /* Slowly rotate the spiral pattern */
    s_galaxy_rotation += 0.01f;
    if (s_galaxy_rotation > 2.0f * M_PI_F)
    {
        s_galaxy_rotation -= 2.0f * M_PI_F;
    }
}

/**
 * @brief Timer callback - update text value every second
 */
static void galaxy_text_timer_cb(void *user_data)
{
    GUI_UNUSED(user_data);
    s_text_value++;
    if (s_text_value > TEXT_VALUE_MAX)
    {
        s_text_value = TEXT_VALUE_MIN;
    }
    snprintf(s_text_buffer, sizeof(s_text_buffer), "%d%%", s_text_value);
    gui_text_content_set(s_galaxy_text, s_text_buffer, strlen(s_text_buffer));
}

gui_particle_widget_t *effect_galaxy_demo_init(void)
{
    gui_obj_t *root = gui_obj_get_root();
    gui_dispdev_t *dc = gui_get_dc();
    int screen_w = dc->screen_width;
    int screen_h = dc->screen_height;

    /* Update center coordinates */
    s_galaxy_center_x = (float)(screen_w / 2);
    s_galaxy_center_y = (float)(screen_h / 2);

    s_galaxy_widget = gui_particle_widget_create(root, "galaxy_demo",
                                                 0, 0, screen_w, screen_h,
                                                 PARTICLE_POOL_SIZE);
    if (s_galaxy_widget == NULL)
    {
        gui_log("Galaxy: Failed to create widget\n");
        return NULL;
    }

    particle_effect_config_t config;
    effect_galaxy_config(&config);

    /* Update ring position for screen size */
    config.shape.ring.cx = s_galaxy_center_x;
    config.shape.ring.cy = s_galaxy_center_y;

    /* Adjust ring radius based on screen size */
    float min_dim = (float)(screen_w < screen_h ? screen_w : screen_h);
    config.shape.ring.outer_r = min_dim * 0.40f;
    config.shape.ring.inner_r = min_dim * 0.35f;

    config.emission.rate = 30.0f;

    gui_particle_widget_add_effect(s_galaxy_widget, &config);
    gui_particle_widget_set_update_cb(s_galaxy_widget, galaxy_widget_update, NULL);

    gui_img_t *img = gui_img_create_from_mem(root, "img", (void *)galaxy, 0, 0, 0, 0);
    uint16_t img_w = gui_img_get_width(img);
    uint16_t img_h = gui_img_get_height(img);
    gui_img_set_attribute(img, "img", (void *)galaxy, (screen_w - img_w) / 2, (screen_h - img_h) / 2);
    gui_img_set_mode(img, IMG_SRC_OVER_MODE);

    s_galaxy_text = gui_text_create(root, "text", 0, 0, 0, 0);
    s_text_value = TEXT_VALUE_MIN;
    snprintf(s_text_buffer, sizeof(s_text_buffer), "%d%%", s_text_value);
    gui_text_set(s_galaxy_text, s_text_buffer, GUI_FONT_SRC_TTF, APP_COLOR_SILVER,
                 strlen(s_text_buffer), 90);
    gui_text_type_set(s_galaxy_text, (void *)_acNotoSans_Regular_vector, FONT_SRC_MEMADDR);
    gui_text_mode_set(s_galaxy_text, MID_CENTER);
    gui_obj_create_timer((gui_obj_t *)s_galaxy_text, TEXT_UPDATE_INTERVAL, true,
                         galaxy_text_timer_cb);

    gui_log("Galaxy: Demo initialized - spiral galaxy effect\n");
    return s_galaxy_widget;
}
