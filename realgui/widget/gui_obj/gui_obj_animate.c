/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_obj_animate.c
  * @brief  object animation
  * @details animation related functions and structures
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
#include "gui_obj_animate.h"
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

void animate_frame_update(gui_animate_t *animate, gui_obj_t *obj)
{
    uint32_t cur_time_gap;
    if (animate && animate->animate)
    {
        animate->Beginning_frame = 0;
        animate->end_frame = 0;
        if (animate->progress_percent == 0.0f)
        {
            animate->Beginning_frame = 1;
        }
        if (animate->progress_percent == 0 && !animate->init)
        {
            animate->init = 1;
            animate->init_time_ms = gui_ms_get();
        }

        animate->cur_time_ms = gui_ms_get();
        cur_time_gap = animate->cur_time_ms - animate->init_time_ms;

        if (animate->repeat_count == 0)
        {
            animate->progress_percent = (float)(cur_time_gap % animate->dur) /
                                        (float)animate->dur;
            if (cur_time_gap / animate->dur >= 1)
            {
                animate->end_frame = 1;
                animate->progress_percent = 1;
                animate->animate = 0;
            }
            animate->current_frame++;
            animate->callback(animate->p, obj, animate);

        }
        else if (animate->repeat_count == -1)
        {
            // uint32_t  round_count = cur_time_gap / animate->dur;


            animate->progress_percent = (float)(cur_time_gap % animate->dur) /
                                        (float)animate->dur;
            if (animate->progress_percent < animate->last_per)
            {

                animate->Beginning_frame = 1;
                animate->end_frame = 1;
                animate->current_repeat_count++;
                animate->current_frame = 0;
            }


            animate->last_per = animate->progress_percent;
            animate->current_frame++;
            animate->callback(animate->p, obj, animate);
        }
        else
        {
            uint32_t  round_count = cur_time_gap / animate->dur;
            if (round_count > animate->repeat_count)
            {
                animate->animate = 0;
                animate->end_frame = 1;
                animate->current_frame = 0;
                return;
            }

            if (round_count > animate->last_round)
            {
                animate->Beginning_frame = 1;
                animate->current_repeat_count++;
                animate->end_frame = 1;
                animate->current_frame = 0;
            }
            animate->last_round = round_count;
            animate->progress_percent = (float)(cur_time_gap % animate->dur) /
                                        (float)animate->dur;
            animate->current_frame++;
            animate->callback(animate->p, obj, animate);
        }
    }
    if (animate && !animate->animate)
    {
        animate->init_time_ms = gui_ms_get();
    }
}


float gui_animation_get_progress(gui_animate_t *animation)
{
    GUI_ASSERT(animation)
    if (animation)
    {
        return animation->progress_percent;
    }
    else
    {
        return 0;
    }
}
