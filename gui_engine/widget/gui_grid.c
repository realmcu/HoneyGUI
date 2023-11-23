/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_grid.h
  * @brief  grid design
  * @details Evenly layout vertically and horizontally
  * @author triton_yu@realsil.com.cn
  * @date 2023/10/19
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <gui_grid.h>
#include "gui_list.h"
#include "tp_algo.h"
#include "gui_img.h"
#include "gui_canvas.h"



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

static void obj_update_att(struct _gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    uint32_t member_count = 0;
    struct gui_grid *this = (void *)obj;

    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    gui_list_for_each_safe(node, tmp, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        obj->x = member_count / this->col * this->gap_col_scale;
        obj->y = member_count % this->col * this->gap_row_scale;

        member_count++;
    }
    if (!this->row_count_flag)
    {
        this->row_count = member_count;
        this->row_count_flag = true;
    }
    if (!this->col_count_flag)
    {
        this->col_count = member_count;
        this->col_count_flag = true;
    }
}

static void deal_img_in_root(gui_obj_t *object, float x, float y)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    gui_list_for_each_safe(node, tmp, &object->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        switch (obj->type)
        {
        case IMAGE_FROM_MEM:
            {


            }
            break;
        case CANVAS:
            {
                gui_canvas_t *img = (void *)obj;
                GUI_UNUSED(img);
            }
            break;
        default:
            break;
        }
        deal_img_in_root(obj, x, y);
    }


}

static void tab_prepare_scale(gui_obj_t *obj)
{
    obj_update_att(obj);
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = (touch_info_t *)tp_get_info();
    struct gui_grid *this = (void *)obj;
    switch (this->style)
    {
    case GRID_CLASSIC:
        break;
    default:
        break;
    }


}
static void gui_grid_ctor(struct gui_grid *this,
                          gui_obj_t *parent, int16_t x,
                          int16_t y,
                          int16_t col,
                          int16_t row,
                          uint16_t gap_col,
                          uint16_t gap_row)
{
    gui_log("x = %d, y = %d, col = %d, row = %d, gap_col = %d, gap_row = %d", x, y, col, row, gap_col,
            gap_row);
    gui_obj_ctor(&this->base, parent, "grid", x, y, gap_col * col, gap_row * row);//700
    //GET_BASE(this)->obj_update_att = obj_update_att;
    GET_BASE(this)->obj_prepare = tab_prepare_scale;
    this->row = row;
    this->col = col;
    this->gap_col = gap_col;
    this->gap_row = gap_row;
    this->gap_col_scale = gap_col;
    this->gap_row_scale = gap_row;
    this->change_threshold = 50;
    this->start_x = x;
    this->start_y = y;
}
/*============================================================================*
 *                           Public Functions
 *============================================================================*/


gui_grid_t *gui_grid_create(void *parent,
                            int16_t x,
                            int16_t y,
                            int16_t col,
                            int16_t row,
                            uint16_t gap_col,
                            uint16_t gap_row)
{
#define _GUI_NEW_gui_grid_createparam this, parent, x, y, col,row,gap_col,gap_row
    GUI_NEW(gui_grid_t, gui_grid_ctor, _GUI_NEW_gui_grid_createparam)
}
void gui_grid_style(gui_grid_t *grid, enum gui_grid_style style)
{
    grid->style = style;
}
/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */





