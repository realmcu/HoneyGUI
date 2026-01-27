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

/** User update callback type */
typedef void (*gui_particle_update_cb_t)(void *user_data);

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

/* ============================================================================
 * Convenience Interfaces for Preset Effects
 * ============================================================================ */

/**
 * @brief Add a firework effect
 *
 * Creates and triggers a firework explosion at the specified position.
 *
 * @param widget Particle widget
 * @param x X coordinate of explosion center
 * @param y Y coordinate of explosion center
 * @param count Number of particles in burst (20-100 recommended)
 */
void gui_particle_widget_add_firework(gui_particle_widget_t *widget,
                                      float x, float y, uint8_t count);

/**
 * @brief Add a firework effect with custom color
 *
 * Creates and triggers a firework explosion at the specified position with custom color.
 *
 * @param widget Particle widget
 * @param x X coordinate of explosion center
 * @param y Y coordinate of explosion center
 * @param count Number of particles in burst (20-100 recommended)
 * @param color Color in ARGB8888 format (0xAARRGGBB), e.g., 0xFFFF0000 for red
 */
void gui_particle_widget_add_firework_color(gui_particle_widget_t *widget,
                                            float x, float y, uint8_t count, uint32_t color);

/**
 * @brief Add a trail effect
 *
 * Enables pointer/cursor trail effect. Call gui_particle_widget_update_trail()
 * to update the trail position.
 *
 * @param widget Particle widget
 */
void gui_particle_widget_add_trail(gui_particle_widget_t *widget);

/**
 * @brief Add a trail effect with custom color
 *
 * Enables pointer/cursor trail effect with custom color.
 *
 * @param widget Particle widget
 * @param color Color in ARGB8888 format (0xAARRGGBB)
 */
void gui_particle_widget_add_trail_color(gui_particle_widget_t *widget, uint32_t color);

/**
 * @brief Update trail effect position
 *
 * Updates the trail emitter with new position. Should be called when
 * pointer/cursor moves.
 *
 * @param widget Particle widget
 * @param x Current X position
 * @param y Current Y position
 */
void gui_particle_widget_update_trail(gui_particle_widget_t *widget, float x, float y);

/**
 * @brief Stop trail effect
 *
 * Stops the trail emitter and resets tracking state. Should be called when
 * pointer/cursor is released.
 *
 * @param widget Particle widget
 */
void gui_particle_widget_stop_trail(gui_particle_widget_t *widget);

/**
 * @brief Add a touch effect
 *
 * Enables touch interaction feedback effect.
 *
 * @param widget Particle widget
 */
void gui_particle_widget_add_touch(gui_particle_widget_t *widget);

/**
 * @brief Add a touch effect with custom color
 *
 * Enables touch interaction feedback effect with custom color.
 *
 * @param widget Particle widget
 * @param color Color in ARGB8888 format (0xAARRGGBB)
 */
void gui_particle_widget_add_touch_color(gui_particle_widget_t *widget, uint32_t color);

/**
 * @brief Trigger touch tap effect
 *
 * @param widget Particle widget
 * @param x Touch X coordinate
 * @param y Touch Y coordinate
 */
void gui_particle_widget_touch_tap(gui_particle_widget_t *widget, float x, float y);

/**
 * @brief Update touch drag effect
 *
 * @param widget Particle widget
 * @param x Current drag X coordinate
 * @param y Current drag Y coordinate
 */
void gui_particle_widget_touch_drag(gui_particle_widget_t *widget, float x, float y);

/**
 * @brief Trigger touch release effect
 *
 * @param widget Particle widget
 * @param x Release X coordinate
 * @param y Release Y coordinate
 */
void gui_particle_widget_touch_release(gui_particle_widget_t *widget, float x, float y);

/**
 * @brief Add a snow effect
 *
 * Enables falling snowflake effect across the widget area.
 *
 * @param widget Particle widget
 * @param intensity Snow intensity (0.0-1.0)
 */
void gui_particle_widget_add_snow(gui_particle_widget_t *widget, float intensity);

/**
 * @brief Add a snow effect with custom color
 *
 * Enables falling snowflake effect with custom color.
 *
 * @param widget Particle widget
 * @param intensity Snow intensity (0.0-1.0)
 * @param color Color in ARGB8888 format (0xAARRGGBB)
 */
void gui_particle_widget_add_snow_color(gui_particle_widget_t *widget, float intensity,
                                        uint32_t color);

/**
 * @brief Set snow effect intensity
 *
 * @param widget Particle widget
 * @param intensity New intensity value (0.0-1.0)
 */
void gui_particle_widget_set_snow_intensity(gui_particle_widget_t *widget, float intensity);

/**
 * @brief Add a bubble effect
 *
 * Enables rising bubble effect from the bottom of the widget.
 *
 * @param widget Particle widget
 */
void gui_particle_widget_add_bubble(gui_particle_widget_t *widget);

/**
 * @brief Add a bubble effect with custom color
 *
 * Enables rising bubble effect with custom color.
 *
 * @param widget Particle widget
 * @param color Color in ARGB8888 format (0xAARRGGBB)
 */
void gui_particle_widget_add_bubble_color(gui_particle_widget_t *widget, uint32_t color);

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

#ifdef __cplusplus
}
#endif

#endif /* GUI_PARTICLE_WIDGET_H */
