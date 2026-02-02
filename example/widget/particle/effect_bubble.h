/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_bubble.h
 * @brief Bubble Particle Effect Configuration and Demo
 *
 * Bubble effect characteristics:
 * - Line emission from bottom
 * - Negative gravity (rising)
 * - Semi-transparent color
 * - Reflect boundary behavior
 * - Scale increase over lifetime
 */

#ifndef EFFECT_BUBBLE_H
#define EFFECT_BUBBLE_H

#include "particle_effect.h"
#include "gui_particle_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Load bubble effect configuration
 * @param config Pointer to configuration to fill
 */
void effect_bubble_config(particle_effect_config_t *config);

/**
 * @brief Initialize bubble demo
 * @return Created widget pointer, NULL on failure
 */
gui_particle_widget_t *effect_bubble_demo_init(void);

#ifdef __cplusplus
}
#endif

#endif /* EFFECT_BUBBLE_H */
