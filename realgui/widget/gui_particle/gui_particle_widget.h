/**
 * @file gui_particle_widget.h
 * @brief Particle System Widget Adapter for HoneyGUI
 *
 * This module provides the HoneyGUI widget wrapper for the particle system.
 * It bridges the independent particle system core with HoneyGUI's widget system.
 *
 * Features:
 * - Inherits from gui_obj_t for seamless integration
 * - Implements prepare/draw/end callbacks
 * - Provides render callback that uses gui_img for particle rendering
 * - Convenience interfaces for adding preset effects
 *
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GUI_PARTICLE_WIDGET_H
#define GUI_PARTICLE_WIDGET_H

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_obj.h"
#include "particle_system.h"
#include "particle_effect.h"

/*============================================================================*
 *                         Types
 *============================================================================*/

/** Effect handle type for managing effects */
typedef uint32_t particle_effect_handle_t;

/** Invalid effect handle constant */
#define PARTICLE_INVALID_HANDLE 0

/** Maximum number of effects per widget */
#define PARTICLE_WIDGET_MAX_EFFECTS 16

/** User update callback type */
typedef void (*gui_particle_update_cb_t)(void *user_data);

/**
 * @brief Effect entry structure for tracking effects
 */
typedef struct particle_effect_entry
{
    particle_effect_handle_t handle;    /**< Effect handle */
    particle_emitter_t *emitter;        /**< Associated emitter */
    particle_behavior_mode_t behavior;  /**< Behavior mode */
    uint8_t active;                     /**< Whether effect is active */
} particle_effect_entry_t;

/**
 * @brief Particle widget structure
 *
 * Extends gui_obj_t to integrate particle system with HoneyGUI.
 */
typedef struct gui_particle_widget
{
    gui_obj_t base;                    /**< Base object structure (must be first) */
    particle_system_t *ps;             /**< Particle system instance */
    particle_platform_config_t config; /**< Platform configuration */

    /* Effect management */
    particle_effect_entry_t effects[PARTICLE_WIDGET_MAX_EFFECTS]; /**< Effect entries */
    uint32_t next_handle;              /**< Next handle to assign */

    /* Trail effect state */
    particle_emitter_t *trail_emitter; /**< Trail effect emitter (if added) */
    float prev_x;                      /**< Previous X position for trail */
    float prev_y;                      /**< Previous Y position for trail */
    uint8_t trail_active;              /**< Trail tracking active flag */

    /* Touch effect state */
    particle_emitter_t *touch_emitter; /**< Touch effect emitter (if added) */
    uint8_t touch_active;              /**< Touch tracking active flag */

    /* Default particle image */
    void *default_image;               /**< Default particle image resource */

    /* User update callback */
    gui_particle_update_cb_t update_cb; /**< User update callback */
    void *update_cb_data;               /**< User data for update callback */
} gui_particle_widget_t;

/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Create a particle system widget
 *
 * Creates a new particle widget and initializes the underlying particle system.
 *
 * @param parent Parent widget to attach to
 * @param name Widget name
 * @param x X coordinate relative to parent
 * @param y Y coordinate relative to parent
 * @param w Widget width
 * @param h Widget height
 * @param pool_capacity Maximum number of particles in the pool
 * @return Pointer to created widget, or NULL on failure
 */
gui_particle_widget_t *gui_particle_widget_create(gui_obj_t *parent, const char *name,
                                                  int16_t x, int16_t y, int16_t w, int16_t h,
                                                  uint16_t pool_capacity);

/**
 * @brief Get the particle system instance
 *
 * Returns the underlying particle system for direct manipulation.
 *
 * @param widget Particle widget
 * @return Pointer to particle system, or NULL if widget is invalid
 */
particle_system_t *gui_particle_widget_get_system(gui_particle_widget_t *widget);

/**
 * @brief Set default particle image
 *
 * Sets the default image used for particles that don't have a specific image.
 *
 * @param widget Particle widget
 * @param image Pointer to image data (HoneyGUI image format)
 */
void gui_particle_widget_set_default_image(gui_particle_widget_t *widget, void *image);

/**
 * @brief Clear all particles
 *
 * Removes all active particles from the system.
 *
 * @param widget Particle widget
 */
void gui_particle_widget_clear(gui_particle_widget_t *widget);

/**
 * @brief Get active particle count
 *
 * @param widget Particle widget
 * @return Number of active particles
 */
uint16_t gui_particle_widget_get_active_count(gui_particle_widget_t *widget);

/**
 * @brief Set user update callback
 *
 * Sets a callback function that will be called every frame during the
 * prepare phase. This can be used to trigger periodic effects like
 * auto-firing fireworks.
 *
 * @param widget Particle widget
 * @param cb Callback function
 * @param user_data User data passed to callback
 */
void gui_particle_widget_set_update_cb(gui_particle_widget_t *widget,
                                       gui_particle_update_cb_t cb,
                                       void *user_data);

/* ============================================================================
 * Configuration-Driven Effect API (Requirements 14.1-14.5)
 * ============================================================================ */

/**
 * @brief Add an effect from configuration
 *
 * Creates an emitter from the provided configuration and adds it to the
 * particle system. Returns a handle for subsequent operations.
 *
 * The function automatically sets up touch event handling based on the
 * behavior mode in the configuration.
 *
 * @param widget Particle widget
 * @param config Effect configuration
 * @return Effect handle, or PARTICLE_INVALID_HANDLE on failure
 *
 * @note Requirements: 14.1, 14.2, 14.4
 */
particle_effect_handle_t gui_particle_widget_add_effect(
    gui_particle_widget_t *widget,
    const particle_effect_config_t *config);

/**
 * @brief Remove an effect by handle
 *
 * Removes the effect associated with the given handle from the widget.
 * The emitter is destroyed and all associated particles will expire naturally.
 *
 * @param widget Particle widget
 * @param handle Effect handle returned by gui_particle_widget_add_effect()
 *
 * @note Requirements: 14.3
 */
void gui_particle_widget_remove_effect(gui_particle_widget_t *widget,
                                       particle_effect_handle_t handle);

/**
 * @brief Trigger a burst for an effect
 *
 * Triggers an immediate burst of particles for the specified effect.
 * Uses the burst_count from the effect's configuration.
 *
 * @param widget Particle widget
 * @param handle Effect handle
 *
 * @note Requirements: 14.5
 */
void gui_particle_widget_trigger_burst(gui_particle_widget_t *widget,
                                       particle_effect_handle_t handle);

/**
 * @brief Update effect position
 *
 * Updates the emitter position for effects that support position following
 * (FOLLOW_TOUCH, TRAIL, TOUCH_FEEDBACK behavior modes).
 *
 * @param widget Particle widget
 * @param handle Effect handle
 * @param x New X position
 * @param y New Y position
 *
 * @note Requirements: 14.5
 */
void gui_particle_widget_update_position(gui_particle_widget_t *widget,
                                         particle_effect_handle_t handle,
                                         float x, float y);

/**
 * @brief Stop an effect (disable emitter)
 *
 * Disables the emitter for the specified effect, stopping particle emission.
 * Existing particles will continue to animate until they expire.
 * The effect can be restarted by calling gui_particle_widget_update_position().
 *
 * @param widget Particle widget
 * @param handle Effect handle
 *
 * @note Requirements: 14.5
 */
void gui_particle_widget_stop_effect(gui_particle_widget_t *widget,
                                     particle_effect_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif /* GUI_PARTICLE_WIDGET_H */
