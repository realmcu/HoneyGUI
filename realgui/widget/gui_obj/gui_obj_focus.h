/**
 * @file gui_obj_focus.h
 * @author howie_wang (howie_wang@realtek.com.cn)
 * @brief Focus management for keyboard input
 * @version 0.1
 * @date 2026-01-26
 *
 * @copyright Copyright (c) 2026
 *
 */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_OBJ_FOCUS_H__
#define __GUI_OBJ_FOCUS_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_obj.h"

/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Set keyboard focus to an object
 * @param obj The object to focus, NULL to clear focus
 */
void gui_obj_focus_set(gui_obj_t *obj);

/**
 * @brief Get the currently focused object
 * @return Pointer to focused object, or NULL if no focus
 */
gui_obj_t *gui_obj_focus_get(void);

/**
 * @brief Check if an object has focus
 * @param obj The object to check
 * @return true if focused, false otherwise
 */
bool gui_obj_focus_is_focused(gui_obj_t *obj);

#ifdef __cplusplus
}
#endif

#endif // __GUI_OBJ_FOCUS_H__
