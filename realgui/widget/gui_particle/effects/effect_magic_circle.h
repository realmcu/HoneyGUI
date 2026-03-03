/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_magic_circle.h
 * @brief Magic Circle Particle Effect - Doctor Strange Style
 *
 * Magic circle effect characteristics:
 * - Particles float upward from ring edges and center
 * - Orange-gold color palette with additive glow
 * - Pure particle effect (no images)
 * - For the full demo with rotating images, use effect_magic_circle_demo_init()
 */

#ifndef EFFECT_MAGIC_CIRCLE_H
#define EFFECT_MAGIC_CIRCLE_H

#include "particle_effect.h"
#include "gui_particle_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create magic circle particle effect
 *
 * Creates orange-gold sparks floating upward from a ring area.
 * Pure particle effect with no image resources. For the full demo
 * with rotating inner/outer circle images, use effect_magic_circle_demo_init().
 *
 * @param parent Parent widget to attach to
 * @param name Widget name
 * @param x X coordinate relative to parent
 * @param y Y coordinate relative to parent
 * @param w Widget width
 * @param h Widget height
 * @return Created widget pointer, NULL on failure
 */
gui_particle_widget_t *effect_magic_circle_create(gui_obj_t *parent, const char *name,
                                                  int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif /* EFFECT_MAGIC_CIRCLE_H */
