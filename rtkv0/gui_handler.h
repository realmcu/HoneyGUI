/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file
   * @brief
   * @author    howie
   * @date      2019-12-26
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */
#ifndef _GUI_HANDLER_H_
#define _GUI_HANDLER_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "stdbool.h"
#include "gui_task.h"
#include "gui_core.h"

typedef void (* gui_cb)(UI_MenuTypeDef *cur_menu, UI_MenuTypeDef *return_menu, void *argv);

void gui_event_new_view_handler(T_RTK_GUI_EVENT *msg);
void gui_event_frame_sync_handler(T_RTK_GUI_EVENT *msg);
void gui_event_timer_handler(T_RTK_GUI_EVENT *msg);
void gui_event_touch_handler(T_RTK_GUI_EVENT *msg);
void gui_event_button_handler(T_RTK_GUI_EVENT *msg);
void gui_event_set_cb(T_RTK_GUI_EVENT *msg);

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */
#endif

