/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_view_rotate.c
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

void gui_view_rotate(gui_view_t *_this, int16_t release)
{
    gui_obj_t *obj = GUI_BASE(_this);
    gui_dispdev_t *dc = gui_get_dc();
    gui_matrix_t temp;
    gui_matrix_t rotate_3D;
    float w = _this->base.w;
    float h = _this->base.h;
    float rotate_degree_x = 0;
    float rotate_degree_y = 0;
    float xoff;
    float yoff;
    float zoff;

    gui_vertex_t v0 = {-w, -h, 0};
    gui_vertex_t v1 = {w,  -h, 0};
    gui_vertex_t v2 = {w,  h,  0};
    gui_vertex_t v3 = {-w, h,  0};

    gui_vertex_t tv0, tv1, tv2, tv3;
    gui_vertex_t rv0, rv1, rv2, rv3;

    int16_t offset_x = 0, offset_y = 0;
    if (_this->current_transition_style == SWITCH_IN_FROM_LEFT_USE_ROTATE)
    {
        offset_x = release - w;
    }
    else if (_this->current_transition_style == SWITCH_IN_FROM_RIGHT_USE_ROTATE)
    {
        offset_x = release + w;
    }
    else if (_this->current_transition_style == SWITCH_IN_FROM_TOP_USE_ROTATE)
    {
        offset_y = release - h;
    }
    else if (_this->current_transition_style == SWITCH_IN_FROM_BOTTOM_USE_ROTATE)
    {
        offset_y = release + h;
    }
    else if (_this->current_transition_style == SWITCH_OUT_TO_LEFT_USE_ROTATE ||
             _this->current_transition_style == SWITCH_OUT_TO_RIGHT_USE_ROTATE)
    {
        offset_x = release;
    }
    else
    {
        offset_y = release;
    }
    rotate_degree_x = 90 * offset_y / (_this->base.h / 2);
    rotate_degree_y = 90 * offset_x / (_this->base.w / 2);
    matrix_compute_rotate(-rotate_degree_x, rotate_degree_y, 0, &rotate_3D);

    matrix_transfrom_rotate(&rotate_3D, &v0, &tv0, 0, 0, 0);
    matrix_transfrom_rotate(&rotate_3D, &v1, &tv1, 0, 0, 0);
    matrix_transfrom_rotate(&rotate_3D, &v2, &tv2, 0, 0, 0);
    matrix_transfrom_rotate(&rotate_3D, &v3, &tv3, 0, 0, 0);

    matrix_compute_rotate(0, 0, 0, &rotate_3D);
    xoff = (float)dc->screen_width / 2;
    yoff = (float)dc->screen_height / 2 ;
    zoff = -(xoff + yoff);

    matrix_transfrom_rotate(&rotate_3D, &tv0, &rv0, xoff, yoff, zoff);
    matrix_transfrom_rotate(&rotate_3D, &tv1, &rv1, xoff, yoff, zoff);
    matrix_transfrom_rotate(&rotate_3D, &tv2, &rv2, xoff, yoff, zoff);
    matrix_transfrom_rotate(&rotate_3D, &tv3, &rv3, xoff, yoff, zoff);

    gui_vertex_t p = {(float)(dc->screen_width) / 2, (float)(dc->screen_height) / 2, -zoff};

    matrix_transfrom_blit(_this->base.w, _this->base.h, &p, &rv0, &rv1, &rv2, &rv3,
                          &temp);

    if (rotate_degree_x > 90 || rotate_degree_x < -90 ||
        rotate_degree_y > 90 || rotate_degree_y < -90)
    {
        matrix_translate((int)_this->base.w, \
                         (int)_this->base.h, \
                         obj->matrix);
    }

    matrix_multiply(obj->matrix, &temp);
}

