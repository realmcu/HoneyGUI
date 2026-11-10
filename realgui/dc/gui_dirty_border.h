/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef GUI_DIRTY_BORDER_H
#define GUI_DIRTY_BORDER_H

#include "guidef.h"
#include "gui_dirty_region.h"

/**
 * @brief Enable or disable dirty region debug borders
 *
 * When enabled, red borders are drawn around dirty regions.
 * When transitioning from enabled to disabled, a cleanup frame
 * automatically erases the last borders.
 *
 * @param enable true to enable, false to disable
 */
void gui_dirty_border_enable(bool enable);

/**
 * @brief Check if dirty region debug borders are enabled
 *
 * @return true if enabled, false if disabled
 */
bool gui_dirty_border_is_enabled(void);

/**
 * @brief Draw red border around a dirty region in the framebuffer
 */
void draw_dirty_border(gui_dispdev_t *dc, gui_rect_t *rect);

/**
 * @brief Prepare dirty border cleanup for current frame.
 *
 * Appends previous frame's dirty regions to the current region list as
 * "cleanup regions" so they get re-rendered (overwriting old borders).
 * Returns the number of REAL dirty regions (excluding cleanup).
 * Use this count when drawing borders to avoid drawing on cleanup regions.
 *
 * @param mgr Dirty region manager
 * @return Number of real dirty regions (before cleanup regions were appended)
 */
uint16_t dirty_border_prepare(gui_dirty_region_manager_t *mgr);

#endif /* GUI_DIRTY_BORDER_H */
