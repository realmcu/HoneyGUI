/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_magic_circle.h
 * @brief Magic Circle Particle Effect - Doctor Strange Style
 *
 * Magic circle effect characteristics:
 * - Inner and outer circle images rotating in opposite directions
 * - Particles float upward from the circle edges and center
 * - Orange-gold color palette with additive glow
 * - Images placed at screen center
 */

#ifndef EFFECT_MAGIC_CIRCLE_H
#define EFFECT_MAGIC_CIRCLE_H

#include "particle_effect.h"
#include "gui_particle_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Load magic circle effect configuration
 * @param config Pointer to configuration to fill
 */
void effect_magic_circle_config(particle_effect_config_t *config);

/**
 * @brief Initialize magic circle demo
 * @return Created widget pointer, NULL on failure
 */
gui_particle_widget_t *effect_magic_circle_demo_init(void);

#ifdef __cplusplus
}
#endif

#endif /* EFFECT_MAGIC_CIRCLE_H */
