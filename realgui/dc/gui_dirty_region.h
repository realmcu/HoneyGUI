/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __GUI_DIRTY_REGION_H__
#define __GUI_DIRTY_REGION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "def_type.h"

/*============================================================================*
 *                         Constants
 *============================================================================*/

/** Maximum number of dirty regions */
#define GUI_MAX_DIRTY_REGIONS 16

/*============================================================================*
 *                         Types
 *============================================================================*/

/**
 * @brief Dirty region manager structure
 */
typedef struct
{
    gui_rect_t regions[GUI_MAX_DIRTY_REGIONS];  ///< Array of dirty rectangles
    uint16_t count;                              ///< Current number of dirty regions
    bool full_refresh;                           ///< Whether full screen refresh is needed
} gui_dirty_region_manager_t;

/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Enable or disable dirty region optimization
 *
 * When disabled, forces full screen refresh every frame.
 * When enabled, only dirty regions are re-rendered.
 *
 * @param enable true to enable dirty region optimization, false to force full refresh
 */
void gui_dirty_region_enable(bool enable);

/**
 * @brief Check if dirty region optimization is enabled
 *
 * @return true if enabled, false if disabled (full refresh mode)
 */
bool gui_dirty_region_is_enabled(void);

/**
 * @brief Mark a rectangular area as dirty (needs redraw)
 *
 * @param rect Rectangle area to redraw (in screen coordinates)
 * @note Automatically clips to screen bounds
 * @note Overlapping or adjacent rectangles are automatically merged
 * @note Falls back to full screen refresh when exceeding max dirty regions
 */
void gui_dirty_set_region(gui_rect_t *rect);

/**
 * @brief Get the dirty region manager instance
 *
 * @return Pointer to dirty region manager
 */
gui_dirty_region_manager_t *gui_dirty_region_get_manager(void);

/**
 * @brief Clear all dirty regions (typically called at frame end)
 */
void gui_dirty_region_clear(void);

#ifdef __cplusplus
}
#endif

#endif /* __GUI_DIRTY_REGION_H__ */
