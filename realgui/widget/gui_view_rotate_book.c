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

void gui_view_rotate_book(gui_obj_t *obj)
{
    gui_view_t *this = (gui_view_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();
    gui_obj_t *parent = obj->parent;
    gui_matrix_t rotate_3D;
    gui_matrix_t temp;
    int16_t idx = this->cur_id.x;
    int16_t idy = this->cur_id.y;
    float w = this->base.w;
    float h = this->base.h;
    float rotate_degree_x = 0;
    float rotate_degree_y = 0;
    bool rotate_90 = 0;

    gui_vertex_t v0 = {-w, -h, 0};
    gui_vertex_t v1 = {w,  -h, 0};
    gui_vertex_t v2 = {w,  h,  0};
    gui_vertex_t v3 = {-w, h,  0};

    gui_vertex_t tv0, tv1, tv2, tv3;
    gui_vertex_t rv0, rv1, rv2, rv3;

    int16_t release_x = this->release_x;
    int16_t release_y = this->release_y;
    if (release_x > this->base.w / 2)
    {
        idx++;
        release_x = release_x - this->base.w;
        rotate_90 = 1;
    }
    if (release_x < -this->base.w / 2)
    {
        idx--;
        release_x = release_x + this->base.w;
        rotate_90 = 1;
    }

    if (release_y > this->base.h / 2)
    {
        idy++;
        release_y = release_y - this->base.h;
        rotate_90 = 1;
    }
    if (release_y < -this->base.h / 2)
    {
        idy--;
        release_y = release_y + this->base.h;
        rotate_90 = 1;
    }

    rotate_degree_x = 90 * release_y / (this->base.h / 2);
    rotate_degree_y = 90 * release_x / (this->base.w / 2);

    matrix_compute_rotate(-rotate_degree_x, rotate_degree_y, 0, &rotate_3D);

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
    if (release_x || release_y)
    {
        // set scope
        uint8_t scope_x = release_x > 0 ? 1 : 0;
        uint8_t scope_y = release_y > 0 ? 1 : 0;
        GUI_TYPE(gui_img_t, this->shot_pave_obj)->scope_flag = 1;
        GUI_TYPE(gui_img_t, this->shot_pave_obj)->scope_x1 = (scope_x + idx) * dc->screen_width / 2;
        GUI_TYPE(gui_img_t, this->shot_pave_obj)->scope_x2 = (scope_x + idx + 1) * dc->screen_width / 2;
        GUI_TYPE(gui_img_t, this->shot_pave_obj)->scope_y1 = (scope_y + idy) * dc->screen_height / 2;
        GUI_TYPE(gui_img_t, this->shot_pave_obj)->scope_y2 = (scope_y + idy + 1) * dc->screen_height / 2;
        if (release_x)
        {
            GUI_TYPE(gui_img_t, this->shot_pave_obj)->scope_y1 = 0;
            GUI_TYPE(gui_img_t, this->shot_pave_obj)->scope_y2 = dc->screen_height;
        }
        else
        {
            GUI_TYPE(gui_img_t, this->shot_pave_obj)->scope_x1 = 0;
            GUI_TYPE(gui_img_t, this->shot_pave_obj)->scope_x2 = dc->screen_width;
        }

        // change obj list
        if (tp->type >= TOUCH_LEFT_SLIDE && tp->type <= TOUCH_DOWN_SLIDE)
        {
            list_restore = 1;
        }
        if (rotate_90 && !list_change) // slide over 90, restore list
        {
            list_change = 1;
            GUI_BASE(parent)->child_list.prev = &obj->brother_list;
            gui_list_t *next_tab = obj->brother_list.next;
            obj->brother_list.next = obj->brother_list.next->next;
            next_tab->next = &obj->brother_list;
            GUI_BASE(parent)->child_list.next = next_tab;
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
            GUI_BASE(parent)->child_list.prev = &obj->brother_list;
            gui_list_t *next_tab = obj->brother_list.next;
            obj->brother_list.next = obj->brother_list.next->next;
            next_tab->next = &obj->brother_list;
            GUI_BASE(parent)->child_list.next = next_tab;
            obj->not_show = 1;
        }
        list_restore = 0;
    }
    matrix_multiply(obj->matrix, &temp);
}

