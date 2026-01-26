/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_API_DC_H__
#define __GUI_API_DC_H__
#ifdef __cplusplus
extern "C" {
#endif


/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "guidef.h"

/*============================================================================*
 *                         Macros
 *============================================================================*/



/*============================================================================*
 *                         Functions
 *============================================================================*/

void gui_dc_info_register(struct gui_dispdev *info);

struct gui_dispdev *gui_get_dc(void);

uint32_t gui_get_screen_width(void);

uint32_t gui_get_screen_height(void);

void gui_display_on(void);
void gui_display_off(void);

int gui_flash_boost(void);
int gui_flash_boost_disable(void);

#ifdef __cplusplus
}
#endif
#endif
