/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_touch.h
 * @brief Touch Feedback Particle Effect
 *
 * Touch effect characteristics:
 * - Point emission at touch position
 * - Linear trajectory with slight damping
 * - Burst mode on touch
 * - Additive blending
 * - Quick fade out
 */

#ifndef EFFECT_TOUCH_H
#define EFFECT_TOUCH_H

#include "particle_effect.h"
#include "gui_particle_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create touch feedback particle effect
 *
 * Creates a particle widget that responds to touch input with
 * burst effects on press and continuous emission while dragging.
 *
 * @param parent Parent widget to attach to
 * @param name Widget name
 * @param x X coordinate relative to parent
 * @param y Y coordinate relative to parent
 * @param w Widget width
 * @param h Widget height
 * @return Created widget pointer, NULL on failure
 */
gui_particle_widget_t *effect_touch_create(gui_obj_t *parent, const char *name,
                                           int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif /* EFFECT_TOUCH_H */
