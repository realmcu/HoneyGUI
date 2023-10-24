/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_curtainview.c
  * @brief curtain effect container widget, which can nest curtains.
  * @details Slide to extend and retract curtains
  * @author triton_yu@realsil.com.cn
  * @date 2023/10/24
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
#include <gui_curtain.h>
#include <string.h>
#include <gui_server.h>
#include "gui_obj.h"
#include <tp_algo.h>
#include "gui_tabview.h"

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
/** @defgroup WIDGET_Exported_Macros WIDGET Exported Macros
  * @{
  */



/** End of WIDGET_Exported_Macros
  * @}
  */
/*============================================================================*
 *                            Variables
 *============================================================================*/
/** @defgroup WIDGET_Exported_Variables WIDGET Exported Variables
  * @{
  */


/** End of WIDGET_Exported_Variables
  * @}
  */

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
/** @defgroup WIDGET_Exported_Functions WIDGET Exported Functions
  * @{
  */


static void curtainview_prepare(gui_obj_t *obj)
{
    gui_curtainview_t *this = (gui_curtainview_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    gui_curtainview_t *ext = (gui_curtainview_t *)obj;
    int frame_step = GUI_FRAME_STEP;
    if (frame_step / 10 >= 1)
    {
        frame_step = frame_step / 10;
    }
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    gui_curtain_t *c_middle = NULL;
    GUI_UNUSED(c_middle);
    gui_curtain_t *c_up = NULL;
    gui_curtain_t *c_down = NULL;
    gui_curtain_t *c_left = NULL;
    gui_curtain_t *c_right = NULL;
    if (tp->released)
    {
        ext->release_flag = true;
    }
    if (tp->pressed)
    {
        ext->release_flag = false;
    }


    gui_list_for_each_safe(node, tmp, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->type == CURTAIN)
        {
            gui_curtain_t *c = (void *)obj;
            switch (c->orientation)
            {
            case CURTAIN_MIDDLE:
                c_middle = c;
                break;
            case CURTAIN_UP:
                c_up = c;
                break;
            case CURTAIN_DOWN:
                c_down = c;
                break;
            case CURTAIN_LEFT:
                c_left = c;
                break;
            case CURTAIN_RIGHT:
                c_right = c;
                break;
            default:
                break;
            }
        }
    }
    if (ext->cur_curtain == CURTAIN_MIDDLE)
    {
        if (ext->init_flag)
        {
            if (c_up)
            {
                GET_BASE(c_up)->not_show = true;
            }
            if (c_down)
            {
                GET_BASE(c_down)->not_show = true;
            }
            if (c_left)
            {
                GET_BASE(c_left)->not_show = true;
            }
            if (c_right)
            {
                GET_BASE(c_right)->not_show = true;
            }
        }
        ext->init_flag = true;
    }
    if (!((obj->parent->dx == 0) && (obj->parent->dy == 0)))
    {
        ext->cur_curtain = CURTAIN_MIDDLE;
        obj->cover = false;
        return;
    }
    if (obj->parent->parent->type == TABVIEW)
    {
        if (!(((gui_tabview_t *)(obj->parent->parent))->cur_id.x == 0 &&
              ((gui_tabview_t *)(obj->parent->parent))->cur_id.y == 0))
        {
            ext->cur_curtain = CURTAIN_MIDDLE;
            obj->cover = false;
            return;
        }

    }
    if (this->mute == true)
    {
        return;
    }


    switch (ext->cur_curtain)
    {
    case CURTAIN_MIDDLE:
        {
            obj->cover = false;
            if ((tp->type == TOUCH_HOLD_Y) || (tp->type == TOUCH_ORIGIN_FROM_Y) ||
                (tp->type == TOUCH_DOWN_SLIDE) || (tp->type == TOUCH_UP_SLIDE) || ext->down_flag)
            {
                if (!ext->down_flag)
                {
                    obj->y = tp->deltaY;
                }
                if (c_up)
                {
                    GET_BASE(c_up)->not_show = false;
                }
                if (c_down)
                {
                    GET_BASE(c_down)->not_show = false;
                }
                if (obj->y > 0 && ext->orientations.up == false)
                {
                    obj->y = 0;
                    if (c_up)
                    {
                        GET_BASE(c_up)->not_show = true;
                    }
                    if (c_down)
                    {
                        GET_BASE(c_down)->not_show = true;
                    }
                }
                if (obj->y < 0 && ext->orientations.down == false)
                {
                    obj->y = 0;
                    if (c_up)
                    {
                        GET_BASE(c_up)->not_show = true;
                    }
                    if (c_down)
                    {
                        GET_BASE(c_down)->not_show = true;
                    }
                }
                if (obj->y < 0 && ext->release_flag)
                {
                    ext->down_flag = true;
                    ext->spring_value -= GUI_FRAME_STEP;
                    obj->y += ext->spring_value;
                    if (obj->y <= -(int)gui_get_screen_height())
                    {
                        ext->down_flag = false;
                        ext->cur_curtain = CURTAIN_DOWN;
                        if (this->done_cb != NULL)
                        {
                            this->done_cb(this);
                        }
                        obj->y = 0;
                        ext->spring_value = 0;
                        ext->release_flag = false;
                    }

                }
                else if (obj->y > 0 && ext->release_flag)
                {
                    ext->down_flag = true;
                    ext->spring_value += GUI_FRAME_STEP;
                    obj->y += ext->spring_value;
                    if (obj->y >= (int)gui_get_screen_height())
                    {
                        ext->down_flag = false;
                        ext->cur_curtain = CURTAIN_UP;
                        obj->y = 0;
                        ext->spring_value = 0;
                        ext->release_flag = false;
                    }
                }
            }
            else if ((tp->type == TOUCH_HOLD_X) || ext->left_flag)
            {
                if (tp->deltaX >= 2 || tp->deltaX <= -2)
                {
                    if (c_left)
                    {
                        GET_BASE(c_left)->not_show = false;
                    }
                    if (c_right)
                    {
                        GET_BASE(c_right)->not_show = false;
                    }
                }


                if (!ext->left_flag)
                {
                    obj->x = tp->deltaX;
                }
                if (obj->x < 0 && ext->orientations.right == false)
                {
                    obj->x = 0;
                    if (c_left)
                    {
                        GET_BASE(c_left)->not_show = true;
                    }
                    if (c_right)
                    {
                        GET_BASE(c_right)->not_show = true;
                    }
                }
                if (obj->x > 0 && ext->orientations.left == false)
                {
                    obj->x = 0;
                    if (c_left)
                    {
                        GET_BASE(c_left)->not_show = true;
                    }
                    if (c_right)
                    {
                        GET_BASE(c_right)->not_show = true;
                    }
                }
                if (obj->x < -(int)gui_get_screen_width() * ext->scoperight * 0.5f)
                {
                    ext->cur_curtain = CURTAIN_RIGHT;
                    obj->x = 0;
                }
                else if (obj->x > (int)gui_get_screen_width() * ext->scopeleft * 0.5f)
                {
                    ext->left_flag = true;
                    ext->spring_value += GUI_FRAME_STEP;
                    obj->x += ext->spring_value;
                    if (obj->x >= (int)gui_get_screen_width()* ext->scopeleft)
                    {
                        ext->left_flag = false;
                        ext->cur_curtain = CURTAIN_LEFT;
                        obj->x = 0;
                        ext->spring_value = 0;
                    }
                }
            }
            else
            {
                obj->x = 0;
                obj->y = 0;
            }
            break;
        }
    case CURTAIN_UP:
        {
            obj->cover = true;
            if (c_up)
            {
                GET_BASE(c_up)->not_show = false;
            }
            if ((tp->type == TOUCH_HOLD_Y))
            {
                if (tp->deltaY < 0)
                {
                    obj->y = tp->deltaY;
                }
                //if (obj->y < -(int)gui_get_screen_height() * ext->scopeup * 0.5f)
                //{
                //    ext->cur_curtain = CURTAIN_MIDDLE;gui_log("CURTAIN_MIDDLE:%d\n",obj->y );
                //    obj->y = 0;
                //}
            }
            else if (tp->type == TOUCH_DOWN_SLIDE)
            {
                ext->cur_curtain = CURTAIN_MIDDLE;
                obj->y = 0;
            }
            else
            {
                obj->y = 0;
            }
            break;
        }
    case CURTAIN_DOWN:
        {
            obj->cover = true;
            if (c_down)
            {
                GET_BASE(c_down)->not_show = false;
            }

            if ((tp->type == TOUCH_HOLD_Y))
            {
                if (tp->deltaY > 0)
                {
                    obj->y = tp->deltaY;
                }
            }
            else if (tp->type == TOUCH_UP_SLIDE)
            {
                ext->cur_curtain = CURTAIN_MIDDLE;
                if (this->done_cb != NULL)
                {
                    this->done_cb(this);
                }
                obj->y = 0;
            }
            else
            {
                obj->y = 0;
            }

            break;
        }
    case CURTAIN_LEFT:
        {
            obj->cover = true;
            if (c_left)
            {
                GET_BASE(c_left)->not_show = false;
            }
            if ((tp->type == TOUCH_HOLD_X))
            {
                if (tp->deltaX < 0)
                {
                    obj->x = tp->deltaX;
                }
            }
            else if (tp->type == TOUCH_LEFT_SLIDE)
            {
                ext->cur_curtain = CURTAIN_MIDDLE;
                obj->x = 0;
            }
            else
            {
                obj->x = 0;
            }
            break;
        }
    case CURTAIN_RIGHT:
        {
            obj->cover = true;
            if (c_right)
            {
                GET_BASE(c_right)->not_show = false;
            }
            if ((tp->type == TOUCH_HOLD_X))
            {
                if (tp->deltaX > 0)
                {
                    obj->x = tp->deltaX;
                }
            }
            else if (tp->type == TOUCH_RIGHT_SLIDE)
            {
                ext->cur_curtain = CURTAIN_MIDDLE;
                obj->x = 0;
            }
            else
            {
                obj->x = 0;
            }
            break;
        }

    default:
        break;
    }
    //gui_log("ext->cur_curtain=%d\n", ext->cur_curtain);
    if (ext->cur_curtain == CURTAIN_MIDDLE)
    {
        obj->cover = false;
    }
    else
    {
        obj->cover = true;
    }


}
static void gui_curtainview_ctor(gui_curtainview_t *this, gui_obj_t *parent, const char *filename,
                                 int16_t x,
                                 int16_t y, int16_t w, int16_t h)
{
    gui_obj_ctor(&this->base, parent, filename, x, y, w, h);
    ((gui_obj_t *)this)->obj_prepare = curtainview_prepare;
    ((gui_obj_t *)this)->type = CURTAINVIEW;
    this->cur_curtain = CURTAIN_MIDDLE;
    this->mute = false;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void gui_curtainview_set_done_cb(gui_curtainview_t *this, void (*cb)(gui_curtainview_t *this))
{
    this->done_cb = cb;
}
gui_curtainview_t *gui_curtainview_create(void *parent, const char *filename, int16_t x,
                                          int16_t y,
                                          int16_t w, int16_t h)
{
#define _paramgui_curtainview_create_ this, parent, filename, x, y, w, h
    GUI_NEW(gui_curtainview_t, gui_curtainview_ctor, _paramgui_curtainview_create_)
}

/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */








