/**
 \internal
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_rect.h
  * @brief Rect widget encapsulation.
  * @details Rect widget encapsulation.
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
#ifndef __GUI_RECT_H__
#define __GUI_RECT_H__
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

/** Rect gradient direction */
typedef enum
{
    RECT_GRADIENT_HORIZONTAL = 0,  /**< Left to right */
    RECT_GRADIENT_VERTICAL,        /**< Top to bottom */
    RECT_GRADIENT_DIAGONAL_TL_BR,  /**< Top-left to bottom-right */
    RECT_GRADIENT_DIAGONAL_TR_BL,  /**< Top-right to bottom-left */
} gui_rect_gradient_dir_t;

/** Rect widget structure. */
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

    int radius;                 /**< Rect radius. */
    gui_color_t color;          /**< Rect color. */
    uint8_t checksum;           /**< Checksum for change detection. */

    // Transformation parameters
    float degrees;              /**< Rotation angle in degrees. */
    float scale_x;              /**< Scale factor in X direction. */
    float scale_y;              /**< Scale factor in Y direction. */
    float offset_x;             /**< Translation offset in X direction. */
    float offset_y;             /**< Translation offset in Y direction. */

    // Gradient parameters
    Gradient *gradient;         /**< Optional gradient for rect fill. */
    bool use_gradient;          /**< Flag to enable gradient rendering. */
    gui_rect_gradient_dir_t gradient_dir; /**< Gradient direction. */
} gui_rounded_rect_t;

/*============================================================================*
 *                         Macros
 *============================================================================*/
#define BLOCK_SIZE 100           /**< Tile size for block-based rendering. */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/**
 * @brief Create a new rect widget.
 * @param parent Parent widget or NULL for root widget.
 * @param name Widget name or NULL for default name.
 * @param x X coordinate relative to widget.
 * @param y Y coordinate relative to widget.
 * @param w Rect width.
 * @param h Rect height.
 * @param radius Rect radius.
 * @param color Rect color.
 * @return Pointer to the created rect widget.
 */
gui_rounded_rect_t *gui_rect_create(void *parent, const char *name, int x, int y,
                                    int w, int h,
                                    int radius, gui_color_t color);

/**
 * @brief Set the style of the rect widget.
 * @param this Pointer to the rect widget.
 * @param x X coordinate relative to widget.
 * @param y Y coordinate relative to widget.
 * @param w Rect width.
 * @param h Rect height.
 * @param radius Rect radius.
 * @param color Rect color.
 */
void gui_rect_set_style(gui_rounded_rect_t *this,
                        int x, int y, int w, int h,
                        int radius, gui_color_t color);
/**
 * @brief Set the opacity of the rect widget.
 * @param this Pointer to the rect widget.
 * @param opacity Opacity value (0-255).
 */
void gui_rect_set_opacity(gui_rounded_rect_t *this, uint8_t opacity);
/**
 * @brief Set the position of the rect widget.
 * @param this Pointer to the rect widget.
 * @param x X coordinate relative to widget.
 * @param y Y coordinate relative to widget.
 */
void gui_rect_set_position(gui_rounded_rect_t *this, int x, int y);

/**
 * @brief Set the size of the rect widget.
 * @param this Pointer to the rect widget.
 * @param w Rect width.
 * @param h Rect height.
 */
void gui_rect_set_size(gui_rounded_rect_t *this, int w, int h);

/**
 * @brief Set the radius of the rect widget.
 * @param this Pointer to the rect widget.
 * @param radius Rect radius.
 */
void gui_rect_set_radius(gui_rounded_rect_t *this, int radius);

/**
 * @brief Set the color of the rect widget.
 * @param this Pointer to the rect widget.
 * @param color Rect color.
 */
void gui_rect_set_color(gui_rounded_rect_t *this, gui_color_t color);

/**
 * @brief Register a click event callback for the rect widget.
 * @param this Pointer to the rect widget.
 * @param callback Callback function pointer.
 * @param parameter Optional parameter to pass to the callback.
 */
void gui_rect_on_click(gui_rounded_rect_t *this, void *callback, void *parameter);

/**
 * @brief Apply rotation transformation to the rect widget.
 * @param this Pointer to the rect widget.
 * @param degrees Rotation angle in degrees (clockwise).
 */
void gui_rect_rotate(gui_rounded_rect_t *this, float degrees);

/**
 * @brief Apply scale transformation to the rect widget.
 * @param this Pointer to the rect widget.
 * @param scale_x Scale factor in X direction.
 * @param scale_y Scale factor in Y direction.
 */
void gui_rect_scale(gui_rounded_rect_t *this, float scale_x, float scale_y);

/**
 * @brief Apply translation transformation to the rect widget.
 * @param this Pointer to the rect widget.
 * @param tx Translation in X direction (pixels).
 * @param ty Translation in Y direction (pixels).
 */
void gui_rect_translate(gui_rounded_rect_t *this, float tx, float ty);

/**
 * @brief Set linear gradient for rect widget.
 * @param this Pointer to the rect widget.
 * @param direction Gradient direction (horizontal, vertical, diagonal).
 */
void gui_rect_set_linear_gradient(gui_rounded_rect_t *this, gui_rect_gradient_dir_t direction);

/**
 * @brief Add color stop to rect gradient.
 * @param this Pointer to the rect widget.
 * @param position Position of color stop (0.0 to 1.0).
 * @param color Color at this stop.
 */
void gui_rect_add_gradient_stop(gui_rounded_rect_t *this, float position, gui_color_t color);

/**
 * @brief Clear gradient and use solid color.
 * @param this Pointer to the rect widget.
 */
void gui_rect_clear_gradient(gui_rounded_rect_t *this);

#ifdef __cplusplus
}
#endif

#endif /* __GUI_RECT_H__ */
