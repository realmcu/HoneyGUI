/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_view_cube.c
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

void gui_view_cube(gui_view_t *this)
{
    gui_obj_t *obj = GUI_BASE(this);
    gui_dispdev_t *dc = gui_get_dc();
    gui_matrix_t temp;
    gui_matrix_t rotate_3D;
    int16_t idx = this->cur_id.x;
    int16_t idy = this->cur_id.y;
    float w = this->base.w;
    float h = this->base.h;
    float d = (w + h) / 2;
    float rotate_degree_x = 0;
    float rotate_degree_y = 0;
    float xoff;
    float yoff;
    float zoff;

    gui_vertex_t v0 = {-w, -h, d};
    gui_vertex_t v1 = {w,  -h, d};
    gui_vertex_t v2 = {w,  h,  d};
    gui_vertex_t v3 = {-w, h,  d};

    gui_vertex_t tv0, tv1, tv2, tv3;
    gui_vertex_t rv0, rv1, rv2, rv3;

    rotate_degree_x = 90 * this->release_y / (this->base.h) + 90.0 * (idy);
    rotate_degree_y = 90 * this->release_x / (this->base.w) + 90.0 * (idx);
    matrix_compute_rotate(-rotate_degree_x, rotate_degree_y, 0, &rotate_3D);

    gui_vertex_t normal = {0, 0, 0};
    matrix_multiply_normal(&rotate_3D, &normal);

    matrix_transfrom_rotate(&rotate_3D, &v0, &tv0, 0, 0, 0);
    matrix_transfrom_rotate(&rotate_3D, &v1, &tv1, 0, 0, 0);
    matrix_transfrom_rotate(&rotate_3D, &v2, &tv2, 0, 0, 0);
    matrix_transfrom_rotate(&rotate_3D, &v3, &tv3, 0, 0, 0);

    matrix_compute_rotate(0, 0, 0, &rotate_3D);
    xoff = (float)dc->screen_width / 2;
    yoff = (float)dc->screen_height / 2 ;
    zoff = -(xoff + yoff) * 2;

    matrix_transfrom_rotate(&rotate_3D, &tv0, &rv0, xoff, yoff, zoff);
    matrix_transfrom_rotate(&rotate_3D, &tv1, &rv1, xoff, yoff, zoff);
    matrix_transfrom_rotate(&rotate_3D, &tv2, &rv2, xoff, yoff, zoff);
    matrix_transfrom_rotate(&rotate_3D, &tv3, &rv3, xoff, yoff, zoff);

    gui_vertex_t p = {(float)(dc->screen_width) / 2, (float)(dc->screen_height) / 2, xoff + yoff};

    matrix_transfrom_blit(this->base.w, this->base.h, &p, &rv0, &rv1, &rv2, &rv3,
                          &temp);

    if (rv0.x > rv1.x || rv0.y > rv2.y)
    {
        obj->active = false;
    }
    else
    {
        obj->active = true;
    }

    if (rotate_degree_x > 70 || rotate_degree_x < -70 ||
        rotate_degree_y > 70 || rotate_degree_y < -70)
    {
        matrix_translate((idx) * (int)this->base.w, \
                         (idy) * (int)this->base.h, \
                         obj->matrix);
    }

    matrix_multiply(obj->matrix, &temp);
}

