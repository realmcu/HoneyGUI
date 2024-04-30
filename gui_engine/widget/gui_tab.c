/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_tab.c
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
#include "tp_algo.h"
#include "kb_algo.h"
#include "gui_tab.h"
#include "gui_curtain.h"
#include "gui_img.h"
#include "gui_cube.h"

/** @defgroup WIDGET WIDGET
  * @{
  */
/*============================================================================*
 *                           Types
 *============================================================================*/
/** @defgroup WIDGET_Exported_Types WIDGET Exported Types
  * @{
  */


/** End of WIDGET_Exported_Types
  * @}
  */

/*============================================================================*
 *                           Constants
 *============================================================================*/
/** @defgroup WIDGET_Exported_Constants WIDGET Exported Constants
  * @{
  */


/** End of WIDGET_Exported_Constants
  * @}
  */

/*============================================================================*
 *                            Macros
 *============================================================================*/
/** @defgroup WIDGET_Exported_Macros WIDGET Exported Macros
  * @{
  */


/** End of WIDGET_Exported_Macros
  * @}
  */
/*============================================================================*
 *                            Variables
 *============================================================================*/
/** @defgroup WIDGET_Exported_Variables WIDGET Exported Variables
  * @{
  */


/** End of WIDGET_Exported_Variables
  * @}
  */

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
/** @defgroup WIDGET_Exported_Functions WIDGET Exported Functions
  * @{
  */

static void gui_tab_input_prepare(gui_obj_t *obj)
{
    gui_tab_t *this = (gui_tab_t *)obj;
    gui_tabview_t *parent = (gui_tabview_t *)(obj->parent);
    int16_t tab_x_gap = this->id.x - parent->cur_id.x;
    int16_t tab_y_gap = this->id.y - parent->cur_id.y;

    if (parent->loop)
    {
        int16_t tab_x_count = -parent->tab_cnt_left + parent->tab_cnt_right + 1;
        int16_t left_count = tab_x_count / 2 + tab_x_count % 2;
        int16_t right_count = tab_x_count / 2;

        int16_t tab_y_count = -parent->tab_cnt_up + parent->tab_cnt_down + 1;
        int16_t up_count = tab_y_count / 2 + tab_y_count % 2;
        int16_t down_count = tab_y_count / 2;

        if (tab_x_gap > right_count)
        {
            tab_x_gap -= tab_x_count;
        }
        else if (tab_x_gap < -left_count)
        {
            tab_x_gap += tab_x_count;
        }

        if (tab_y_gap > down_count)
        {
            tab_y_gap -= tab_y_count;
        }
        else if (tab_x_gap < -up_count)
        {
            tab_y_gap += tab_y_count;
        }
    }

    matrix_translate((tab_x_gap) * (int)this->base.w, \
                     (tab_y_gap) * (int)this->base.h, \
                     obj->matrix);
}

static void gui_tab_prepare(gui_obj_t *obj)
{
    gui_tab_t *this = (gui_tab_t *)obj;
    kb_info_t *kb = kb_get_info();
    gui_tabview_t *parent = (gui_tabview_t *)(obj->parent);

    int16_t tab_x_gap = this->id.x - parent->cur_id.x;
    int16_t tab_y_gap = this->id.y - parent->cur_id.y;

    if (parent->loop)
    {
        int16_t tab_x_count = - parent->tab_cnt_left + parent->tab_cnt_right + 1;
        int16_t left_count = tab_x_count / 2 + tab_x_count % 2;
        int16_t right_count = tab_x_count / 2;

        int16_t tab_y_count = -parent->tab_cnt_up + parent->tab_cnt_down + 1;
        int16_t up_count = tab_y_count / 2 + tab_y_count % 2;
        int16_t down_count = tab_y_count / 2;

        if (tab_x_gap > right_count)
        {
            tab_x_gap -= tab_x_count;
        }
        else if (tab_x_gap < -left_count)
        {
            tab_x_gap += tab_x_count;
        }

        if (tab_y_gap > down_count)
        {
            tab_y_gap -= tab_y_count;
        }
        else if (tab_x_gap < -up_count)
        {
            tab_y_gap += tab_y_count;
        }
    }

    if (parent->enable_pre_load)
    {
        if (parent->tab_need_pre_load == true)
        {
            if (tab_x_gap == 0)
            {
                gui_img_tree_convert_to_img((gui_obj_t *)this->rte_obj, NULL, parent->center_shot);

                if (this->shot_obj == NULL)
                {
                    this->shot_obj = (gui_obj_t *)gui_img_create_from_mem(obj,  "shot", (void *)parent->center_shot, 0,
                                                                          0, 0, 0);
                }
                else
                {
                    gui_img_set_attribute((gui_img_t *)(this->shot_obj), NULL, parent->center_shot, 0, 0);
                }
            }
            else if (tab_x_gap == 1)
            {
                gui_img_tree_convert_to_img((gui_obj_t *)this->rte_obj, NULL, parent->right_shot);

                if (this->shot_obj == NULL)
                {
                    this->shot_obj = (gui_obj_t *)gui_img_create_from_mem(obj,  "shot", (void *)parent->right_shot, 0,
                                                                          0, 0, 0);
                }
                else
                {
                    gui_img_set_attribute((gui_img_t *)(this->shot_obj), NULL, parent->right_shot, 0, 0);
                }
            }
            else if (tab_x_gap == -1)
            {
                gui_img_tree_convert_to_img((gui_obj_t *)this->rte_obj, NULL, parent->left_shot);

                if (this->shot_obj == NULL)
                {
                    this->shot_obj = (gui_obj_t *)gui_img_create_from_mem(obj,  "shot", (void *)parent->left_shot, 0, 0,
                                                                          0, 0);
                }
                else
                {
                    gui_img_set_attribute((gui_img_t *)(this->shot_obj), NULL, parent->left_shot, 0, 0);
                }
            }
        }

        if (parent->release_x != 0)
        {
            if ((tab_x_gap == 0) || (tab_x_gap == -1) || (tab_x_gap == 1))
            {
                gui_obj_show(this->shot_obj, true);
                gui_obj_show(this->rte_obj, false);
            }
        }
        else
        {
            if ((tab_x_gap == 0) || (tab_x_gap == -1) || (tab_x_gap == 1))
            {
                gui_obj_show(this->shot_obj, false);
                gui_obj_show(this->rte_obj, true);
            }
        }
    }

    if (this->style == CLASSIC)
    {
        matrix_translate(tab_x_gap * (int)this->base.w + parent->release_x, \
                         tab_y_gap * (int)this->base.h + parent->release_y, \
                         obj->matrix);
    }
    else if (this->style == TAB_CUBE)
    {
        gui_tab_cube(obj, tab_x_gap, tab_y_gap);
    }
    else if (this->style == TAB_ROTATE)
    {
        gui_tab_rotate(obj, tab_x_gap, tab_y_gap);
    }
    else if (this->style == REDUCTION)
    {
        gui_tab_reduction(obj, tab_x_gap, tab_y_gap);
    }
    else if (this->style == FADE)
    {
        gui_tab_fade(obj, tab_x_gap, tab_y_gap);
    }
    else if (this->style == REDUCTION_FADE)
    {
        gui_tab_reduction_fade(obj, tab_x_gap, tab_y_gap);
    }

    if ((kb->type == KB_SHORT) && (obj->event_dsc_cnt > 0))
    {
        gui_obj_event_set(obj, GUI_EVENT_KB_SHORT_CLICKED);
    }
}

static void gui_tab_destroy(gui_obj_t *obj)
{
    if (obj->parent != NULL)
    {
        gui_tab_t *tab = (gui_tab_t *)obj;
        gui_tabview_t *tabview = (gui_tabview_t *)obj->parent;

        if (tabview->tab_cnt > 0)
        {
            tabview->tab_cnt--;
        }

        if (tab->id.x > 0)
        {
            if (tabview->tab_cnt_right > 0)
            {
                tabview->tab_cnt_right--;
            }
        }
        else if (tab->id.x < 0)
        {
            if (tabview->tab_cnt_left < 0)
            {
                tabview->tab_cnt_left++;
            }
        }

        if (tab->id.y > 0)
        {
            if (tabview->tab_cnt_down > 0)
            {
                tabview->tab_cnt_down--;
            }
        }
        else if (tab->id.y < 0)
        {
            if (tabview->tab_cnt_up < 0)
            {
                tabview->tab_cnt_up++;
            }
        }
    }
}

static void gui_tab_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            {
                gui_tab_input_prepare(obj);
            }
            break;

        case OBJ_PREPARE:
            {
                gui_tab_prepare(obj);
            }
            break;

        case OBJ_DESTORY:
            {
                gui_tab_destroy(obj);
            }
            break;

        default:
            break;
        }
    }
}

static void gui_tab_ctor(gui_tab_t  *this,
                         gui_obj_t  *parent,
                         const char *filename,
                         int16_t     x,
                         int16_t     y,
                         int16_t     w,
                         int16_t     h,
                         int16_t     idx,
                         int16_t     idy)
{

    gui_obj_ctor(&this->base, parent, filename, x, y, w, h);

    GET_BASE(this)->obj_cb = gui_tab_cb;
    GET_BASE(this)->has_input_prepare_cb = true;
    GET_BASE(this)->has_prepare_cb = true;
    GET_BASE(this)->has_destroy_cb = true;
    GET_BASE(this)->type = TAB;

    gui_tabview_t *parent_ext = (gui_tabview_t *)parent;
    this->style = parent_ext->style;
    this->id.x = idx;
    this->id.y = idy;
    this->id.z = idy;

    this->normal.x = 0;
    this->normal.y = 0;
    this->normal.z = -1.0f;

    parent_ext->tab_cnt++;
    if (idx > 0)
    {
        parent_ext->tab_cnt_right++;
    }
    else if (idx < 0)
    {
        parent_ext->tab_cnt_left--;
    }

    if (idy > 0)
    {
        parent_ext->tab_cnt_down++;
    }
    else if (idy < 0)
    {
        parent_ext->tab_cnt_up--;
    }
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
gui_obj_t *gui_tab_get_rte_obj(gui_tab_t *this)
{
    gui_tabview_t *tabview = (gui_tabview_t *)(this->base.parent);

    if (tabview->enable_pre_load == false)
    {
        return (gui_obj_t *)this;
    }

    return this->rte_obj;
}

void gui_tab_update_preload(gui_obj_t *obj)
{
    gui_tabview_t *parent = (gui_tabview_t *)(obj->parent);
    gui_tab_t *this = (gui_tab_t *)obj;
    int16_t tab_x_gap = this->id.x - parent->cur_id.x;
    int16_t tab_y_gap = this->id.y - parent->cur_id.y;
    GUI_UNUSED(tab_y_gap);

    if (parent->enable_pre_load)
    {
        if (tab_x_gap == 0)
        {
            gui_img_tree_convert_to_img((gui_obj_t *)this->rte_obj, NULL, parent->center_shot);

            if (this->shot_obj == NULL)
            {
                this->shot_obj = (gui_obj_t *)gui_img_create_from_mem(obj,  "shot", (void *)parent->center_shot, 0,
                                                                      0, 0, 0);
            }
            else
            {
                gui_img_set_attribute((gui_img_t *)(this->shot_obj), NULL, parent->center_shot, 0, 0);
            }
        }
        else if (tab_x_gap == 1)
        {
            gui_img_tree_convert_to_img((gui_obj_t *)this->rte_obj, NULL, parent->right_shot);

            if (this->shot_obj == NULL)
            {
                this->shot_obj = (gui_obj_t *)gui_img_create_from_mem(obj,  "shot", (void *)parent->right_shot, 0,
                                                                      0, 0, 0);
            }
            else
            {
                gui_img_set_attribute((gui_img_t *)(this->shot_obj), NULL, parent->right_shot, 0, 0);
            }
        }
        else if (tab_x_gap == -1)
        {
            gui_img_tree_convert_to_img((gui_obj_t *)this->rte_obj, NULL, parent->left_shot);

            if (this->shot_obj == NULL)
            {
                this->shot_obj = (gui_obj_t *)gui_img_create_from_mem(obj,  "shot", (void *)parent->left_shot, 0, 0,
                                                                      0, 0);
            }
            else
            {
                gui_img_set_attribute((gui_img_t *)(this->shot_obj), NULL, parent->left_shot, 0, 0);
            }
        }
    }
    else
    {
        GUI_ASSERT(parent->enable_pre_load == true);
    }
}

gui_tab_t *gui_tab_create(void       *parent,
                          const char *name,
                          int16_t     x,
                          int16_t     y,
                          int16_t     w,
                          int16_t     h,
                          int16_t     idx,
                          int16_t     idy)
{
    if (w == 0)
    {
        w = (int)gui_get_screen_width();
    }

    if (h == 0)
    {
        h = (int)gui_get_screen_height();
    }

    gui_tab_t *this = gui_malloc(sizeof(gui_tab_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_tab_t));

    gui_tab_ctor(this, (gui_obj_t *)parent, name, x, y, w, h, idx, idy);

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }
    GET_BASE(this)->create_done = true;
    this->rte_obj = gui_obj_create(this, "tab_rte", this->base.x, this->base.y, this->base.w,
                                   this->base.h);
    return this;
}

void gui_tab_set_style(gui_tab_t *this, T_SLIDE_STYLE style)
{
    this->style = style;
}

/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
