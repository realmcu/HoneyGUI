/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_galaxy.h
 * @brief Galaxy Spiral Particle Effect
 *
 * Galaxy effect characteristics:
 * - Spiral arm structure with logarithmic distribution
 * - Particles orbit and slowly drift toward center
 * - Blue/purple outer color transitioning to white/pink core
 * - Rotating spiral pattern
 * - Creates a spinning galaxy visual effect
 */

#ifndef EFFECT_GALAXY_H
#define EFFECT_GALAXY_H

#include "particle_effect.h"
#include "gui_particle_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Load galaxy effect configuration
 * @param config Pointer to configuration to fill
 */
void effect_galaxy_config(particle_effect_config_t *config);

/**
 * @brief Initialize galaxy demo
 * @return Created widget pointer, NULL on failure
 */
gui_particle_widget_t *effect_galaxy_demo_init(void);

#ifdef __cplusplus
}
#endif

#endif /* EFFECT_GALAXY_H */
