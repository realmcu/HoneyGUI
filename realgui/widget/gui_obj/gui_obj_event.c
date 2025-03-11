/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_obj_event.c
  * @brief  object event handling
  * @details event related functions and structures
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
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include <stdio.h>
#include "gui_obj.h"
#include "gui_obj_event.h"
#include "gui_api.h"

/*============================================================================*
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                           Constants
 *============================================================================*/


/*============================================================================*
 *                            Macros
 *============================================================================*/

#define MAX_EVENT_CNT   10

/*============================================================================*
 *                            Variables
 *============================================================================*/

static gui_event_cb_t event_cb[MAX_EVENT_CNT];
static gui_event_t event_code[MAX_EVENT_CNT];
static void *event_cb_param[MAX_EVENT_CNT];
static void *event_obj[MAX_EVENT_CNT];
static uint8_t event_cnt = 0;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/


/*============================================================================*
*                           Public Functions
*============================================================================*/

void gui_obj_add_event_cb(void           *obj,
                          gui_event_cb_t  event_cb,
                          gui_event_t     filter,
                          void           *user_data)
{
    gui_obj_t *object = (gui_obj_t *)obj;

    object->event_dsc_cnt++;
    object->event_dsc = gui_realloc(object->event_dsc, sizeof(gui_event_dsc_t) * object->event_dsc_cnt);

    gui_event_dsc_t *event_dsc = object->event_dsc + object->event_dsc_cnt - 1;
    event_dsc->event_cb = event_cb;
    event_dsc->filter = filter;
    event_dsc->user_data = user_data;
    event_dsc->event_code = GUI_EVENT_INVALIDE;
}

void gui_obj_enable_event(gui_obj_t *obj, gui_event_t event_code)
{
    if (!obj->gesture)
    {
        for (uint8_t i = 0; i < obj->event_dsc_cnt; i++)
        {
            gui_event_dsc_t *event_dsc = obj->event_dsc + i;
            event_dsc->event_code = event_code;
        }
    }
}

void gui_obj_event_handle(gui_obj_t *obj)
{
    for (uint32_t i = 0; i < obj->event_dsc_cnt; i++)
    {
        gui_event_dsc_t *event_dsc = obj->event_dsc + i;
        if (event_dsc->filter == event_dsc->event_code)
        {
            event_cb[event_cnt] = event_dsc->event_cb;
            event_code[event_cnt] = event_dsc->event_code;
            event_cb_param[event_cnt] = event_dsc->user_data;
            event_obj[event_cnt] = obj;
            event_dsc->event_code = GUI_EVENT_INVALIDE;
            event_cnt++;
        }
    }
}

void gui_obj_event_dispatch(bool enable_event)
{
    if (enable_event == true)
    {
        for (uint32_t i = 0; i < event_cnt; i++)
        {
            event_cb[i](event_obj[i], event_code[i], event_cb_param[i]);
        }
    }
    event_cnt = 0;
}
// #endif