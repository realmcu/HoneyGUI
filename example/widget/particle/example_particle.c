/**
 * @file example_particle.c
 * @brief Particle System Examples for HoneyGUI
 *
 * This file demonstrates the particle system widget with various effects.
 * Each effect (firework, trail, touch, snow, bubble, custom) is implemented
 * in separate files (effect_*.c/h) with its own configuration and callbacks.
 *
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "gui_server.h"
#include "gui_components_init.h"

/* Launcher with selection menu */
#include "particle_launcher.h"

/* Individual effect modules */
#include "effect_firework.h"
#include "effect_trail.h"
#include "effect_touch.h"
#include "effect_snow.h"
#include "effect_bubble.h"
#include "effect_custom.h"
#include "effect_tunnel.h"
#include "effect_rocket.h"
#include "effect_vortex.h"
#include "effect_lightning.h"
#include "effect_galaxy.h"
#include "effect_fireflies.h"
#include "effect_ripple.h"
#include "effect_rain.h"

/*============================================================================*
 *                    Application Entry Point
 *============================================================================*/

/**
 * @brief Particle demo entry point
 *
 * Two modes available:
 * 1. USE_LAUNCHER (default): Shows a selection menu to choose effects
 * 2. Direct load: Uncomment a specific effect to load it directly
 */

/* Enable launcher mode by default - comment out to use direct load */
#define USE_LAUNCHER

static int particle_demo_init(void)
{
#ifdef USE_LAUNCHER
    /* Launcher mode: Show selection menu */
    particle_launcher_init();
#else
    /* Direct load mode: Uncomment ONE of the following demos */

    /* Galaxy: Spiral galaxy with rotating arms and color gradient (default) */
    effect_galaxy_demo_init();

    /* Firework: Auto-burst at random positions, particles spread outward with gravity */
    // effect_firework_demo_init();

    /* Trail: Drag to emit trailing particles in opposite direction of movement */
    // effect_trail_demo_init();

    /* Touch: Touch feedback - burst on tap, continuous on drag, burst on release */
    // effect_touch_demo_init();

    /* Snow: Snowflakes fall from top with light gravity and random wind */
    // effect_snow_demo_init();

    /* Bubble: Bubbles rise from bottom, bounce off boundaries */
    // effect_bubble_demo_init();

    /* Custom: Example showing how to use callbacks for special behaviors */
    // effect_custom_demo_init();

    /* Tunnel: Warp speed effect - particles radiate from center outward (binary load demo) */
    // effect_tunnel_demo_init();

    /* Rocket: Thruster exhaust with flame and smoke trail - drag to move */
    // effect_rocket_demo_init();

    /* Vortex: Energy spiral - particles orbit inward toward center with rainbow colors */
    // effect_vortex_demo_init();

    /* Lightning: Electric arc discharge between two points with branching */
    // effect_lightning_demo_init();

    /* Fireflies: Floating glowing dots with breathing opacity (RECT + BREATHE) */
    // effect_fireflies_demo_init();

    /* Ripple: Touch-triggered concentric rings with secondary waves (on_particle_death) */
    // effect_ripple_demo_init();

    /* Rain: Falling raindrops with splash on impact (LINE + BOUNDARY_KILL + on_death) */
    // effect_rain_demo_init();
#endif

    gui_set_keep_active_time(0xFFFFFFFF);
    return 0;
}

GUI_INIT_APP_EXPORT(particle_demo_init);
