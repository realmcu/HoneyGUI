/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include "guidef.h"
#include "acc_api.h"
#include "gui_server.h"
#include "gui_obj.h"
#include "gui_img.h"
#include "gui_view.h"
#include "gui_view_api.h"


/*============================================================================*
 *                           Private Functions
 *============================================================================*/
/* Slide the view along one axis. */
static void gui_view_translation(gui_view_t *_this, int16_t release)
{
    gui_obj_t *obj = (gui_obj_t *)_this;
    int16_t w = obj->w;
    int16_t h = obj->h;
    int16_t dx = 0;
    int16_t dy = 0;

    switch (_this->current_transition_style)
    {
    case SWITCH_OUT_TO_RIGHT_USE_TRANSLATION:
        dx = release;
        break;
    case SWITCH_OUT_TO_LEFT_USE_TRANSLATION:
        dx = -release;
        break;
    case SWITCH_IN_FROM_LEFT_USE_TRANSLATION:
        dx = release - w;
        break;
    case SWITCH_IN_FROM_RIGHT_USE_TRANSLATION:
        dx = -release + w;
        break;
    case SWITCH_OUT_TO_TOP_USE_TRANSLATION:
        dy = -release;
        break;
    case SWITCH_OUT_TO_BOTTOM_USE_TRANSLATION:
        dy = release;
        break;
    case SWITCH_IN_FROM_TOP_USE_TRANSLATION:
        dy = release - h;
        break;
    case SWITCH_IN_FROM_BOTTOM_USE_TRANSLATION:
        dy = -release + h;
        break;

    default:
        break;
    }

    /* Move the draw position, not x/y: the view must keep receiving touch
     * events at its layout position while it slides. Set here rather than in
     * gui_fb.c because gui_view_prepare() runs as OBJ_PRE_PROCESS, after the
     * frame's draw position was computed. The next frame restores draw_x/draw_y
     * from abs_x/abs_y, so the offset does not accumulate. */
    obj->draw_x = (int16_t)(obj->abs_x + dx);
    obj->draw_y = (int16_t)(obj->abs_y + dy);
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void gui_view_transition(gui_view_t *_this, int16_t release)
{
    if (_this->current_transition_style >= SWITCH_OUT_TO_LEFT_USE_TRANSLATION &&
        _this->current_transition_style <= SWITCH_IN_FROM_BOTTOM_USE_TRANSLATION)
    {
        gui_view_translation(_this, release);
    }
}
