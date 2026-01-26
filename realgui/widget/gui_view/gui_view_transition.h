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
#include "gui_matrix.h"


void gui_view_translation(gui_view_t *_this, int16_t release);

void gui_view_cube(gui_view_t *_this, int16_t release);

void gui_view_rotate(gui_view_t *_this, int16_t release);

void gui_view_reduction(gui_view_t *_this, int16_t release);

void gui_view_blur(gui_view_t *this, int16_t release);

void view_transition_animation(void *obj, float pro);


#ifdef __cplusplus
}
#endif
#endif
