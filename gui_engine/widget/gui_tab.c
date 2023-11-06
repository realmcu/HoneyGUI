/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_tab.c
  * @brief tab widget
  * @details tab widget
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
#include <gui_server.h>
#include <gui_obj.h>
#include <tp_algo.h>
#include "gui_tab.h"
#include "gui_canvas.h"


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

static void tab_prepare(gui_obj_t *obj)
{
    gui_tab_t *this = (gui_tab_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    gui_tabview_t *parent = (gui_tabview_t *)(obj->parent);
    if (this->style == CLASSIC)
    {
        obj->tx = (this->id.x - parent->cur_id.x) * (int)this->base.w;
        obj->ty = (this->id.y - parent->cur_id.y) * (int)this->base.h;
        return;
    }
    else if (this->style == FADE)
    {
        float x = 1.0f - (float)abs(obj->dx) / this->base.w;
        if (x < 0.2f)
        {
            x = 0.2f;
        }
        if (x >= 1.0f)
        {
            x = 1.0f;
        }
        obj->sx = x;
        obj->sy = x;
    }
    else if (this->style == REDUCTION_FADE)
    {
        float x = 1.0f - (float)abs(obj->dx) / this->base.w;
        if (x < 0.2f)
        {
            x = 0.2f;
        }
        if (x >= 1.0f)
        {
            x = 1.0f;
        }
        obj->sx = x;
        obj->sy = x;
    }
    else if (this->style == REDUCTION)
    {
        float s;
        obj->tx = (this->id.x - parent->cur_id.x) * (int)this->base.w;
        obj->ty = (this->id.y - parent->cur_id.y) * (int)this->base.h;
        int sx = abs(obj->dx + obj->ax + obj->tx);
        sx = sx % this->base.w;
        s = 1.0f - (float)sx / this->base.w;

        if (s < 0.2f)
        {
            s = 0.2f;
        }
        if (s >= 1.0f)
        {
            s = 1.0f;
        }
        obj->sx = s;
        obj->sy = s;

    }
}

static void gui_tab_ctor(gui_tab_t *this, gui_obj_t *parent, const char *filename, int16_t x,
                         int16_t y,
                         int16_t w, int16_t h, int16_t idx, int16_t idy)
{

    gui_obj_ctor(&this->base, parent, filename, x, y, w, h);

    GET_BASE(this)->obj_prepare = tab_prepare;
    GET_BASE(this)->type = TAB;

    gui_tabview_t *parent_ext = (gui_tabview_t *)parent;
    this->style = parent_ext->style;
    this->id.x = idx;
    this->id.y = idy;
    this->id.z = idy;
    parent_ext->tab_cnt++;
    if (idx > 0)
    {
        parent_ext->tab_cnt_right++;
    }
    else if (idx < 0)
    {
        parent_ext->tab_cnt_left--;
    }
    if (idy > 0)
    {
        parent_ext->tab_cnt_down++;
    }
    else if (idy < 0)
    {
        parent_ext->tab_cnt_up--;
    }

}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

gui_tab_t *gui_tab_create(void *parent, const char *name, int16_t x, int16_t y,
                          int16_t w, int16_t h, int16_t idx, int16_t idy)
{
    if (w == 0)
    {
        w = (int)gui_get_screen_width();
    }
    if (h == 0)
    {
        h = (int)gui_get_screen_height();
    }

    gui_tab_t *this = gui_malloc(sizeof(gui_tab_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_tab_t));

    gui_tab_ctor(this, (gui_obj_t *)parent, name, x, y, w, h, idx, idy);

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }
    GET_BASE(this)->create_done = true;
    return this;
}

void gui_tab_set_style(gui_tab_t *this, SLIDE_STYLE style)
{
    this->style = style;
}

/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
