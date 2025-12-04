/**
 \internal
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_obj_timer.c
  * @brief  object animation
  * @details animation related functions and structures
  * @author howie_wang@realsil.com.cn
  * @date 2025/03/07
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
 \endinternal
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include <stdio.h>
#include "gui_obj.h"
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


/*============================================================================*
 *                            Variables
 *============================================================================*/


/*============================================================================*
 *                           Private Functions
 *============================================================================*/


/*============================================================================*
*                           Public Functions
*============================================================================*/

void gui_obj_create_timer(gui_obj_t *obj, uint32_t interval, bool reload, void (*callback)(void *))
{
    if (obj->timer == NULL)
    {
        obj->timer = gui_malloc(sizeof(gui_obj_timer_t));
        GUI_ASSERT(obj->timer != NULL);
        memset(obj->timer, 0x00, sizeof(gui_obj_timer_t));
    }

    obj->timer->interval_ms = interval;
    obj->timer->reload = reload;
    obj->timer->p_timer_callback = callback;

}

void gui_obj_delete_timer(gui_obj_t *obj)
{
    if (obj->timer != NULL)
    {
        gui_free(obj->timer);
        obj->timer = NULL;
    }
}

void gui_obj_start_timer(gui_obj_t *obj)
{

    GUI_ASSERT(obj->timer != NULL);

    obj->timer->expire_time = gui_ms_get() + obj->timer->interval_ms;
}

void gui_obj_stop_timer(gui_obj_t *obj)
{

    GUI_ASSERT(obj->timer != NULL);

    obj->timer->expire_time = 0xFFFFFFFF;
}

void gui_obj_timer_handler(gui_obj_t *obj)
{
    if (obj->timer == NULL)
    {
        return;
    }

    if (obj->timer->p_timer_callback == NULL)
    {
        return;
    }

    if (obj->timer->expire_time < gui_ms_get())
    {
        if (obj->timer->reload)
        {
            obj->timer->expire_time = gui_ms_get() + obj->timer->interval_ms;
        }
        else
        {
            gui_obj_stop_timer(obj);
        }
        obj->timer->p_timer_callback(obj);
    }
}
