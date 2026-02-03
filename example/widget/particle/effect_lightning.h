/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_lightning.h
 * @brief Lightning Arc Particle Effect - Electric Discharge Between Two Points
 *
 * Lightning effect characteristics:
 * - Point emission from start position
 * - Particles form branching arc paths toward target
 * - Electric blue/white color with glow
 * - Random jitter creates natural lightning appearance
 * - Periodic discharge bursts
 */

#ifndef EFFECT_LIGHTNING_H
#define EFFECT_LIGHTNING_H

#include "particle_effect.h"
#include "gui_particle_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Load lightning effect configuration
 * @param config Pointer to configuration to fill
 */
void effect_lightning_config(particle_effect_config_t *config);

/**
 * @brief Initialize lightning demo
 * @return Created widget pointer, NULL on failure
 */
gui_particle_widget_t *effect_lightning_demo_init(void);

#ifdef __cplusplus
}
#endif

#endif /* EFFECT_LIGHTNING_H */
