/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_snow.h
 * @brief Snow Particle Effect
 *
 * Snow effect characteristics:
 * - Line emission from top
 * - Gravity trajectory with wind
 * - White solid color
 * - Continuous emission
 * - Slow rotation
 */

#ifndef EFFECT_SNOW_H
#define EFFECT_SNOW_H

#include "particle_effect.h"
#include "gui_particle_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create snow particle effect
 *
 * Creates a particle widget with snow effect attached to the given parent.
 * All coordinates are auto-calculated from the provided width/height.
 * No external image or font resources required.
 *
 * @param parent Parent widget to attach to
 * @param name Widget name
 * @param x X coordinate relative to parent
 * @param y Y coordinate relative to parent
 * @param w Widget width (snow emission line width)
 * @param h Widget height (snow fall distance)
 * @return Created widget pointer, NULL on failure
 */
gui_particle_widget_t *effect_snow_create(gui_obj_t *parent, const char *name,
                                          int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif /* EFFECT_SNOW_H */
