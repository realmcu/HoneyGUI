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

extern bool g_SnapShotCacheNeedUpdate;
extern bool g_SnapShotPreCache; // whether cache all layout views or not

/* GUI view transition */
void gui_view_transition(gui_view_t *_this, int16_t release);

/* GUI view animation */
void gui_view_animation(gui_view_t *_this, int16_t release);

/* GUI view snapshot */
void gui_view_show_snapshot(gui_view_t *_this, bool show);
void gui_view_create_snapshot(gui_view_t *_this);
void gui_view_cache_snapshot_data(gui_view_t *_this);
void gui_view_save_common_data(gui_view_t *old_view, gui_view_t *new_view);
void gui_view_update_snapshot(void *msg);

#ifdef __cplusplus
}
#endif
#endif
