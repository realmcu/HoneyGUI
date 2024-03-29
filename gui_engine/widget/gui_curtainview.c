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
#include <string.h>
#include "guidef.h"
#include "tp_algo.h"
#include "gui_obj.h"
#include "gui_server.h"
#include "gui_tabview.h"
#include "gui_tab.h"
#include "gui_curtain.h"

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
static void gui_curtainview_input_prepare(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    gui_curtainview_t *this = (gui_curtainview_t *)obj;
    GUI_UNUSED(tp);

    if (gui_obj_in_rect(obj, 0, 0, gui_get_screen_width(), gui_get_screen_height()) == false)
    {
        return;
    }

    switch (this->cur_curtain)
    {
    case CURTAIN_MIDDLE:
        {
            if (this->has_up_curtain == true)
            {
                if (this->cur_curtain_flag || this->middle_flag_up)
                {
                    gui_obj_skip_other_up_hold(obj);
                    gui_obj_skip_other_left_hold(obj);
                    gui_obj_skip_other_right_hold(obj);
                }

                gui_obj_skip_other_down_hold(obj);

                obj->skip_tp_down_hold = false;
            }

            if (this->has_down_curtain == true)
            {
                if (this->cur_curtain_flag || this->middle_flag)
                {
                    gui_obj_skip_other_down_hold(obj);
                    gui_obj_skip_other_left_hold(obj);
                    gui_obj_skip_other_right_hold(obj);
                }

                gui_obj_skip_other_up_hold(obj);

                obj->skip_tp_up_hold = false;
            }

            if (this->has_left_curtain == true)
            {
                if (this->cur_curtain_flag || this->middle_flag_left || this->left_flag)
                {
                    gui_obj_skip_other_left_hold(obj);
                }

                gui_obj_skip_other_right_hold(obj);
            }

            if (this->has_right_curtain == true)
            {
                gui_obj_skip_other_left_hold(obj);
            }
        }
        break;

    case CURTAIN_UP:
        {
            gui_obj_skip_other_down_hold(obj);
            gui_obj_skip_other_up_hold(obj);
            gui_obj_skip_other_right_hold(obj);
            gui_obj_skip_other_left_hold(obj);
        }
        break;

    case CURTAIN_DOWN:
        {
            gui_obj_skip_other_down_hold(obj);
            gui_obj_skip_other_up_hold(obj);
            gui_obj_skip_other_right_hold(obj);
            gui_obj_skip_other_left_hold(obj);
        }
        break;

    case CURTAIN_LEFT:
        {
            gui_obj_skip_other_left_hold(obj);
            gui_obj_skip_other_right_hold(obj);
        }
        break;
    case CURTAIN_RIGHT:
        {
        }
        break;

    default:
        break;
    }
}

static void gui_curtainview_prepare(gui_obj_t *obj)
{
    gui_curtainview_t *this = (gui_curtainview_t *)obj;
    touch_info_t *tp = tp_get_info();

    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    gui_curtain_t *c_middle = NULL;
    GUI_UNUSED(c_middle);
    gui_curtain_t *c_up = NULL;
    gui_curtain_t *c_down = NULL;
    gui_curtain_t *c_left = NULL;
    gui_curtain_t *c_right = NULL;
    float scope;

    gui_list_for_each_safe(node, tmp, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);

        if (obj->type == CURTAIN)
        {
            gui_curtain_t *c = (void *)obj;
            switch (c->orientation)
            {
            case CURTAIN_MIDDLE:
                {
                    c_middle = c;
                }
                break;

            case CURTAIN_UP:
                {
                    c_up = c;
                }
                break;

            case CURTAIN_DOWN:
                {
                    c_down = c;
                }
                break;

            case CURTAIN_LEFT:
                {
                    c_left = c;
                }
                break;

            case CURTAIN_RIGHT:
                {
                    c_right = c;
                }
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

    switch (tp->type)
    {
    case TOUCH_HOLD_X:
        {
            if ((obj->skip_tp_left_hold) && (tp->deltaX  < 0))
            {
                break;
            }

            if ((obj->skip_tp_right_hold) && (tp->deltaX  > 0))
            {
                break;
            }

            if (this->middle_flag || this->middle_flag_up || this->down_flag ||
                this->cur_curtain == CURTAIN_DOWN || this->up_flag || this->cur_curtain == CURTAIN_UP)
            {
                this->release_x = 0;
                this->middle_flag_left = 0;
                break;
            }

            if (this->middle_flag_left)
            {
                break;
            }

            this->release_y = 0;
            if (this->cur_curtain == CURTAIN_LEFT)
            {
                if (tp->deltaX < 0)
                {
                    this->release_x = tp->deltaX;
                    scope = 1.0f;

                    if (c_left)
                    {
                        scope = c_left->scope;
                    }

                    if (_UI_ABS(tp->deltaX) > (int)((float)gui_get_screen_width() / 2.0f * scope))
                    {
                        this->left_flag = 1;
                    }
                }
                break;
            }

            this->release_x = tp->deltaX;
            scope = 1.0f;

            if (c_left)
            {
                scope = c_left->scope;
            }

            if (this->release_x > (int)((float)gui_get_screen_width() * scope))
            {
                this->release_x = (int)((float)gui_get_screen_width() * scope);
            }

            if (this->release_x != 0 && this->cur_curtain == CURTAIN_MIDDLE)
            {
                if (this->release_x > 0)
                {
                    if (c_left)
                    {
                        GET_BASE(c_left)->not_show = false;
                    }
                }
            }

            scope = 1.0f;

            if (c_left)
            {
                scope = c_left->scope;
            }

            if (this->cur_curtain == CURTAIN_MIDDLE && tp->deltaX > 0 &&
                tp->deltaX > (int)((float)gui_get_screen_width() / 2.0f * scope))
            {
                this->middle_flag_left = 1;
            }
        }
        break;

    case TOUCH_HOLD_Y:
        {
            if ((obj->skip_tp_up_hold) && (tp->deltaY  < 0))
            {
                break;
            }

            if ((obj->skip_tp_down_hold) && (tp->deltaY  > 0))
            {
                break;
            }

            if (this->middle_flag_left || this->left_flag)
            {
                this->release_y = 0;
                this->middle_flag = 0;
                this->middle_flag_up = 0;
                break;
            }

            this->release_x = 0;

            if ((this->cur_curtain == CURTAIN_DOWN) && (tp->deltaY > 0))
            {
                this->release_y = tp->deltaY;
                scope = 1.0f;

                if (c_down)
                {
                    scope = c_down->scope;
                }

                if (_UI_ABS(tp->deltaY) > (int)((float)gui_get_screen_height() / 2.0f * scope))
                {
                    this->down_flag = 1;
                    if (this->done_cb != NULL)
                    {
                        this->done_cb(this);
                    }
                }
            }

            if ((this->cur_curtain == CURTAIN_UP) && (tp->deltaY < 0))
            {
                this->release_y = tp->deltaY;
                scope = 1.0f;

                if (c_up)
                {
                    scope = c_up->scope;
                }

                if (_UI_ABS(tp->deltaY) > (int)((float)gui_get_screen_height() / 2.0f * scope))
                {
                    this->up_flag = 1;
                    if (this->done_cb != NULL)
                    {
                        this->done_cb(this);
                    }
                }
            }

            if ((this->cur_curtain == CURTAIN_MIDDLE) && (tp->deltaY < 0) && (this->middle_flag_up == 0))
            {
                this->release_y = tp->deltaY;

                if (c_down)
                {
                    GET_BASE(c_down)->not_show = false;
                }
            }

            if ((this->cur_curtain == CURTAIN_MIDDLE) && (tp->deltaY > 0) && (this->middle_flag == 0))
            {
                this->release_y = tp->deltaY;

                if (c_up)
                {
                    GET_BASE(c_up)->not_show = false;
                }
            }

            if ((this->cur_curtain == CURTAIN_MIDDLE) && (tp->deltaY < 0) && (this->middle_flag_up == 0))
            {
                scope = 1.0f;

                if (c_down)
                {
                    scope = c_down->scope;
                }

                if (_UI_ABS(tp->deltaY) > (int)((float)gui_get_screen_height() / 2.0f * scope))
                {
                    this->middle_flag = 1;
                    if (this->done_cb != NULL)
                    {
                        this->done_cb(this);
                    }
                }
                else
                {
                    this->middle_flag = 0;
                }
            }

            if ((this->cur_curtain == CURTAIN_MIDDLE) && (tp->deltaY > 0) && (this->middle_flag == 0))
            {
                scope = 1.0f;

                if (c_up)
                {
                    scope = c_up->scope;
                }

                if (_UI_ABS(tp->deltaY) > (int)((float)gui_get_screen_height() / 2.0f * scope))
                {
                    this->middle_flag_up = 1;
                }

                else
                {
                    this->middle_flag_up = 0;
                }
            }
        }
        break;

    case TOUCH_LEFT_SLIDE:
        {
        }
        break;

    case TOUCH_RIGHT_SLIDE:
        {
        }
        break;

    case TOUCH_DOWN_SLIDE:
        {
        }
        break;

    case TOUCH_UP_SLIDE:
        {
        }
        break;

    case TOUCH_ORIGIN_FROM_X:
        {
        }
        break;

    case TOUCH_ORIGIN_FROM_Y:
        {
        }
        break;

    default:
        {
            this->cur_curtain_flag = 0;
        }
        break;
    }

    if ((this->middle_flag || this->middle_flag_up) && tp->type != TOUCH_HOLD_Y)
    {
        if (this->middle_flag)
        {
            scope = 1.0f;

            if (c_down)
            {
                scope = c_down->scope;
            }

            if (this->release_y >= -((float)gui_get_screen_height()*scope - GUI_FRAME_STEP))
            {
                this->release_y -= GUI_FRAME_STEP;

                if (c_down)
                {
                    GET_BASE(c_down)->not_show = false;
                }
            }
            else
            {
                this->release_y = 0;
                this->middle_flag = 0;
                this->cur_curtain = CURTAIN_DOWN;

                if (c_down)
                {
                    GET_BASE(c_down)->not_show = false;
                }
            }
        }

        if (this->middle_flag_up && tp->type != TOUCH_HOLD_Y)
        {
            scope = 1.0f;

            if (c_up)
            {
                scope = c_up->scope;
            }

            if (this->release_y <= ((float)gui_get_screen_height()*scope - GUI_FRAME_STEP))
            {
                this->release_y += GUI_FRAME_STEP;

                if (c_up)
                {
                    GET_BASE(c_up)->not_show = false;
                }
            }
            else
            {
                this->release_y = 0;
                this->middle_flag_up = 0;
                this->cur_curtain = CURTAIN_UP;

                if (c_up)
                {
                    GET_BASE(c_up)->not_show = false;
                }
            }
        }
    }
    else if (!this->middle_flag && this->release_y < 0 && this->cur_curtain == CURTAIN_MIDDLE &&
             tp->type != TOUCH_HOLD_Y)
    {
        if (this->release_y <= 0)
        {
            if (_UI_ABS(this->release_y + GUI_FRAME_STEP) < GUI_FRAME_STEP)
            {
                this->release_y = 0;

                if (c_down)
                {
                    GET_BASE(c_down)->not_show = true;
                }
            }
            else
            {
                this->release_y += GUI_FRAME_STEP;

                if (c_down)
                {
                    GET_BASE(c_down)->not_show = false;
                }
            }
        }
    }
    else if (!this->middle_flag_up && this->release_y > 0 && this->cur_curtain == CURTAIN_MIDDLE &&
             tp->type != TOUCH_HOLD_Y)
    {
        if (this->release_y >= 0)
        {
            if (_UI_ABS(this->release_y - GUI_FRAME_STEP) < GUI_FRAME_STEP)
            {
                this->release_y = 0;

                if (c_up)
                {
                    GET_BASE(c_up)->not_show = true;
                }
            }
            else
            {
                this->release_y -= GUI_FRAME_STEP;

                if (c_up)
                {
                    GET_BASE(c_up)->not_show = false;
                }
            }
        }
    }
    else if (this->cur_curtain == CURTAIN_MIDDLE && this->release_x > 0 && !this->middle_flag_left)
    {
        if (this->release_x >= 0 + GUI_FRAME_STEP)
        {
            this->release_x -= GUI_FRAME_STEP;

            if (c_left)
            {
                GET_BASE(c_left)->not_show = false;
            }
        }
    }
    else if (this->middle_flag_left)
    {
        scope = 1.0f;

        if (c_left)
        {
            scope = c_left->scope;
        }
        if (this->release_x <= ((float)gui_get_screen_width()*scope - GUI_FRAME_STEP))
        {
            this->release_x += GUI_FRAME_STEP;

            if (c_left)
            {
                GET_BASE(c_left)->not_show = false;
            }
        }
        else
        {
            this->release_x = 0;
            this->middle_flag_left = 0;
            this->cur_curtain = CURTAIN_LEFT;

            if (c_left)
            {
                GET_BASE(c_left)->not_show = false;
            }
        }
    }
    else if (this->down_flag && tp->type != TOUCH_HOLD_Y)
    {
        scope = 1.0f;

        if (c_down)
        {
            scope = c_down->scope;
        }

        if (this->release_y <= ((float)gui_get_screen_height()*scope - GUI_FRAME_STEP))
        {
            this->release_y += GUI_FRAME_STEP;
        }
        else
        {
            this->release_y = 0;
            this->down_flag = 0;
            this->cur_curtain = CURTAIN_MIDDLE;
        }
    }
    else if (this->up_flag && tp->type != TOUCH_HOLD_Y)
    {
        scope = 1.0f;

        if (c_up)
        {
            scope = c_up->scope;
        }

        if (this->release_y >= -((float)gui_get_screen_height()*scope - GUI_FRAME_STEP))
        {
            this->release_y -= GUI_FRAME_STEP;
        }
        else
        {
            this->release_y = 0;
            this->up_flag = 0;
            this->cur_curtain = CURTAIN_MIDDLE;
        }
    }
    else if (this->left_flag && tp->type != TOUCH_HOLD_X)
    {
        scope = 1.0f;

        if (c_left)
        {
            scope = c_left->scope;
        }

        if (this->release_x >= -(int)(((float)gui_get_screen_width()*scope - GUI_FRAME_STEP)))
        {
            this->release_x -= GUI_FRAME_STEP;
        }
        else
        {
            this->release_x = 0;
            this->left_flag = 0;
            this->cur_curtain = CURTAIN_MIDDLE;
            this->cur_curtain_flag = 1;
        }
    }
    else
    {
        /* code */
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
    }

    uint8_t last = this->checksum;
    this->checksum = 0;
    this->checksum = gui_obj_checksum(0, (uint8_t *)this, sizeof(gui_curtainview_t));

    if (last != this->checksum)
    {
        gui_fb_change();
    }
}

static void gui_curtainview_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            gui_curtainview_input_prepare(obj);
            break;

        case OBJ_PREPARE:
            gui_curtainview_prepare(obj);
            break;

        default:
            break;
        }
    }
}

static void gui_curtainview_ctor(gui_curtainview_t *this,
                                 gui_obj_t         *parent,
                                 const char        *filename,
                                 int16_t            x,
                                 int16_t            y,
                                 int16_t            w,
                                 int16_t            h)
{
    gui_obj_ctor(&this->base, parent, filename, x, y, w, h);

    ((gui_obj_t *)this)->type = CURTAINVIEW;
    ((gui_obj_t *)this)->obj_cb = gui_curtainview_cb;
    ((gui_obj_t *)this)->has_input_prepare_cb = true;
    ((gui_obj_t *)this)->has_prepare_cb = true;
    this->cur_curtain = CURTAIN_MIDDLE;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void gui_curtainview_set_done_cb(gui_curtainview_t *this, void (*cb)(gui_curtainview_t *this))
{
    this->done_cb = cb;
}

gui_curtainview_t *gui_curtainview_create(void       *parent,
                                          const char *filename,
                                          int16_t     x,
                                          int16_t     y,
                                          int16_t     w,
                                          int16_t     h)
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
