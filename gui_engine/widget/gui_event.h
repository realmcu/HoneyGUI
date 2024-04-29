/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file
  * @brief
  * @details
  * @author
  * @date
  * @version
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
 \endinternal
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_EVENT_H__
#define __GUI_EVENT_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/** @defgroup WIDGET WIDGET
  * @brief
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup WIDGET_Exported_Types WIDGET Exported Types
  * @brief
  * @{
  */

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

    GUI_EVENT_DISPLAY_ON              = 0x0016,
    GUI_EVENT_DISPLAY_OFF             = 0x0017,
    GUI_EVENT_EXTERN_IO_JS            = 0x0018,  // extern io event: route to js middle layer

    GUI_EVENT_USER_DEFINE             = 0x0100,
} gui_event_t;

typedef void (*gui_event_cb_t)(void *obj, gui_event_t e);

typedef struct _gui_event_dsc_t
{
    gui_event_cb_t event_cb;
    void *user_data;
    gui_event_t filter;
    gui_event_t event_code;
} gui_event_dsc_t;

/** End of WIDGET_Exported_Types
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif
