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
// static void deal_img_in_root(gui_obj_t *object, int ayend, int *out)
// {
//     gui_list_t *node = NULL;
//     gui_list_for_each(node, &object->child_list)
//     {
//         gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
//         if (!obj->not_show)
//         {
//             obj->ax = obj->x + obj->parent->ax;
//             obj->ay = obj->y + obj->parent->ay;
//             if (ayend < obj->ay + obj->h) { ayend = obj->ay + obj->h; }
//             if (*out < ayend)
//             {
//                 *out = ayend;
//             }
//         }
//         deal_img_in_root(obj, ayend, out);
//     }
// }
static void page_height(gui_obj_t *object, gui_obj_t *page)
{
    gui_list_t *node = NULL;
    gui_list_for_each(node, &object->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (!obj->not_show)
        {
            obj->ay = obj->y + obj->parent->ay;
            int buttom = obj->ay + obj->h - page->ay;
            //gui_log("buttom:%d\n",buttom);
            if (page->h < buttom)
            {
                page->h = buttom;
            }

        }
        page_height(obj, page);
    }
}
static void gui_page_add_scroll_bar(gui_page_t *this, void *bar_pic)
{
    this->src_mode = IMG_SRC_MEMADDR;
    this->scroll_bar = gui_img_create_from_mem(this->base.parent, "scroll_bar", bar_pic, 0, 0, 0, 0);
    gui_img_get_height(this->scroll_bar);
    this->scroll_bar->base.x = this->base.w - 3 - this->scroll_bar->base.w;
}
static void gui_page_add_scroll_bar_from_fs(gui_page_t *this, void *bar_pic)
{
    this->src_mode = IMG_SRC_FILESYS;
    this->scroll_bar = gui_img_create_from_fs(this->base.parent, bar_pic, 0, 0);
    gui_img_get_height(this->scroll_bar);
    this->scroll_bar->base.x = this->base.w - 3 - this->scroll_bar->base.w;
}
static void set_offset(gui_page_t *this, int offset)
{
    this->yold = offset;
    this->base.y = offset;
}

static int get_offset(gui_page_t *this)
{
    return this->base.y;
}

_gui_api_page_t gui_page_api =
{
    .set_offset = set_offset,
    .get_offset = get_offset,
    .gui_page_add_scroll_bar = gui_page_add_scroll_bar,
    .gui_page_add_scroll_bar_from_fs = gui_page_add_scroll_bar_from_fs,
};

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
static void (obj_update_att)(struct _gui_obj_t *o)
{
    gui_page_t *obj = (void *)o;
    if (obj->animate && obj->animate->animate)
    {
        size_t frame_count = obj->animate->dur * (1000 / 15) / (1000);
        obj->animate->callback(obj->animate->p);
        obj->animate->current_frame++;

        if (obj->animate->current_frame > frame_count)
        {
            if (obj->animate->repeatCount == 0)
            {
                obj->animate->animate = false;
            }
            else if (obj->animate->repeatCount < 0)
            {
                obj->animate->current_frame = 0;
            }
            else if (obj->animate->repeatCount > 0)
            {
                obj->animate->current_repeat_count++;
                if (obj->animate->current_repeat_count >= obj->animate->repeatCount)
                {
                    obj->animate->animate = false;
                }
                else
                {
                    obj->animate->current_frame = 0;
                }
            }
        }
        obj->animate->progress_percent = ((float)(obj->animate->current_frame)) / ((float)(
                                                                                       frame_count));

    }
}
void gui_page_set_animate(gui_page_t *o, uint32_t dur, int repeatCount, void *callback, void *p)
{
    gui_animate_t *animate = ((gui_page_t *)o)->animate;
    if (!(animate))
    {
        animate = gui_malloc(sizeof(gui_animate_t));
    }
    memset((animate), 0, sizeof(gui_animate_t));
    animate->animate = true;
    animate->dur = dur;
    animate->callback = (void (*)(void *))callback;
    animate->repeatCount = repeatCount;
    animate->p = p;
    ((gui_page_t *)o)->animate = animate;
}
//static void center_alignment(gui_page_t *page)
//{
//    if (page->align_hight)
//    {
//        gui_obj_t *obj = GET_BASE(page);
//        int alien = _UI_ABS(obj->y) % page->align_hight;
//        if (alien > page->align_hight / 2)
//        {
//            page->target = obj->y - page->align_hight + alien;
//        }
//        else
//        {
//            page->target = obj->y + alien;
//        }

//    }


//}
void page_update(gui_obj_t *obj)
{
    obj_update_att(obj);
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    // int ay = 0;
    // deal_img_in_root(obj, obj->y + obj->h, &ay);
    // obj->h = ay - obj->y;
    obj->h = gui_get_screen_height();
    page_height(obj, obj);
    gui_page_t *page = (gui_page_t *)obj;
    //gui_log("obj->y:%d,speed:%d,type%d,%d,%d,%d\n",obj->y, GUI_TYPE(gui_page_t, obj)->speed, tp->type,tp->deltaY,tp->released,tp->y);
    if (obj->parent->ay != 0)
    {
        return;
    }

    if ((obj->ax + obj->tx < (int)gui_get_screen_width()) && ((obj->ax + obj->tx + obj->w) >= 0) && \
        (obj->ay + obj->ty < (int)gui_get_screen_height()) && ((obj->ay + obj->ty + obj->h) >= 0))
    {
        if ((tp->x > ((gui_page_t *)obj)->start_x) && (tp->x < ((gui_page_t *)obj)->start_x + obj->w))
        {
            if ((tp->y > ((gui_page_t *)obj)->start_y) && (tp->y < ((gui_page_t *)obj)->start_y + obj->h))
            {
                if ((tp->type == TOUCH_HOLD_Y))
                {
                    // gui_log("obj->y:%d,%d",obj->y, ((gui_page_t *)obj)->start_y-(obj->h-gui_get_screen_height()));
                    obj->y = tp->deltaY + ((gui_page_t *)obj)->yold;
                    if (obj->y > ((gui_page_t *)obj)->start_y)
                    {
                        obj->y = ((gui_page_t *)obj)->start_y;

                    }
                    else if (obj->y < (((gui_page_t *)obj)->start_y - (obj->h - (int)gui_get_screen_height())) &&
                             obj->y != 0)
                    {
                        // gui_log("obj->yyyy:%d,%d",obj->y, ((gui_page_t *)obj)->start_y-(obj->h-(int)gui_get_screen_height()));
                        obj->y = ((gui_page_t *)obj)->start_y - (obj->h - (int)gui_get_screen_height());
                    }
                    int recode_num = 4;
                    for (size_t i = 0; i < recode_num; i++)
                    {
                        page->recode[i] = page->recode[i + 1];
                    }
                    page->recode[recode_num] = tp->deltaY;
                    GUI_TYPE(gui_page_t, obj)->speed = page->recode[recode_num] - page->recode[0];
                    int max_speed = 60;
                    int min_speed = 7;
                    if (GUI_TYPE(gui_page_t, obj)->speed > max_speed)
                    {
                        GUI_TYPE(gui_page_t, obj)->speed = max_speed;
                    }
                    else if (GUI_TYPE(gui_page_t, obj)->speed < -max_speed)
                    {
                        GUI_TYPE(gui_page_t, obj)->speed = -max_speed;
                    }
                    if (GUI_TYPE(gui_page_t, obj)->speed > 0 && GUI_TYPE(gui_page_t, obj)->speed < min_speed)
                    {
                        GUI_TYPE(gui_page_t, obj)->speed = min_speed;
                    }
                    else if (GUI_TYPE(gui_page_t, obj)->speed < 0 && GUI_TYPE(gui_page_t, obj)->speed > -min_speed)
                    {
                        GUI_TYPE(gui_page_t, obj)->speed = -min_speed;
                    }
                    GUI_TYPE(gui_page_t, obj)->target = (GUI_TYPE(gui_page_t, obj)->speed * GUI_TYPE(gui_page_t,
                                                         obj)->speed + _UI_ABS(GUI_TYPE(gui_page_t, obj)->speed)) / 2;

                    int top_height = -obj->y;
                    int buttom_height = obj->h + obj->y - (int)gui_get_screen_height();
                    if (GUI_TYPE(gui_page_t, obj)->speed > 0)
                    {
                        if (GUI_TYPE(gui_page_t, obj)->target > top_height)
                        {
                            GUI_TYPE(gui_page_t, obj)->target = top_height;
                        }
                        GUI_TYPE(gui_page_t, obj)->target = obj->y + GUI_TYPE(gui_page_t, obj)->target;

                    }
                    else if (GUI_TYPE(gui_page_t, obj)->speed < 0)
                    {
                        if (GUI_TYPE(gui_page_t, obj)->target > buttom_height)
                        {
                            GUI_TYPE(gui_page_t, obj)->target = buttom_height;
                        }
                        GUI_TYPE(gui_page_t, obj)->target = obj->y - GUI_TYPE(gui_page_t, obj)->target;
                    }
                    else
                    {
                        GUI_TYPE(gui_page_t, obj)->target = obj->y;
                    }


                    if (page->align_hight > 0)
                    {
                        int alien = _UI_ABS(GUI_TYPE(gui_page_t, obj)->target) % page->align_hight;
                        if (alien > page->align_hight / 2)
                        {
                            page->target = page->target - page->align_hight + alien;
                            if (_UI_ABS(page->speed) == 7 || _UI_ABS(page->speed) == 0)
                            {
                                /* code */


                                if (page->target > obj->y)
                                {
                                    page->speed = 7;
                                }
                                else
                                {
                                    page->speed = -7;
                                }
                            }

                        }
                        else
                        {
                            page->target = page->target + alien;
                            if (_UI_ABS(page->speed) == 7 || _UI_ABS(page->speed) == 0)
                            {
                                /* code */


                                if (page->target > obj->y)
                                {
                                    page->speed = 7;
                                }
                                else
                                {
                                    page->speed = -7;
                                }
                            }
                        }
                    }



                    gui_obj_event_set(obj, GUI_EVENT_8);

                }
                else if (tp->released)
                {
                    if (tp->type == TOUCH_UP_SLIDE)
                    {
                        gui_log("page TOUCH_UP_SLIDE\n");
                    }
                    else if (tp->type == TOUCH_DOWN_SLIDE)
                    {
                        gui_log("page TOUCH_DOWN_SLIDE\n");
                    }
                    if (GUI_TYPE(gui_page_t, obj)->speed != 0)
                    {
                        GUI_TYPE(gui_page_t, obj)->release = true;


                    }
                    else if (GUI_TYPE(gui_page_t, obj)->speed == 0)
                    {
                        gui_obj_event_set(obj, GUI_EVENT_7);
                    }
                }
                else if (tp->pressed)
                {
                    GUI_TYPE(gui_page_t, obj)->release = false;
                    GUI_TYPE(gui_page_t, obj)->speed = 0;
                    memset(page->recode, 0, 10);
                }
                else
                {
                    if (GUI_TYPE(gui_page_t, obj)->release)
                    {
                        ////gui_log("target:%d,%d,%d\n",GUI_TYPE(gui_page_t, obj)->target,page->speed,obj->y);
                        //gui_log("%d\n",__LINE__);
                        if (GUI_TYPE(gui_page_t, obj)->speed == 0)
                        {
                            GUI_TYPE(gui_page_t, obj)->release = false;

                        }
                        ////gui_log("%d,%d\n",obj->y, GUI_TYPE(gui_page_t, obj)->speed);

                        if (GUI_TYPE(gui_page_t, obj)->speed > 3)
                        {
                            obj->y += GUI_TYPE(gui_page_t, obj)->speed;
                            GUI_TYPE(gui_page_t, obj)->speed -= 1;//gui_log("%d\n",__LINE__);
                        }
                        else if (GUI_TYPE(gui_page_t, obj)->speed < -3)
                        {
                            obj->y += GUI_TYPE(gui_page_t, obj)->speed;
                            GUI_TYPE(gui_page_t, obj)->speed += 1;//gui_log("%d\n",__LINE__);
                        }
                        {

                            if (GUI_TYPE(gui_page_t, obj)->speed > 0 && obj->y >= page->target)
                            {
                                GUI_TYPE(gui_page_t, obj)->speed = 0;
                                obj->y = page->target; //gui_log("%d\n",__LINE__);
                            }
                            if (GUI_TYPE(gui_page_t, obj)->speed < 0 && obj->y <= page->target)
                            {
                                GUI_TYPE(gui_page_t, obj)->speed = 0;
                                obj->y = page->target; //gui_log("%d\n",__LINE__);
                            }

                        }
                        if (page->align_hight > 0 && GUI_TYPE(gui_page_t, obj)->speed <= 3 &&
                            GUI_TYPE(gui_page_t, obj)->speed >= -3)
                        {
                            gui_obj_event_set(obj, GUI_EVENT_7);
                            if (GUI_TYPE(gui_page_t, obj)->speed > 0 && obj->y >= page->target)
                            {
                                GUI_TYPE(gui_page_t, obj)->speed = 0;
                                obj->y = page->target; //gui_log("%d\n",__LINE__);
                            }
                            if (GUI_TYPE(gui_page_t, obj)->speed < 0 && obj->y <= page->target)
                            {
                                GUI_TYPE(gui_page_t, obj)->speed = 0;
                                obj->y = page->target; //gui_log("%d\n",__LINE__);
                            }
                            obj->y += GUI_TYPE(gui_page_t, obj)->speed;  //gui_log("%d\n",__LINE__);
                        }
                        else
                        {
                            gui_obj_event_set(obj, GUI_EVENT_8);
                        }
//gui_log("target:%d,%d,%d\n",GUI_TYPE(gui_page_t, obj)->target,page->speed,obj->y);

                    }
                    if (obj->y > ((gui_page_t *)obj)->start_y)
                    {
                        obj->y = ((gui_page_t *)obj)->start_y;
                        GUI_TYPE(gui_page_t, obj)->release = false;
                        gui_obj_event_set(obj, GUI_EVENT_7);
                    }
                    else if (obj->y < (((gui_page_t *)obj)->start_y - (obj->h - (int)gui_get_screen_height())) &&
                             obj->y != 0)
                    {
                        GUI_TYPE(gui_page_t, obj)->release = false;
                        // gui_log("obj->yyyy:%d,%d",obj->y, ((gui_page_t *)obj)->start_y-(obj->h-(int)gui_get_screen_height()));
                        obj->y = ((gui_page_t *)obj)->start_y - (obj->h - (int)gui_get_screen_height());
                        gui_obj_event_set(obj, GUI_EVENT_7);
                    }
                    ((gui_page_t *)obj)->yold = obj->y;
                }

            }
        }


        if (((gui_page_t *)obj)->scroll_bar)
        {
            // ((gui_page_t *)obj)->scroll_bar->base.y = ((((gui_page_t *)obj)->start_y - obj->y) *
            //                                            gui_get_screen_height() / obj->h);
        }
        // gui_log("obj->y:%d,%d, %d\n", obj->y, obj->ay, obj->parent->ay);
        if (obj->y == ((gui_page_t *)obj)->start_y)
        {
            obj->cover = false;
        }
        else
        {
            obj->cover = true;
        }
    }



}
static void page_update_rebound(gui_obj_t *obj)
{
    obj_update_att(obj);
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    // int ay = 0;
    // deal_img_in_root(obj, obj->y + obj->h, &ay);
    // obj->h = ay - obj->y;
    obj->h = gui_get_screen_height();
    page_height(obj, obj);
    if (obj->parent->ay != 0)
    {
        return;
    }
    gui_page_t *page = (gui_page_t *)obj;
    if ((obj->ax + obj->tx < (int)gui_get_screen_width()) && ((obj->ax + obj->tx + obj->w) >= 0))
    {
        if ((tp->x > ((gui_page_t *)obj)->start_x) && (tp->x < ((gui_page_t *)obj)->start_x + obj->w))
        {
            //if ((tp->y > ((gui_page_t *)obj)->start_y) && (tp->y < ((gui_page_t *)obj)->start_y + obj->h))
            {
                if ((tp->type == TOUCH_HOLD_Y))
                {
                    // gui_log("obj->y:%d,%d",obj->y, ((gui_page_t *)obj)->start_y-(obj->h-gui_get_screen_height()));
                    if (tp->deltaY + tp->y < 0)
                    {
                        tp->deltaY = -tp->y;
                    }
                    obj->y = tp->deltaY + ((gui_page_t *)obj)->yold;
                    if (obj->y > ((gui_page_t *)obj)->start_y)
                    {
                        // obj->y = ((gui_page_t *)obj)->start_y;
                        GUI_TYPE(gui_page_t, obj)->status = 1;
                    }
                    else if (obj->y < (((gui_page_t *)obj)->start_y - (obj->h - (int)gui_get_screen_height())) &&
                             obj->y != 0)
                    {
                        // gui_log("obj->yyyy:%d,%d",obj->y, ((gui_page_t *)obj)->start_y-(obj->h-(int)gui_get_screen_height()));
                        // obj->y = ((gui_page_t *)obj)->start_y - (obj->h - (int)gui_get_screen_height());
                        GUI_TYPE(gui_page_t, obj)->status = 2;
                    }

                    int recode_num = 4;
                    for (size_t i = 0; i < recode_num; i++)
                    {
                        page->recode[i] = page->recode[i + 1];
                    }
                    page->recode[recode_num] = tp->deltaY;
                    GUI_TYPE(gui_page_t, obj)->speed = page->recode[recode_num] - page->recode[0];
                    int max_speed = 60;
                    int min_speed = 7;
                    if (GUI_TYPE(gui_page_t, obj)->speed > max_speed)
                    {
                        GUI_TYPE(gui_page_t, obj)->speed = max_speed;
                    }
                    else if (GUI_TYPE(gui_page_t, obj)->speed < -max_speed)
                    {
                        GUI_TYPE(gui_page_t, obj)->speed = -max_speed;
                    }
                    if (GUI_TYPE(gui_page_t, obj)->speed > 0 && GUI_TYPE(gui_page_t, obj)->speed < min_speed)
                    {
                        GUI_TYPE(gui_page_t, obj)->speed = min_speed;
                    }
                    else if (GUI_TYPE(gui_page_t, obj)->speed < 0 && GUI_TYPE(gui_page_t, obj)->speed > -min_speed)
                    {
                        GUI_TYPE(gui_page_t, obj)->speed = -min_speed;
                    }
                    GUI_TYPE(gui_page_t, obj)->target = (GUI_TYPE(gui_page_t, obj)->speed * GUI_TYPE(gui_page_t,
                                                         obj)->speed + _UI_ABS(GUI_TYPE(gui_page_t, obj)->speed)) / 2;

                    int top_height = -obj->y;
                    int buttom_height = obj->h + obj->y - (int)gui_get_screen_height();
                    if (GUI_TYPE(gui_page_t, obj)->speed > 0)
                    {
                        if (GUI_TYPE(gui_page_t, obj)->target > top_height)
                        {
                            GUI_TYPE(gui_page_t, obj)->target = top_height;
                        }
                        GUI_TYPE(gui_page_t, obj)->target = obj->y + GUI_TYPE(gui_page_t, obj)->target;

                    }
                    else if (GUI_TYPE(gui_page_t, obj)->speed < 0)
                    {
                        if (GUI_TYPE(gui_page_t, obj)->target > buttom_height)
                        {
                            GUI_TYPE(gui_page_t, obj)->target = buttom_height;
                        }
                        GUI_TYPE(gui_page_t, obj)->target = obj->y - GUI_TYPE(gui_page_t, obj)->target;
                    }
                    else
                    {
                        GUI_TYPE(gui_page_t, obj)->target = obj->y;
                    }


                    if (page->align_hight > 0)
                    {
                        int alien = _UI_ABS(GUI_TYPE(gui_page_t, obj)->target) % page->align_hight;
                        if (alien > page->align_hight / 2)
                        {
                            page->target = page->target - page->align_hight + alien;
                            if (_UI_ABS(page->speed) == min_speed || _UI_ABS(page->speed) == 0)
                            {
                                /* code */


                                if (page->target > obj->y)
                                {
                                    page->speed = min_speed;
                                }
                                else
                                {
                                    page->speed = -min_speed;
                                }
                            }

                        }
                        else
                        {
                            page->target = page->target + alien;
                            if (_UI_ABS(page->speed) == min_speed || _UI_ABS(page->speed) == 0)
                            {
                                /* code */


                                if (page->target > obj->y)
                                {
                                    page->speed = min_speed;
                                }
                                else
                                {
                                    page->speed = -min_speed;
                                }
                            }
                        }
                    }
                    //gui_log("status:%d,%d,%d,%d,%d,%d\n",GUI_TYPE(gui_page_t, obj)->status, page->recode[0], page->recode[1],page->recode[2],page->recode[3],page->recode[4]);
                    gui_obj_event_set(obj, GUI_EVENT_8);
                    if (GUI_TYPE(gui_page_t, obj)->status == 1)
                    {
                        GUI_TYPE(gui_page_t, obj)->speed = -40;
                    }
                    else if (GUI_TYPE(gui_page_t, obj)->status == 2)
                    {
                        GUI_TYPE(gui_page_t, obj)->speed = 40;
                    }


                }
                else if (tp->released)
                {
                    if (tp->type == TOUCH_UP_SLIDE)
                    {
                        gui_log("page TOUCH_UP_SLIDE\n");
                    }
                    else if (tp->type == TOUCH_DOWN_SLIDE)
                    {
                        gui_log("page TOUCH_DOWN_SLIDE\n");
                    }



                    if (GUI_TYPE(gui_page_t, obj)->speed != 0)
                    {
                        GUI_TYPE(gui_page_t, obj)->release = true;


                    }
                    else if (GUI_TYPE(gui_page_t, obj)->speed == 0)
                    {
                        gui_obj_event_set(obj, GUI_EVENT_7);
                    }
                }
                else if (tp->pressed)
                {
                    GUI_TYPE(gui_page_t, obj)->release = false;
                    GUI_TYPE(gui_page_t, obj)->speed = 0;
                    GUI_TYPE(gui_page_t, obj)->status = 0;
                    memset(page->recode, 0, 10);
                }
                else
                {
                    switch (GUI_TYPE(gui_page_t, obj)->status)
                    {
                    case 0:
                        {
                            if (GUI_TYPE(gui_page_t, obj)->release)
                            {
                                if (GUI_TYPE(gui_page_t, obj)->speed == 0)
                                {
                                    GUI_TYPE(gui_page_t, obj)->release = false;

                                }
                                //gui_log("%d,%d\n",obj->y, GUI_TYPE(gui_page_t, obj)->speed);

                                if (GUI_TYPE(gui_page_t, obj)->speed > 3)
                                {
                                    obj->y += GUI_TYPE(gui_page_t, obj)->speed;
                                    GUI_TYPE(gui_page_t, obj)->speed -= 1;//gui_log("%d\n",__LINE__);
                                }
                                else if (GUI_TYPE(gui_page_t, obj)->speed < -3)
                                {
                                    obj->y += GUI_TYPE(gui_page_t, obj)->speed;
                                    GUI_TYPE(gui_page_t, obj)->speed += 1;//gui_log("%d\n",__LINE__);
                                }
                                {

                                    if (GUI_TYPE(gui_page_t, obj)->speed > 0 && obj->y >= page->target)
                                    {
                                        GUI_TYPE(gui_page_t, obj)->speed = 0;
                                        obj->y = page->target; //gui_log("%d\n",__LINE__);
                                    }
                                    if (GUI_TYPE(gui_page_t, obj)->speed < 0 && obj->y <= page->target)
                                    {
                                        GUI_TYPE(gui_page_t, obj)->speed = 0;
                                        obj->y = page->target; //gui_log("%d\n",__LINE__);
                                    }

                                }
                                if (page->align_hight > 0 && GUI_TYPE(gui_page_t, obj)->speed <= 3 &&
                                    GUI_TYPE(gui_page_t, obj)->speed >= -3)
                                {
                                    gui_obj_event_set(obj, GUI_EVENT_7);
                                    if (GUI_TYPE(gui_page_t, obj)->speed > 0 && obj->y >= page->target)
                                    {
                                        GUI_TYPE(gui_page_t, obj)->speed = 0;
                                        obj->y = page->target; //gui_log("%d\n",__LINE__);
                                    }
                                    if (GUI_TYPE(gui_page_t, obj)->speed < 0 && obj->y <= page->target)
                                    {
                                        GUI_TYPE(gui_page_t, obj)->speed = 0;
                                        obj->y = page->target; //gui_log("%d\n",__LINE__);
                                    }
                                    obj->y += GUI_TYPE(gui_page_t, obj)->speed;  //gui_log("%d\n",__LINE__);
                                }
                                else
                                {
                                    gui_obj_event_set(obj, GUI_EVENT_8);
                                }



                            }
                            if (obj->y > ((gui_page_t *)obj)->start_y)
                            {
                                obj->y = ((gui_page_t *)obj)->start_y;
                                GUI_TYPE(gui_page_t, obj)->release = false;
                                gui_obj_event_set(obj, GUI_EVENT_7);

                            }
                            else if (obj->y < (((gui_page_t *)obj)->start_y - (obj->h - (int)gui_get_screen_height())) &&
                                     obj->y != 0)
                            {
                                GUI_TYPE(gui_page_t, obj)->release = false;
                                // gui_log("obj->yyyy:%d,%d",obj->y, ((gui_page_t *)obj)->start_y-(obj->h-(int)gui_get_screen_height()));
                                obj->y = ((gui_page_t *)obj)->start_y - (obj->h - (int)gui_get_screen_height());
                                gui_obj_event_set(obj, GUI_EVENT_7);

                            }
                        }
                        break;
                    case 1:
                        {
                            if (GUI_TYPE(gui_page_t, obj)->release)
                            {
                                if (GUI_TYPE(gui_page_t, obj)->speed == 0)
                                {
                                    GUI_TYPE(gui_page_t, obj)->release = false;

                                }

                                if (GUI_TYPE(gui_page_t, obj)->speed > 3)
                                {
                                    obj->y += GUI_TYPE(gui_page_t, obj)->speed;
                                    GUI_TYPE(gui_page_t, obj)->speed -= 1;//gui_log("%d\n",__LINE__);
                                }
                                else if (GUI_TYPE(gui_page_t, obj)->speed < -3)
                                {
                                    obj->y += GUI_TYPE(gui_page_t, obj)->speed;
                                    GUI_TYPE(gui_page_t, obj)->speed += 1;//gui_log("%d\n",__LINE__);
                                }
                                {

                                    if (GUI_TYPE(gui_page_t, obj)->speed > 0 && obj->y >= page->target)
                                    {
                                        GUI_TYPE(gui_page_t, obj)->speed = 0;
                                        obj->y = page->target; //gui_log("%d\n",__LINE__);
                                    }
                                    if (GUI_TYPE(gui_page_t, obj)->speed < 0 && obj->y <= page->target)
                                    {
                                        GUI_TYPE(gui_page_t, obj)->speed = 0;
                                        obj->y = page->target; //gui_log("%d\n",__LINE__);
                                    }

                                }
                                if (page->align_hight > 0 && GUI_TYPE(gui_page_t, obj)->speed <= 3 &&
                                    GUI_TYPE(gui_page_t, obj)->speed >= -3)
                                {
                                    gui_obj_event_set(obj, GUI_EVENT_7);
                                    if (GUI_TYPE(gui_page_t, obj)->speed > 0 && obj->y >= page->target)
                                    {
                                        GUI_TYPE(gui_page_t, obj)->speed = 0;
                                        obj->y = page->target; //gui_log("%d\n",__LINE__);
                                    }
                                    if (GUI_TYPE(gui_page_t, obj)->speed < 0 && obj->y <= page->target)
                                    {
                                        GUI_TYPE(gui_page_t, obj)->speed = 0;
                                        obj->y = page->target; //gui_log("%d\n",__LINE__);
                                    }
                                    obj->y += GUI_TYPE(gui_page_t, obj)->speed;  //gui_log("%d\n",__LINE__);
                                }
                                else
                                {
                                    gui_obj_event_set(obj, GUI_EVENT_8);
                                }
                                gui_obj_event_set(obj, GUI_EVENT_8);


                            }
                            if (obj->y < ((gui_page_t *)obj)->start_y)
                            {
                                obj->y = ((gui_page_t *)obj)->start_y;
                                GUI_TYPE(gui_page_t, obj)->release = false;
                                gui_obj_event_set(obj, GUI_EVENT_7);

                            }
                            else if (obj->y < (((gui_page_t *)obj)->start_y - (obj->h - (int)gui_get_screen_height())) &&
                                     obj->y != 0)
                            {
                                GUI_TYPE(gui_page_t, obj)->release = false;
                                // gui_log("obj->yyyy:%d,%d",obj->y, ((gui_page_t *)obj)->start_y-(obj->h-(int)gui_get_screen_height()));
                                obj->y = ((gui_page_t *)obj)->start_y - (obj->h - (int)gui_get_screen_height());

                            }
                        }
                        break;
                    case 2:
                        {
                            if (GUI_TYPE(gui_page_t, obj)->release)
                            {
                                if (GUI_TYPE(gui_page_t, obj)->speed == 0)
                                {
                                    GUI_TYPE(gui_page_t, obj)->release = false;

                                }

                                if (GUI_TYPE(gui_page_t, obj)->speed > 3)
                                {
                                    obj->y += GUI_TYPE(gui_page_t, obj)->speed;
                                    GUI_TYPE(gui_page_t, obj)->speed -= 1;//gui_log("%d\n",__LINE__);
                                }
                                else if (GUI_TYPE(gui_page_t, obj)->speed < -3)
                                {
                                    obj->y += GUI_TYPE(gui_page_t, obj)->speed;
                                    GUI_TYPE(gui_page_t, obj)->speed += 1;//gui_log("%d\n",__LINE__);
                                }
                                {

                                    if (GUI_TYPE(gui_page_t, obj)->speed > 0 && obj->y >= page->target)
                                    {
                                        GUI_TYPE(gui_page_t, obj)->speed = 0;
                                        obj->y = page->target; //gui_log("%d\n",__LINE__);
                                    }
                                    if (GUI_TYPE(gui_page_t, obj)->speed < 0 && obj->y <= page->target)
                                    {
                                        GUI_TYPE(gui_page_t, obj)->speed = 0;
                                        obj->y = page->target; //gui_log("%d\n",__LINE__);
                                    }

                                }
                                if (page->align_hight > 0 && GUI_TYPE(gui_page_t, obj)->speed <= 3 &&
                                    GUI_TYPE(gui_page_t, obj)->speed >= -3)
                                {
                                    gui_obj_event_set(obj, GUI_EVENT_7);
                                    if (GUI_TYPE(gui_page_t, obj)->speed > 0 && obj->y >= page->target)
                                    {
                                        GUI_TYPE(gui_page_t, obj)->speed = 0;
                                        obj->y = page->target; //gui_log("%d\n",__LINE__);
                                    }
                                    if (GUI_TYPE(gui_page_t, obj)->speed < 0 && obj->y <= page->target)
                                    {
                                        GUI_TYPE(gui_page_t, obj)->speed = 0;
                                        obj->y = page->target; //gui_log("%d\n",__LINE__);
                                    }
                                    obj->y += GUI_TYPE(gui_page_t, obj)->speed;  //gui_log("%d\n",__LINE__);
                                }
                                else
                                {
                                    gui_obj_event_set(obj, GUI_EVENT_8);
                                }


                            }
                            if (obj->y > ((gui_page_t *)obj)->start_y)
                            {
                                obj->y = ((gui_page_t *)obj)->start_y;
                                GUI_TYPE(gui_page_t, obj)->release = false;

                            }
                            else if (obj->y > (((gui_page_t *)obj)->start_y - (obj->h - (int)gui_get_screen_height())) &&
                                     obj->y != 0)
                            {
                                GUI_TYPE(gui_page_t, obj)->release = false;
                                // gui_log("obj->yyyy:%d,%d",obj->y, ((gui_page_t *)obj)->start_y-(obj->h-(int)gui_get_screen_height()));
                                obj->y = ((gui_page_t *)obj)->start_y - (obj->h - (int)gui_get_screen_height());
                                gui_obj_event_set(obj, GUI_EVENT_7);


                            }
                        }
                        break;
                    default:
                        break;
                    }

                    ((gui_page_t *)obj)->yold = obj->y;
                }
            }
        }


        if (((gui_page_t *)obj)->scroll_bar)
        {
            // ((gui_page_t *)obj)->scroll_bar->base.y = ((((gui_page_t *)obj)->start_y - obj->y) *
            //                                            gui_get_screen_height() / obj->h);
        }
        // gui_log("obj->y:%d,%d, %d\n", obj->y, obj->ay, obj->parent->ay);
        if (obj->y == ((gui_page_t *)obj)->start_y)
        {
            obj->cover = false;
        }
        else
        {
            obj->cover = true;
        }
    }



}

void gui_page_ctor(gui_page_t *this, gui_obj_t *parent, const char *filename, int16_t x,
                   int16_t y, int16_t w, int16_t h)
{
    gui_obj_ctor(&this->base, parent, filename, x, y, w, h
                );
    GET_BASE(this)->type = PAGE;
    GET_BASE(this)->obj_prepare = page_update;
    this->base.type = PAGE;
    this->start_x = x;
    this->start_y = y;
}

gui_page_t *gui_page_create(void *parent, const char *filename, int16_t x, int16_t y,
                            int16_t w, int16_t h)
{
#define _GUI_NEW_gui_page_create_param this, parent, filename, x, y, w, h
    GUI_NEW(gui_page_t, gui_page_ctor, _GUI_NEW_gui_page_create_param)
}
void gui_page_rebound(gui_page_t *this, bool rebound)
{
    if (rebound)
    {
        GET_BASE(this)->obj_prepare = page_update_rebound;
    }
    else
    {
        GET_BASE(this)->obj_prepare = page_update;
    }


}
void gui_page_center_alignment(gui_page_t *page, int align_hight)
{
    page->align_hight = align_hight;
}
/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
