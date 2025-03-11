/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_obj.h
  * @brief  object widget
  * @details create a object
  * @author howie_wang@realsil.com.cn
  * @date 2025/03/07
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
 \endinternal
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
  * @brief create event widget.
  * @param obj pointer to the GUI object.
  * @param event_cb switching events.
  * @param filter how to trigger events.
  * @param user_data data to be transmitted.
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
  * @brief this_widget API only for Widget, not for Application.
  * @param obj Pointer to the GUI object.
  * @param event_code switching events.
  * <b>Example usage</b>
  * \code{.c}
  * static void app_main_task(gui_obj_t *obj)
  * {
  *    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED);
  *
  * }
  * \endcode
  */
void gui_obj_enable_event(gui_obj_t *obj, gui_event_t event_code);

/**
 * @brief this_widget API only for Widget, not for Application.
 * @param obj Pointer to the GUI object.
 */
void gui_obj_event_handle(gui_obj_t *obj);

/**
 * @brief this_widget API only for Widget, not for Application.
 */
void gui_obj_event_dispatch(bool enable_event);

#ifdef __cplusplus
}
#endif

#endif
