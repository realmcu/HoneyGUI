/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_tab_fade.c
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
#include <string.h>
#include "guidef.h"
#include "gui_server.h"
#include "gui_obj.h"
#include "gui_tab.h"

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

void gui_tab_fade(gui_obj_t *obj, int16_t tab_x_gap, int16_t tab_y_gap)
{
    gui_tab_t *this = (gui_tab_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();
    gui_tabview_t *parent = (gui_tabview_t *)(obj->parent);
    int sx = abs((tab_x_gap) * (int)this->base.w + parent->release_x);
    float s;

    matrix_translate((tab_x_gap) * (int)this->base.w + parent->release_x, \
                     (tab_y_gap) * (int)this->base.h + parent->release_y, \
                     obj->matrix);

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

    matrix_translate(dc->screen_width / 2, dc->screen_height / 2, obj->matrix);
    matrix_scale(s, s, obj->matrix);
    matrix_translate(-dc->screen_width / 2, -dc->screen_height / 2, obj->matrix);
}

