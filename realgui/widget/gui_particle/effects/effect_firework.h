/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_firework.h
 * @brief Firework Particle Effect
 *
 * Firework effect characteristics:
 * - Point emission at center
 * - Gravity trajectory (falling particles)
 * - Random colors from palette
 * - Burst mode emission
 * - Fade out opacity with ease-out
 */

#ifndef EFFECT_FIREWORK_H
#define EFFECT_FIREWORK_H

#include "particle_effect.h"
#include "gui_particle_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create firework particle effect
 *
 * Creates a particle widget with periodic firework bursts.
 * New fireworks auto-trigger every 1.5 seconds at random positions.
 *
 * @param parent Parent widget to attach to
 * @param name Widget name
 * @param x X coordinate relative to parent
 * @param y Y coordinate relative to parent
 * @param w Widget width
 * @param h Widget height
 * @return Created widget pointer, NULL on failure
 */
gui_particle_widget_t *effect_firework_create(gui_obj_t *parent, const char *name,
                                              int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif /* EFFECT_FIREWORK_H */
