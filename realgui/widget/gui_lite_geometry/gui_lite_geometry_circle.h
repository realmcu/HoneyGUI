/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_lite_geometry_circle.h
  * @brief Lite circle widget encapsulation.
  * @details Lite circle widget encapsulation.
  * @author wenjing_jiang@realsil.com.cn
  * @date 2025/11/15
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
#ifndef __GUI_LITE_GEOMETRY_CIRCLE_H__
#define __GUI_LITE_GEOMETRY_CIRCLE_H__
#ifdef __cplusplus
extern "C" {
#endif
/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_obj.h"
#include "draw_img.h"
#include "lite_geometry.h"

/*============================================================================*
 *                         Types
 *============================================================================*/
/** Lite circle widget structure. */
typedef struct
{
    gui_obj_t base;             /**< Base widget. */

    // Drawing resources
    draw_img_t *draw_img;       /**< Drawing image object. */
    uint8_t *block_buffer;      /**< Block buffer for tiled rendering. */
    uint32_t block_buffer_size; /**< Block buffer size. */
    bool use_block_drawing;     /**< Use block drawing mode. */
    uint16_t block_cols;        /**< Number of block columns. */
    uint16_t block_rows;        /**< Number of block rows. */

    // Drawing context
    DrawContext draw_ctx;       /**< Drawing context. */
    uint8_t opacity_value;      /**< Opacity value. */

    // Circle geometry data
    int x;                      /**< Center X coordinate relative to widget. */
    int y;                      /**< Center Y coordinate relative to widget. */
    int radius;                 /**< Circle radius. */
    uint32_t color;             /**< Circle color. */
} gui_lite_circle_t;

/*============================================================================*
 *                         Macros
 *============================================================================*/
#define BLOCK_SIZE 100           /**< Tile size for block-based rendering. */


/*============================================================================*
 *                         Functions
 *============================================================================*/
/**
 * @brief Create a new lite circle widget.
 * @param parent Parent widget or NULL for root widget.
 * @param name Widget name or NULL for default name.
 * @return Pointer to the created lite circle widget.
 */
gui_lite_circle_t *gui_lite_circle_create(void *parent, const char *name);

/**
 * @brief Set the style of the lite circle widget.
 * @param this Pointer to the lite circle widget.
 * @param x Center X coordinate relative to widget.
 * @param y Center Y coordinate relative to widget.
 * @param radius Circle radius.
 * @param color Circle color.
 */
void gui_lite_circle_set_style(gui_lite_circle_t *this,
                               int x, int y,
                               int radius, uint32_t color);

/**
 * @brief Set the position of the lite circle widget.
 * @param this Pointer to the lite circle widget.
 * @param x Center X coordinate relative to widget.
 * @param y Center Y coordinate relative to widget.
 */
void gui_lite_circle_set_position(gui_lite_circle_t *this, int x, int y);

/**
 * @brief Set the radius of the lite circle widget.
 * @param this Pointer to the lite circle widget.
 * @param radius Circle radius.
 */
void gui_lite_circle_set_radius(gui_lite_circle_t *this, int radius);

/**
 * @brief Set the color of the lite circle widget.
 * @param this Pointer to the lite circle widget.
 * @param color Circle color.
 */
void gui_lite_circle_set_color(gui_lite_circle_t *this, uint32_t color);

/**
 * @brief Register a click event callback for the lite circle widget.
 * @param this Pointer to the lite circle widget.
 * @param callback Callback function pointer.
 * @param parameter Optional parameter to pass to the callback.
 */
void gui_lite_circle_on_click(gui_lite_circle_t *this, void *callback, void *parameter);

#ifdef __cplusplus
}
#endif

#endif /* __GUI_LITE_GEOMETRY_CIRCLE_H__ */
