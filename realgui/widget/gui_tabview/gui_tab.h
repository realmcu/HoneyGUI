/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_tab.h
  * @brief tab widget header file
  * @details construct and change tab widget
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/25
  * @version v1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
 \endinternal
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_TAB_H__
#define __GUI_TAB_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_tabview.h"
#include "gui_matrix.h"


/*============================================================================*
 *                         Types
 *============================================================================*/

/** @brief  tab widget ID structure */
typedef struct
{
    int32_t x;
    int32_t y;
    int32_t z;
} gui_tab_id_t;

/** @brief  tab widget structure */
typedef struct
{
    gui_obj_t base;             //!< base structure
    gui_tab_id_t id;
    T_SLIDE_STYLE style;
    gui_vertex_t normal;
    gui_obj_t *shot_obj; // shot
    gui_obj_t *shot_pave_obj; // shot for TAB_ROTATE_BOOK style
    gui_obj_t *rte_obj; //run time envriment
} gui_tab_t;

/** @brief  tab widget stacking structure */
typedef struct
{
    float scale;
    float location;
    uint8_t opacity;
} gui_tab_stacking_t;

/*============================================================================*
 *                         Constants
 *============================================================================*/


/*============================================================================*
 *                         Macros
 *============================================================================*/


/*============================================================================*
 *                         Variables
 *============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Creates a tab widget, which should be nested in a tabview.
 *
 * @param parent The parent widget it is nested in.
 * @param filename The tab widget's name.
 * @param x The X-axis coordinate of the widget.
 * @param y The Y-axis coordinate of the widget.
 * @param w The width of the widget.
 * @param h The height of the widget.
 * @param idx The X-axis index in a tabview.
 * @param idy The Y-axis index in a tabview.
 * @return The widget object pointer.
 */
gui_tab_t *gui_tab_create(void       *parent,
                          const char *name,
                          int16_t     x,
                          int16_t     y,
                          int16_t     w,
                          int16_t     h,
                          int16_t     idx,
                          int16_t     idy);

/**
 * @brief Gets the runtime environment object.
 *
 * @param _this Tab widget pointer.
 */
gui_obj_t *gui_tab_get_rte_obj(gui_tab_t *_this);

/**
 * @brief Sets the style of the tab.
 *
 * @param _this Tab widget pointer.
 * @param style Slide style.
 */
void gui_tab_set_style(gui_tab_t *_this, T_SLIDE_STYLE style);

/**
 * @brief Updates the preload buffer when preload is enabled.
 *
 * @param obj Tab widget pointer.
 */
void gui_tab_update_preload(gui_obj_t *obj);

/**
 * @brief Rotates the tab widget.
 *
 * @param obj Tab widget pointer.
 * @param tab_x_gap The relative position difference in the X direction.
 * @param tab_y_gap The relative position difference in the Y direction.
 */
void gui_tab_rotate(gui_obj_t *obj, int16_t tab_x_gap, int16_t tab_y_gap);

/**
 * @brief Reduces the tab widget.
 *
 * @param obj Tab widget pointer.
 * @param tab_x_gap The relative position difference in the X direction.
 * @param tab_y_gap The relative position difference in the Y direction.
 */
void gui_tab_reduction(gui_obj_t *obj, int16_t tab_x_gap, int16_t tab_y_gap);

/**
 * @brief Reduces and fades the tab widget.
 *
 * @param obj Tab widget pointer.
 * @param tab_x_gap The relative position difference in the X direction.
 * @param tab_y_gap The relative position difference in the Y direction.
 */
void gui_tab_reduction_fade(gui_obj_t *obj, int16_t tab_x_gap, int16_t tab_y_gap);

/**
 * @brief Fades the tab widget.
 *
 * @param obj Tab widget pointer.
 * @param tab_x_gap The relative position difference in the X direction.
 * @param tab_y_gap The relative position difference in the Y direction.
 */
void gui_tab_fade(gui_obj_t *obj, int16_t tab_x_gap, int16_t tab_y_gap);

/**
 * @brief Applies a cube effect to the tab widget.
 *
 * @param obj Tab widget pointer.
 * @param tab_x_gap The relative position difference in the X direction.
 * @param tab_y_gap The relative position difference in the Y direction.
 */
void gui_tab_cube(gui_obj_t *obj, int16_t tab_x_gap, int16_t tab_y_gap);

/**
 * @brief Applies a rotate book effect to the tab widget.
 *
 * @param obj Tab widget pointer.
 * @param tab_x_gap The relative position difference in the X direction.
 * @param tab_y_gap The relative position difference in the Y direction.
 */
void gui_tab_rotate_book(gui_obj_t *obj, int16_t tab_x_gap, int16_t tab_y_gap);

#ifdef __cplusplus
}
#endif

#endif
