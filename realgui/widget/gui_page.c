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
#include <gui_img.h>
#include "gui_obj.h"
#include <gui_curtain.h>
#include <tp_algo.h>
#include "wheel_algo.h"

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

static void gui_page_set_height(gui_obj_t *object, gui_obj_t *page)
{
    if ((page->matrix != NULL))
    {
        if (page->matrix->m[0][0] != 1 || page->matrix->m[1][1] != 1)
        {
            page->h = INT16_MAX;
            return;
        }
    }
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
            if (obj->matrix == NULL || page->matrix == NULL)
            {
                gui_log("Error: obj->matrix is NULL\n");
                return;
            }
            int ay = obj->matrix->m[1][2];
            int buttom = ay + obj->h - page->matrix->m[1][2];



            if (page->h < buttom)
            {
                page->h = buttom;
            }
        }
        gui_page_set_height(obj, page);
    }
}

static void gui_page_set_width(gui_obj_t *object, gui_obj_t *page)
{
    if ((page->matrix != NULL))
    {
        if (page->matrix->m[0][0] != 1 || page->matrix->m[1][1] != 1)
        {
            page->w = INT16_MAX;
            return;
        }
    }
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
            if (obj->matrix == NULL || page->matrix == NULL)
            {
                gui_log("Error: obj->matrix is NULL\n");
                return;
            }
            int ay = obj->matrix->m[0][2];
            int buttom = ay + obj->w - page->matrix->m[0][2];



            if (page->w < buttom)
            {
                page->w = buttom;
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
    if (this->animate)
    {
        animate_frame_update(this->animate, obj);
    }
}

void gui_page_set_animate(gui_page_t *this,
                          uint32_t    dur,
                          int         repeat_count,
                          void       *callback,
                          void       *p)
{
    GUI_SET_ANIMATE_HELPER
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
    int max_speed = GUI_PAGE_MAX_SPEED;
    int min_speed = GUI_PAGE_MIN_SPEED;

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

static void gui_page_update_speed_x(gui_obj_t *obj)
{
    gui_page_t *this = (gui_page_t *)obj;
    touch_info_t *tp = tp_get_info();
    int recode_num = 4;

    for (size_t i = 0; i < recode_num; i++)
    {
        this->recode[i] = this->recode[i + 1];
    }

    this->recode[recode_num] = tp->deltaX;
    this->speed = this->recode[recode_num] - this->recode[0];
    int max_speed = GUI_PAGE_MAX_SPEED;
    int min_speed = GUI_PAGE_MIN_SPEED;

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

    int top_height = -obj->x;
    int buttom_height = obj->w + obj->x - (int)gui_get_screen_width();
    gui_log("1111this->target:%d\n", this->target);
    if (this->speed > 0)
    {
        if (this->target > top_height)
        {
            this->target = top_height;
        }
        this->target = obj->x + this->target;
    }
    else if (this->speed < 0)
    {
        if (this->target > buttom_height)
        {
            this->target = buttom_height;
        }
        this->target = obj->x - this->target;
    }
    else
    {
        this->target = obj->x;
    }
    gui_log("2222this->target:%d\n", this->target);
    if (this->align_hight > 0)
    {
        int alien = _UI_ABS(this->target) % this->align_hight;
        if (alien > this->align_hight / 2)
        {
            this->target = this->target - this->align_hight + alien;

            if (_UI_ABS(this->speed) == min_speed || _UI_ABS(this->speed) == 0)
            {
                if (this->target > obj->x)
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
                if (this->target > obj->x)
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
    gui_log("3333this->target:%d\n", this->target);
}
static void gui_page_hold_y(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    gui_page_t *this = (gui_page_t *)obj;

    obj->y = tp->deltaY + this->yold;
    gui_obj_enable_event(obj, GUI_EVENT_8);

    if (obj->y > this->start_y)/*@TOP*/
    {
        obj->y = this->start_y;
        gui_obj_enable_event(obj, GUI_EVENT_7);
    }
    else if (obj->y < (this->start_y - (obj->h - (int)gui_get_screen_height()))
             && obj->y != 0)/*@BOTTOM*/
    {
        gui_obj_enable_event(obj, GUI_EVENT_7);
        obj->y = this->start_y - (obj->h - (int)gui_get_screen_height());
    }

    gui_page_update_speed(obj);
}

static void gui_page_hold_x(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    gui_page_t *this = (gui_page_t *)obj;

    obj->x = tp->deltaX + this->yold;
    gui_obj_enable_event(obj, GUI_EVENT_8);

    if (obj->x > this->start_x)/*@TOP*/
    {
        obj->x = this->start_x;
        gui_obj_enable_event(obj, GUI_EVENT_7);
    }
    else if (obj->x < (this->start_x - (obj->w - (int)gui_get_screen_width()))
             && obj->x != 0)/*@BOTTOM*/
    {
        gui_obj_enable_event(obj, GUI_EVENT_7);
        obj->x = this->start_x - (obj->w - (int)gui_get_screen_width());
    }

    gui_page_update_speed_x(obj);
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
        gui_obj_enable_event(obj, GUI_EVENT_8);
        this->speed -= 1;
    }
    else if (this->speed < -3)
    {
        obj->y += this->speed;
        gui_obj_enable_event(obj, GUI_EVENT_8);
        this->speed += 1;
    }
}

static void gui_page_update_inertial_x(gui_obj_t *obj)
{
    gui_page_t *this = (gui_page_t *)obj;

    if (this->speed == 0)
    {
        this->release = false;
    }

    if (this->speed > 3)
    {
        obj->x += this->speed;
        gui_obj_enable_event(obj, GUI_EVENT_8);
        this->speed -= 1;
    }
    else if (this->speed < -3)
    {
        obj->x += this->speed;
        gui_obj_enable_event(obj, GUI_EVENT_8);
        this->speed += 1;
    } gui_log("this->speed:%d\n", this->speed);
}
static void gui_page_update_boundary(gui_obj_t *obj)
{
    gui_page_t *this = (gui_page_t *)obj;

    if (obj->y > this->start_y)/*@TOP*/
    {
        obj->y = this->start_y;
        gui_obj_enable_event(obj, GUI_EVENT_7);
        this->release = false;
    }
    else if (obj->y < (this->start_y - (obj->h - (int)gui_get_screen_height()))
             && obj->y != 0)/*@BOTTOM*/
    {
        this->release = false;
        obj->y = this->start_y - (obj->h - (int)gui_get_screen_height());
        gui_obj_enable_event(obj, GUI_EVENT_7);
    }
}
static void  gui_page_update_boundary_x(gui_obj_t *obj)
{
    gui_page_t *this = (gui_page_t *)obj;

    if (obj->x > this->start_x)/*@TOP*/
    {
        obj->x = this->start_x;
        gui_obj_enable_event(obj, GUI_EVENT_7);
        this->release = false;
    }
    else if (obj->x < (this->start_x - (obj->w - (int)gui_get_screen_width()))
             && obj->x != 0)/*@BOTTOM*/
    {
        this->release = false;
        obj->x = this->start_x - (obj->w - (int)gui_get_screen_width());
        gui_obj_enable_event(obj, GUI_EVENT_7);
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
                gui_obj_enable_event(obj, GUI_EVENT_7);
            }

            obj->y += this->speed;

            if (this->speed != 0)
            {
                gui_obj_enable_event(obj, GUI_EVENT_8);
            }
        }
        else
        {
            this->speed = 0;
            this->release = false;
            gui_obj_enable_event(obj, GUI_EVENT_7);
        }
    }
}
static void gui_page_update_alien_x(gui_obj_t *obj)
{
    gui_page_t *this = (gui_page_t *)obj;

    if ((this->speed <= 3) && (this->speed >= -3))
    {
        if (this->align_hight > 0)
        {
            if (((this->speed > 0) && (obj->x >= this->target))
                || ((this->speed < 0) && (obj->x <= this->target)))
            {
                this->speed = 0;
                this->release = false;
                obj->x = this->target;
                gui_obj_enable_event(obj, GUI_EVENT_7);
            }

            obj->x += this->speed;

            if (this->speed != 0)
            {
                gui_obj_enable_event(obj, GUI_EVENT_8);
            }
        }
        else
        {
            this->speed = 0;
            this->release = false;
            gui_obj_enable_event(obj, GUI_EVENT_7);
        }
    } gui_log("this->target:%d, obj->x :%d\n", this->target, obj->x);
}
void gui_page_input_prepare(gui_obj_t *obj)
{
    if (obj->gesture)
    {
        return;
    }

    touch_info_t *tp = tp_get_info();
    gui_page_t *this = (gui_page_t *)obj;

    GUI_UNUSED(tp);

    if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == false)
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



    int ax = 0 ;
    int ay = 0;
    {
        gui_obj_t *o = obj;
        while (o->parent != NULL)
        {
            o = o->parent;
            ax += o->x;
            ay += o->y;
        }
    }
    bool touch_in = 0;
    if (ax < tp->x && (ax + obj->w) > tp->x && ay < tp->y && (ay + obj->parent->h) > tp->y)
    {
        touch_in = 1;
    }

    if (touch_in)
    {
        //if ((tp->x > this->start_x) && (tp->x < this->start_x + obj->w))
        {
            //if ((tp->y > this->start_y) && (tp->y < this->start_y + obj->h))
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
                        gui_obj_enable_event(obj, GUI_EVENT_8);
                    }
                    else

                        if (this->align_hight > 0 && obj->y % this->align_hight != 0)
                        {
                            this->release = true;
                            if (_UI_ABS(obj->y % this->align_hight) < this->align_hight / 2)
                            {
                                this->target = obj->y - (obj->y % this->align_hight);
                                this->speed = GUI_PAGE_MIN_SPEED;
                            }
                            else
                            {
                                this->target = obj->y - this->align_hight + _UI_ABS(obj->y % this->align_hight);
                                this->speed = -GUI_PAGE_MIN_SPEED;
                            }
                        }

                    gui_obj_enable_event(obj, GUI_EVENT_7);

                }
                else if (tp->pressed)
                {
                    this->release = false;
                    this->speed = 0;
                    memset(this->recode, 0, 10);
                    gui_obj_enable_event(obj, GUI_EVENT_7);
                }


            }
        }

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
    uint8_t last = this->checksum;
    this->checksum = 0;
    this->checksum = gui_obj_checksum(0, (uint8_t *)this, sizeof(gui_page_t));
    if (last != this->checksum)
    {
        gui_fb_change();
    }
}

void gui_page_update_x(gui_obj_t *obj)
{
    gui_page_update_att(obj);
    obj->w = gui_get_screen_width();
    gui_page_set_width(obj, obj);

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

    if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true)
    {
        if ((tp->x > this->start_x) && (tp->x < this->start_x + obj->w))
        {
            if ((tp->y > this->start_y) && (tp->y < this->start_y + obj->h))
            {
                if ((tp->type == TOUCH_HOLD_X))
                {
                    gui_page_hold_x(obj);
                }
                else if (tp->released)
                {
                    int max_speed = GUI_PAGE_MAX_SPEED;

                    if (tp->type == TOUCH_LEFT_SLIDE)
                    {
                        gui_log("page TOUCH_UP_SLIDE\n");

                        if (this->speed == 0)
                        {
                            this->speed = -max_speed;
                        }
                    }
                    else if (tp->type == TOUCH_RIGHT_SLIDE)
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
                        gui_obj_enable_event(obj, GUI_EVENT_8);
                    }
                    else

                        if (this->align_hight > 0 && obj->x % this->align_hight != 0)
                        {
                            this->release = true;
                            if (_UI_ABS(obj->x % this->align_hight) < this->align_hight / 2)
                            {
                                this->target = obj->x - (obj->x % this->align_hight);
                                this->speed = GUI_PAGE_MIN_SPEED;
                            }
                            else
                            {
                                this->target = obj->x - this->align_hight + _UI_ABS(obj->x % this->align_hight);
                                this->speed = -GUI_PAGE_MIN_SPEED;
                            }
                        }

                    gui_obj_enable_event(obj, GUI_EVENT_7);

                }
                else if (tp->pressed)
                {
                    this->release = false;
                    this->speed = 0;
                    memset(this->recode, 0, 10);
                    gui_obj_enable_event(obj, GUI_EVENT_7);
                }


            }
        }

    }
    if (tp->type != TOUCH_HOLD_X)
    {
        if (this->release)
        {
            gui_page_update_inertial_x(obj);
            gui_page_update_alien_x(obj);
        }
        gui_page_update_boundary_x(obj);
        this->yold = obj->x;
    }
    uint8_t last = this->checksum;
    this->checksum = 0;
    this->checksum = gui_obj_checksum(0, (uint8_t *)this, sizeof(gui_page_t));
    if (last != this->checksum)
    {
        gui_fb_change();
    }
}
void gui_page_loop_update(gui_obj_t *obj)
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

    if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true)
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
                        gui_obj_enable_event(obj, GUI_EVENT_8);
                    }
                    else

                        if (this->align_hight > 0 && obj->y % this->align_hight != 0)
                        {
                            this->release = true;
                            if (_UI_ABS(obj->y % this->align_hight) < this->align_hight / 2)
                            {
                                this->target = obj->y - (obj->y % this->align_hight);
                                this->speed = GUI_PAGE_MIN_SPEED;
                            }
                            else
                            {
                                this->target = obj->y - this->align_hight + _UI_ABS(obj->y % this->align_hight);
                                this->speed = -GUI_PAGE_MIN_SPEED;
                            }
                        }

                    gui_obj_enable_event(obj, GUI_EVENT_7);

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
                    this->yold = obj->y;
                }
            }
        }
    }

    uint8_t last = this->checksum;
    this->checksum = 0;
    this->checksum = gui_obj_checksum(0, (uint8_t *)this, sizeof(gui_img_t));

    if (last != this->checksum)
    {
        gui_fb_change();
    }
}
void gui_page_update_rebound(gui_obj_t *obj)
{
    gui_page_update_att(obj);

    obj->h = gui_get_screen_height();
    gui_page_set_height(obj, obj);

    touch_info_t *tp = tp_get_info();
    touch_info_t *wheel = wheel_get_info();
    gui_page_t *this = (gui_page_t *)obj;

    if (obj->parent->matrix && obj->parent->matrix->m[1][2] != 0)
    {
        return;
    }

    if (tp->pressed && gui_obj_point_in_obj_rect(obj, tp->x, tp->y))
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
            gui_obj_enable_event(obj, GUI_EVENT_8);

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
            int max_speed = GUI_PAGE_MAX_SPEED;
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
                gui_obj_enable_event(obj, GUI_EVENT_8);
            }
            else
            {
                if (this->align_hight > 0 && obj->y % this->align_hight != 0)
                {
                    this->release = true;
                    if (_UI_ABS(obj->y % this->align_hight) < this->align_hight / 2)
                    {
                        this->target = obj->y - (obj->y % this->align_hight);
                        this->speed = GUI_PAGE_MIN_SPEED;
                    }
                    else
                    {
                        this->target = obj->y - this->align_hight + _UI_ABS(obj->y % this->align_hight);
                        this->speed = -GUI_PAGE_MIN_SPEED;
                    }
                }
                gui_obj_enable_event(obj, GUI_EVENT_7);
            }
        }
        else if (tp->pressed)
        {
            this->release = false;
            this->speed = 0;
            this->status = 0;
            memset(this->recode, 0, 10);
            gui_obj_enable_event(obj, GUI_EVENT_7);
        }
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
                    gui_obj_enable_event(obj, GUI_EVENT_7);
                }
                else if (obj->y < (this->start_y - (obj->h - (int)gui_get_screen_height())) &&
                         obj->y != 0)/*@BOTTOM*/
                {
                    this->release = false;
                    obj->y = this->start_y - (obj->h - (int)gui_get_screen_height());
                    gui_obj_enable_event(obj, GUI_EVENT_7);
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
                    obj->x = this->start_x;
                    this->release = false;
                    gui_obj_enable_event(obj, GUI_EVENT_7);
                }
                else if (obj->y < (this->start_y - (obj->h - (int)gui_get_screen_height())) &&
                         obj->y != 0)/*@BOTTOM*/
                {
                    this->release = false;
                    obj->y = this->start_y - (obj->h - (int)gui_get_screen_height());
                    gui_obj_enable_event(obj, GUI_EVENT_7);
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
                    gui_obj_enable_event(obj, GUI_EVENT_7);
                }
                else if (obj->y > (this->start_y - (obj->h - (int)gui_get_screen_height())) &&
                         obj->y != 0)/*@BOTTOM*/
                {
                    this->release = false;
                    obj->y = this->start_y - (obj->h - (int)gui_get_screen_height());
                    gui_obj_enable_event(obj, GUI_EVENT_7);

                }
            }
            break;

        default:
            break;
        }
        this->yold = obj->y;
    }
    uint8_t last = this->checksum;
    this->checksum = 0;
    this->checksum = gui_obj_checksum(0, (uint8_t *)this, sizeof(gui_page_t));
    if (last != this->checksum)
    {
        gui_fb_change();
    }
}
void gui_page_update_rebound_x(gui_obj_t *obj)
{
    gui_page_update_att(obj);

    obj->w = gui_get_screen_width();
    gui_page_set_width(obj, obj);

    touch_info_t *tp = tp_get_info();
    touch_info_t *wheel = wheel_get_info();
    gui_page_t *this = (gui_page_t *)obj;

    if (obj->parent->matrix && obj->parent->matrix->m[0][2] != 0)
    {
        return;
    }

    if (tp->pressed && gui_obj_point_in_obj_rect(obj, tp->x, tp->y))
    {
        this->press = true;
    }
    if ((tp->y > this->start_y) && (tp->y < this->start_y + obj->h))
    {
        if ((tp->type == TOUCH_HOLD_X) && (this->press == true))
        {
            if (tp->deltaX + tp->x < 0)
            {
                tp->deltaX = -tp->x;
            }

            obj->x = tp->deltaX + this->yold;
            gui_obj_enable_event(obj, GUI_EVENT_8);

            if (obj->x > this->start_x)
            {
                this->status = 1;
            }
            else if (obj->x < (this->start_x - (obj->w - (int)gui_get_screen_width())) &&
                     obj->x != 0)
            {
                this->status = 2;
            }

            gui_page_update_speed_x(obj);

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
            int max_speed = GUI_PAGE_MAX_SPEED;
            this->press = 0;

            if (tp->type == TOUCH_LEFT_SLIDE)
            {
                gui_log("page TOUCH_UP_SLIDE\n");
                if (this->speed == 0)
                {
                    this->speed = -max_speed;
                }
            }

            else if (tp->type == TOUCH_RIGHT_SLIDE)
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
                gui_obj_enable_event(obj, GUI_EVENT_8);
            }
            else
            {
                if (this->align_hight > 0 && obj->x % this->align_hight != 0)
                {
                    this->release = true;
                    if (_UI_ABS(obj->x % this->align_hight) < this->align_hight / 2)
                    {
                        this->target = obj->x - (obj->x % this->align_hight);
                        this->speed = GUI_PAGE_MIN_SPEED;
                    }
                    else
                    {
                        this->target = obj->x - this->align_hight + _UI_ABS(obj->x % this->align_hight);
                        this->speed = -GUI_PAGE_MIN_SPEED;
                    }
                }
                gui_obj_enable_event(obj, GUI_EVENT_7);
            }
        }
        else if (tp->pressed)
        {
            this->release = false;
            this->speed = 0;
            this->status = 0;
            memset(this->recode, 0, 10);
            gui_obj_enable_event(obj, GUI_EVENT_7);
        }
    }
    if (tp->type != TOUCH_HOLD_X)
    {
        switch (this->status)
        {
        case PAGE_REBOUND_NOT:
            {
                if (this->release)
                {
                    gui_page_update_inertial_x(obj);
                    gui_page_update_alien_x(obj);
                }

                if (obj->x > this->start_x)/*@TOP*/
                {
                    obj->x = this->start_x;
                    this->release = false;
                    gui_obj_enable_event(obj, GUI_EVENT_7);
                }
                else if (obj->x < (this->start_x - (obj->w - (int)gui_get_screen_width())) &&
                         obj->x != 0)/*@BOTTOM*/
                {
                    this->release = false;
                    obj->x = this->start_x - (obj->w - (int)gui_get_screen_width());
                    gui_obj_enable_event(obj, GUI_EVENT_7);
                }
            }
            break;

        case PAGE_REBOUND_UP:
            {
                if (this->release)
                {
                    gui_page_update_inertial_x(obj);
                    gui_page_update_alien_x(obj);
                }

                if (obj->x < this->start_x)/*@TOP*/
                {
                    obj->x = this->start_x;
                    this->release = false;
                    gui_obj_enable_event(obj, GUI_EVENT_7);
                }
                else if (obj->x < (this->start_x - (obj->w - (int)gui_get_screen_width())) &&
                         obj->x != 0)/*@BOTTOM*/
                {
                    this->release = false;
                    obj->x = this->start_x - (obj->w - (int)gui_get_screen_width());
                    gui_obj_enable_event(obj, GUI_EVENT_7);
                }
            }
            break;

        case PAGE_REBOUND_DOWN:
            {
                if (this->release)
                {
                    gui_page_update_inertial_x(obj);
                    gui_page_update_alien_x(obj);
                }

                if (obj->x > this->start_x)/*@TOP*/
                {
                    obj->x = this->start_x;
                    this->release = false;
                    gui_obj_enable_event(obj, GUI_EVENT_7);
                }
                else if (obj->x > (this->start_x - (obj->w - (int)gui_get_screen_width())) &&
                         obj->x != 0)/*@BOTTOM*/
                {
                    this->release = false;
                    obj->x = this->start_x - (obj->w - (int)gui_get_screen_width());
                    gui_obj_enable_event(obj, GUI_EVENT_7);

                }
            }
            break;

        default:
            break;
        }
        this->yold = obj->x;
    }
    uint8_t last = this->checksum;
    this->checksum = 0;
    this->checksum = gui_obj_checksum(0, (uint8_t *)this, sizeof(gui_page_t));
    if (last != this->checksum)
    {
        gui_fb_change();
    }
}
static void prepare_rebound(gui_obj_t *obj, bool *take_over, touch_info_t *tp)
{
    gui_page_update_att(obj);

    obj->h = gui_get_screen_height();
    gui_page_set_height(obj, obj);
    gui_page_t *this = (gui_page_t *)obj;

    if (obj->parent->matrix && obj->parent->matrix->m[1][2] != 0)
    {
        return;
    }

    if (tp->pressed && gui_obj_point_in_obj_rect(obj, tp->x, tp->y))
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
            gui_obj_enable_event(obj, GUI_EVENT_8);

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
            int max_speed = GUI_PAGE_MAX_SPEED;
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
                gui_obj_enable_event(obj, GUI_EVENT_8);
            }
            else
            {
                if (this->align_hight > 0 && obj->y % this->align_hight != 0)
                {
                    this->release = true;
                    if (_UI_ABS(obj->y % this->align_hight) < this->align_hight / 2)
                    {
                        this->target = obj->y - (obj->y % this->align_hight);
                        this->speed = GUI_PAGE_MIN_SPEED;
                    }
                    else
                    {
                        this->target = obj->y - this->align_hight + _UI_ABS(obj->y % this->align_hight);
                        this->speed = -GUI_PAGE_MIN_SPEED;
                    }
                }
                gui_obj_enable_event(obj, GUI_EVENT_7);
            }
        }
        else if (tp->pressed)
        {
            this->release = false;
            this->speed = 0;
            this->status = 0;
            memset(this->recode, 0, 10);
            gui_obj_enable_event(obj, GUI_EVENT_7);
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
                        if (this->release == false)
                        {
                            *take_over = 0;
                        }

                    }

                    if (obj->y > this->start_y)/*@TOP*/
                    {
                        obj->y = this->start_y;
                        this->release = false;
                        gui_obj_enable_event(obj, GUI_EVENT_7);
                        *take_over = 0;
                    }
                    else if (obj->y < (this->start_y - (obj->h - (int)gui_get_screen_height())) &&
                             obj->y != 0)/*@BOTTOM*/
                    {
                        this->release = false;
                        obj->y = this->start_y - (obj->h - (int)gui_get_screen_height());
                        gui_obj_enable_event(obj, GUI_EVENT_7);
                        *take_over = 0;
                    }
                }
                break;

            case PAGE_REBOUND_UP:
                {
                    if (this->release)
                    {
                        gui_page_update_inertial(obj);
                        gui_page_update_alien(obj);
                        if (this->release == false)
                        {
                            *take_over = 0;
                        }
                    }

                    if (obj->y < this->start_y)/*@TOP*/
                    {
                        obj->y = this->start_y;
                        this->release = false;
                        gui_obj_enable_event(obj, GUI_EVENT_7);
                        *take_over = 0;
                    }
                    else if (obj->y < (this->start_y - (obj->h - (int)gui_get_screen_height())) &&
                             obj->y != 0)/*@BOTTOM*/
                    {
                        this->release = false;
                        obj->y = this->start_y - (obj->h - (int)gui_get_screen_height());
                        gui_obj_enable_event(obj, GUI_EVENT_7);
                        *take_over = 0;
                    }
                }
                break;

            case PAGE_REBOUND_DOWN:
                {
                    if (this->release)
                    {
                        gui_page_update_inertial(obj);
                        gui_page_update_alien(obj);
                        if (this->release == false)
                        {
                            *take_over = 0;
                        }
                    }

                    if (obj->y > this->start_y)/*@TOP*/
                    {
                        obj->y = this->start_y;
                        this->release = false;
                        gui_obj_enable_event(obj, GUI_EVENT_7);
                        *take_over = 0;
                    }
                    else if (obj->y > (this->start_y - (obj->h - (int)gui_get_screen_height())) &&
                             obj->y != 0)/*@BOTTOM*/
                    {
                        this->release = false;
                        obj->y = this->start_y - (obj->h - (int)gui_get_screen_height());
                        gui_obj_enable_event(obj, GUI_EVENT_7);
                        *take_over = 0;

                    }
                }
                break;

            default:
                break;
            }
            this->yold = obj->y;
        }
    }
}
static void prepare_rebound_wheel(gui_obj_t *obj, touch_info_t *tp)
{
    gui_page_update_att(obj);

    obj->h = gui_get_screen_height();
    gui_page_set_height(obj, obj);
    gui_page_t *this = (gui_page_t *)obj;

    if (obj->parent->matrix && obj->parent->matrix->m[1][2] != 0)
    {
        return;
    }
    if (tp->pressed)
    {
        this->speed = 0;
        this->release = 1;
    }
    if (!this->release)
    {
        tp->history_y = obj->y;
        return;
    }

    if (tp->pressing)
    {

        obj->y = tp->history_y;
        gui_obj_enable_event(obj, GUI_EVENT_8);
        if (obj->y > this->start_y)
        {
            this->status = 1;
            this->speed = -GUI_PAGE_MAX_SPEED;
        }
        else if (obj->y < (this->start_y - (obj->h - (int)gui_get_screen_height())) &&
                 obj->y != 0)
        {
            this->status = 2;
            this->speed = GUI_PAGE_MAX_SPEED;
        }

    }
    else
    {
        if (this->speed != 0)
        {
            gui_page_update_inertial(obj);
            if (this->speed < 0 && obj->y <= this->start_y)
            {
                obj->y = this->start_y;
                this->speed = 0;
                tp->history_y = obj->y;
                this->release = 0;
            }
            if (this->speed > 0 && obj->y > (this->start_y - (obj->h - (int)gui_get_screen_height())))
            {
                obj->y = (this->start_y - (obj->h - (int)gui_get_screen_height()));
                this->speed = 0;
                tp->history_y = obj->y ;
                this->release = 0;
            }

        }

    }


}
void gui_page_prepare_rebound(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    touch_info_t *wheel = wheel_get_info();
    static bool tp_over;
    if (tp->pressed)
    {
        tp_over = 1;
    }
    if (tp_over)
    {
        prepare_rebound(obj, &tp_over, tp);
    }
    else
    {
        prepare_rebound_wheel(obj, wheel);
    }



}
void gui_page_destroy(gui_obj_t *obj)
{
    gui_page_t *this = (gui_page_t *)obj;
    if (this->animate)
    {
        gui_free(this->animate);
        this->animate = NULL;
    }
}

static void gui_page_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            gui_page_input_prepare(obj);
            break;

        case OBJ_PREPARE:
            gui_page_update(obj);
            break;

        case OBJ_DESTROY:
            gui_page_destroy(obj);
            break;

        default:
            break;
        }
    }
}
static void gui_page_cb_x(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            gui_page_input_prepare(obj);
            break;

        case OBJ_PREPARE:
            gui_page_update_x(obj);
            break;

        case OBJ_DESTROY:
            gui_page_destroy(obj);
            break;

        default:
            break;
        }
    }
}
static void gui_page_loop_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            gui_page_input_prepare(obj);
            break;

        case OBJ_PREPARE:
            gui_page_loop_update(obj);
            break;

        case OBJ_DESTROY:
            gui_page_destroy(obj);
            break;

        default:
            break;
        }
    }
}
static void gui_page_rebound_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            gui_page_input_prepare(obj);
            break;

        case OBJ_PREPARE:

            gui_page_update_rebound(obj);
            break;

        case OBJ_DESTROY:
            gui_page_destroy(obj);
            break;

        default:
            break;
        }
    }
}
static void gui_page_rebound_cb_x(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            gui_page_input_prepare(obj);
            break;

        case OBJ_PREPARE:

            gui_page_update_rebound_x(obj);
            break;

        case OBJ_DESTROY:
            gui_page_destroy(obj);
            break;

        default:
            break;
        }
    }
}
void gui_page_ctor(gui_page_t *this,
                   gui_obj_t  *parent,
                   const char *name,
                   int16_t     x,
                   int16_t     y,
                   int16_t     w,
                   int16_t     h)
{
    gui_obj_ctor(&this->base, parent, name, x, y, w, h);
    GET_BASE(this)->type = PAGE;
    GET_BASE(this)->obj_cb = gui_page_cb;
    GET_BASE(this)->has_input_prepare_cb = true;
    GET_BASE(this)->has_prepare_cb = true;
    GET_BASE(this)->has_destroy_cb = true;
    this->base.type = PAGE;
    this->start_x = x;
    this->start_y = y;
}

gui_page_t *gui_page_create(void       *parent,
                            const char *name,
                            int16_t     x,
                            int16_t     y,
                            int16_t     w,
                            int16_t     h)
{
#define _GUI_NEW_gui_page_create_param this, parent, name, x, y, w, h
    GUI_NEW(gui_page_t, gui_page_ctor, _GUI_NEW_gui_page_create_param)
}
gui_page_t *gui_page_create_horizontal(void       *parent,
                                       const char *name,
                                       int16_t     x,
                                       int16_t     y,
                                       int16_t     w,
                                       int16_t     h)
{
    gui_page_t *page = gui_page_create(parent, name, x, y, w, h);
    GUI_BASE(page)->obj_cb = gui_page_cb_x;
    return page;
}
void gui_page_rebound(gui_page_t *this, bool rebound)
{
    if (rebound)
    {
        GET_BASE(this)->obj_cb = gui_page_rebound_cb;
        GET_BASE(this)->has_input_prepare_cb = true;
        GET_BASE(this)->has_prepare_cb = true;
        GET_BASE(this)->has_destroy_cb = true;
    }
    else
    {
        GET_BASE(this)->obj_cb = gui_page_cb;
        GET_BASE(this)->has_input_prepare_cb = true;
        GET_BASE(this)->has_prepare_cb = true;
        GET_BASE(this)->has_destroy_cb = true;
    }
}
void gui_page_rebound_horizontal(gui_page_t *this, bool rebound)
{
    if (rebound)
    {
        GET_BASE(this)->obj_cb = gui_page_rebound_cb_x;
        GET_BASE(this)->has_input_prepare_cb = true;
        GET_BASE(this)->has_prepare_cb = true;
        GET_BASE(this)->has_destroy_cb = true;
    }
    else
    {
        GET_BASE(this)->obj_cb = gui_page_cb_x;
        GET_BASE(this)->has_input_prepare_cb = true;
        GET_BASE(this)->has_prepare_cb = true;
        GET_BASE(this)->has_destroy_cb = true;
    }
}
void gui_page_loop(gui_page_t *this)
{
    GET_BASE(this)->obj_cb = gui_page_loop_cb;
    GET_BASE(this)->has_input_prepare_cb = true;
    GET_BASE(this)->has_prepare_cb = true;
    GET_BASE(this)->has_destroy_cb = true;
}
void gui_page_center_alignment(gui_page_t *this, int align_hight)
{
    this->align_hight = align_hight;
}

void gui_page_set_only_top_slide(gui_page_t *this, bool flag)
{
    this->top_slide_only = flag;
}

