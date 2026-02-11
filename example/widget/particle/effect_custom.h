/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_custom.h
 * @brief Custom Particle Effect with Callbacks
 *
 * Custom effect characteristics:
 * - Circle emission from center
 * - Gravity trajectory with wind
 * - Rainbow color mode
 * - Custom callbacks for particle init and update
 * - Sine wave motion modifier
 */

#ifndef EFFECT_CUSTOM_H
#define EFFECT_CUSTOM_H

#include "particle_effect.h"
#include "gui_particle_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create custom callback particle effect
 *
 * Creates a particle widget with custom init/update callbacks
 * demonstrating sine wave motion and rainbow colors.
 *
 * @param parent Parent widget to attach to
 * @param name Widget name
 * @param x X coordinate relative to parent
 * @param y Y coordinate relative to parent
 * @param w Widget width
 * @param h Widget height
 * @return Created widget pointer, NULL on failure
 */
gui_particle_widget_t *effect_custom_create(gui_obj_t *parent, const char *name,
                                            int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif /* EFFECT_CUSTOM_H */
