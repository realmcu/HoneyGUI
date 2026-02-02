/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_rocket.h
 * @brief Rocket Thruster Exhaust Particle Effect
 *
 * Rocket exhaust effect characteristics:
 * - Point emission from rocket nozzle position
 * - Conical spread (downward with slight spread angle)
 * - Gradient color: bright yellow/orange -> red -> fade out
 * - Fast particles with short lifetime
 * - Additive blending for bright flame effect
 * - Optional smoke trail with slower, darker particles
 */

#ifndef EFFECT_ROCKET_H
#define EFFECT_ROCKET_H

#include "particle_effect.h"
#include "gui_particle_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Load rocket exhaust effect configuration
 * @param config Pointer to configuration to fill
 */
void effect_rocket_config(particle_effect_config_t *config);

/**
 * @brief Initialize rocket exhaust demo
 * @return Created widget pointer, NULL on failure
 */
gui_particle_widget_t *effect_rocket_demo_init(void);

#ifdef __cplusplus
}
#endif

#endif /* EFFECT_ROCKET_H */
