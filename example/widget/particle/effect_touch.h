/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_touch.h
 * @brief Touch Feedback Particle Effect Configuration and Demo
 *
 * Touch effect characteristics:
 * - Point emission at touch position
 * - Linear trajectory with slight damping
 * - Burst mode on touch
 * - Additive blending
 * - Quick fade out
 */

#ifndef EFFECT_TOUCH_H
#define EFFECT_TOUCH_H

#include "particle_effect.h"
#include "gui_particle_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Load touch feedback effect configuration
 * @param config Pointer to configuration to fill
 */
void effect_touch_config(particle_effect_config_t *config);

/**
 * @brief Initialize touch demo
 * @return Created widget pointer, NULL on failure
 */
gui_particle_widget_t *effect_touch_demo_init(void);

#ifdef __cplusplus
}
#endif

#endif /* EFFECT_TOUCH_H */
