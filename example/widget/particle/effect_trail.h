/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_trail.h
 * @brief Trail Particle Effect Configuration and Demo
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
 * @brief Load trail effect configuration
 * @param config Pointer to configuration to fill
 */
void effect_trail_config(particle_effect_config_t *config);

/**
 * @brief Initialize trail demo
 * @return Created widget pointer, NULL on failure
 */
gui_particle_widget_t *effect_trail_demo_init(void);

#ifdef __cplusplus
}
#endif

#endif /* EFFECT_TRAIL_H */
