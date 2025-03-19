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
static void view_move_process(gui_view_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    kb_info_t *kb = kb_get_info();
    gui_obj_t *obj = (gui_obj_t *)this;
    VIEW_SWITCH_STYLE style = this->style;

    if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y))
    {
        switch (tp->type)
        {
        case TOUCH_HOLD_X:
            {
                if (!this->view_tp)
                {
                    break;
                }

                this->release_his = this->release_x;
                this->release_x = tp->deltaX;
                if ((this->release_his != 0 && (this->release_his ^ this->release_x) < 0) || !this->release_x)
                {
                    this->moveback = 1;
                }
                // Limiting the range of movement
                if (abs(this->release_x) > obj->w)
                {
                    if (this->release_x > 0)
                    {
                        this->release_x = obj->w;
                    }
                    else
                    {
                        this->release_x = -obj->w;
                    }
                }
                if (!this->event && this->view_switch_ready)
                {
                    this->event |= (this->view_left   && gui_obj_enable_event(obj, GUI_EVENT_TOUCH_MOVE_LEFT));
                    this->event |= (this->view_right  && gui_obj_enable_event(obj, GUI_EVENT_TOUCH_MOVE_RIGHT));
                }
            }
            break;

        case TOUCH_HOLD_Y:
            {
                if (!this->view_tp)
                {
                    break;
                }
                this->release_his = this->release_y;
                this->release_y = tp->deltaY;
                if (abs(this->release_y) > obj->h)
                {
                    if (this->release_y > 0)
                    {
                        this->release_y = obj->h;
                    }
                    else
                    {
                        this->release_y = -obj->h;
                    }
                }
                if (this->release_his != 0 && (this->release_his ^ this->release_y) < 0)
                {
                    this->moveback = 1;
                }
                if (!this->event && this->view_switch_ready)
                {
                    this->event |= (this->view_up   && gui_obj_enable_event(obj, GUI_EVENT_TOUCH_MOVE_UP));
                    this->event |= (this->view_down && gui_obj_enable_event(obj, GUI_EVENT_TOUCH_MOVE_DOWN));
                }
            }
            break;

        case TOUCH_LEFT_SLIDE:
            {
                if (this->event && this->release_x < 0 && (this->view_left || !this->view_switch_ready))
                {
                    gui_log("[VIEW]TOUCH_LEFT_SLIDE\n");
                    this->view_tp = 0;
                    this->cur_id.x -= 1;
                    this->release_x = this->release_x + this->base.w;
                }
            }
            break;

        case TOUCH_RIGHT_SLIDE:
            {
                if (this->event && this->release_x > 0 && (this->view_right || !this->view_switch_ready))
                {
                    gui_log("[VIEW]TOUCH_RIGHT_SLIDE\n");
                    this->view_tp = 0;
                    this->cur_id.x += 1;
                    this->release_x = this->release_x - this->base.w;
                }
            }
            break;

        case TOUCH_DOWN_SLIDE:
            {
                if (this->event && this->release_y > 0 && (this->view_down || !this->view_switch_ready))
                {
                    gui_log("[VIEW]TOUCH_DOWN_SLIDE\n");
                    this->view_tp = 0;
                    this->cur_id.y += 1;
                    this->release_y = this->release_y - this->base.h;
                }
            }
            break;

        case TOUCH_UP_SLIDE:
            {
                if (this->event && this->release_y < 0 && (this->view_up || !this->view_switch_ready))
                {
                    gui_log("[VIEW]TOUCH_UP_SLIDE\n");
                    this->view_tp = 0;
                    this->cur_id.y -= 1;
                    this->release_y = this->release_y + this->base.h;
                }
            }
            break;
        default:
            break;
        }
    }
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

    // touch
    if (!this->event && this->view_switch_ready)
    {
        this->event |= (this->view_click       && gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED));
        this->event |= (this->view_touch_long  && gui_obj_enable_event(obj, GUI_EVENT_TOUCH_LONG));
        if (this->event)
        {
            this->style = VIEW_ANIMATION_NULL;
            return;
        }
    }
    //keyboard
    if (!this->event && this->view_switch_ready)
    {
        this->event |= (this->view_button      && gui_obj_enable_event(obj, GUI_EVENT_KB_SHORT_CLICKED));
        this->event |= (this->view_button_long && gui_obj_enable_event(obj, GUI_EVENT_KB_LONG_CLICKED));
        if (this->event)
        {
            this->style = VIEW_ANIMATION_NULL;
            return;
        }
    }
    // move process
    view_move_process(this);
}