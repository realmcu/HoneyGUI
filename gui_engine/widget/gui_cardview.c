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

#define CARDVIEW_SLIDE_SPEED 20

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

static void gui_cardview_input_prepare(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    gui_cardview_t *this = (gui_cardview_t *)obj;
    GUI_UNUSED(tp);

    if (gui_obj_in_rect(obj, 0, 0, gui_get_screen_width(), gui_get_screen_height()) == false)
    {
        return;
    }

    if (this->offset_y == 0)
    {
        gui_obj_skip_other_up_hold(obj);
    }
    else
    {
        gui_obj_skip_other_up_hold(obj);
        gui_obj_skip_other_down_hold(obj);
    }
}

static void gui_cardview_prepare(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    gui_cardview_t *this = (gui_cardview_t *)obj;
    uint8_t last = this->checksum;
    bool skip = false;

    switch (tp->type)
    {
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

            if ((abs(this->offset_y) > (this->total_cnt - 2) * this->card_height) && (tp->deltaY  < 0))
            {
                break;
            }
            this->hold_y = tp->deltaY;

            for (size_t i = 0; i < 4; i++)
            {
                this->recode[i] = this->recode[i + 1];
            }

            this->recode[4] = tp->deltaY ;
            this->speed = this->recode[4] - this->recode[0];

            if (this->speed >= CARDVIEW_SLIDE_SPEED)
            {
                this->speed = CARDVIEW_SLIDE_SPEED;
            }
            else if (this->speed <= -CARDVIEW_SLIDE_SPEED)
            {
                this->speed = -CARDVIEW_SLIDE_SPEED;
            }

            skip = true;
        }
        break;

    case TOUCH_DOWN_SLIDE:
    case TOUCH_UP_SLIDE:
    case TOUCH_ORIGIN_FROM_Y:
        {
            int16_t tmp;

            memset(this->recode, 0, sizeof(this->recode));

            if ((obj->skip_tp_up_hold) && (tp->deltaY  < 0))
            {
                break;
            }

            if ((obj->skip_tp_down_hold) && (tp->deltaY  > 0))
            {
                break;
            }
            if ((abs(this->offset_y) > (this->total_cnt - 2) * this->card_height) && (tp->deltaY  < 0))
            {
                break;
            }

            this->offset_y += this->target_y;
            this->hold_y = this->hold_y - this->target_y;

            if (this->offset_y > 0)
            {
                this->target_y = this->target_y - this->offset_y;
                this->offset_y = 0;
                this->hold_y = this->hold_y - this->target_y;
            }

            if (this->speed >= CARDVIEW_SLIDE_SPEED)
            {
                tmp = this->hold_y + this->offset_y;
                this->offset_y = 0;
                this->hold_y = tmp;
            }

            if (this->speed <= -CARDVIEW_SLIDE_SPEED)
            {
                tmp = this->hold_y + this->offset_y;
                this->offset_y = -(this->height - 2 * this->card_height);
                this->hold_y = tmp - this->offset_y;
            }
        }
        break;

    default:
        break;
    }

    if (skip == false)
    {
        if (this->hold_y >= GUI_FRAME_STEP)
        {
            this->hold_y -= GUI_FRAME_STEP;
        }
        else if (this->hold_y <= -GUI_FRAME_STEP)
        {
            this->hold_y += GUI_FRAME_STEP;
        }
        else
        {
            this->hold_y = 0;
        }
    }
    else
    {

        int tmp = abs(this->hold_y);
        int v = abs(tmp) % this->card_height;

        if (v > this->card_height / 2)
        {
            tmp = ((tmp / this->card_height) + 1) * this->card_height;
        }
        else
        {
            tmp = ((tmp / this->card_height)) * this->card_height;
        }

        if (this->hold_y  < 0)
        {
            this->target_y = -tmp;
        }
        else
        {
            this->target_y = tmp;
        }
    }
    skip = false;

    this->checksum = 0;
    this->checksum = gui_obj_checksum(0, (uint8_t *)this, sizeof(gui_cardview_t));

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

static void gui_cardview_destory(gui_obj_t *obj)
{

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

        case OBJ_DESTORY:
            gui_cardview_destory(obj);
            break;

        default:
            break;
        }
    }
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void gui_cardview_status_cb(gui_cardview_t *this, void (*cb)(gui_cardview_t *this))
{
    this->status_cb = cb;
}

void gui_cardview_set_style(gui_cardview_t *this, T_SLIDE_STYLE style)
{
    this->style = style;
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

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }

    GET_BASE(this)->create_done = true;
    return this;
}
/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
