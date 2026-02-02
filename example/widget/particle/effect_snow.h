/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_snow.h
 * @brief Snow Particle Effect Configuration and Demo
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
 * @brief Load snow effect configuration
 * @param config Pointer to configuration to fill
 */
void effect_snow_config(particle_effect_config_t *config);

/**
 * @brief Initialize snow demo
 * @return Created widget pointer, NULL on failure
 */
gui_particle_widget_t *effect_snow_demo_init(void);

#ifdef __cplusplus
}
#endif

#endif /* EFFECT_SNOW_H */
