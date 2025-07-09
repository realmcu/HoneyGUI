/**
 * @file def_event.h
 * @author wenjing_jiang(wenjing_jiang@realsil.com.cn)
 * @brief
 * @version 0.1
 * @date 2024-06-14
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __DEF_EVENT_H__
#define __DEF_EVENT_H__



#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>


/*============================================================================*
 *                         Types
 *============================================================================*/

/** @brief  ... */

typedef enum
{
    GUI_EVENT_INVALIDE                = 0x0000,
    GUI_EVENT_SYSTEM_RESET,
    GUI_EVENT_SYSTEM_FREE,
    GUI_EVENT_SYSTEM_SLEEP,
    GUI_EVENT_SYSTEM_WAKEUP,
    GUI_EVENT_SYSTEM_REBOOT,
    GUI_EVENT_SYSTEM_POWER_OFF,
    GUI_EVENT_SYSTEM_UPDATE,
    /** touch device events*/
    GUI_EVENT_TOUCH_CLICKED          = 0x0100,
    GUI_EVENT_TOUCH_DOUBLE_CLICKED,
    GUI_EVENT_TOUCH_LONG,
    GUI_EVENT_TOUCH_PRESSED,
    GUI_EVENT_TOUCH_PRESSING, // GUI_EVENT_TOUCH_HOLD,
    GUI_EVENT_TOUCH_RELEASED,

    // GUI_EVENT_TOUCH_MOVE,
    GUI_EVENT_TOUCH_MOVE_LEFT,
    GUI_EVENT_TOUCH_MOVE_RIGHT,
    GUI_EVENT_TOUCH_MOVE_UP,
    GUI_EVENT_TOUCH_MOVE_DOWN,

    GUI_EVENT_TOUCH_SCROLL_HORIZONTAL,
    GUI_EVENT_TOUCH_SCROLL_VERTICAL,

    GUI_EVENT_TOUCH_TOUCH_RIGHT_SLIDE,
    GUI_EVENT_TOUCH_TOUCH_LEFT_SLIDE,


    /** keyboard device events*/
    GUI_EVENT_KB_UP_PRESSED          = 0x0200,
    GUI_EVENT_KB_UP_RELEASED,
    GUI_EVENT_KB_DOWN_PRESSED,
    GUI_EVENT_KB_DOWN_RELEASED,
    GUI_EVENT_KB_SHORT_CLICKED,
    GUI_EVENT_KB_LONG_CLICKED,

    GUI_EVENT_VIEW_SWITCH_DIRECT,
    /** wheel device events*/

} gui_event_t;

typedef void (*gui_event_cb_t)(void *obj, gui_event_t e, void *param);
/**
 * @brief The parameters for a GUI event callback function.
 *
 * This macro defines the parameters that a GUI event callback function should receive.
 * It includes an object pointer, an event type, and an additional parameter.
 *
 * @param obj Pointer to the object associated with the event.
 * @param e The event type (an enumeration or integer representing the event).
 * @param param Additional event-specific data passed to the callback function.
 */
#define GUI_EVENT_CALLBACK_PARAMETER void *obj, gui_event_t e, void *param

/**
 * @brief Callback function definition for GUI events.
 *
 * This macro defines the structure of a GUI event callback function.
 * The function signature includes an object pointer, an event type, and an additional parameter.
 *
 * @param function_name The name of the function to be defined.
 *
 * @note The function defined by this macro must match the specified signature.
 */
#define GUI_EVENT_CALLBACK_FUNCTION_DEFINE(function_name) void function_name(GUI_EVENT_CALLBACK_PARAMETER)
typedef struct _gui_event_dsc_t
{
    gui_event_cb_t event_cb;
    void *user_data;
    gui_event_t filter;
} gui_event_dsc_t;

#ifdef __cplusplus
}
#endif

#endif

