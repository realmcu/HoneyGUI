/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_tab_rotate.c
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
#include "gui_img.h"
#include "gui_view.h"
#include "tp_algo.h"

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

void gui_view_rotate_book(gui_obj_t *obj, int16_t tab_x_gap, int16_t tab_y_gap)
{
    static gui_list_t *parent_child_list_next = NULL;
    gui_list_t list = GUI_BASE(obj)->parent->child_list;
    if (list.prev == list.next)
    {
        parent_child_list_next = &obj->brother_list;
        return;
    }
    gui_view_t *this = (gui_view_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();
    gui_obj_t *parent = obj->parent;
    gui_matrix_t rotate_3D;
    gui_matrix_t temp;
    int16_t release_x;
    float w = this->base.w;
    float h = this->base.h;
    float rotate_degree;
    bool rotate_90 = 0;
    gui_vertex_t v0 = {-w, -h, 0};
    gui_vertex_t v1 = {w,  -h, 0};
    gui_vertex_t v2 = {w,  h,  0};
    gui_vertex_t v3 = {-w, h,  0};

    gui_vertex_t tv0, tv1, tv2, tv3;
    gui_vertex_t rv0, rv1, rv2, rv3;

    release_x = this->release_x;
    if (release_x > this->base.w / 2)
    {
        tab_x_gap++;
        release_x = release_x - this->base.w;
        rotate_90 = 1;
    }

    if (release_x < -this->base.w / 2)
    {
        tab_x_gap--;
        release_x = release_x + this->base.w;
        rotate_90 = 1;
    }

    rotate_degree = 90 * release_x / (this->base.w / 2);

    matrix_compute_rotate(0, rotate_degree, 0, &rotate_3D);

    matrix_transfrom_rotate(&rotate_3D, &v0, &tv0, 0, 0, 0);
    matrix_transfrom_rotate(&rotate_3D, &v1, &tv1, 0, 0, 0);
    matrix_transfrom_rotate(&rotate_3D, &v2, &tv2, 0, 0, 0);
    matrix_transfrom_rotate(&rotate_3D, &v3, &tv3, 0, 0, 0);

    matrix_compute_rotate(0, 0, 0, &rotate_3D);
    float xoff = (float)dc->screen_width / 2;
    float yoff = (float)dc->screen_height / 2 ;
    float zoff = -(xoff + yoff);

    matrix_transfrom_rotate(&rotate_3D, &tv0, &rv0, xoff, yoff, zoff);
    matrix_transfrom_rotate(&rotate_3D, &tv1, &rv1, xoff, yoff, zoff);
    matrix_transfrom_rotate(&rotate_3D, &tv2, &rv2, xoff, yoff, zoff);
    matrix_transfrom_rotate(&rotate_3D, &tv3, &rv3, xoff, yoff, zoff);

    gui_vertex_t p = {(float)(dc->screen_width) / 2, (float)(dc->screen_height) / 2, -zoff};

    matrix_transfrom_blit(this->base.w, this->base.h, &p, &rv0, &rv1, &rv2, &rv3,
                          &temp);

    static bool list_change = 1; //0: list change; 1: list restore
    static bool list_restore = 0;
    touch_info_t *tp = tp_get_info();
    if (release_x)
    {
        if (release_x > 0)
        {
            GUI_TYPE(gui_img_t, this->shot_pave_obj)->scope_x1 = (1 + tab_x_gap) * dc->screen_width / 2;
            GUI_TYPE(gui_img_t, this->shot_pave_obj)->scope_x2 = (2 + tab_x_gap) * dc->screen_width / 2;
            GUI_TYPE(gui_img_t, this->shot_pave_obj)->scope_flag = 1;
        }
        else if (release_x < 0)
        {
            GUI_TYPE(gui_img_t, this->shot_pave_obj)->scope_x1 = tab_x_gap * dc->screen_width / 2;
            GUI_TYPE(gui_img_t, this->shot_pave_obj)->scope_x2 = (1 + tab_x_gap) * dc->screen_width / 2;
            GUI_TYPE(gui_img_t, this->shot_pave_obj)->scope_flag = 1;
        }
        if (tp->type == TOUCH_LEFT_SLIDE || tp->type == TOUCH_RIGHT_SLIDE)
        {
            list_restore = 1;
        }
        if (rotate_90 && !list_change) // slide over 90, restore list
        {
            list_change = 1;
            GUI_BASE(parent)->child_list.next = parent_child_list_next;
            GUI_BASE(parent)->child_list.prev = &obj->brother_list;
            gui_list_t *next_tab = obj->brother_list.next;
            obj->brother_list.next = obj->brother_list.next->next;
            next_tab->next = &obj->brother_list;
        }
        if (!rotate_90 && list_change && !list_restore)
        {
            list_change = 0;
            gui_list_t *next_tab = obj->brother_list.next;
            obj->brother_list.next = obj->brother_list.next->next;
            next_tab->next = &obj->brother_list;
            GUI_BASE(parent)->child_list.next = next_tab;
            GUI_BASE(parent)->child_list.prev = next_tab->next;
        }
    }
    else
    {
        if (!list_change)
        {
            list_change = 1;
            GUI_BASE(parent)->child_list.next = parent_child_list_next;
            GUI_BASE(parent)->child_list.prev = &obj->brother_list;
            gui_list_t *next_tab = obj->brother_list.next;
            obj->brother_list.next = obj->brother_list.next->next;
            next_tab->next = &obj->brother_list;
            obj->not_show = 1;
        }
        list_restore = 0;
        // matrix_translate((tab_x_gap) * 2 * (int)this->base.w, \
        //                  (tab_y_gap) * 2 * (int)this->base.h, \
        //                  obj->matrix); //todo multi 2 for bug fix
    }
    matrix_multiply(obj->matrix, &temp);
}

