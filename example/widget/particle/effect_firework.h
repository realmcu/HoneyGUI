/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_firework.h
 * @brief Firework Particle Effect Configuration and Demo
 *
 * Firework effect characteristics:
 * - Point emission at center
 * - Gravity trajectory (falling particles)
 * - Random colors from palette
 * - Burst mode emission
 * - Fade out opacity with ease-out
 */

#ifndef EFFECT_FIREWORK_H
#define EFFECT_FIREWORK_H

#include "particle_effect.h"
#include "gui_particle_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Load firework effect configuration
 * @param config Pointer to configuration to fill
 */
void effect_firework_config(particle_effect_config_t *config);

/**
 * @brief Initialize firework demo
 * @return Created widget pointer, NULL on failure
 */
gui_particle_widget_t *effect_firework_demo_init(void);

#ifdef __cplusplus
}
#endif

#endif /* EFFECT_FIREWORK_H */
