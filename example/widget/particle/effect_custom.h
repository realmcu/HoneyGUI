/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_custom.h
 * @brief Custom Particle Effect with Callbacks Demo
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
 * @brief Load custom effect configuration
 * @param config Pointer to configuration to fill
 */
void effect_custom_config(particle_effect_config_t *config);

/**
 * @brief Initialize custom demo
 * @return Created widget pointer, NULL on failure
 */
gui_particle_widget_t *effect_custom_demo_init(void);

#ifdef __cplusplus
}
#endif

#endif /* EFFECT_CUSTOM_H */
