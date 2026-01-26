/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_OBJ_SUPPRESS_H__
#define __GUI_OBJ_SUPPRESS_H__



#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"


/*============================================================================*
 *                         Types
 *============================================================================*/


/*============================================================================*
*                         Macros
*============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Set event suppression for an object.
 * @param object Pointer to the GUI object.
 * @param filter Event filter to suppress.
 */
void gui_obj_suppress_set(gui_obj_t *object, gui_event_t filter);

/**
 * @brief Check if an event is suppressed for an object.
 * @param object Pointer to the GUI object.
 * @param event Event to check.
 * @return True if event is suppressed, false otherwise.
 */
bool gui_obj_event_is_suppressed(gui_obj_t *object, gui_event_t event);


#ifdef __cplusplus
}
#endif

#endif
