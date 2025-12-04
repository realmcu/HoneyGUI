/**
 \internal
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_obj_suppress.c
  * @brief  object suppression functions
  * @details suppression related functions for handling touch events
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
#include "gui_obj_suppress.h"
#include "gui_api.h"
#include "tp_algo.h"

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
static void gui_obj_suppress_child_event(gui_obj_t *obj, gui_event_t filter)
{
    gui_node_list_t *node = NULL;
    gui_node_list_t *tmp = NULL;
    gui_list_for_each_safe(node, tmp, &obj->child_list)
    {
        gui_obj_t *o = gui_list_entry(node, gui_obj_t, brother_list);
        for (uint8_t i = 0; i < o->event_dsc_cnt; i++)
        {
            gui_event_dsc_t *event_dsc = o->event_dsc + i;
            if (event_dsc->filter == filter)
            {
                obj->suppress_conflict_obj_list = gui_realloc(o->suppress_conflict_obj_list,
                                                              sizeof(gui_obj_t *) * (obj->suppress_conflict_obj_cnt + 1));
                obj->suppress_conflict_obj_list[obj->suppress_conflict_obj_cnt] = o;
                obj->suppress_conflict_obj_cnt++;
                break;
            }
        }
        gui_obj_suppress_child_event(o, filter);
    }
}

/*============================================================================*
*                           Public Functions
*============================================================================*/

void gui_obj_suppress_set(gui_obj_t *object, gui_event_t filter)
{
    /**
     * This object is added to the suppression list of the parent object that has the same event.
     */
    for (gui_obj_t *o = object->parent; o->parent != NULL; o = o->parent)
    {
        for (uint8_t i = 0; i < o->event_dsc_cnt; i++)
        {
            gui_event_dsc_t *event_dsc = o->event_dsc + i;
            if (event_dsc->filter == filter)
            {
                o->suppress_conflict_obj_list = gui_realloc(o->suppress_conflict_obj_list,
                                                            sizeof(gui_obj_t *) * (o->suppress_conflict_obj_cnt + 1));
                o->suppress_conflict_obj_list[o->suppress_conflict_obj_cnt] = object;
                o->suppress_conflict_obj_cnt++;
                break;
            }
        }
    }
    /**
     * This child object is added to the suppression list that has the same event.
     */
    gui_obj_suppress_child_event(object, filter);
}


bool gui_obj_event_is_suppressed(gui_obj_t *object, gui_event_t event)
{
    touch_info_t *tp = tp_get_info();
    for (uint8_t i = 0; i < object->suppress_conflict_obj_cnt; i++)
    {
        gui_obj_t *o = object->suppress_conflict_obj_list[i];
        for (uint8_t j = 0; j < o->event_dsc_cnt; j++)
        {
            gui_event_dsc_t *event_dsc = o->event_dsc + j;
            if ((event_dsc->filter == event) && (gui_obj_point_in_obj_rect(o, tp->x, tp->y) == true))
            {
                return true;
            }
        }
    }
    return false;
}


