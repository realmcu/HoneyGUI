/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_view_blur.c
  * @brief view widget
  * @details view widget
  * @author shel_deng@realsil.com.cn
  * @date 2025/2/18
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
#include "gui_img.h"
#include "gui_view.h"
#include "gui_view_transition.h"
#include "gui_post_process.h"


/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static void prepare_blur_param(gui_view_t *this, gui_rect_t new_rect, float blur_ratio,
                               BLUR_DIRECTION dir)
{
    uint8_t blur_degree;
    if (blur_ratio >= 0.5f)
    {
        blur_degree = 225;
    }
    else if (blur_ratio <= 0.1f)
    {
        blur_degree = 0;
        return;
    }
    else
    {
        blur_degree = (uint8_t)(562.5f * blur_ratio - 56.25f);
    }
    uint8_t *blur_param = this->blur_param;
    if (blur_param == NULL)
    {
        blur_param = gui_malloc(sizeof(post_process_event));
        this->blur_param = blur_param;
        memset(blur_param, 0, sizeof(post_process_event));
        post_process_event *event = (post_process_event *)blur_param;
        event->param = gui_malloc(sizeof(post_process_blur_param));
        memset(event->param, 0, sizeof(post_process_blur_param));
        post_process_blur_param *param = (post_process_blur_param *)event->param;
        param->area = new_rect;
        param->blur_degree = blur_degree;
        param->dir = dir;
        event->type = POST_PROCESS_BLUR;
        if (param->cache_mem == NULL)
        {
            blur_prepare(&param->area, &param->cache_mem);
        }
    }
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void gui_view_blur(gui_view_t *this, int16_t release)
{
    gui_obj_t *obj = (gui_obj_t *)this;
    int16_t w = obj->w;
    int16_t h = obj->h;
    int16_t x1 = this->base.x;
    int16_t x2 = this->base.x + w - 1;
    int16_t y1 = this->base.y;
    int16_t y2 = this->base.y + h - 1;
    static float blur_ratio = 0.0f;
    static BLUR_DIRECTION dir = BLUR_HORIZONTAL;
    gui_view_t *next_view = gui_view_get_next();
    gui_view_t *current_view = gui_view_get_current();

    if (current_view->current_transition_style == SWITCH_OUT_STILL_USE_BLUR)
    {
        if (next_view)
        {
            if (next_view->current_transition_style == SWITCH_IN_FROM_LEFT_USE_TRANSLATION)
            {
                x2 = release - 1;
                blur_ratio = abs(release) / (float)w;
                dir = BLUR_HORIZONTAL;
            }
            else if (next_view->current_transition_style == SWITCH_IN_FROM_RIGHT_USE_TRANSLATION)
            {
                x1 = w + release;
                blur_ratio = abs(release) / (float)w;
                dir = BLUR_HORIZONTAL_REVERSE;
            }
            else if (next_view->current_transition_style == SWITCH_IN_FROM_TOP_USE_TRANSLATION)
            {
                y2 = release - 1;
                blur_ratio = abs(release) / (float)h;
                dir = BLUR_VERTICAL;

            }
            else if (next_view->current_transition_style == SWITCH_IN_FROM_BOTTOM_USE_TRANSLATION)
            {
                y1 = h + release;
                blur_ratio = abs(release) / (float)h;
                dir = BLUR_VERTICAL_REVERSE;
            }
            this = next_view;
        }
    }
    else
    {
        if (current_view->current_transition_style == SWITCH_OUT_TO_LEFT_USE_TRANSLATION)
        {
            x2 = w + release - 1;
            blur_ratio = 1 - abs(release) / (float)w;
            dir = BLUR_HORIZONTAL;
        }
        else if (current_view->current_transition_style == SWITCH_OUT_TO_RIGHT_USE_TRANSLATION)
        {
            x1 = release - 1;
            blur_ratio = 1 - abs(release) / (float)w;
            dir = BLUR_HORIZONTAL_REVERSE;
        }
        else if (current_view->current_transition_style == SWITCH_OUT_TO_TOP_USE_TRANSLATION)
        {
            y2 = h + release - 1;
            blur_ratio = 1 - abs(release) / (float)h;
            dir = BLUR_VERTICAL;
        }
        else if (current_view->current_transition_style == SWITCH_OUT_TO_BOTTOM_USE_TRANSLATION)
        {
            y1 = release - 1;
            blur_ratio = 1 - abs(release) / (float)h;
            dir = BLUR_VERTICAL_REVERSE;
        }
        this = current_view;
    }
    this->base.need_preprocess = true;
    gui_rect_t new_rect = {.x1 = x1, .y1 = y1, .x2 = x2, .y2 = y2};
    prepare_blur_param(this, new_rect, blur_ratio, dir);
}


