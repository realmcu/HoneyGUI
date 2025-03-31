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
 * @brief create a grid widget.
 * @param parent the father widget the grid nested in.
 * @param x the X-axis coordinate.
 * @param y the Y-axis coordinate.
 * @param row Rows number.
 * @param col Columns number.
 * @param gap_col gap between two Columns.
 * @param gap_row gap between two Rows.
 * @return return the widget object pointer.
 * <b>Example usage</b>
 * \code{.c}
 * {
 *   void *array[] = {
 *       DOG40_BIN,
 *       DOG60_BIN,
 *       DOG80_BIN,
 *       DOG100_BIN,
 *       DOG120_BIN,
 *       DOG140_BIN,
 *       DOG160_BIN,
 *       DOG180_BIN,
 *       DOG200_BIN,
 *       DOG220_BIN,
 *       DOG240_BIN,};
 *   gui_grid_t *grid = gui_grid_create(&(app->screen), 100, 200, 7
 *                                      , 2, 30, 30);
 *   for (size_t i = 0; i < 6; i++)
 *   {
 *       gui_img_creat_from_mem(grid, "grid_image", array[i], 0, 0, 0, 0);
 *   }
 * }
 * \endcode
 */
gui_grid_t *gui_grid_create(void     *parent,
                            int16_t   x,
                            int16_t   y,
                            int16_t   row,
                            int16_t   col,
                            uint16_t  gap_col,
                            uint16_t  gap_row);

/**
 * @brief config grid style.
 *
 * @param grid grid wodget pointer.
 * @param style GRID_CLASSIC, GRID_SCALE.
 * <b>Example usage</b>
 * \code{.c}
 * {
 *   void *array[] = {
 *       DOG40_BIN,
 *       DOG60_BIN,
 *       DOG80_BIN,
 *       DOG100_BIN,
 *       DOG120_BIN,
 *       DOG140_BIN,
 *       DOG160_BIN,
 *       DOG180_BIN,
 *       DOG200_BIN,
 *       DOG220_BIN,
 *       DOG240_BIN,};
 *   gui_grid_t *grid = gui_grid_create(&(app->screen), 100, 200, 7
 *                                      , 2, 30, 30);
 *   gui_grid_style(grid, GRID_SCALE);
 *   for (size_t i = 0; i < 6; i++)
 *   {
 *       gui_img_creat_from_mem(grid, "grid_image", array[i], 0, 0, 0, 0);
 *   }
 * }
 * \endcode
 */
void gui_grid_style(gui_grid_t *grid, T_GUI_GRID_STYLE style);


#ifdef __cplusplus
}
#endif

#endif
