/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_rocket.h
 * @brief Rocket Thruster Exhaust Particle Effect
 *
 * Rocket exhaust effect characteristics:
 * - Point emission from upper-right area of widget
 * - Conical spread toward lower-left
 * - Gradient color: bright yellow/orange -> red -> fade out
 * - Fast particles with short lifetime
 * - Additive blending for bright flame effect
 * - Smoke trail with slower, darker particles
 */

#ifndef EFFECT_ROCKET_H
#define EFFECT_ROCKET_H

#include "particle_effect.h"
#include "gui_particle_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create rocket exhaust particle effect
 *
 * Creates a dual-layer particle effect (flame + smoke).
 * Emission point defaults to upper-right area of widget (70% w, 10% h),
 * with exhaust directed toward lower-left. Adjust widget x/y position
 * to place the emission point where needed.
 *
 * @param parent Parent widget to attach to
 * @param name Widget name
 * @param x X coordinate relative to parent
 * @param y Y coordinate relative to parent
 * @param w Widget width
 * @param h Widget height
 * @return Created widget pointer, NULL on failure
 */
gui_particle_widget_t *effect_rocket_create(gui_obj_t *parent, const char *name,
                                            int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif /* EFFECT_ROCKET_H */
