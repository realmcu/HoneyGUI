/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      hub_task.h
   * @brief     Routines to create App task and handle events & messages
   * @author    howie
   * @date      2019-12-26
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */
#ifndef _GUI_TASK_H_
#define _GUI_TASK_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "stdbool.h"
#include "wristband_gui.h"



/**  @brief IO message definition for communications between tasks*/
typedef struct
{
    uint16_t type;
    uint16_t subtype;
    uint32_t ts;//system tick
    union
    {
        uint32_t  param;
        void     *buf;
        RtkTouchRawData_t touch_raw;
    } u;
} T_RTK_GUI_EVENT;

/**  @brief IO subtype definitions for @ref UI_MSG_LCD type */
typedef enum
{
    GUI_EVENT_NEW_VIEW,
    GUI_EVENT_FRAME_SYNC,
    GUI_EVENT_TIMER,
    GUI_EVENT_TOUCH,
    GUI_EVENT_BUTTON,
    GUI_EVENT_SET_CB,
    GUI_AUTO_TEST,
} T_GUI_EVENT_TYPE;

/**  @brief IO subtype definitions for @ref GUI_EVENT_TIMER type */
typedef enum
{
    GUI_NEW_TODO_0,
    GUI_NEW_TODO_1,
} T_GUI_EVENT_NEW_VIEW;

/**  @brief IO subtype definitions for @ref GUI_EVENT_TIMER type */
typedef enum
{
    GUI_TOUCH_INT,
    GUI_TOUCH_TIMEOUT,
} T_GUI_EVENT_TOUCH;

/**  @brief IO subtype definitions for @ref GUI_EVENT_TIMER type */
typedef enum
{
    GUI_TIMER_DISPLAY_CB,
    GUI_TIMER_DYNAMIC_CB,
    GUI_TIMER_SLIDE_CB,
    GUI_TIMER_ROLL_CB,

    GUI_TIMER_DISPLAY_START,
    GUI_TIMER_DYNAMIC_START,
    GUI_TIMER_SLIDE_START,
    GUI_TIMER_ROLL_START,

    GUI_TIMER_DISPLAY_STOP,
    GUI_TIMER_DYNAMIC_STOP,
    GUI_TIMER_SLIDE_STOP,
} T_GUI_EVENT_TIMER;

/**  @brief IO subtype definitions for @ref GUI_EVENT_TIMER type */
typedef enum
{
    GUI_SET_DISPLAY_CB,
    GUI_SET_DYNAMIC_CB,
    GUI_SET_DISPLAY_INFO_CB,
    GUI_SET_MENU_INIT_CB,
} T_GUI_EVENT_SET_CB;

/**  @brief IO subtype definitions for @ref GUI_EVENT_TIMER type */
typedef enum
{
    GUI_BUTTON_PRESS_0,
    GUI_BUTTON_RELEASE_0,
    GUI_BUTTON_SHORT_0,
    GUI_BUTTON_LONG_0,
    GUI_BUTTON_PRESS_1,
    GUI_BUTTON_RELEASE_1,
    GUI_BUTTON_SHORT_1,
    GUI_BUTTON_LONG_1,
} T_GUI_EVENT_BUTTON;


#define GUI_TASK_DISPLAY_TIMER_INTERVAL     5000
#define GUI_TASK_DYNAMIC_TIMER_INTERVAL     500
#define GUI_TASK_SLIDE_TIMER_INTERVAL     40

int gui_task_init(void);
void gui_task_handle_event(T_RTK_GUI_EVENT *event);
bool post_event_to_gui_task(T_RTK_GUI_EVENT *p_msg, uint32_t line);
UI_MenuTypeDef *gui_get_menu_addr_by_name(const char *name);

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */
#endif

