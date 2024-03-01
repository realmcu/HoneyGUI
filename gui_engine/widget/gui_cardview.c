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


static void cardview_prepare(gui_obj_t *obj)
{
    GUI_UNUSED(obj);
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    gui_obj_t *root = (gui_obj_t *)obj;
    gui_cardview_t *this = (gui_cardview_t *)obj;

    uint32_t cx = dc->fb_width / 2;
    uint32_t cy = dc->fb_height / 2;
    GUI_UNUSED(root);
    GUI_UNUSED(cx);
    GUI_UNUSED(cy);
    if (this->mute == true)
    {
        return;
    }


    switch (tp->type)
    {
    case TOUCH_HOLD_Y:
        this->release_y = tp->deltaY + this->remain_y;
        if (this->release_y > 0)
        {
            this->release_y = 0;
        }
        {
            if (GUI_TYPE(gui_cardview_t, obj)->count == 5)
            {
                GUI_TYPE(gui_cardview_t, obj)->count = 0;
            }
            GUI_TYPE(gui_cardview_t, obj)->count++;

            if (GUI_TYPE(gui_cardview_t, obj)->count == 1)
            {
                GUI_TYPE(gui_cardview_t, obj)->y_last = tp->deltaY;
            }
            if (GUI_TYPE(gui_cardview_t, obj)->count == 5)
            {
                //gui_log("((gui_cardview_t *)obj)->yold:%d,%d,%d\n",obj->y-GUI_TYPE(gui_cardview_t, obj)->y_last, obj->y, GUI_TYPE(gui_cardview_t, obj)->y_last);
                if (GUI_TYPE(gui_cardview_t, obj)->y_last != 0)
                {
                    GUI_TYPE(gui_cardview_t, obj)->speed = tp->deltaY - GUI_TYPE(gui_cardview_t, obj)->y_last;
                }
                if (GUI_TYPE(gui_cardview_t, obj)->speed > 40)
                {
                    GUI_TYPE(gui_cardview_t, obj)->speed = 40;
                }
                if (GUI_TYPE(gui_cardview_t, obj)->speed < -40)
                {
                    GUI_TYPE(gui_cardview_t, obj)->speed = -40;
                }
                GUI_TYPE(gui_cardview_t, obj)->y_last = 0;
            }//gui_log("%d,%d,%d\n",tp->deltaY,GUI_TYPE(gui_cardview_t, obj)->y_last,GUI_TYPE(gui_cardview_t, obj)->speed);
        }

        break;
    case TOUCH_DOWN_SLIDE:
        this->remain_y = this->release_y;
        GUI_TYPE(gui_cardview_t, obj)->y_last = 0;
        break;
    case TOUCH_UP_SLIDE:
        this->remain_y = this->release_y;
        GUI_TYPE(gui_cardview_t, obj)->y_last = 0;
        break;
    default:
        GUI_TYPE(gui_cardview_t, obj)->y_last = 0;
        {
            if (GUI_TYPE(gui_cardview_t, obj)->speed > 0)
            {
                this->release_y += GUI_TYPE(gui_cardview_t, obj)->speed;
                GUI_TYPE(gui_cardview_t, obj)->speed -= 1;
            }
            else if (GUI_TYPE(gui_cardview_t, obj)->speed < 0)
            {
                this->release_y += GUI_TYPE(gui_cardview_t, obj)->speed;
                GUI_TYPE(gui_cardview_t, obj)->speed += 1;
            }
        }
        if (this->release_y > 0)
        {
            this->release_y = 0;
        }
        if (this->release_y < -((GUI_TYPE(gui_cardview_t, obj)->tab_cnt_down - 1)*obj->h))
        {
            this->release_y = -((GUI_TYPE(gui_cardview_t, obj)->tab_cnt_down - 1) * obj->h);
        }

        this->remain_y = this->release_y;
        break;
    }
    int offset = (gui_get_screen_width() % this->base.h) / 2 - obj->y;
    if (_UI_ABS((this->release_y - offset) % (this->base.h)) > 10)
    {
        if (this->release_y > 10)
        {
            this->release_y -= 10;
        }
        else if (this->release_y < -10)
        {
            this->release_y += 10;
        }
        else
        {
            this->release_y = 0;
        }
    }
    else
    {
        this->release_y += _UI_ABS((this->release_y - offset) % (this->base.h));
    }

    if (this->status_cb != NULL)
    {
        this->status_cb(this);
    }
// gui_log("%d ",this->release_y);



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

void gui_cardview_mute(gui_cardview_t *this)
{

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

    gui_list_init(&this->tab_list);
    this->mute = true;



    GET_BASE(this)->create_done = true;
    return this;
}
/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */





