/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_view_reduction.c
  * @brief view widget
  * @details view widget
  * @author shel_deng@realsil.com.cn
  * @date 2025/2/18
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include "guidef.h"
#include "gui_server.h"
#include "gui_obj.h"
#include "gui_view.h"
#include "gui_view_transition.h"

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

void gui_view_reduction(gui_view_t *_this, int16_t release)
{
    gui_obj_t *obj = GUI_BASE(_this);
    int16_t w = _this->base.w;
    int16_t h = _this->base.h;
    float s;

    int16_t offset_x = 0, offset_y = 0;
    if (_this->current_transition_style == SWITCH_IN_FROM_LEFT_USE_REDUCTION)
    {
        offset_x = release - w;
    }
    else if (_this->current_transition_style == SWITCH_IN_FROM_RIGHT_USE_REDUCTION)
    {
        offset_x = release + w;
    }
    else if (_this->current_transition_style == SWITCH_IN_FROM_TOP_USE_REDUCTION)
    {
        offset_y = release - h;
    }
    else if (_this->current_transition_style == SWITCH_IN_FROM_BOTTOM_USE_REDUCTION)
    {
        offset_y = release + h;
    }
    else if (_this->current_transition_style == SWITCH_OUT_TO_LEFT_USE_REDUCTION ||
             _this->current_transition_style == SWITCH_OUT_TO_RIGHT_USE_REDUCTION)
    {
        offset_x = release;
    }
    else
    {
        offset_y = release;
    }

    int sx = abs(offset_x);
    int sy = abs(offset_y);

    matrix_translate(offset_x, offset_y, obj->matrix);

    sx = sx % w;
    sy = sy % h;
    if (sx)
    {
        s = 1.0f - (float)sx / w;
    }
    else
    {
        s = 1.0f - (float)sy / h;
    }

    if (s < 0.2f)
    {
        s = 0.2f;
    }

    if (s >= 1.0f)
    {
        s = 1.0f;
    }

    matrix_translate(w / 2, h / 2, obj->matrix);
    matrix_scale(s, s, obj->matrix);
    matrix_translate(-w / 2, -h / 2, obj->matrix);
}
