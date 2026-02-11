/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_trail.h
 * @brief Trail Particle Effect
 *
 * Trail effect characteristics:
 * - Point emission following position
 * - Linear trajectory with damping
 * - Solid color
 * - Continuous emission
 * - Scale fade out
 */

#ifndef EFFECT_TRAIL_H
#define EFFECT_TRAIL_H

#include "particle_effect.h"
#include "gui_particle_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create trail particle effect
 *
 * Creates a touch-following trail effect. Particles emit from
 * the touch position and fade out with damping.
 *
 * @param parent Parent widget to attach to
 * @param name Widget name
 * @param x X coordinate relative to parent
 * @param y Y coordinate relative to parent
 * @param w Widget width
 * @param h Widget height
 * @return Created widget pointer, NULL on failure
 */
gui_particle_widget_t *effect_trail_create(gui_obj_t *parent, const char *name,
                                           int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif /* EFFECT_TRAIL_H */
