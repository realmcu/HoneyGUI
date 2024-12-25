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
    /** Input device events*/
    GUI_EVENT_TOUCH_PRESSED           = 0x0001,
    GUI_EVENT_TOUCH_PRESSING          = 0x0002,
    GUI_EVENT_TOUCH_CLICKED           = 0x0003,
    GUI_EVENT_TOUCH_LONG              = 0x0004,
    GUI_EVENT_TOUCH_RELEASED          = 0x0005,
    GUI_EVENT_TOUCH_TOUCH_RIGHT_SLIDE = 0x0006,
    GUI_EVENT_TOUCH_TOUCH_LEFT_SLIDE  = 0x0007,

    GUI_EVENT_KB_UP_PRESSED           = 0x0008,
    GUI_EVENT_KB_UP_RELEASED          = 0x0009,
    GUI_EVENT_KB_DOWN_PRESSED         = 0x000A,
    GUI_EVENT_KB_DOWN_RELEASED        = 0x000B,

    GUI_EVENT_KB_SHORT_CLICKED        = 0x000C,
    GUI_EVENT_KB_LONG_CLICKED         = 0x000D,
    /** Special events*/
    /** Other events*/
    GUI_EVENT_1                       = 0x000E,
    GUI_EVENT_2                       = 0x000F,
    GUI_EVENT_3                       = 0x0010,
    GUI_EVENT_4                       = 0x0011,
    GUI_EVENT_5                       = 0x0012,
    GUI_EVENT_6                       = 0x0013,
    GUI_EVENT_7                       = 0x0014,
    GUI_EVENT_8                       = 0x0015,


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
    gui_event_t event_code;
} gui_event_dsc_t;

#ifdef __cplusplus
}
#endif

#endif

