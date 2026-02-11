/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_fireflies.h
 * @brief Fireflies Particle Effect - Floating Glowing Dots
 *
 * Fireflies effect characteristics:
 * - Rectangle emission (full area random spawn)
 * - Breathing opacity effect
 * - Slow random drift motion
 * - Warm yellow-green glow colors
 */

#ifndef EFFECT_FIREFLIES_H
#define EFFECT_FIREFLIES_H

#include "particle_effect.h"
#include "gui_particle_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create fireflies particle effect
 *
 * Creates floating glowing dots with breathing opacity
 * and random drift motion.
 *
 * @param parent Parent widget to attach to
 * @param name Widget name
 * @param x X coordinate relative to parent
 * @param y Y coordinate relative to parent
 * @param w Widget width
 * @param h Widget height
 * @return Created widget pointer, NULL on failure
 */
gui_particle_widget_t *effect_fireflies_create(gui_obj_t *parent, const char *name,
                                               int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif /* EFFECT_FIREFLIES_H */
