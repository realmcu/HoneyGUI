/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_curtain.c
  * @brief create a curtain effect widget,which should be nested in a curtainview.
  * @details Slide to extend and retract curtains
  * @author triton_yu@realsil.com.cn
  * @date 2023/10/24
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
#include "gui_obj.h"
#include "gui_server.h"
#include "gui_img.h"
#include "gui_curtain.h"


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

static void gui_curtain_input_prepare(gui_obj_t *obj)
{
    gui_curtain_t *this = (gui_curtain_t *)obj;
    gui_curtainview_t *curtainview = (gui_curtainview_t *)obj->parent;

    int16_t dy = 0;
    int16_t dx = 0;
    int8_t cur_idy = 0;
    int8_t cur_idx = 0;
    int8_t idy = 0;
    int8_t idx = 0;

    switch (curtainview->cur_curtain)
    {
    case CURTAIN_UP:
        {
            cur_idy = -1;
            cur_idx = 0;
        }
        break;

    case CURTAIN_DOWN:
        {
            cur_idy = 1;
            cur_idx = 0;
        }
        break;

    case CURTAIN_LEFT:
        {
            cur_idy = 0;
            cur_idx = -1;
        }
        break;

    case CURTAIN_RIGHT:
        {
            cur_idy = 0;
            cur_idx = 1;
        }
        break;

    case CURTAIN_MIDDLE:
        {
            cur_idy = 0;
            cur_idx = 0;
        }
        break;

    default:
        break;
    }

    switch (this->orientation)
    {
    case CURTAIN_UP:
        {
            idy = -1;
            idx = 0;
        }
        break;

    case CURTAIN_DOWN:
        {
            idy = 1;
            idx = 0;
        }
        break;

    case CURTAIN_LEFT:
        {
            idy = 0;
            idx = -1;
        }
        break;

    case CURTAIN_RIGHT:
        {
            idy = 0;
            idx = 1;
        }
        break;

    case CURTAIN_MIDDLE:
        {
            idy = 0;
            idx = 0;
        }
        break;

    default:
        break;
    }

    if (cur_idy == idy)
    {
        dy = (int)gui_get_screen_height() * idy * (1 - this->scope);
    }
    else
    {
        dy = (idy - cur_idy) * (int)gui_get_screen_height();
    }

    dy += curtainview->release_y;

    if (cur_idx == idx)
    {
        dx = (int)gui_get_screen_width() * idx * (1 - this->scope);
    }
    else
    {
        dx = (idx - cur_idx) * (int)gui_get_screen_width();
    }

    matrix_translate(dx, dy, obj->matrix);
}

static void gui_curtain_prepare(gui_obj_t *obj)
{
    gui_curtain_t *this = (gui_curtain_t *)obj;
    gui_obj_t *parent = obj->parent;
    gui_curtainview_t *curtainview = (gui_curtainview_t *)parent;

    int8_t cur_idy = 0;
    int8_t cur_idx = 0;
    int8_t idy = 0;
    int8_t idx = 0;

    switch (curtainview->cur_curtain)
    {
    case CURTAIN_UP:
        {
            cur_idy = -1;
            cur_idx = 0;
        }
        break;

    case CURTAIN_DOWN:
        {
            cur_idy = 1;
            cur_idx = 0;
        }
        break;

    case CURTAIN_LEFT:
        {
            cur_idy = 0;
            cur_idx = -1;
        }
        break;

    case CURTAIN_RIGHT:
        {
            cur_idy = 0;
            cur_idx = 1;
        }
        break;

    case CURTAIN_MIDDLE:
        {
            cur_idy = 0;
            cur_idx = 0;
        }
        break;

    default:
        break;
    }

    switch (this->orientation)
    {
    case CURTAIN_UP:
        {
            idy = -1;
            idx = 0;
        }
        break;

    case CURTAIN_DOWN:
        {
            idy = 1;
            idx = 0;
        }
        break;

    case CURTAIN_LEFT:
        {
            idy = 0;
            idx = -1;
        }
        break;

    case CURTAIN_RIGHT:
        {
            idy = 0;
            idx = 1;
        }
        break;

    case CURTAIN_MIDDLE:
        {
            idy = 0;
            idx = 0;
        }
        return;

    default:
        break;
    }

    int16_t dy = 0;
    int16_t dx = 0;

    if (cur_idy == idy)
    {
        dy = (int)gui_get_screen_height() * idy * (1 - this->scope);
    }
    else
    {
        dy = (idy - cur_idy) * (int)gui_get_screen_height();
    }

    dy += curtainview->release_y;

    if (cur_idx == idx)
    {
        dx = (int)gui_get_screen_width() * idx * (1 - this->scope);
    }
    else
    {
        dx = (idx - cur_idx) * (int)gui_get_screen_width();
    }

    dx += curtainview->release_x;

    matrix_translate(dx, dy, obj->matrix);
}

static void gui_curtain_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            gui_curtain_input_prepare(obj);
            break;

        case OBJ_PREPARE:
            gui_curtain_prepare(obj);
            break;

        default:
            break;
        }
    }
}

void gui_curtain_ctor(gui_curtain_t     *this,
                      gui_obj_t         *parent,
                      const char        *filename,
                      int16_t            x,
                      int16_t            y,
                      int16_t            w,
                      int16_t            h,
                      T_GUI_CURTAIN_ENUM orientation,
                      float              scope)
{
    gui_obj_ctor(&this->base, parent, filename, x, y, w, h);
    ((gui_obj_t *)this)->obj_cb = gui_curtain_cb;
    ((gui_obj_t *)this)->has_input_prepare_cb = true;
    ((gui_obj_t *)this)->has_prepare_cb = true;
    ((gui_obj_t *)this)->type = CURTAIN;

    if (scope == 0)
    {
        scope = 1.0;
    }

    this->scope = scope;

    if (parent != NULL)
    {
        gui_curtainview_t *parent_ext = (gui_curtainview_t *)parent;
        this->orientation = orientation;
        parent_ext->curtain_cnt++;

        switch (orientation)
        {
        case CURTAIN_UP:
            {
                parent_ext->orientations.up = true;
                parent_ext->scope_up = this->scope;
            }
            break;

        case CURTAIN_DOWN:
            {
                parent_ext->orientations.down = true;
                parent_ext->scope_down = this->scope;
            }
            break;

        case CURTAIN_LEFT:
            {
                parent_ext->orientations.left = true;
                parent_ext->scope_left = this->scope;
            }
            break;

        case CURTAIN_RIGHT:
            {
                parent_ext->orientations.right = true;
                parent_ext->scope_right = this->scope;
            }
            break;

        default:
            break;
        }
    }
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

gui_curtain_t *gui_curtain_create(void              *parent,
                                  const char        *filename,
                                  int16_t            x,
                                  int16_t            y,
                                  int16_t            w,
                                  int16_t            h,
                                  T_GUI_CURTAIN_ENUM orientation,
                                  float              scope)
{
    gui_curtainview_t *curtainview = (gui_curtainview_t *)parent;

    if (orientation == CURTAIN_MIDDLE)
    {
        curtainview->has_center_curtain = true;
    }
    else if (orientation == CURTAIN_LEFT)
    {
        curtainview->has_left_curtain = true;
    }
    else if (orientation == CURTAIN_RIGHT)
    {
        curtainview->has_right_curtain = true;
    }
    else if (orientation == CURTAIN_UP)
    {
        curtainview->has_up_curtain = true;
    }
    else if (orientation == CURTAIN_DOWN)
    {
        curtainview->has_down_curtain = true;
    }

#define _paramgui_curtain_create_ this, parent, filename, x, y, w, h, orientation, scope
    GUI_NEW(gui_curtain_t, gui_curtain_ctor, _paramgui_curtain_create_)
}
