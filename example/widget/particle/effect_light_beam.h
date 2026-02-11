/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_light_beam.h
 * @brief Light Beam Particle Effect - Multi-beam Reflecting Lasers
 *
 * Light beam effect characteristics:
 * - Multiple colored laser beams bouncing off edges
 * - Red, green, blue beams with additive blending
 * - Bounce flash particles on wall impact
 * - Beams cross and blend naturally
 */

#ifndef EFFECT_LIGHT_BEAM_H
#define EFFECT_LIGHT_BEAM_H

#include "particle_effect.h"
#include "gui_particle_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create light beam particle effect
 *
 * Creates 3 colored laser beams (red, green, blue) that bounce
 * off widget edges with flash effects on impact.
 *
 * @param parent Parent widget to attach to
 * @param name Widget name
 * @param x X coordinate relative to parent
 * @param y Y coordinate relative to parent
 * @param w Widget width
 * @param h Widget height
 * @return Created widget pointer, NULL on failure
 */
gui_particle_widget_t *effect_light_beam_create(gui_obj_t *parent, const char *name,
                                                int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif /* EFFECT_LIGHT_BEAM_H */
