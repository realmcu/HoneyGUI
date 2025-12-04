/**
 \internal
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_lite_geometry_round_rect.h
  * @brief Lite round rect widget encapsulation.
  * @details Lite round rect widget encapsulation.
  * @author wenjing_jiang@realsil.com.cn
  * @date 2025/11/15
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
 \endinternal
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_LITE_GEOMETRY_ROUND_RECT_H__
#define __GUI_LITE_GEOMETRY_ROUND_RECT_H__
#ifdef __cplusplus
extern "C" {
#endif
/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_obj.h"
#include "draw_img.h"
// #include "lite_geometry.h"

/*============================================================================*
 *                         Types
 *============================================================================*/
/** Lite arc widget structure. */
typedef struct
{
    gui_obj_t base;             /**< Base widget. */
    draw_img_t *circle_00;
    draw_img_t *circle_01;
    draw_img_t *circle_10;
    draw_img_t *circle_11;
    uint8_t *circle_data;
    draw_img_t *rect_0;
    draw_img_t *rect_1;
    draw_img_t *rect_2;
    uint8_t *rect_data;
    uint8_t opacity_value;      /**< Opacity value. */

    int radius;                 /**< Round rect radius. */
    gui_color_t color;          /**< Round rect color. */
    uint8_t checksum;           /**< Checksum for change detection. */
} gui_lite_round_rect_t;

/*============================================================================*
 *                         Macros
 *============================================================================*/
#define BLOCK_SIZE 100           /**< Tile size for block-based rendering. */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/**
 * @brief Create a new lite round rect widget.
 * @param parent Parent widget or NULL for root widget.
 * @param name Widget name or NULL for default name.
 * @param x Center X coordinate relative to widget.
 * @param y Center Y coordinate relative to widget.
 * @param w Round rect width.
 * @param h Round rect height.
 * @param radius Round rect radius.
 * @param color Round rect color.
 * @return Pointer to the created lite round rect widget.
 */
gui_lite_round_rect_t *gui_lite_round_rect_create(void *parent, const char *name, int x, int y,
                                                  int w, int h,
                                                  int radius, gui_color_t color);

/**
 * @brief Set the style of the lite round rect widget.
 * @param this Pointer to the lite round rect widget.
 * @param x Center X coordinate relative to widget.
 * @param y Center Y coordinate relative to widget.
 * @param w Round rect width.
 * @param h Round rect height.
 * @param radius Round rect radius.
 * @param color Round rect color.
 */
void gui_lite_round_rect_set_style(gui_lite_round_rect_t *this,
                                   int x, int y, int w, int h,
                                   int radius, gui_color_t color);

/**
 * @brief Set the position of the lite round rect widget.
 * @param this Pointer to the lite round rect widget.
 * @param x Center X coordinate relative to widget.
 * @param y Center Y coordinate relative to widget.
 */
void gui_lite_round_rect_set_position(gui_lite_round_rect_t *this, int x, int y);

/**
 * @brief Set the size of the lite round rect widget.
 * @param this Pointer to the lite round rect widget.
 * @param w Round rect width.
 * @param h Round rect height.
 */
void gui_lite_round_rect_set_size(gui_lite_round_rect_t *this, int w, int h);

/**
 * @brief Set the radius of the lite round rect widget.
 * @param this Pointer to the lite round rect widget.
 * @param radius Round rect radius.
 */
void gui_lite_round_rect_set_radius(gui_lite_round_rect_t *this, int radius);

/**
 * @brief Set the color of the lite round rect widget.
 * @param this Pointer to the lite round rect widget.
 * @param color Round rect color.
 */
void gui_lite_round_rect_set_color(gui_lite_round_rect_t *this, gui_color_t color);

/**
 * @brief Register a click event callback for the lite round rect widget.
 * @param this Pointer to the lite round rect widget.
 * @param callback Callback function pointer.
 * @param parameter Optional parameter to pass to the callback.
 */
void gui_lite_round_rect_on_click(gui_lite_round_rect_t *this, void *callback, void *parameter);

#ifdef __cplusplus
}
#endif

#endif /* __GUI_LITE_GEOMETRY_ROUND_RECT_H__ */
