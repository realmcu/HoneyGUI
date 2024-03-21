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

static void input_prepare(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    GUI_UNUSED(tp);
    if (gui_obj_in_rect(obj, 0, 0, gui_get_screen_width(), gui_get_screen_height()) == false)
    {
        return;
    }
    gui_obj_skip_other_up_hold(obj);
    gui_obj_skip_other_down_hold(obj);
}

static void cardview_prepare(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    gui_cardview_t *this = (gui_cardview_t *)obj;



    switch (tp->type)
    {
    case TOUCH_HOLD_Y:
        if ((obj->skip_tp_up_hold) && (tp->deltaY  < 0))
        {
            break;
        }
        if ((obj->skip_tp_down_hold) && (tp->deltaY  > 0))
        {
            break;
        }
        this->release_y = tp->deltaY;
        break;
    case TOUCH_DOWN_SLIDE:
        break;
    case TOUCH_UP_SLIDE:
        break;
    default:
        break;
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

    //gui_log("[cardveiew] release_y = %d, this->checksum =%d \n", this->release_y, this->checksum);

    uint8_t last = this->checksum;
    this->checksum = 0;
    this->checksum = gui_checksum(0, (uint8_t *)this, sizeof(gui_cardview_t));

    if (last != this->checksum)
    {
        gui_fb_change();
    }

    if (this->status_cb != NULL)
    {
        this->status_cb(this);
    }

}

static void cardview_draw_cb(gui_obj_t *obj)
{
    gui_cardview_t *this = (gui_cardview_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();
    GUI_UNUSED(this);
    GUI_UNUSED(dc);


}
static void cardview_end(gui_obj_t *obj)
{

}
static void cardview_destory(gui_obj_t *obj)
{

}


/*============================================================================*
 *                           Public Functions
 *============================================================================*/


void gui_cardview_status_cb(gui_cardview_t *this, void (*cb)(gui_cardview_t *this))
{
    this->status_cb = cb;
}


void gui_cardview_set_style(gui_cardview_t *this, SLIDE_STYLE style)
{
    this->style = style;
}

gui_cardview_t *gui_cardview_create(void *parent,  const char *name,
                                    int16_t x, int16_t y, int16_t w, int16_t h)
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
    root->obj_input_prepare = input_prepare;
    root->obj_prepare = cardview_prepare;
    root->obj_draw = cardview_draw_cb;
    root->obj_end = cardview_end;
    root->obj_destory = cardview_destory;

    //for self


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





