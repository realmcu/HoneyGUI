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
 * @brief Create lightning arc particle effect
 *
 * Creates periodic lightning strikes from top-center (10% h)
 * to bottom-center (90% h) with branching bolts.
 *
 * @param parent Parent widget to attach to
 * @param name Widget name
 * @param x X coordinate relative to parent
 * @param y Y coordinate relative to parent
 * @param w Widget width
 * @param h Widget height
 * @return Created widget pointer, NULL on failure
 */
gui_particle_widget_t *effect_lightning_create(gui_obj_t *parent, const char *name,
                                               int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif /* EFFECT_LIGHTNING_H */
