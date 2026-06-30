/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __APP_MAIN_DASHBOARD_H__
#define __APP_MAIN_DASHBOARD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "root_image_800_480/ui_resource.h"
#include "gui_view.h"
#include "gui_components_init.h"
#include "dashboard_adapt.h"
#include "gui_view_instance.h"

/*============================================================================*
 *                           Variables
 *============================================================================*/

extern uint8_t scene_flag;
extern gui_win_t *win_map;

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define GUI_WIDGET_TRY_EXCEPT(obj) {GUI_ASSERT(obj != NULL);GUI_ASSERT((GUI_BASE(obj)->magic == GUI_MAGIC_NUMBER))}



/*============================================================================*
 *                           Public Functions
 *============================================================================*/
// Map
void map_design(void);
void exit_map(void);
void change_pos_indicator_color(void);

#ifdef __cplusplus
}
#endif
#endif


