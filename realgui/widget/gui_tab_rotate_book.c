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
#include "gui_tab.h"
#include "gui_curtain.h"
#include "gui_img.h"
#include "gui_cube.h"

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

void gui_tab_rotate_book(gui_obj_t *obj, int16_t tab_x_gap, int16_t tab_y_gap)
{
    gui_tab_t *this = (gui_tab_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();
    gui_tabview_t *parent = (gui_tabview_t *)(obj->parent);
    gui_matrix_t rotate_3D;
    gui_matrix_t temp;
    int32_t release_x;
    int32_t release_x_his;
    float w = this->base.w;
    float h = this->base.h;
    float rotate_degree;
    int16_t tab_x_gap_his = tab_x_gap;
    gui_vertex_t v0 = {-w, -h, 0};
    gui_vertex_t v1 = {w,  -h, 0};
    gui_vertex_t v2 = {w,  h,  0};
    gui_vertex_t v3 = {-w, h,  0};

    gui_vertex_t tv0, tv1, tv2, tv3;
    gui_vertex_t rv0, rv1, rv2, rv3;

    release_x = parent->release_x;
    release_x_his = release_x;
    if (release_x > this->base.w / 2)
    {
        tab_x_gap++;
        release_x = release_x - this->base.w;
    }

    if (release_x < -this->base.w / 2)
    {
        tab_x_gap--;
        release_x = release_x + this->base.w;
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
    static gui_list_t *parent_child_list_next = NULL;

    if (release_x > 0)
    {
        if (tab_x_gap == 0 || tab_x_gap == -1)
        {
            GUI_TYPE(gui_img_t, this->shot_pave_obj)->scope_x1 = (1 + tab_x_gap) * dc->screen_width / 2;
            GUI_TYPE(gui_img_t, this->shot_pave_obj)->scope_x2 = (2 + tab_x_gap) * dc->screen_width / 2;
            GUI_TYPE(gui_img_t, this->shot_pave_obj)->scope_flag = 1;
        }
        else
        {
            matrix_translate((tab_x_gap) * 2 * (int)this->base.w, \
                             (tab_y_gap) * 2 * (int)this->base.h, \
                             obj->matrix); //todo multi 2 for bug fix
        }
        if (tab_x_gap == 0)
        {
            if (!list_change) // slide over 90, restore list
            {
                list_change = 1;
                GUI_BASE(parent)->child_list.next = parent_child_list_next;
                gui_list_t *next_tab = obj->brother_list.next;
                obj->brother_list.next = obj->brother_list.next->next;
                next_tab->next = &obj->brother_list;
            }
        }
    }
    else if (release_x < 0)
    {
        if (tab_x_gap == 0 || tab_x_gap == 1)
        {
            GUI_TYPE(gui_img_t, this->shot_pave_obj)->scope_x1 = tab_x_gap * dc->screen_width / 2;
            GUI_TYPE(gui_img_t, this->shot_pave_obj)->scope_x2 = (1 + tab_x_gap) * dc->screen_width / 2;
            GUI_TYPE(gui_img_t, this->shot_pave_obj)->scope_flag = 1;
        }
        else
        {
            matrix_translate((tab_x_gap) * 2 * (int)this->base.w, \
                             (tab_y_gap) * 2 * (int)this->base.h, \
                             obj->matrix); //todo multi 2 for bug fix
        }
        if (tab_x_gap == 0 && list_change &&
            GUI_BASE(parent)->child_list.prev != &obj->brother_list)// one right slide can only change list once
        {
            // when there is no more tabs on the left
            list_change = 0;
            gui_list_t *next_tab = obj->brother_list.next;
            obj->brother_list.next = obj->brother_list.next->next;
            next_tab->next = &obj->brother_list;
            GUI_BASE(parent)->child_list.next = next_tab;
        }
    }
    else if (release_x == 0)
    {
        if (list_change)
        {
            parent_child_list_next = GUI_BASE(parent)->child_list.next;
        }
        list_change = 1;
        // when left slide over 90, need to restore list and draw shots here
        if (tab_x_gap == 1 && GUI_BASE(parent)->child_list.next == &obj->brother_list)
        {
            GUI_BASE(parent)->child_list.next = parent_child_list_next;
            gui_list_t *next_tab = obj->brother_list.next;
            obj->brother_list.next = obj->brother_list.next->next;
            next_tab->next = &obj->brother_list;

            // draw center_shot
            {
                gui_tab_t *tab = (gui_tab_t *)gui_list_entry(next_tab, gui_obj_t, brother_list);
                gui_img_tree_convert_to_img((gui_obj_t *)tab->rte_obj, NULL, parent->center_shot);
                if (!tab->shot_obj)
                {
                    tab->shot_obj = (gui_obj_t *)gui_img_create_from_mem(tab,  "shot",
                                                                         (void *)parent->center_shot, 0,
                                                                         0, 0, 0);
                    tab->shot_pave_obj = (gui_obj_t *)gui_img_create_from_mem(tab,  "shot_1",
                                                                              (void *)parent->center_shot, 0,
                                                                              0, 0, 0);
                    gui_img_set_mode((gui_img_t *)(tab->shot_pave_obj), IMG_BYPASS_MODE);
                    gui_img_set_mode((gui_img_t *)(tab->shot_obj), IMG_BYPASS_MODE);
                    extern void obj_pave_shot_cb(void *p, void *this_widget, gui_animate_t *animate);
                    gui_img_set_animate((gui_img_t *)(tab->shot_pave_obj), 1000, -1, obj_pave_shot_cb, (void *)obj);
                }
                else
                {
                    gui_img_set_attribute((gui_img_t *)(tab->shot_obj), NULL, parent->center_shot, 0, 0);
                    gui_img_set_attribute((gui_img_t *)(tab->shot_pave_obj), NULL, parent->center_shot, 0, 0);
                }
            }
            // draw left_shot
            {
                if (&GUI_BASE(parent)->child_list != next_tab->prev)
                {
                    gui_tab_t *tab = (gui_tab_t *)gui_list_entry(next_tab->prev, gui_obj_t, brother_list);
                    gui_img_tree_convert_to_img((gui_obj_t *)tab->rte_obj, NULL, parent->left_shot);
                    if (!tab->shot_obj)
                    {
                        tab->shot_obj = (gui_obj_t *)gui_img_create_from_mem(tab,  "shot",
                                                                             (void *)parent->left_shot, 0,
                                                                             0, 0, 0);
                        tab->shot_pave_obj = (gui_obj_t *)gui_img_create_from_mem(tab,  "shot_1",
                                                                                  (void *)parent->left_shot, 0,
                                                                                  0, 0, 0);
                        gui_img_set_mode((gui_img_t *)(tab->shot_pave_obj), IMG_BYPASS_MODE);
                        gui_img_set_mode((gui_img_t *)(tab->shot_obj), IMG_BYPASS_MODE);
                        extern void obj_pave_shot_cb(void *p, void *this_widget, gui_animate_t *animate);
                        gui_img_set_animate((gui_img_t *)(tab->shot_pave_obj), 1000, -1, obj_pave_shot_cb, (void *)obj);
                    }
                    else
                    {
                        gui_img_set_attribute((gui_img_t *)(tab->shot_obj), NULL, parent->left_shot, 0, 0);
                        gui_img_set_attribute((gui_img_t *)(tab->shot_pave_obj), NULL, parent->left_shot, 0, 0);
                    }
                }
            }
        }
        matrix_translate((tab_x_gap) * 2 * (int)this->base.w, \
                         (tab_y_gap) * 2 * (int)this->base.h, \
                         obj->matrix); //todo multi 2 for bug fix
    }
    // prevent left silde when displaying first tab and right silde when displaying last tab
    if (tab_x_gap_his == 0 && ((GUI_BASE(parent)->child_list.prev == &obj->brother_list &&
                                release_x_his < 0) ||
                               (GUI_BASE(parent)->child_list.next == &obj->brother_list && release_x_his > 0)))
    {
        matrix_identity(&temp);
        gui_obj_show(this->shot_pave_obj, 0);
        matrix_multiply(obj->matrix, &temp);
    }
    matrix_multiply(obj->matrix, &temp);
}

