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
#include "gui_list.h"

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

void gui_list_rotate_curl(gui_list_note_t *this, float degree)
{
    gui_obj_t *obj = GUI_BASE(this);
    gui_list_t *list = (gui_list_t *)obj->parent;
    gui_matrix_t rotate_3D;
    gui_matrix_t temp;
    float w = obj->w;
    float h = obj->h;
    float rotate_degree_x = 0;
    float rotate_degree_y = 0;
    if (list->dir == HORIZONTAL)
    {
        rotate_degree_x = -degree;
    }
    else
    {
        rotate_degree_y = degree;
    }

    gui_vertex_t v0 = {0, 0, 0};
    gui_vertex_t v1 = {w * 2, 0, 0};
    gui_vertex_t v2 = {w * 2, h * 2, 0};
    gui_vertex_t v3 = {0, h * 2,  0};

    gui_vertex_t tv0, tv1, tv2, tv3;
    gui_vertex_t rv0, rv1, rv2, rv3;

    matrix_compute_rotate(rotate_degree_x, rotate_degree_y, 0, &rotate_3D);

    matrix_transfrom_rotate(&rotate_3D, &v0, &tv0, 0, 0, 0);
    matrix_transfrom_rotate(&rotate_3D, &v1, &tv1, 0, 0, 0);
    matrix_transfrom_rotate(&rotate_3D, &v2, &tv2, 0, 0, 0);
    matrix_transfrom_rotate(&rotate_3D, &v3, &tv3, 0, 0, 0);

    matrix_compute_rotate(0, 0, 0, &rotate_3D);
    float xoff = -(float)w / 2;
    float yoff = -(float)h / 2 ;
    float zoff = (xoff + yoff) * 2; //multiply 2 to make it look good

    matrix_transfrom_rotate(&rotate_3D, &tv0, &rv0, xoff, yoff, zoff);
    matrix_transfrom_rotate(&rotate_3D, &tv1, &rv1, xoff, yoff, zoff);
    matrix_transfrom_rotate(&rotate_3D, &tv2, &rv2, xoff, yoff, zoff);
    matrix_transfrom_rotate(&rotate_3D, &tv3, &rv3, xoff, yoff, zoff);

    gui_vertex_t p = {(float)w / 2, (float)h / 2, -zoff};

    matrix_transfrom_blit(this->base.w, this->base.h, &p, &rv0, &rv1, &rv2, &rv3,
                          &temp);

    matrix_multiply(obj->matrix, &temp);
}

