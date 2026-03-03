/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_vortex.h
 * @brief Energy Vortex Particle Effect - Spiral Orbit Effect
 *
 * Vortex effect characteristics:
 * - Ring emission from outer edge
 * - Orbital trajectory (particles spiral toward center)
 * - Rainbow color cycling over lifetime
 * - Scale shrinks as particles approach center
 * - Creates a "black hole" or "energy vortex" visual effect
 */

#ifndef EFFECT_VORTEX_H
#define EFFECT_VORTEX_H

#include "particle_effect.h"
#include "gui_particle_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create energy vortex particle effect
 *
 * Creates a spiral vortex where particles orbit inward from
 * the outer ring toward the center. Radii auto-calculated
 * from widget dimensions.
 *
 * @param parent Parent widget to attach to
 * @param name Widget name
 * @param x X coordinate relative to parent
 * @param y Y coordinate relative to parent
 * @param w Widget width
 * @param h Widget height
 * @return Created widget pointer, NULL on failure
 */
gui_particle_widget_t *effect_vortex_create(gui_obj_t *parent, const char *name,
                                            int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif /* EFFECT_VORTEX_H */
