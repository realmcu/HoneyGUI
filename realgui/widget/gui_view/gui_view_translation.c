/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_view_translation.c
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
#include "gui_img.h"
#include "gui_view.h"
#include "gui_view_transition.h"


/*============================================================================*
 *                           Private Functions
 *============================================================================*/

void gui_view_translation(gui_view_t *_this, int16_t release)
{
    gui_obj_t *obj = (gui_obj_t *)_this;
    int16_t w = obj->w;
    int16_t h = obj->h;

    int16_t offset_x, offset_y;
    if (_this->current_event == GUI_EVENT_TOUCH_MOVE_LEFT ||
        _this->current_event == GUI_EVENT_TOUCH_MOVE_RIGHT)
    {
        offset_x = release;
        offset_y = release * (h / w);
    }
    else
    {
        offset_x = release * (w / h);
        offset_y = release;
    }

    if (_this->current_transition_style == SWITCH_OUT_TO_RIGHT_USE_TRANSLATION)
    {
        matrix_translate(offset_x, 0, obj->matrix);
    }
    else if (_this->current_transition_style == SWITCH_OUT_TO_LEFT_USE_TRANSLATION)
    {
        matrix_translate(offset_x, 0, obj->matrix);
    }
    else if (_this->current_transition_style == SWITCH_IN_FROM_LEFT_USE_TRANSLATION)
    {
        matrix_translate(abs(offset_x) - w, 0, obj->matrix);
    }
    else if (_this->current_transition_style == SWITCH_IN_FROM_RIGHT_USE_TRANSLATION)
    {
        matrix_translate(-abs(offset_x) + w, 0, obj->matrix);
    }
    else if (_this->current_transition_style == SWITCH_OUT_TO_TOP_USE_TRANSLATION)
    {
        matrix_translate(0, offset_y, obj->matrix);
    }
    else if (_this->current_transition_style == SWITCH_OUT_TO_BOTTOM_USE_TRANSLATION)
    {
        matrix_translate(0, offset_y, obj->matrix);
    }
    else if (_this->current_transition_style == SWITCH_IN_FROM_TOP_USE_TRANSLATION)
    {
        matrix_translate(0, abs(release) - h, obj->matrix);
    }
    else if (_this->current_transition_style == SWITCH_IN_FROM_BOTTOM_USE_TRANSLATION)
    {
        matrix_translate(0, -abs(release) + h, obj->matrix);
    }
    else if (_this->current_transition_style == SWITCH_IN_FROM_TOP_RIGHT_USE_TRANSLATION)
    {
        matrix_translate(-abs(offset_x) + w, abs(offset_y) - h, obj->matrix);
    }
    else if (_this->current_transition_style == SWITCH_IN_CENTER_ZOOM_FADE)
    {
        float scale = (float)abs(offset_x) / w;
        obj->opacity_value = (uint32_t)(UINT8_MAX * scale);

        matrix_translate(obj->w / 2, obj->h / 2, obj->matrix);
        matrix_scale(scale, scale, obj->matrix);
        matrix_translate(obj->w / -2, obj->h / -2, obj->matrix);
    }
}


