/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_page.c
  * @brief page widget
  * @details page widget
  * @author howie_wang@realsil.com.cn
  * @date 2023/11/07
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
#include <gui_page.h>
#include <string.h>
#include <gui_server.h>
#include <gui_img.h>
#include "gui_obj.h"
#include <gui_curtain.h>
#include <tp_algo.h>

/** @defgroup WIDGET WIDGET
  * @{
  */
/*============================================================================*
 *                           Types
 *============================================================================*/
/** @defgroup WIDGET_Exported_Types WIDGET Exported Types
  * @{
  */

/** End of WIDGET_Exported_Types
  * @}
  */

/*============================================================================*
 *                           Constants
 *============================================================================*/
/** @defgroup WIDGET_Exported_Constants WIDGET Exported Constants
  * @{
  */

/** End of WIDGET_Exported_Constants
  * @}
  */

/*============================================================================*
 *                            Macros
 *============================================================================*/
/** @defgroup SUBMOUDLE_Exported_Macros SUBMOUDLE Exported Macros
  * @{
  */

/** End of SUBMOUDLE_Exported_Macros
  * @}
  */

/*============================================================================*
 *                            Variables
 *============================================================================*/
/** @defgroup SUBMOUDLE_Exported_Variables SUBMOUDLE Exported Variables
  * @{
  */

/** End of SUBMOUDLE_Exported_Variables
  * @}
  */

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
/** @defgroup WIDGET_Exported_Functions WIDGET Exported Functions
  * @{
  */

static void gui_page_set_height(gui_obj_t *object, gui_obj_t *page)
{
    gui_list_t *node = NULL;
    gui_list_for_each(node, &object->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);

        if (!obj->not_show)
        {
            if ((obj->matrix != NULL) && (obj->parent->matrix))
            {
                memcpy(obj->matrix, obj->parent->matrix, sizeof(gui_matrix_t));
                matrix_translate(obj->x, obj->y, obj->matrix);
            }

            int ay = obj->matrix->m[1][2];
            int buttom = ay + obj->h - page->matrix->m[1][2];

            //gui_log("buttom:%d\n",buttom);

            if (page->h < buttom)
            {
                page->h = buttom;
            }
        }
        gui_page_set_height(obj, page);
    }
}

void gui_page_add_scroll_bar(gui_page_t *this, void *bar_pic, IMG_SOURCE_MODE_TYPE src_mode)
{
    if (src_mode == IMG_SRC_MEMADDR)
    {
        this->src_mode = IMG_SRC_MEMADDR;
        this->scroll_bar = gui_img_create_from_mem(this->base.parent,
                                                   "scroll_bar",
                                                   bar_pic,
                                                   0, 0, 0, 0);
    }
    else
    {
        this->src_mode = IMG_SRC_FILESYS;
        this->scroll_bar = gui_img_create_from_fs(this->base.parent, "scroll_bar", bar_pic, 0, 0, 0, 0);
    }
    gui_img_get_height(this->scroll_bar);
    this->scroll_bar->base.x = this->base.w - 3 - this->scroll_bar->base.w;
}

void gui_page_set_offset(gui_page_t *this, int offset)
{
    this->yold = offset;
    this->base.y = offset;
}

int gui_page_get_offset(gui_page_t *this)
{
    return this->base.y;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
static void gui_page_update_att(gui_obj_t *obj)
{
    gui_page_t *this = (void *)obj;
    size_t frame_count;

    if (this->animate && this->animate->animate)
    {
        frame_count = this->animate->dur * (1000 / 15) / (1000);
        this->animate->callback(this->animate->p);
        this->animate->current_frame++;

        if (this->animate->current_frame > frame_count)
        {
            if (this->animate->repeat_count == 0)
            {
                this->animate->animate = false;
            }
            else if (this->animate->repeat_count < 0)
            {
                this->animate->current_frame = 0;
            }
            else if (this->animate->repeat_count > 0)
            {
                this->animate->current_repeat_count++;
                if (this->animate->current_repeat_count >= this->animate->repeat_count)
                {
                    this->animate->animate = false;
                }
                else
                {
                    this->animate->current_frame = 0;
                }
            }
        }

        this->animate->progress_percent = ((float)(this->animate->current_frame)) / ((float)(
                frame_count));
    }
}

void gui_page_set_animate(gui_page_t *this,
                          uint32_t    dur,
                          int         repeat_count,
                          void       *callback,
                          void       *p)
{
    gui_animate_t *animate = this->animate;

    if (!(animate))
    {
        animate = gui_malloc(sizeof(gui_animate_t));
    }

    memset((animate), 0, sizeof(gui_animate_t));
    animate->animate = true;
    animate->dur = dur;
    animate->callback = (void (*)(void *))callback;
    animate->repeat_count = repeat_count;
    animate->p = p;
    this->animate = animate;
}

static void gui_page_update_speed(gui_obj_t *obj)
{
    gui_page_t *this = (gui_page_t *)obj;
    touch_info_t *tp = tp_get_info();
    int recode_num = 4;

    for (size_t i = 0; i < recode_num; i++)
    {
        this->recode[i] = this->recode[i + 1];
    }

    this->recode[recode_num] = tp->deltaY;
    this->speed = this->recode[recode_num] - this->recode[0];
    int max_speed = 60;
    int min_speed = 7;

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

    int top_height = -obj->y;
    int buttom_height = obj->h + obj->y - (int)gui_get_screen_height();

    if (this->speed > 0)
    {
        if (this->target > top_height)
        {
            this->target = top_height;
        }
        this->target = obj->y + this->target;
    }
    else if (this->speed < 0)
    {
        if (this->target > buttom_height)
        {
            this->target = buttom_height;
        }
        this->target = obj->y - this->target;
    }
    else
    {
        this->target = obj->y;
    }

    if (this->align_hight > 0)
    {
        int alien = _UI_ABS(this->target) % this->align_hight;
        if (alien > this->align_hight / 2)
        {
            this->target = this->target - this->align_hight + alien;

            if (_UI_ABS(this->speed) == min_speed || _UI_ABS(this->speed) == 0)
            {
                if (this->target > obj->y)
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
                if (this->target > obj->y)
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
    gui_page_t *this = (gui_page_t *)obj;

    obj->y = tp->deltaY + this->yold;
    gui_obj_event_set(obj, GUI_EVENT_8);

    if (obj->y > this->start_y)/*@TOP*/
    {
        obj->y = this->start_y;
        gui_obj_event_set(obj, GUI_EVENT_7);
    }
    else if (obj->y < (this->start_y - (obj->h - (int)gui_get_screen_height()))
             && obj->y != 0)/*@BOTTOM*/
    {
        gui_obj_event_set(obj, GUI_EVENT_7);
        obj->y = this->start_y - (obj->h - (int)gui_get_screen_height());
    }

    gui_page_update_speed(obj);
}

static void gui_page_update_inertial(gui_obj_t *obj)
{
    gui_page_t *this = (gui_page_t *)obj;

    if (this->speed == 0)
    {
        this->release = false;
    }

    if (this->speed > 3)
    {
        obj->y += this->speed;
        gui_obj_event_set(obj, GUI_EVENT_8);
        this->speed -= 1;
    }
    else if (this->speed < -3)
    {
        obj->y += this->speed;
        gui_obj_event_set(obj, GUI_EVENT_8);
        this->speed += 1;
    }
}

static void gui_page_update_boundary(gui_obj_t *obj)
{
    gui_page_t *this = (gui_page_t *)obj;

    if (obj->y > this->start_y)/*@TOP*/
    {
        obj->y = this->start_y;
        gui_obj_event_set(obj, GUI_EVENT_7);
        this->release = false;
    }
    else if (obj->y < (this->start_y - (obj->h - (int)gui_get_screen_height()))
             && obj->y != 0)/*@BOTTOM*/
    {
        this->release = false;
        obj->y = this->start_y - (obj->h - (int)gui_get_screen_height());
        gui_obj_event_set(obj, GUI_EVENT_7);
    }
}

static void gui_page_update_layer(gui_obj_t *obj)
{
    gui_page_t *this = (gui_page_t *)obj;

    if (obj->y == this->start_y)
    {
        obj->cover = false;
    }
    else
    {
        obj->cover = true;
    }
}

static void gui_page_update_alien(gui_obj_t *obj)
{
    gui_page_t *this = (gui_page_t *)obj;

    if ((this->speed <= 3) && (this->speed >= -3))
    {
        if (this->align_hight > 0)
        {
            if (((this->speed > 0) && (obj->y >= this->target))
                || ((this->speed < 0) && (obj->y <= this->target)))
            {
                this->speed = 0;
                this->release = false;
                obj->y = this->target;
                gui_obj_event_set(obj, GUI_EVENT_7);
            }

            obj->y += this->speed;

            if (this->speed != 0)
            {
                gui_obj_event_set(obj, GUI_EVENT_8);
            }
        }
        else
        {
            this->speed = 0;
            this->release = false;
            gui_obj_event_set(obj, GUI_EVENT_7);
        }
    }
}

static void gui_page_input_prepare(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    gui_page_t *this = (gui_page_t *)obj;

    GUI_UNUSED(tp);

    if (gui_point_in_obj_rect(obj, tp->x, tp->y) == false)
    {
        return;
    }

    if (obj->parent->matrix && obj->parent->matrix->m[1][2] != 0)
    {
        return;
    }

    if (obj->y != this->start_y)
    {
        gui_obj_skip_other_down_hold(obj);
        if (this->top_slide_only)
        {
            gui_obj_skip_other_left_hold(obj);
            gui_obj_skip_other_right_hold(obj);
        }
    }

    if (obj->y != (this->start_y - (obj->h - (int)gui_get_screen_height())))
    {
        gui_obj_skip_other_up_hold(obj);
    }
}

void gui_page_update(gui_obj_t *obj)
{
    gui_page_update_att(obj);
    obj->h = gui_get_screen_height();
    gui_page_set_height(obj, obj);

    touch_info_t *tp = tp_get_info();
    gui_page_t *this = (gui_page_t *)obj;

    if (this->gesture_flag)
    {
        return;
    }

    if (obj->parent->matrix && obj->parent->matrix->m[1][2] != 0)
    {
        return;
    }

    if (gui_point_in_obj_rect(obj, tp->x, tp->y) == true)
    {
        if ((tp->x > this->start_x) && (tp->x < this->start_x + obj->w))
        {
            if ((tp->y > this->start_y) && (tp->y < this->start_y + obj->h))
            {
                if ((tp->type == TOUCH_HOLD_Y))
                {
                    gui_page_hold_y(obj);
                }
                else if (tp->released)
                {
                    int max_speed = GUI_PAGE_MAX_SPEED;

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
                        gui_obj_event_set(obj, GUI_EVENT_8);
                    }
                    else
                    {
                        gui_obj_event_set(obj, GUI_EVENT_7);
                    }
                }
                else if (tp->pressed)
                {
                    this->release = false;
                    this->speed = 0;
                    memset(this->recode, 0, 10);
                    gui_obj_event_set(obj, GUI_EVENT_7);
                }

                if (tp->type != TOUCH_HOLD_Y)
                {
                    if (this->release)
                    {
                        gui_page_update_inertial(obj);
                        gui_page_update_alien(obj);
                    }
                    gui_page_update_boundary(obj);
                    this->yold = obj->y;
                }
            }
        }
        gui_page_update_layer(obj);
    }
}

static void gui_page_update_rebound(gui_obj_t *obj)
{
    gui_page_update_att(obj);

    obj->h = gui_get_screen_height();
    gui_page_set_height(obj, obj);

    touch_info_t *tp = tp_get_info();
    gui_page_t *this = (gui_page_t *)obj;

    if (obj->parent->matrix && obj->parent->matrix->m[1][2] != 0)
    {
        return;
    }

    if (tp->pressed && gui_point_in_obj_rect(obj, tp->x, tp->y))
    {
        this->press = true;
    }

    if ((tp->x > this->start_x) && (tp->x < this->start_x + obj->w))
    {
        if ((tp->type == TOUCH_HOLD_Y) && (this->press == true))
        {
            if (tp->deltaY + tp->y < 0)
            {
                tp->deltaY = -tp->y;
            }

            obj->y = tp->deltaY + this->yold;
            gui_obj_event_set(obj, GUI_EVENT_8);

            if (obj->y > this->start_y)
            {
                this->status = 1;
            }
            else if (obj->y < (this->start_y - (obj->h - (int)gui_get_screen_height())) &&
                     obj->y != 0)
            {
                this->status = 2;
            }

            gui_page_update_speed(obj);

            if (this->status == 1)
            {
                this->speed = -40;
            }
            else if (this->status == 2)
            {
                this->speed = 40;
            }
        }
        else if (tp->released)
        {
            int max_speed = 60;
            this->press = 0;

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
                gui_obj_event_set(obj, GUI_EVENT_8);
            }
            else
            {
                gui_obj_event_set(obj, GUI_EVENT_7);
            }
        }
        else if (tp->pressed)
        {
            this->release = false;
            this->speed = 0;
            this->status = 0;
            memset(this->recode, 0, 10);
            gui_obj_event_set(obj, GUI_EVENT_7);
        }

        if (tp->type != TOUCH_HOLD_Y)
        {
            switch (this->status)
            {
            case PAGE_REBOUND_NOT:
                {
                    if (this->release)
                    {
                        gui_page_update_inertial(obj);
                        gui_page_update_alien(obj);
                    }

                    if (obj->y > this->start_y)/*@TOP*/
                    {
                        obj->y = this->start_y;
                        this->release = false;
                        gui_obj_event_set(obj, GUI_EVENT_7);
                    }
                    else if (obj->y < (this->start_y - (obj->h - (int)gui_get_screen_height())) &&
                             obj->y != 0)/*@BOTTOM*/
                    {
                        this->release = false;
                        obj->y = this->start_y - (obj->h - (int)gui_get_screen_height());
                        gui_obj_event_set(obj, GUI_EVENT_7);
                    }
                }
                break;

            case PAGE_REBOUND_UP:
                {
                    if (this->release)
                    {
                        gui_page_update_inertial(obj);
                        gui_page_update_alien(obj);
                    }

                    if (obj->y < this->start_y)/*@TOP*/
                    {
                        obj->y = this->start_y;
                        this->release = false;
                        gui_obj_event_set(obj, GUI_EVENT_7);
                    }
                    else if (obj->y < (this->start_y - (obj->h - (int)gui_get_screen_height())) &&
                             obj->y != 0)/*@BOTTOM*/
                    {
                        this->release = false;
                        obj->y = this->start_y - (obj->h - (int)gui_get_screen_height());
                        gui_obj_event_set(obj, GUI_EVENT_7);
                    }
                }
                break;

            case PAGE_REBOUND_DOWN:
                {
                    if (this->release)
                    {
                        gui_page_update_inertial(obj);
                        gui_page_update_alien(obj);
                    }

                    if (obj->y > this->start_y)/*@TOP*/
                    {
                        obj->y = this->start_y;
                        this->release = false;
                        gui_obj_event_set(obj, GUI_EVENT_7);
                    }
                    else if (obj->y > (this->start_y - (obj->h - (int)gui_get_screen_height())) &&
                             obj->y != 0)/*@BOTTOM*/
                    {
                        this->release = false;
                        obj->y = this->start_y - (obj->h - (int)gui_get_screen_height());
                        gui_obj_event_set(obj, GUI_EVENT_7);

                    }
                }
                break;

            default:
                break;
            }
            this->yold = obj->y;
        }
    }
    gui_page_update_layer(obj);
}

void gui_page_destory(gui_obj_t *obj)
{
    gui_page_t *this = (gui_page_t *)obj;
    if (this->animate)
    {
        gui_free(this->animate);
    }
}

void gui_page_ctor(gui_page_t *this,
                   gui_obj_t  *parent,
                   const char *filename,
                   int16_t     x,
                   int16_t     y,
                   int16_t     w,
                   int16_t     h)
{
    gui_obj_ctor(&this->base, parent, filename, x, y, w, h);
    GET_BASE(this)->type = PAGE;
    GET_BASE(this)->obj_prepare = gui_page_update;
    GET_BASE(this)->obj_input_prepare = gui_page_input_prepare;
    GET_BASE(this)->obj_destory = gui_page_destory;
    this->base.type = PAGE;
    this->start_x = x;
    this->start_y = y;
}

gui_page_t *gui_page_create(void       *parent,
                            const char *filename,
                            int16_t     x,
                            int16_t     y,
                            int16_t     w,
                            int16_t     h)
{
#define _GUI_NEW_gui_page_create_param this, parent, filename, x, y, w, h
    GUI_NEW(gui_page_t, gui_page_ctor, _GUI_NEW_gui_page_create_param)
}

void gui_page_rebound(gui_page_t *this, bool rebound)
{
    if (rebound)
    {
        GET_BASE(this)->obj_prepare = gui_page_update_rebound;
    }
    else
    {
        GET_BASE(this)->obj_prepare = gui_page_update;
    }
}

void gui_page_center_alignment(gui_page_t *this, int align_hight)
{
    this->align_hight = align_hight;
}

void gui_page_set_only_top_slide(gui_page_t *this, bool flag)
{
    this->top_slide_only = flag;
}

/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
