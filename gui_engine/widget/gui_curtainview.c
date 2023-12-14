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
#include "gui_tab.h"
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
static gui_curtain_t *get_child(gui_obj_t *object, gui_curtain_enum_t orientation)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    gui_list_for_each_safe(node, tmp, &object->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->type == CURTAIN && GUI_TYPE(gui_curtain_t, obj)->orientation == orientation)
        {
            return (void *)obj;
        }

    }
    return NULL;
}

static void curtainview_prepare(gui_obj_t *obj)
{
    gui_curtainview_t *this = (gui_curtainview_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
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
        this->release_flag = true;
    }
    if (tp->pressed)
    {
        this->release_flag = false;
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
    if (this->cur_curtain == CURTAIN_MIDDLE)
    {
        if (this->init_flag)
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
        this->init_flag = true;
    }
    if (!((obj->parent->dx == 0) && (obj->parent->dy == 0)))
    {
        this->cur_curtain = CURTAIN_MIDDLE;
        obj->cover = false;
        return;
    }
    if (obj->parent->parent && obj->parent->parent->type == TABVIEW)
    {
        if (!(((gui_tabview_t *)(obj->parent->parent))->cur_id.x == ((gui_tab_t *)(obj->parent))->id.x &&
              ((gui_tabview_t *)(obj->parent->parent))->cur_id.y == ((gui_tab_t *)(obj->parent))->id.y))
        {
            this->cur_curtain = CURTAIN_MIDDLE;
            obj->cover = false;
            return;
        }

    }
    if (this->mute == true)
    {
        return;
    }
    bool cover1 = false;
    gui_tree_get_cover(obj, PAGE, &cover1);
    if (cover1)
    {
        return;
    }


    switch (this->cur_curtain)
    {
    case CURTAIN_MIDDLE:
        {
            obj->cover = false;
            if ((tp->type == TOUCH_HOLD_Y) || (tp->type == TOUCH_ORIGIN_FROM_Y) ||
                (tp->type == TOUCH_DOWN_SLIDE) || (tp->type == TOUCH_UP_SLIDE) || this->down_flag)
            {
                if (!this->down_flag)
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
                if (obj->y > 0 && this->orientations.up == false)
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
                if (obj->y < 0 && this->orientations.down == false)
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
                if (obj->y < 0 && this->release_flag)
                {
                    this->down_flag = true;
                    this->spring_value -= GUI_FRAME_STEP;
                    obj->y += this->spring_value;
                    gui_curtain_t *child =  get_child(obj, CURTAIN_DOWN);
                    float scope = 1;
                    if (child)
                    {
                        scope = child->scope;
                    }

                    if (obj->y <= -(int)gui_get_screen_height()*scope)
                    {
                        this->down_flag = false;
                        this->cur_curtain = CURTAIN_DOWN;
                        if (this->done_cb != NULL)
                        {
                            this->done_cb(this);
                        }
                        obj->y = 0;
                        this->spring_value = 0;
                        this->release_flag = false;
                    }

                }
                else if (obj->y > 0 && this->release_flag)
                {
                    this->down_flag = true;
                    this->spring_value += GUI_FRAME_STEP;
                    obj->y += this->spring_value;
                    gui_curtain_t *child =  get_child(obj, CURTAIN_UP);
                    float scope = 1;
                    if (child)
                    {
                        scope = child->scope;
                    }
                    if (obj->y >= (int)gui_get_screen_height()*scope)
                    {
                        this->down_flag = false;
                        this->cur_curtain = CURTAIN_UP;
                        obj->y = 0;
                        this->spring_value = 0;
                        this->release_flag = false;
                    }
                }
            }
            else if ((tp->type == TOUCH_HOLD_X) || this->left_flag)
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


                if (!this->left_flag)
                {
                    obj->x = tp->deltaX;
                }
                if (obj->x < 0 && this->orientations.right == false)
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
                if (obj->x > 0 && this->orientations.left == false)
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
                if (obj->x < -(int)gui_get_screen_width() * this->scoperight * 0.5f)
                {
                    this->cur_curtain = CURTAIN_RIGHT;
                    obj->x = 0;
                }
                else if (obj->x > (int)gui_get_screen_width() * this->scopeleft * 0.5f)
                {
                    this->left_flag = true;
                    this->spring_value += GUI_FRAME_STEP;
                    obj->x += this->spring_value;
                    if (obj->x >= (int)gui_get_screen_width()* this->scopeleft)
                    {
                        this->left_flag = false;
                        this->cur_curtain = CURTAIN_LEFT;
                        obj->x = 0;
                        this->spring_value = 0;
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
            }
            else if (tp->type == TOUCH_UP_SLIDE)
            {
                this->cur_curtain = CURTAIN_MIDDLE;
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
            else if (tp->type == TOUCH_DOWN_SLIDE)
            {
                this->cur_curtain = CURTAIN_MIDDLE;
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
                this->cur_curtain = CURTAIN_MIDDLE;
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
                this->cur_curtain = CURTAIN_MIDDLE;
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
    if (this->cur_curtain == CURTAIN_MIDDLE)
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








