/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_vortex.h
 * @brief Energy Vortex Particle Effect - Spiral Inward Effect
 *
 * Vortex effect characteristics:
 * - Ring emission from outer edge
 * - Orbital trajectory (particles spiral toward center)
 * - Rainbow color cycling over lifetime
 * - Scale shrinks as particles approach center
 * - Creates a "black hole" or "energy vortex" visual effect
 */

#ifndef EFFECT_VORTEX_H
#define EFFECT_VORTEX_H

#include "particle_effect.h"
#include "gui_particle_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Load vortex effect configuration
 * @param config Pointer to configuration to fill
 */
void effect_vortex_config(particle_effect_config_t *config);

/**
 * @brief Initialize vortex demo
 * @return Created widget pointer, NULL on failure
 */
gui_particle_widget_t *effect_vortex_demo_init(void);

#ifdef __cplusplus
}
#endif

#endif /* EFFECT_VORTEX_H */
