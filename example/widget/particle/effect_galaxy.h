/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_galaxy.h
 * @brief Galaxy Spiral Particle Effect
 *
 * Galaxy effect characteristics:
 * - Spiral arm structure with logarithmic distribution
 * - Particles orbit and slowly drift toward center
 * - Blue/purple outer color transitioning to white/pink core
 * - Rotating spiral pattern
 * - Creates a spinning galaxy visual effect
 */

#ifndef EFFECT_GALAXY_H
#define EFFECT_GALAXY_H

#include "particle_effect.h"
#include "gui_particle_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create galaxy spiral particle effect
 *
 * Creates a spinning galaxy with spiral arms. Pure particle effect
 * with no image/font resources. For the full demo with center image
 * and text overlay, use effect_galaxy_demo_init() from particle_demo.h.
 *
 * @param parent Parent widget to attach to
 * @param name Widget name
 * @param x X coordinate relative to parent
 * @param y Y coordinate relative to parent
 * @param w Widget width
 * @param h Widget height
 * @return Created widget pointer, NULL on failure
 */
gui_particle_widget_t *effect_galaxy_create(gui_obj_t *parent, const char *name,
                                            int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif /* EFFECT_GALAXY_H */
