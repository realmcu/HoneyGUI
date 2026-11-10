/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_VIEW_TRANSITON_H__
#define __GUI_VIEW_TRANSITON_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_fb.h"
#include "gui_obj.h"

/* GUI view transition */
void gui_view_transition(gui_view_t *_this, int16_t release);

#ifdef __cplusplus
}
#endif
#endif
