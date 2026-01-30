/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_OBJ_EVENT_H__
#define __GUI_OBJ_EVENT_H__



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
  * @brief Create event widget.
  * @param obj Pointer to the GUI object.
  * @param event_cb Switching events.
  * @param filter How to trigger events.
  * @param user_data Data to be transmitted.
  * <b>Example usage</b>
  * \code{.c}
  * static void app_main_task(void *parent)
  * {
  *    gui_win_t *clock = gui_win_create(parent, "clock", 0, 84, 320, 300);
  *    gui_obj_add_event_cb(clock, (gui_event_cb_t)show_clock, GUI_EVENT_TOUCH_CLICKED, NULL);
  *
  * }
  * \endcode
  */
void gui_obj_add_event_cb(void           *obj,
                          gui_event_cb_t  event_cb,
                          gui_event_t     filter,
                          void           *user_data);

/**
  * @brief This API only for Widget, not for Application.
  * @param obj Pointer to the GUI object.
  * @param event_code Switching events.
  * <b>Example usage</b>
  * \code{.c}
  * static void app_main_task(gui_obj_t *obj)
  * {
  *    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED);
  *
  * }
  * \endcode
  */
void gui_obj_enable_event(gui_obj_t *obj, gui_event_t event_code, const void *indev_name);

/**
 * @brief This API only for Widget, not for Application.
 * @param enable_event Enable or disable event dispatch.
 */
void gui_obj_event_dispatch(bool enable_event);

#ifdef __cplusplus
}
#endif

#endif
