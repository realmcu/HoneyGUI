/**
\internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_grid.h
  * @brief grid design
  * @details Evenly layout vertically and horizontally
  * @author triton_yu@realsil.com.cn
  * @date 2023/10/19
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
\endinternal
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_GRID_H__
#define __GUI_GRID_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_fb.h"
#include "gui_obj.h"


/*============================================================================*
 *                         Types
 *============================================================================*/

/* T_GUI_GRID_STYLE enum start*/
/** @brief  support two style */
typedef enum
{
    GRID_CLASSIC,         //!< Evenly layout vertically and horizontally
    GRID_SCALE,           //!< scale by slide
} T_GUI_GRID_STYLE;
/* T_GUI_GRID_STYLE enum end*/


/*============================================================================*
 *                         Constants
 *============================================================================*/


/*============================================================================*
 *                         Macros
 *============================================================================*/


/*============================================================================*
 *                         Variables
 *============================================================================*/

typedef struct gui_grid
{
    gui_obj_t base;
    int16_t row;
    int16_t col;
    uint16_t gap_col;
    uint16_t gap_row;
    uint16_t gap_col_scale;
    uint16_t gap_row_scale;
    float scale;
    int yold;
    int start_x;
    int start_y;
    int id_offset;
    int row_count;
    bool row_count_flag;
    bool col_count_flag;
    T_GUI_GRID_STYLE style;
    int col_count;
    int change_threshold;
} gui_grid_t;


/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Create a grid widget.
 * @param parent The father widget the grid is nested in.
 * @param x The X-axis coordinate.
 * @param y The Y-axis coordinate.
 * @param row Number of rows.
 * @param col Number of columns.
 * @param gap_col Gap between two columns.
 * @param gap_row Gap between two rows.
 * @return Return the grid widget pointer.
 */
gui_grid_t *gui_grid_create(void     *parent,
                            int16_t   x,
                            int16_t   y,
                            int16_t   row,
                            int16_t   col,
                            uint16_t  gap_col,
                            uint16_t  gap_row);

/**
 * @brief Configure the grid style.
 * @param grid The grid widget pointer.
 * @param style The grid style (GRID_CLASSIC or GRID_SCALE).
 */
void gui_grid_style(gui_grid_t *grid, T_GUI_GRID_STYLE style);


#ifdef __cplusplus
}
#endif

#endif
