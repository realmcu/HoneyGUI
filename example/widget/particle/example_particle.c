/**
 * @file example_particle.c
 * @brief Particle System Examples for HoneyGUI
 *
 * This file demonstrates the particle system widget with various effects.
 * Each effect is implemented in separate files (effect_*.c/h).
 *
 * The launcher provides a selection menu to browse all effects.
 * For direct API usage, see particle_demo.c/h for all effect_xxx_create() calls.
 *
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "gui_server.h"
#include "gui_components_init.h"
#include "particle_launcher.h"

/*============================================================================*
 *                    Application Entry Point
 *============================================================================*/

#ifdef _HONEYGUI_SIMULATOR_
unsigned char *resource_root = NULL;
extern const unsigned char _binary_root_0x704D1400_bin_start[];
extern const unsigned char _binary_root_0x704D1400_bin_end[];
extern const unsigned char _binary_root_0x704D1400_bin_size[];
#endif

static int particle_demo_init(void)
{
#ifdef _HONEYGUI_SIMULATOR_
    resource_root = (unsigned char *)_binary_root_0x704D1400_bin_start;
#endif

    particle_launcher_init();
    gui_set_keep_active_time(0xFFFFFFFF);
    return 0;
}

GUI_INIT_APP_EXPORT(particle_demo_init);
