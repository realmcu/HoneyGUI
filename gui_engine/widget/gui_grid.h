/**
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
#include <guidef.h>
#include <gui_fb.h>
#include "gui_obj.h"

/** @defgroup WIDGET WIDGET
  * @brief
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup WIDGET_Exported_Types WIDGET Exported Types
  * @brief
  * @{
  */

/** @brief  support two style */
enum gui_grid_style
{
    GRID_CLASSIC,         //!< Evenly layout vertically and horizontally
    GRID_SCALE,           //!< scale by slide
};

/** End of WIDGET_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/
/** @defgroup WIDGET_Exported_Constants WIDGET Exported Constants
  * @brief
  * @{
  */


/** End of WIDGET_Exported_Constants
  * @}
  */

/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup WIDGET_Exported_Macros WIDGET Exported Macros
  * @brief
  * @{
  */



/** End of WIDGET_Exported_Macros
  * @}
  */

/*============================================================================*
 *                         Variables
 *============================================================================*/
/** @defgroup WIDGET_Exported_Variables WIDGET Exported Variables
  * @brief
  * @{
  */
typedef struct gui_grid
{
    gui_obj_t base;
    int16_t row; int16_t col; uint16_t gap_col; uint16_t gap_row; uint16_t gap_col_scale;
    uint16_t gap_row_scale;
    float scale;
    int yold;
    int start_x;
    int start_y;
    int id_offset;
    int row_count;
    bool row_count_flag;
    bool col_count_flag;
    enum gui_grid_style style;
    int col_count;
    int change_threshold;
    void (*ctor)(struct gui_grid *this, gui_obj_t *parent, int16_t x,
                 int16_t y, int16_t row, int16_t col, uint16_t gap_col, uint16_t gap_row);
} gui_grid_t;

/** End of WIDGET_Exported_Variables
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup WIDGET_Exported_Functions WIDGET Exported Functions
  * @brief
  * @{
  */

/**
 * @brief create a grid widget.
 * @param parent the father widget the grid nested in.
 * @param x the X-axis coordinate.
 * @param y the Y-axis coordinate.
 * @param row Rows number.
 * @param col Columns number.
 * @param gap_col gap between two Columns.
 * @param gap_row gap between two Rows.
 * @return return the widget object pointer
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
 *       gui_magic_img_create_from_mem(grid, "grid_image", array[i], 0, 0, 0, 0);
 *   }}
 * \endcode
 */
gui_grid_t *gui_grid_create(void *parent,
                            int16_t x,
                            int16_t y,
                            int16_t row,
                            int16_t col,
                            uint16_t gap_col,
                            uint16_t gap_row);
/**
 * @brief config grid style
 *
 * @param grid grid wodget pointer
 * @param style GRID_CLASSIC, GRID_SCALE
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
 *       gui_magic_img_create_from_mem(grid, "grid_image", array[i], 0, 0, 0, 0);
 *   }}
 * \endcode
 */
void gui_grid_style(gui_grid_t *grid, enum gui_grid_style style);


/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
#ifdef __cplusplus
}
#endif



#endif



