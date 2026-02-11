/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_ripple.h
 * @brief Ripple Particle Effect - Concentric Circle Expansion
 *
 * Ripple effect characteristics:
 * - Touch-triggered concentric ring expansion
 * - Secondary ripple waves on particle death
 * - Cyan-blue color with additive glow
 * - Auto-trigger at center when no touch input
 */

#ifndef EFFECT_RIPPLE_H
#define EFFECT_RIPPLE_H

#include "particle_effect.h"
#include "gui_particle_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create ripple particle effect
 *
 * Creates expanding concentric rings triggered by touch.
 * Auto-triggers at center if no touch for 3 seconds.
 *
 * @param parent Parent widget to attach to
 * @param name Widget name
 * @param x X coordinate relative to parent
 * @param y Y coordinate relative to parent
 * @param w Widget width
 * @param h Widget height
 * @return Created widget pointer, NULL on failure
 */
gui_particle_widget_t *effect_ripple_create(gui_obj_t *parent, const char *name,
                                            int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif /* EFFECT_RIPPLE_H */
