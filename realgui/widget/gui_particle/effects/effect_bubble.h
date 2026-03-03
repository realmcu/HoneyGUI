/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_bubble.h
 * @brief Bubble Particle Effect
 *
 * Bubble effect characteristics:
 * - Line emission from bottom
 * - Negative gravity (rising)
 * - Semi-transparent color
 * - Reflect boundary behavior
 * - Scale increase over lifetime
 */

#ifndef EFFECT_BUBBLE_H
#define EFFECT_BUBBLE_H

#include "particle_effect.h"
#include "gui_particle_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create bubble particle effect
 *
 * Creates rising bubble particles from the bottom edge.
 * No external resources required.
 *
 * @param parent Parent widget to attach to
 * @param name Widget name
 * @param x X coordinate relative to parent
 * @param y Y coordinate relative to parent
 * @param w Widget width (bubble emission line width)
 * @param h Widget height (bubble rise distance)
 * @return Created widget pointer, NULL on failure
 */
gui_particle_widget_t *effect_bubble_create(gui_obj_t *parent, const char *name,
                                            int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif /* EFFECT_BUBBLE_H */
