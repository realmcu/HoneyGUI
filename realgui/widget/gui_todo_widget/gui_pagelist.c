/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_pagelist.c
  * @brief pagelist widget
  * @details A ram friendly page widget to show large amount of file lists.
  *          Please refer to demo "example\screen_454_454_bb2plus_watch\gui_record\pagelist_test.c"
  * @author haonan_shi@realsil.com
  * @date 2023/12/15
  * @version v0.1
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include "gui_obj.h"
#include "tp_algo.h"
#include "gui_pagelistview.h"
#include "gui_pagelist.h"
#include "gui_switch.h"

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

static void gui_pagelist_input_prepare(gui_obj_t *obj)
{

}
static void gui_pagelist_prepare(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    gui_pagelist_t *this = (gui_pagelist_t *)obj;

    //set pagelist h according to list_gap_y real_list_max
    if (!this->h_set_done)
    {
        if ((this->list_first != NULL) && (this->list_last != NULL))
        {
            obj->h = (int)gui_get_screen_height() + (this->list_first->base.h + this->list_gap_y) *
                     this->real_list_max;
        }
        this->h_set_done = true;
        gui_log("obj->h = %d\n", obj->h);
    }

    if (obj->parent->y/*todo*/ != 0)
    {
        return;
    }

    if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true)
    {
        // valid touch range xy
        if ((tp->x > this->x_init) && (tp->x < (this->x_init + obj->w)))
        {
            if ((tp->y > this->y_init) && tp->y < (this->y_init + obj->h))
            {
                //cal axis of pagelist
                //gui_log("root->y = %d, tp->y = %d, tp->deltaY = %d, this->y_stop_scroll = %d, this->y_init = %d\n", root->y, tp->y , tp->deltaY, this->y_stop_scroll, this->y_init);
                if (tp->type == TOUCH_HOLD_Y)
                {
                    //update pagelist y
                    obj->y = tp->deltaY + this->y_stop_scroll;
                }
                else
                {
                    //update y_stop_scroll
                    this->y_stop_scroll = obj->y;
                }

                if ((obj->y > this->y_init) || (obj->h <= (int)gui_get_screen_width())) //this.y_init
                {
                    //pagelist reach top
                    obj->y = this->y_init;
                }
                else if (obj->h > (int)gui_get_screen_width())
                {
                    if (obj->y < (0 - (obj->h - (int)gui_get_screen_width() - ((int)gui_get_screen_width() -
                                                                               this->show_border_bottom) - this->list_gap_y)))//0 - root.h
                    {
                        //pagelist reach bottom
                        obj->y = 0 - (obj->h - (int)gui_get_screen_width() - ((int)gui_get_screen_width() -
                                                                              this->show_border_bottom) - this->list_gap_y);
                    }
                }

                if (this->list_first != NULL)
                {
                    gui_obj_t *list_next = NULL;
                    int16_t ay_list_first = this->base.y/*todo*/ + this->list_first->base.y;
                    this->list_first->touch_disable = false;

                    if ((ay_list_first + this->list_first->base.h) < this->show_border_top)
                    {
                        this->list_first->touch_disable = true;

                        if ((tp->deltaY < 0) && (this->list_count < this->real_list_max))
                        {
                            if ((ay_list_first + this->list_first->base.h + this->list_gap_y)
                                < this->show_border_top)
                            {
                                gui_log("up:  this->list_count = %d\n", this->list_count);
                                gui_log("up:  this->list_first y = %d ay_list_first = %d\n",
                                        this->list_first->base.y,
                                        ay_list_first);
                                //find next list obj
                                list_next = gui_list_entry(this->list_first->base.brother_list.next,
                                                           gui_obj_t,
                                                           brother_list);

                                //judge if node is parent
                                //assume we have 4 widget gui_switches
                                //gui_switch[0] brother list: 1->2->3->parent(offset child_list)->0->1....
                                if (gui_list_entry(this->list_first->base.brother_list.next, gui_obj_t,
                                                   child_list) == this->list_first->base.parent)
                                {
                                    list_next = gui_list_entry(list_next->brother_list.next,
                                                               gui_obj_t,
                                                               brother_list);
                                }

                                this->update_list_first_cb(this, this->list_first);

                                //move first list to bottom
                                this->list_first->base.y = this->list_last->base.y + this->list_last->base.h + this->list_gap_y;
                                this->list_last = this->list_first;
                                this->list_first = (gui_switch_t *)list_next;
                                this->list_count++;

                                if (this->list_count > this->real_list_max)
                                {
                                    this->list_count = this->real_list_max;
                                }
                                gui_log("up2:  this->list_last y = %d \n", this->list_last->base.y);
                            }
                        }
                    }
                }

                //cal axis of list and scroll bar
                if (this->list_last != NULL)
                {
                    gui_obj_t *list_prev = NULL;
                    int16_t ay_list_last = this->base.y/*todo*/ + this->list_last->base.y;
                    this->list_last->touch_disable = false;

                    if (ay_list_last > this->show_border_bottom)
                    {
                        this->list_last->touch_disable = true;

                        if ((tp->deltaY > 0) && (this->list_count > this->show_list_max))
                        {
                            if (ay_list_last > (this->show_border_bottom + this->list_gap_y))
                            {
                                gui_log("down:  this->list_count = %d\n", this->list_count);
                                gui_log("down:  this->list_last y = %d ay_list_last = %d\n",
                                        this->list_last->base.y, ay_list_last);
                                //find previous list obj
                                list_prev = gui_list_entry(this->list_last->base.brother_list.prev,
                                                           gui_obj_t,
                                                           brother_list);

                                //judge if node is parent
                                //assume we have 4 widget gui_switches
                                //gui_switch[0] brother list: 1->2->3->parent(offset child_list)->0->1....
                                if (gui_list_entry(this->list_last->base.brother_list.prev, gui_obj_t,
                                                   child_list) == this->list_last->base.parent)
                                {
                                    list_prev = gui_list_entry(list_prev->brother_list.prev,
                                                               gui_obj_t,
                                                               brother_list);
                                }

                                this->update_list_last_cb(this, this->list_last);

                                //move last list to top
                                this->list_last->base.y = this->list_first->base.y - this->list_gap_y - this->list_last->base.h;
                                this->list_first = this->list_last;
                                this->list_last = (gui_switch_t *)list_prev;
                                this->list_count--;

                                if (this->list_count < this->show_list_max)
                                {
                                    this->list_count = this->show_list_max;
                                }
                                gui_log("down2:  this->list_first y = %d\n", this->list_first->base.y);
                            }
                        }
                    }
                }

                if (this->scroll_bar != NULL)
                {
                    //todo: move scroll bar
                }
            }
        }
    }
}

static void gui_pagelist_draw(gui_pagelist_t *this)
{

}
static void gui_pagelist_end(gui_pagelist_t *this)
{

}
static void gui_pagelist_destroy(gui_pagelist_t *this)
{

}

static void gui_pagelist_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            gui_pagelist_input_prepare(obj);
            break;
        case OBJ_PREPARE:
            gui_pagelist_prepare(obj);
            break;

        case OBJ_DRAW:
            gui_pagelist_draw((gui_pagelist_t *)obj);
            break;

        case OBJ_END:
            gui_pagelist_end((gui_pagelist_t *)obj);
            break;

        case OBJ_DESTROY:
            gui_pagelist_destroy((gui_pagelist_t *)obj);
            break;

        default:
            break;
        }
    }
}

static void gui_pagelist_ctor(gui_pagelist_t *this,
                              gui_obj_t      *parent,
                              const char     *name,
                              int16_t         x,
                              int16_t         y,
                              int16_t         w,
                              int16_t         h)
{
    //for root class
    gui_obj_t *root = (gui_obj_t *)this;
    gui_obj_ctor(root, parent, name, x, y, w, h);

    root->obj_cb = gui_pagelist_cb;
    root->has_input_prepare_cb = true;
    root->has_prepare_cb = true;
    root->has_draw_cb = true;
    root->has_end_cb = true;
    root->has_destroy_cb = true;

    //for self
    this->base.type = PAGELIST;
    this->x_init = x;
    this->y_init = y;

    gui_pagelistview_t *obj_parent = (gui_pagelistview_t *)parent;
    if (obj_parent->base.type == PAGELISTVIEW)
    {
        this->show_border_bottom = obj_parent->bottom_mask_border;
        this->show_border_top = obj_parent->top_mask_border;
    }
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void gui_pagelist_add_scroll_bar(gui_pagelist_t *this, void *bar_pic, IMG_SOURCE_MODE_TYPE src_mode)
{
    if (src_mode == IMG_SRC_MEMADDR)
    {
        this->src_mode = IMG_SRC_MEMADDR;
        this->scroll_bar = gui_img_create_from_mem(this->base.parent, "scroll_bar", bar_pic, 0, 0, 0, 0);
    }
    else
    {
        this->src_mode = IMG_SRC_FILESYS;
        this->scroll_bar =  gui_img_create_from_fs(this->base.parent, "scroll_bar", bar_pic, 0, 0, 0, 0);;
    }
    gui_img_get_height(this->scroll_bar);
    this->scroll_bar->base.x = this->base.w - 3 - this->scroll_bar->base.w;
}

void gui_pagelist_increase_real_list_max(gui_pagelist_t *this, int16_t real_list_max)
{
    if (real_list_max > 0)
    {
        this->real_list_max += real_list_max;

        if (this->real_list_max > 256)
        {
            this->real_list_max = 256;
        }
        this->h_set_done = false;
    }
}

void gui_pagelist_decrease_real_list_max(gui_pagelist_t *this, int16_t real_list_max)
{
    if (real_list_max > 0)
    {
        this->real_list_max -= real_list_max;
        if (this->real_list_max < 0)
        {
            this->real_list_max = 0;
        }
        if (this->real_list_max < this->show_list_max)
        {
            this->show_list_max = this->real_list_max;
        }
        if (this->list_count >= this->real_list_max)
        {
            this->list_count = this->real_list_max;
        }
        this->h_set_done = false;
    }
}

void gui_pagelist_set_att(gui_pagelist_t *this,
                          int16_t         real_list_max,
                          int16_t         show_list_max,
                          int16_t         list_gap_y,
                          gui_switch_t   *list_first,
                          gui_switch_t   *list_last)
{
    this->real_list_max = real_list_max;
    this->show_list_max = show_list_max;
    this->list_gap_y = list_gap_y;

    if (list_first != NULL)
    {
        this->list_first = list_first;
    }

    if (list_last != NULL)
    {
        this->list_last = list_last;
    }
    this->list_count = show_list_max;
    this->h_set_done = false;
}

void gui_pagelist_add_list_update_cb(gui_pagelist_t          *this,
                                     gui_pagelist_update_cb_t update_list_first_cb,
                                     gui_pagelist_update_cb_t update_list_last_cb)
{
    if (this != NULL)
    {
        this->update_list_first_cb = update_list_first_cb;
        this->update_list_last_cb = update_list_last_cb;
    }
}

/**
 * @brief
 *
 * @param parent
 * @param name
 * @param data
 * @param x
 * @param y
 * @param w
 * @param h
 * @return gui_pagelist_t*
 */
gui_pagelist_t *gui_pagelist_create(void       *parent,
                                    const char *name,
                                    int16_t     x,
                                    int16_t     y,
                                    int16_t     w,
                                    int16_t     h)
{
    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "PAGELIST";
    }
    gui_pagelist_t *this = gui_malloc(sizeof(gui_pagelist_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_pagelist_t));

    gui_pagelist_ctor(this, (gui_obj_t *)parent, name, x, y, w, h);

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }

    GET_BASE(this)->create_done = true;
    return this;
}
