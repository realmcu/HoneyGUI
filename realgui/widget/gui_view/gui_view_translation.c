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

    switch (_this->current_transition_style)
    {
    case SWITCH_OUT_TO_RIGHT_USE_TRANSLATION:
        matrix_translate(release, 0, obj->matrix);
        break;
    case SWITCH_OUT_TO_LEFT_USE_TRANSLATION:
        matrix_translate(-release, 0, obj->matrix);
        break;
    case SWITCH_IN_FROM_LEFT_USE_TRANSLATION:
        matrix_translate(release - w, 0, obj->matrix);
        break;
    case SWITCH_IN_FROM_RIGHT_USE_TRANSLATION:
        matrix_translate(-release + w, 0, obj->matrix);
        break;
    case SWITCH_OUT_TO_TOP_USE_TRANSLATION:
        matrix_translate(0, -release, obj->matrix);
        break;
    case SWITCH_OUT_TO_BOTTOM_USE_TRANSLATION:
        matrix_translate(0, release, obj->matrix);
        break;
    case SWITCH_IN_FROM_TOP_USE_TRANSLATION:
        matrix_translate(0, release - h, obj->matrix);
        break;
    case SWITCH_IN_FROM_BOTTOM_USE_TRANSLATION:
        matrix_translate(0, -release + h, obj->matrix);
        break;

    default:
        break;
    }

}


