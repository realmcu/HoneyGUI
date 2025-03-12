/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_view_prepare_internal.c
  * @brief the view widget is a kind of container that makes switching more convenient and effectively reduces memory consumption.
  * @details switch the current view by sliding or clicking
  * @author shel_deng@realsil.com.cn
  * @date 2025/02/18
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include "guidef.h"
#include "gui_server.h"
#include "gui_obj.h"
#include "tp_algo.h"
#include "gui_view.h"
#include "kb_algo.h"
#include "gui_img.h"


/*============================================================================*
 *                            Macros
 *============================================================================*/
#define VIEW_TRANSITION_DURATION_MS 1000

/*============================================================================*
 *                            Variables
 *============================================================================*/

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static void prepare_touch_process(gui_view_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    kb_info_t *kb = kb_get_info();
    gui_obj_t *obj = (gui_obj_t *)this;
    VIEW_SWITCH_STYLE style = this->style;

    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_LONG);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_TOUCH_LEFT_SLIDE);


    // restore view offset
    if (tp->pressing == false || !this->view_tp)
    {
        if (this->release_x >= GUI_FRAME_STEP)
        {
            this->release_x -= GUI_FRAME_STEP;
        }
        else if (this->release_x <= -GUI_FRAME_STEP)
        {
            this->release_x += GUI_FRAME_STEP;
        }
        else
        {
            this->release_x = 0;
        }

        if (this->release_y >= GUI_FRAME_STEP)
        {
            this->release_y -= GUI_FRAME_STEP;
        }
        else if (this->release_y <= -GUI_FRAME_STEP)
        {
            this->release_y += GUI_FRAME_STEP;
        }
        else
        {
            this->release_y = 0;
        }
    }

    gui_list_t *list = &(obj->parent->child_list);
    if (list->next == list->prev || style >= VIEW_ANIMATION_NULL)
    {
        return; // skip slide animation
    }

    extern void gui_view_reduction(gui_view_t *this);
    extern void gui_view_cube(gui_view_t *this);
    extern void gui_view_rotate(gui_view_t *this);
    extern void gui_view_rotate_book(gui_view_t *this);

    if (style == VIEW_TRANSPLATION)
    {
        matrix_translate(this->cur_id.x * (int)this->base.w + this->release_x, \
                         this->cur_id.y * (int)this->base.h + this->release_y, \
                         obj->matrix);
    }
    else if (style == VIEW_CUBE)
    {
        gui_view_cube(this);
    }
    else if (style == VIEW_ROTATE)
    {
        gui_view_rotate(this);
    }
    else if (style == VIEW_REDUCTION)
    {
        gui_view_reduction(this);
    }
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void gui_view_prepare_internal(gui_view_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    kb_info_t *kb = kb_get_info();
    gui_obj_t *obj = (gui_obj_t *)this;

    // animation update
    animate_frame_update(this->animate, obj);

    // keyboard
    if (!this->event && (kb->type == KB_SHORT) &&
        this->view_button && this->view_switch_ready)
    {
        this->event = 1;
        gui_obj_enable_event(obj, GUI_EVENT_KB_SHORT_CLICKED);
        return;
    }
    if (!this->event && (kb->type == KB_LONG) &&
        this->view_button_long && this->view_switch_ready)
    {
        this->event = 1;
        gui_obj_enable_event(obj, GUI_EVENT_KB_LONG_CLICKED);
        return;
    }

    // touch
    if (this->style < VIEW_ANIMATION_NULL)
    {
        prepare_touch_process(this);
    }
}
