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
            if (animate->progress_percent == 0.0f)
            {
                animate->Beginning_frame = 1;
            }
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
            if (animate->progress_percent == 0.0f)
            {
                animate->Beginning_frame = 1;
            }
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
            if (animate->progress_percent == 0.0f)
            {
                animate->Beginning_frame = 1;
            }
            animate->current_frame++;
            animate->callback(animate->p, obj, animate);
        }
    }
    if (animate && !animate->animate)
    {
        animate->init_time_ms = gui_ms_get();
    }
}

gui_animate_t *gui_obj_set_animate(gui_animate_t *animate,
                                   uint32_t      dur,
                                   int           repeat_count,
                                   void         *callback,
                                   void         *p)
{
    if (!(animate))
    {
        animate = gui_malloc(sizeof(gui_animate_t));
    }

    memset((animate), 0, sizeof(gui_animate_t));
    animate->animate = true;
    animate->dur = dur;
    animate->callback = (gui_animate_callback_t)callback;
    animate->repeat_count = repeat_count;
    animate->p = p;
    return animate;
}


void gui_obj_restart_animate(gui_animate_t *animate)
{
    gui_log("gui_obj_restart_animate\n");
    animate->animate = true;
    animate->init_time_ms = 0;
    animate->cur_time_ms = 0;
    animate->current_repeat_count = 0;
    animate->current_frame = 0;
    animate->last_round = 0;
    animate->progress_percent = 0;
    animate->last_per = 0;
    animate->init = 0;
    animate->Beginning_frame = 0;
    animate->end_frame = 0;
}
void gui_obj_stop_animate(gui_animate_t *animate)
{
    if (!animate)
    {
        gui_log("gui_obj_stop_animate failed\n");
        return;
    }

    //gui_log("gui_obj_stop_animate\n");
    animate->animate = false;
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

gui_animate_t *gui_get_animation_from_array(gui_animate_t **animation_array, int array_length,
                                            int index)
{
    if (array_length - 1 < index)
    {
        return 0;
    }
    return animation_array[index];
}
gui_error_t gui_aniamtion_set_name(gui_animate_t *animate, const char *name)
{
    if (!animate || !name)
    {
        return GUI_ERROR_NULL;
    }
    animate->name = name;
    return GUI_SUCCESS;
}
#include "gui_img.h"
gui_animate_t *gui_aniamtion_get_by_name(gui_obj_t *widget, const char *name)
{
    if (!GUI_WIDGET_VALID(widget) || !name)
    {
        return 0;
    }
    {
        switch (widget->type)
        {
        case IMAGE_FROM_MEM:
            {
                gui_img_t *img = GUI_CAST widget;
                if (img->animate_array_length == 0)
                {
                    if (img->animate && img->animate->name && GUI_STRINGS_EQUAL(img->animate->name, name))
                    {
                        return img->animate;
                    }
                }
                else
                {
                    for (size_t i = 0; i < img->animate_array_length; i++)
                    {
                        if (((gui_animate_t **)img->animate)[i]
                            && ((gui_animate_t **)img->animate)[i]->name
                            && GUI_STRINGS_EQUAL(((gui_animate_t **)img->animate)[i]->name, name))
                        {
                            return (((gui_animate_t **)img->animate)[i]);
                        }
                    }

                }

            }
            break;

        default:
            break;
        }
    }
    return 0;
}