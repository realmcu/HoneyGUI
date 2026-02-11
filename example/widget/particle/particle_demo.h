/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file particle_demo.h
 * @brief Demo entry functions for all particle effects
 *
 * Each demo creates a full-screen effect on the root widget,
 * including any image/font resources needed for demonstration.
 * These are used by the particle launcher selection menu.
 *
 * For production use, call effect_xxx_create() directly from
 * the individual effect headers instead.
 */

#ifndef PARTICLE_DEMO_H
#define PARTICLE_DEMO_H

#include "gui_particle_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

gui_particle_widget_t *effect_galaxy_demo_init(void);
gui_particle_widget_t *effect_firework_demo_init(void);
gui_particle_widget_t *effect_trail_demo_init(void);
gui_particle_widget_t *effect_touch_demo_init(void);
gui_particle_widget_t *effect_snow_demo_init(void);
gui_particle_widget_t *effect_bubble_demo_init(void);
gui_particle_widget_t *effect_custom_demo_init(void);
gui_particle_widget_t *effect_tunnel_demo_init(void);
gui_particle_widget_t *effect_rocket_demo_init(void);
gui_particle_widget_t *effect_vortex_demo_init(void);
gui_particle_widget_t *effect_lightning_demo_init(void);
gui_particle_widget_t *effect_fireflies_demo_init(void);
gui_particle_widget_t *effect_ripple_demo_init(void);
gui_particle_widget_t *effect_rain_demo_init(void);
gui_particle_widget_t *effect_magic_circle_demo_init(void);
gui_particle_widget_t *effect_light_beam_demo_init(void);

#ifdef __cplusplus
}
#endif

#endif /* PARTICLE_DEMO_H */
