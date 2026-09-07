/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_FB_H__
#define __GUI_FB_H__

#ifdef __cplusplus
extern "C" {
#endif


/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_api.h"
#include "gui_obj.h"
#include "gui_dirty_region.h"
#include "gui_dirty_border.h"


/*============================================================================*
 *                         Types
 *============================================================================*/


/*============================================================================*
 *                         Constants
 *============================================================================*/


/*============================================================================*
 *                         Macros
 *============================================================================*/


/*============================================================================*
 *                         Variables
 *============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief entire update process
 *
 * @param parent the widget tree
 */
void gui_fb_disp(gui_obj_t *root, bool enable_event);


/**
 * @brief this means framebuffer have change, need update framebuffer
 *
 */
void gui_fb_change(void);

void gui_set_bg_color(gui_color_t color);

/**
 * @brief Enable or disable skipping framebuffer clear before rendering.
 *
 * @param skip true to skip framebuffer clear, false to clear normally
 */
void gui_fb_skip_clear(bool skip);


/**
 * @brief Get the theoretical FPS based on CPU render time.
 *
 * @return Theoretical render FPS.
 */
uint32_t gui_fb_fps(void);

/**
 * @brief Get the CPU render time of the latest frame.
 *
 * Tear-effect waiting, display synchronization and LCD update time are excluded.
 *
 * @return CPU render time in milliseconds.
 */
uint32_t gui_fb_render_time_ms(void);

#ifdef __cplusplus
}
#endif

#endif








