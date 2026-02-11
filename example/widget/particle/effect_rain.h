/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_rain.h
 * @brief Rain Particle Effect - Falling Raindrops with Splash
 *
 * Rain effect characteristics:
 * - Line emission from top edge
 * - Diagonal fall with wind
 * - Splash particles on ground impact
 * - Two-layer system (raindrops + splashes)
 */

#ifndef EFFECT_RAIN_H
#define EFFECT_RAIN_H

#include "particle_effect.h"
#include "gui_particle_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create rain particle effect
 *
 * Creates falling raindrops with splash on ground impact.
 * Ground level is at (h - 30) pixels.
 *
 * @param parent Parent widget to attach to
 * @param name Widget name
 * @param x X coordinate relative to parent
 * @param y Y coordinate relative to parent
 * @param w Widget width
 * @param h Widget height
 * @return Created widget pointer, NULL on failure
 */
gui_particle_widget_t *effect_rain_create(gui_obj_t *parent, const char *name,
                                          int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif /* EFFECT_RAIN_H */
