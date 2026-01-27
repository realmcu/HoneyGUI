/**
 * @file particle_effect.h
 * @brief Preset Particle Effects - Pre-configured effect templates
 *
 * This module provides preset particle effects including:
 * - Firework: Explosive burst effect with gravity and fade
 * - Trail: Pointer/cursor trail effect
 * - Touch: Touch interaction feedback effect
 * - Snow: Falling snowflake effect
 * - Bubble: Rising bubble effect
 *
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PARTICLE_EFFECT_H
#define PARTICLE_EFFECT_H

#include "particle_system.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * Firework Effect (Requirements 5.1-5.5)
 * ============================================================================ */

/**
 * @brief Create a firework effect emitter
 *
 * Creates an emitter configured for firework explosion effect:
 * - Spherical burst pattern with random velocity variation
 * - Gravity and damping for realistic falling
 * - Opacity fade from 100% to 0% over lifetime
 *
 * @param config Platform configuration with memory functions
 * @param x X coordinate of explosion center
 * @param y Y coordinate of explosion center
 * @param particle_count Number of particles in burst (20-100 recommended)
 * @return Pointer to created emitter, or NULL on failure
 */
particle_emitter_t *effect_firework_create(particle_platform_config_t *config,
                                           float x, float y, uint8_t particle_count);

/* ============================================================================
 * Trail Effect (Requirements 6.1-6.5)
 * ============================================================================ */

/**
 * @brief Create a pointer trail effect emitter
 *
 * Creates an emitter configured for pointer/cursor trail effect:
 * - Emission rate proportional to movement speed
 * - Initial velocity opposite to movement direction
 * - Damping for smooth deceleration
 * - Scale fade from 100% to 0% over lifetime
 *
 * @param config Platform configuration with memory functions
 * @return Pointer to created emitter, or NULL on failure
 */
particle_emitter_t *effect_trail_create(particle_platform_config_t *config);

/**
 * @brief Update trail effect with new pointer position
 *
 * Updates the trail emitter based on pointer movement.
 * Calculates velocity direction opposite to movement.
 *
 * @param emitter Trail emitter to update
 * @param x Current X position
 * @param y Current Y position
 * @param prev_x Previous X position
 * @param prev_y Previous Y position
 */
void effect_trail_update(particle_emitter_t *emitter, float x, float y,
                         float prev_x, float prev_y);

/* ============================================================================
 * Touch Effect (Requirements 7.1-7.5)
 * ============================================================================ */

/**
 * @brief Create a touch effect emitter
 *
 * Creates an emitter configured for touch interaction feedback:
 * - Circular burst pattern around touch point
 * - Configurable expansion speed
 * - Supports tap, drag, and release events
 *
 * @param config Platform configuration with memory functions
 * @return Pointer to created emitter, or NULL on failure
 */
particle_emitter_t *effect_touch_create(particle_platform_config_t *config);

/**
 * @brief Trigger touch tap effect
 *
 * Emits a burst of 5-20 particles at the touch location.
 *
 * @param emitter Touch emitter
 * @param pool Particle pool to allocate from
 * @param x Touch X coordinate
 * @param y Touch Y coordinate
 */
void effect_touch_tap(particle_emitter_t *emitter, particle_pool_t *pool,
                      float x, float y);

/**
 * @brief Update touch drag effect
 *
 * Continuously emits particles along the drag path.
 *
 * @param emitter Touch emitter
 * @param x Current drag X coordinate
 * @param y Current drag Y coordinate
 */
void effect_touch_drag(particle_emitter_t *emitter, float x, float y);

/**
 * @brief Trigger touch release effect
 *
 * Emits a final burst with increased initial velocity.
 *
 * @param emitter Touch emitter
 * @param pool Particle pool to allocate from
 * @param x Release X coordinate
 * @param y Release Y coordinate
 */
void effect_touch_release(particle_emitter_t *emitter, particle_pool_t *pool,
                          float x, float y);

/* ============================================================================
 * Snow Effect (Requirements 8.1-8.5)
 * ============================================================================ */

/**
 * @brief Create a snow effect emitter
 *
 * Creates an emitter configured for falling snowflake effect:
 * - Emission from top edge of screen
 * - Light gravity with horizontal sway
 * - Slow rotation for tumbling motion
 * - Particles deactivate at bottom edge
 *
 * @param config Platform configuration with memory functions
 * @param screen_width Width of the screen for emission line
 * @return Pointer to created emitter, or NULL on failure
 */
particle_emitter_t *effect_snow_create(particle_platform_config_t *config,
                                       float screen_width);

/**
 * @brief Set snow effect intensity
 *
 * Adjusts emission rate and particle size based on intensity.
 *
 * @param emitter Snow emitter
 * @param intensity Intensity value (0.0-1.0)
 */
void effect_snow_set_intensity(particle_emitter_t *emitter, float intensity);

/* ============================================================================
 * Bubble Effect (Requirements 9.1-9.5)
 * ============================================================================ */

/**
 * @brief Create a bubble effect emitter
 *
 * Creates an emitter configured for rising bubble effect:
 * - Emission from bottom edge of screen
 * - Negative gravity (upward motion)
 * - Horizontal sway motion
 * - Subtle size pulsation
 * - Velocity reflection at horizontal boundaries
 *
 * @param config Platform configuration with memory functions
 * @param screen_width Width of the screen for emission line
 * @param screen_height Height of the screen for boundary
 * @return Pointer to created emitter, or NULL on failure
 */
particle_emitter_t *effect_bubble_create(particle_platform_config_t *config,
                                         float screen_width, float screen_height);

#ifdef __cplusplus
}
#endif

#endif /* PARTICLE_EFFECT_H */
