/**
 * @file example_particle.c
 * @brief Particle System Examples for HoneyGUI
 *
 * This file demonstrates the particle system widget with various effects:
 * - Firework effect (Requirements 11.1)
 * - Trail effect (Requirements 11.2)
 * - Touch effect (Requirements 11.3)
 * - Snow effect (Requirements 11.4)
 * - Bubble effect (Requirements 11.5)
 *
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "guidef.h"
#include "gui_obj.h"
#include "gui_api_os.h"
#include "gui_server.h"
#include "gui_components_init.h"
#include "gui_particle_widget.h"
#include "tp_algo.h"

/*============================================================================*
 *                           Constants
 *============================================================================*/

/** Screen dimensions - adjust based on target display */
#ifndef DRV_LCD_WIDTH
#define DRV_LCD_WIDTH  480
#endif

#ifndef DRV_LCD_HEIGHT
#define DRV_LCD_HEIGHT 480
#endif

/** Particle pool capacity for each demo */
#define PARTICLE_POOL_SIZE 256

/** Particle colors (ARGB8888 format) */
#define COLOR_RED       0xFFFF0000
#define COLOR_GREEN     0xFF00FF00
#define COLOR_BLUE      0xFF0000FF
#define COLOR_YELLOW    0xFFFFFF00
#define COLOR_MAGENTA   0xFFFF00FF
#define COLOR_CYAN      0xFF00FFFF
#define COLOR_ORANGE    0xFFFF8800
#define COLOR_WHITE     0xFFFFFFFF
#define COLOR_LIGHTBLUE 0xFF87CEEB
#define COLOR_PINK      0xFFFFB6C1

/*============================================================================*
 *                            Variables
 *============================================================================*/

/** Particle widget instances for each demo */
static gui_particle_widget_t *firework_widget = NULL;
static gui_particle_widget_t *trail_widget = NULL;
static gui_particle_widget_t *touch_widget = NULL;
static gui_particle_widget_t *snow_widget = NULL;
static gui_particle_widget_t *bubble_widget = NULL;

/** Timer counter for firework auto-trigger (used in demo_firework_update) */
static uint32_t firework_timer = 0;



/*============================================================================*
 *                           Private Functions
 *============================================================================*/

/**
 * @brief Simple pseudo-random number generator
 */
static uint32_t simple_rand(void)
{
    static uint32_t seed = 12345;
    seed = seed * 1103515245 + 12345;
    return (seed >> 16) & 0x7FFF;
}

/**
 * @brief Get random float in range [min, max]
 */
static float rand_float(float min, float max)
{
    return min + (float)(simple_rand() % 1000) / 1000.0f * (max - min);
}

/*============================================================================*
 *                    Firework Effect Demo (Requirements 11.1)
 *============================================================================*/

/**
 * @brief Update firework demo - triggers new fireworks periodically with random colors
 *
 * This function also cleans up completed firework emitters to prevent hitting
 * the MAX_EMITTERS limit and allow continuous firework effects.
 */
static void demo_firework_update(void)
{
    if (firework_widget == NULL || firework_widget->ps == NULL)
    {
        return;
    }

    particle_system_t *ps = firework_widget->ps;
    uint32_t current_time = gui_ms_get();

    /* Clean up disabled emitters (completed fireworks) to free up slots */
    for (int i = (int)ps->emitter_count - 1; i >= 0; i--)
    {
        particle_emitter_t *emitter = ps->emitters[i];
        if (emitter != NULL && !emitter->enabled)
        {
            /* Remove and destroy the completed emitter */
            particle_system_remove_emitter(ps, emitter);
            particle_emitter_destroy(emitter, &firework_widget->config);
        }
    }

    /* Trigger new firework every 1.5 seconds */
    if (current_time - firework_timer > 1500)
    {
        float x = rand_float(100.0f, (float)(DRV_LCD_WIDTH - 100));
        float y = rand_float(100.0f, (float)(DRV_LCD_HEIGHT / 2));
        uint8_t count = 30 + (simple_rand() % 50);  /* 30-80 particles */

        /* Random color selection for fireworks */
        uint32_t colors[] =
        {
            COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_YELLOW,
            COLOR_MAGENTA, COLOR_CYAN, COLOR_ORANGE, COLOR_WHITE
        };
        uint32_t color = colors[simple_rand() % 8];

        gui_particle_widget_add_firework_color(firework_widget, x, y, count, color);
        firework_timer = current_time;
    }
}

/**
 * @brief Firework update callback - triggers new fireworks periodically
 */
static void firework_update_callback(void *user_data)
{
    GUI_UNUSED(user_data);
    demo_firework_update();
}

/**
 * @brief Initialize firework effect demo
 *
 * Creates a particle widget and periodically triggers firework explosions
 * at random positions on the screen.
 */
static void demo_firework_init(void)
{
    gui_obj_t *root = gui_obj_get_root();

    /* Create particle widget for firework effect */
    firework_widget = gui_particle_widget_create(root, "firework_demo",
                                                 0, 0, DRV_LCD_WIDTH, DRV_LCD_HEIGHT,
                                                 PARTICLE_POOL_SIZE);

    if (firework_widget == NULL)
    {
        return;
    }

    /* Set update callback for auto-triggering fireworks */
    gui_particle_widget_set_update_cb(firework_widget, firework_update_callback, NULL);

    /* Trigger initial firework with red color */
    float x = rand_float(100.0f, (float)(DRV_LCD_WIDTH - 100));
    float y = rand_float(100.0f, (float)(DRV_LCD_HEIGHT / 2));
    gui_particle_widget_add_firework_color(firework_widget, x, y, 50, 0xFFFF0000);

    firework_timer = gui_ms_get();
}

/*============================================================================*
 *                    Trail Effect Demo (Requirements 11.2)
 *============================================================================*/

/**
 * @brief Update trail demo with touch input
 */
static void demo_trail_update(float x, float y)
{
    if (trail_widget == NULL)
    {
        return;
    }

    gui_particle_widget_update_trail(trail_widget, x, y);
}

/** Track if trail was active last frame */
static uint8_t trail_was_pressing = 0;

/**
 * @brief Trail update callback - follows real touch input
 */
static void trail_update_callback(void *user_data)
{
    GUI_UNUSED(user_data);

    if (trail_widget == NULL)
    {
        return;
    }

    /* Get touch input */
    touch_info_t *tp = tp_get_info();
    if (tp == NULL)
    {
        return;
    }

    /* Check if touch is active */
    uint8_t is_pressing = tp->pressing;

    /* Also check touch type for hold events */
    if (tp->type == TOUCH_HOLD_X || tp->type == TOUCH_HOLD_Y ||
        tp->type == TOUCH_SHORT || tp->type == TOUCH_LONG)
    {
        is_pressing = 1;
    }

    /* Update trail when touch is active */
    if (is_pressing)
    {
        /* Calculate current position from first touch + delta */
        float x = (float)(tp->x + tp->deltaX);
        float y = (float)(tp->y + tp->deltaY);
        demo_trail_update(x, y);
        trail_was_pressing = 1;
    }
    else if (trail_was_pressing)
    {
        /* Stop trail when touch is released */
        gui_particle_widget_stop_trail(trail_widget);
        trail_was_pressing = 0;
    }
}

/**
 * @brief Initialize trail effect demo
 *
 * Creates a particle widget with trail effect that follows touch/pointer input.
 */
static void demo_trail_init(void)
{
    gui_obj_t *root = gui_obj_get_root();

    /* Create particle widget for trail effect */
    trail_widget = gui_particle_widget_create(root, "trail_demo",
                                              0, 0, DRV_LCD_WIDTH, DRV_LCD_HEIGHT,
                                              PARTICLE_POOL_SIZE);

    if (trail_widget == NULL)
    {
        return;
    }

    /* Add trail effect with cyan color */
    gui_particle_widget_add_trail_color(trail_widget, 0xFF00FFFF);

    /* Set update callback for touch-based trail */
    gui_particle_widget_set_update_cb(trail_widget, trail_update_callback, NULL);
}

/*============================================================================*
 *                    Touch Effect Demo (Requirements 11.3)
 *============================================================================*/

/** Track touch state */
static uint8_t touch_was_pressing = 0;

/**
 * @brief Handle touch tap event
 */
static void demo_touch_tap(float x, float y)
{
    if (touch_widget == NULL)
    {
        return;
    }

    gui_particle_widget_touch_tap(touch_widget, x, y);
}

/**
 * @brief Handle touch drag event
 */
static void demo_touch_drag(float x, float y)
{
    if (touch_widget == NULL)
    {
        return;
    }

    gui_particle_widget_touch_drag(touch_widget, x, y);
}

/**
 * @brief Handle touch release event
 */
static void demo_touch_release(float x, float y)
{
    if (touch_widget == NULL)
    {
        return;
    }

    gui_particle_widget_touch_release(touch_widget, x, y);
}

/**
 * @brief Touch update callback - handles touch input
 */
static void touch_update_callback(void *user_data)
{
    GUI_UNUSED(user_data);

    if (touch_widget == NULL)
    {
        return;
    }

    /* Get touch input */
    touch_info_t *tp = tp_get_info();
    if (tp == NULL)
    {
        return;
    }

    /* Calculate current position */
    float x = (float)(tp->x + tp->deltaX);
    float y = (float)(tp->y + tp->deltaY);

    /* Handle touch pressed event */
    if (tp->pressed && !touch_was_pressing)
    {
        demo_touch_tap(x, y);
        touch_was_pressing = 1;
    }
    /* Handle touch dragging */
    else if (tp->pressing && touch_was_pressing)
    {
        demo_touch_drag(x, y);
    }
    /* Handle touch released event */
    else if (tp->released && touch_was_pressing)
    {
        demo_touch_release(x, y);
        touch_was_pressing = 0;
    }
}

/**
 * @brief Initialize touch effect demo
 *
 * Creates a particle widget with touch feedback effect.
 */
static void demo_touch_init(void)
{
    gui_obj_t *root = gui_obj_get_root();

    /* Create particle widget for touch effect */
    touch_widget = gui_particle_widget_create(root, "touch_demo",
                                              0, 0, DRV_LCD_WIDTH, DRV_LCD_HEIGHT,
                                              PARTICLE_POOL_SIZE);

    if (touch_widget == NULL)
    {
        return;
    }

    /* Add touch effect with magenta color */
    gui_particle_widget_add_touch_color(touch_widget, 0xFFFF00FF);

    /* Set update callback for touch input handling */
    gui_particle_widget_set_update_cb(touch_widget, touch_update_callback, NULL);
}

/*============================================================================*
 *                    Snow Effect Demo (Requirements 11.4)
 *============================================================================*/

/**
 * @brief Initialize snow effect demo
 *
 * Creates a particle widget with falling snowflake effect.
 */
static void demo_snow_init(void)
{
    gui_obj_t *root = gui_obj_get_root();

    /* Create particle widget for snow effect */
    snow_widget = gui_particle_widget_create(root, "snow_demo",
                                             0, 0, DRV_LCD_WIDTH, DRV_LCD_HEIGHT,
                                             PARTICLE_POOL_SIZE);

    if (snow_widget == NULL)
    {
        return;
    }

    /* Add snow effect with medium intensity and light blue color */
    gui_particle_widget_add_snow_color(snow_widget, 0.5f, 0xFFCCEEFF);
}

/*============================================================================*
 *                    Bubble Effect Demo (Requirements 11.5)
 *============================================================================*/

/**
 * @brief Initialize bubble effect demo
 *
 * Creates a particle widget with rising bubble effect.
 */
static void demo_bubble_init(void)
{
    gui_obj_t *root = gui_obj_get_root();

    /* Create particle widget for bubble effect */
    bubble_widget = gui_particle_widget_create(root, "bubble_demo",
                                               0, 0, DRV_LCD_WIDTH, DRV_LCD_HEIGHT,
                                               PARTICLE_POOL_SIZE);

    if (bubble_widget == NULL)
    {
        return;
    }

    /* Add bubble effect with light cyan color */
    gui_particle_widget_add_bubble_color(bubble_widget, 0xFF88DDFF);
}

/*============================================================================*
 *                    Application Entry Point
 *============================================================================*/

/**
 * @brief Particle demo entry point
 */
static int particle_demo_init(void)
{
    /* Select one demo to run (uncomment desired effect) */
    demo_firework_init();
    // demo_trail_init();
    // demo_touch_init();
    // demo_snow_init();
    // demo_bubble_init();

    gui_set_keep_active_time(0xFFFFFFFF);
    return 0;
}

GUI_INIT_APP_EXPORT(particle_demo_init);

/* end of file */
