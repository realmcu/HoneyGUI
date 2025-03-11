/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_cardview.c
  * @brief 3d container
  * @details slide to swap
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/25
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include <string.h>
#include <gui_matrix.h>
#include <gui_obj.h>
#include <tp_algo.h>
#include "gui_cardview.h"


/*============================================================================*
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                           Constants
 *============================================================================*/


/*============================================================================*
 *                            Macros
 *============================================================================*/

#define CARDVIEW_SLIDE_SPEED 20


/*============================================================================*
 *                            Variables
 *============================================================================*/


/*============================================================================*
 *                           Private Functions
 *============================================================================*/

static void gui_cardview_update_att(gui_obj_t *obj)
{
    gui_cardview_t *this = (void *)obj;
    if (this->animate)
    {
        animate_frame_update(this->animate, obj);
    }
}

static void gui_cardview_input_prepare(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    gui_cardview_t *this = (gui_cardview_t *)obj;
    GUI_UNUSED(tp);

    if (gui_obj_in_rect(obj, 0, 0, gui_get_screen_width(), gui_get_screen_height()) == false)
    {
        return;
    }

    if (this->hold_y == 0)
    {
        gui_obj_skip_other_up_hold(obj);
    }
    else
    {
        gui_obj_skip_other_up_hold(obj);
        gui_obj_skip_other_down_hold(obj);
    }
}
#define GUI_MAX_SPEED 60
#define GUI_MIN_SPEED 7
static void gui_page_update_speed(gui_obj_t *obj)
{
    gui_cardview_t *this = (gui_cardview_t *)obj;
    touch_info_t *tp = tp_get_info();
    int recode_num = 4;

    for (size_t i = 0; i < recode_num; i++)
    {
        this->recode[i] = this->recode[i + 1];
    }

    this->recode[recode_num] = tp->deltaY;
    this->speed = this->recode[recode_num] - this->recode[0];
    int max_speed = GUI_MAX_SPEED;
    int min_speed = GUI_MIN_SPEED;

    if (this->speed > max_speed)
    {
        this->speed = max_speed;
    }
    else if (this->speed < -max_speed)
    {
        this->speed = -max_speed;
    }

    if ((this->speed > 0) && (this->speed < min_speed))
    {
        this->speed = min_speed;
    }
    else if ((this->speed < 0) && (this->speed > -min_speed))
    {
        this->speed = -min_speed;
    }

    this->target = (this->speed * this->speed + _UI_ABS(this->speed)) / 2;

    int top_height = -this->hold_y;
    int buttom_height = this->height + this->card_height * this->bottom_space_count + this->hold_y -
                        (int)gui_get_screen_height();
    if (this->speed > 0)
    {
        if (this->target > top_height)
        {
            this->target = top_height;
        }
        this->target = this->hold_y + this->target;
    }
    else if (this->speed < 0)
    {
        if (this->target > buttom_height)
        {
            this->target = buttom_height;
        }
        this->target = this->hold_y - this->target;
    }
    else
    {
        this->target = this->hold_y;
    }

    if (this->align_hight > 0)
    {
        int alien = _UI_ABS(this->target) % this->align_hight;
        if (alien > this->align_hight / 2)
        {
            this->target = this->target - this->align_hight + alien;

            if (_UI_ABS(this->speed) == min_speed || _UI_ABS(this->speed) == 0)
            {
                if (this->target > this->hold_y)
                {
                    this->speed = min_speed;
                }
                else
                {
                    this->speed = -min_speed;
                }
            }
        }
        else
        {
            this->target = this->target + alien;
            if (_UI_ABS(this->speed) == min_speed || _UI_ABS(this->speed) == 0)
            {
                /* code */
                if (this->target > this->hold_y)
                {
                    this->speed = min_speed;
                }
                else
                {
                    this->speed = -min_speed;
                }
            }
        }
    }
}

static void gui_page_hold_y(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    gui_cardview_t *this = (gui_cardview_t *)obj;

    this->hold_y = tp->deltaY + this->yold;

    if (this->hold_y > this->start_y)/*@TOP*/
    {
        this->hold_y = this->start_y;
    }
    else if (this->hold_y < (this->start_y - (this->height + this->card_height *
                                              this->bottom_space_count - (int)gui_get_screen_height()))
             && this->hold_y != 0)/*@BOTTOM*/
    {
        gui_obj_enable_event(obj, GUI_EVENT_7);
        this->hold_y = this->start_y - (this->height + this->card_height * this->bottom_space_count -
                                        (int)gui_get_screen_height());
    }

    gui_page_update_speed(obj);
}

static void gui_page_update_inertial(gui_obj_t *obj)
{
    gui_cardview_t *this = (gui_cardview_t *)obj;

    if (this->speed == 0)
    {
        this->release = false;
    }

    if (this->speed > 3)
    {
        this->hold_y += this->speed;
        gui_obj_enable_event(obj, GUI_EVENT_8);
        this->speed -= 1;
    }
    else if (this->speed < -3)
    {
        this->hold_y += this->speed;
        gui_obj_enable_event(obj, GUI_EVENT_8);
        this->speed += 1;
    }
}

static void gui_page_update_boundary(gui_obj_t *obj)
{
    gui_cardview_t *this = (gui_cardview_t *)obj;

    if (this->hold_y > this->start_y)/*@TOP*/
    {
        this->hold_y = this->start_y;
        gui_obj_enable_event(obj, GUI_EVENT_7);
        this->release = false;
    }
    else if (this->hold_y < (this->start_y - (this->height + this->card_height *
                                              this->bottom_space_count - (int)gui_get_screen_height()))
             && this->hold_y != 0)/*@BOTTOM*/
    {
        this->release = false;
        this->hold_y = this->start_y - (this->height + this->card_height * this->bottom_space_count -
                                        (int)gui_get_screen_height());
        gui_obj_enable_event(obj, GUI_EVENT_7);
    }
}


static void gui_page_update_alien(gui_obj_t *obj)
{
    gui_cardview_t *this = (gui_cardview_t *)obj;
    if ((this->speed <= 3) && (this->speed >= -3))
    {
        if (this->align_hight > 0)
        {
            if (((this->speed > 0) && (this->hold_y >= this->target))
                || ((this->speed < 0) && (this->hold_y <= this->target)))
            {
                this->speed = 0;
                this->release = false;
                this->hold_y = this->target;

            }

            this->hold_y += this->speed;

            if (this->speed != 0)
            {

            }
        }
        else
        {
            this->speed = 0;
            this->release = false;

        }
    }
}
static void gui_cardview_prepare(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    gui_cardview_t *this = (gui_cardview_t *)obj;
    uint8_t last = this->checksum;
    gui_cardview_update_att(obj);
    this->align_hight = this->card_height;
    if (this->gesture_flag)
    {
        return;
    }
    if ((obj->skip_tp_up_hold) && (tp->deltaY  < 0))
    {
        return;
    }
    if ((obj->skip_tp_down_hold) && (tp->deltaY  > 0))
    {
        return;
    }

    if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true)
    {
        if ((tp->x > this->start_x) && (tp->x < this->start_x + obj->w))
        {
            if ((tp->y > this->start_y) && (tp->y < this->start_y + this->height + this->card_height))
            {
                if ((tp->type == TOUCH_HOLD_Y))
                {
                    gui_page_hold_y(obj);
                }
                else if (tp->released)
                {
                    int max_speed = CARDVIEW_SLIDE_SPEED;

                    if (tp->type == TOUCH_UP_SLIDE)
                    {
                        gui_log("page TOUCH_UP_SLIDE\n");

                        if (this->speed == 0)
                        {
                            this->speed = -max_speed;
                        }
                    }
                    else if (tp->type == TOUCH_DOWN_SLIDE)
                    {
                        gui_log("page TOUCH_DOWN_SLIDE\n");

                        if (this->speed == 0)
                        {
                            this->speed = max_speed;
                        }
                    }
                    if (this->speed != 0)
                    {
                        this->release = true;
                    }
                    else
                    {
                        if (this->align_hight > 0 && this->hold_y % this->align_hight != 0)
                        {
                            this->release = true;
                            if (_UI_ABS(this->hold_y % this->align_hight) < this->align_hight / 2)
                            {
                                this->target = this->hold_y - (this->hold_y % this->align_hight);
                                this->speed = GUI_MIN_SPEED;
                            }
                            else
                            {
                                this->target = this->hold_y - this->align_hight + _UI_ABS(this->hold_y % this->align_hight);
                                this->speed = -GUI_MIN_SPEED;
                            }
                        }
                    }
                }
                else if (tp->pressed)
                {
                    this->release = false;
                    this->speed = 0;
                    memset(this->recode, 0, 10);
                    gui_obj_enable_event(obj, GUI_EVENT_7);
                }

                if (tp->type != TOUCH_HOLD_Y)
                {
                    if (this->release)
                    {
                        gui_page_update_inertial(obj);
                        gui_page_update_alien(obj);
                    }
                    gui_page_update_boundary(obj);
                    this->yold = this->hold_y;
                }
            }
        }
    }
    this->checksum = 0;
    this->checksum = gui_obj_checksum(0, (uint8_t *)this, sizeof(gui_cardview_t));

    if (tp->type == TOUCH_ORIGIN_FROM_Y)
    {
        if (tp->deltaY <= -10)
        {
            gui_obj_enable_event(obj, GUI_EVENT_1);
        }
        else if (tp->deltaY > 10)
        {
            gui_obj_enable_event(obj, GUI_EVENT_2);
        }
    }

    if (last != this->checksum)
    {
        gui_fb_change();
    }

    if (this->status_cb != NULL)
    {
        this->status_cb(this);
    }
}

static void gui_cardview_draw_cb(gui_obj_t *obj)
{
    gui_cardview_t *this = (gui_cardview_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();
    GUI_UNUSED(this);
    GUI_UNUSED(dc);
}

static void gui_cardview_end(gui_obj_t *obj)
{

}

static void gui_cardview_destroy(gui_obj_t *obj)
{
    gui_cardview_t *this = (void *)obj;
    if (this->animate)
    {
        gui_free(this->animate);
        this->animate = NULL;
    }
}

static void gui_cardview_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            gui_cardview_input_prepare(obj);
            break;

        case OBJ_PREPARE:
            gui_cardview_prepare(obj);
            break;

        case OBJ_DRAW:
            gui_cardview_draw_cb(obj);
            break;

        case OBJ_END:
            gui_cardview_end(obj);
            break;

        case OBJ_DESTROY:
            gui_cardview_destroy(obj);
            break;

        default:
            break;
        }
    }
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void gui_cardview_set_animate(gui_cardview_t *this,
                              uint32_t        dur,
                              int             repeat_count,
                              void           *callback,
                              void           *p)
{
    GUI_SET_ANIMATE_HELPER
}

void gui_cardview_status_cb(gui_cardview_t *this, void (*cb)(gui_cardview_t *this))
{
    this->status_cb = cb;
}

void gui_cardview_set_style(gui_cardview_t *this, T_SLIDE_STYLE style)
{
    this->style = style;
}

void gui_cardview_set_bottom_space(gui_cardview_t *this, uint8_t bottom_space)
{
    this->bottom_space_count = bottom_space;
}

void gui_cardview_up(gui_cardview_t *this, void *callback, void *parameter)
{
    gui_obj_add_event_cb(this, (gui_event_cb_t)callback, GUI_EVENT_1, parameter);
}

void gui_cardview_down(gui_cardview_t *this, void *callback, void *parameter)
{
    gui_obj_add_event_cb(this, (gui_event_cb_t)callback, GUI_EVENT_2, parameter);
}

gui_cardview_t *gui_cardview_create(void       *parent,
                                    const char *name,
                                    int16_t     x,
                                    int16_t     y,
                                    int16_t     w,
                                    int16_t     h)
{
    GUI_ASSERT(parent != NULL);

    if (name == NULL)
    {
        name = "cardview";
    }

    gui_cardview_t *this = gui_malloc(sizeof(gui_cardview_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_cardview_t));

    //for base class
    gui_obj_t *base = (gui_obj_t *)this;
    gui_obj_ctor(base, parent, name, x, y, w, h);

    //for root class
    gui_obj_t *root = (gui_obj_t *)this;
    root->type = CARDVIEW;
    root->obj_cb = gui_cardview_cb;
    root->has_input_prepare_cb = true;
    root->has_prepare_cb = true;
    root->has_draw_cb = true;
    root->has_end_cb = true;
    root->has_destroy_cb = true;

    this->style = REDUCTION;
    this->bottom_space_count = 1;

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }

    GET_BASE(this)->create_done = true;
    return this;
}
void gui_cardview_alignment(gui_cardview_t *this, int align_hight)
{
    this->align_hight = align_hight;
}

