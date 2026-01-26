/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Map Module Header
 *============================================================================*/
#ifndef __APP_MAP_H__
#define __APP_MAP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "gui_win.h"

/*============================================================================*
 *                           Variables
 *============================================================================*/
extern gui_win_t *win_map;

/*============================================================================*
 *                           Function Declarations
 *============================================================================*/

/**
 * @brief Design and create map UI
 */
void map_design(void);

/**
 * @brief Exit map with animation
 */
void exit_map(void);

/**
 * @brief Change position indicator color
 */
void change_pos_indicator_color(void);

void app_map_update_navi_icon(const uint8_t *image_data);
#ifdef _HONEYGUI_SIMULATOR_
/**
 * @brief Start navigation test mode (Simulator only)
 *
 * This function simulates navigation data updates for UI testing.
 * It will:
 * - Enable bluetooth and navigation status
 * - Show the map
 * - Simulate navigation data updates every 2 seconds
 * - Update distance from 1500m to 50m, then reset to 2000m
 */
void app_map_test_navigation(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __APP_MAP_H__ */
