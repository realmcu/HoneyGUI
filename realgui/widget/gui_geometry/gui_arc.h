/**
 \internal
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_arc.h
  * @brief Arc widget encapsulation.
  * @details Arc widget encapsulation.
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
#ifndef __GUI_ARC_H__
#define __GUI_ARC_H__
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
/** Arc widget structure. */
typedef struct
{
    gui_obj_t base;             /**< Base widget. */

    // Drawing resources
    draw_img_t *draw_img;       /**< Drawing image object. */
    uint8_t *pixel_buffer;      /**< Cached pixel buffer. */
    uint32_t buffer_size;       /**< Buffer size. */
    bool buffer_valid;          /**< Buffer cache valid flag. */

    // Drawing context
    DrawContext draw_ctx;       /**< Drawing context. */
    uint8_t opacity_value;      /**< Opacity value. */

    // Arc geometry data
    int x;                      /**< Center X coordinate relative to widget. */
    int y;                      /**< Center Y coordinate relative to widget. */
    int radius;                 /**< Arc radius. */
    float start_angle;          /**< Start angle in degrees. */
    float end_angle;            /**< End angle in degrees. */
    float line_width;           /**< Line width. */
    gui_color_t color;             /**< Arc color (stored as uint32_t internally). */

    // Cache for dirty checking
    int cached_x;
    int cached_y;
    int cached_radius;
    float cached_start_angle;
    float cached_end_angle;
    float cached_line_width;
    gui_color_t cached_color;

    // Transformation parameters
    float degrees;              /**< Rotation angle in degrees. */
    float scale_x;              /**< Scale factor in X direction. */
    float scale_y;              /**< Scale factor in Y direction. */
    float offset_x;             /**< Translation offset in X direction. */
    float offset_y;             /**< Translation offset in Y direction. */
} gui_arc_t;

/*============================================================================*
 *                         Macros
 *============================================================================*/
#define BLOCK_SIZE 100           /**< Tile size for block-based rendering. */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/**
 * @brief Create a new arc widget.
 * @param parent Parent widget or NULL for top-level widget.
 * @param name Name of the widget.
 * @param x Center X coordinate.
 * @param y Center Y coordinate.
 * @param radius Arc radius.
 * @param start_angle Start angle in degrees.
 * @param end_angle End angle in degrees.
 * @param line_width Line width.
 * @param color Arc color.
 * @return Pointer to the created arc widget.
 */
gui_arc_t *gui_arc_create(void *parent, const char *name,
                          int x, int y, int radius,
                          float start_angle, float end_angle,
                          float line_width, gui_color_t color);

/**
 * @brief Move arc geometry.
 * @param this Pointer to the arc widget.
 * @param x New center X coordinate relative to widget.
 * @param y New center Y coordinate relative to widget.
 */
void gui_arc_set_position(gui_arc_t *this, int x, int y);

/**
 * @brief Set the radius of the arc widget.
 * @param this Pointer to the arc widget.
 * @param radius Arc radius.
 */
void gui_arc_set_radius(gui_arc_t *this, int radius);
/**
 * @brief Set the opacity of the arc widget.
 * @param this Pointer to the arc widget.
 * @param opacity Opacity value (0-255).
 */
void gui_arc_set_opacity(gui_arc_t *this, uint8_t opacity);
/**
 * @brief Set the color of the arc widget.
 * @param this Pointer to the arc widget.
 * @param color Arc color.
 */
void gui_arc_set_color(gui_arc_t *this, gui_color_t color);
/**
 * @brief Set the start angle of the arc widget.
 * @param this Pointer to the arc widget.
 * @param start_angle Start angle in degrees.
 */
void gui_arc_set_start_angle(gui_arc_t *this, float start_angle);

/**
 * @brief Set the end angle of the arc widget.
 * @param this Pointer to the arc widget.
 * @param end_angle End angle in degrees.
 */
void gui_arc_set_end_angle(gui_arc_t *this, float end_angle);

/**
 * @brief Set the line width of the arc widget.
 * @param this Pointer to the arc widget.
 * @param line_width Line width.
 */
void gui_arc_set_line_width(gui_arc_t *this, float line_width);

/**
 * @brief Register click event callback for arc widget.
 * @param this Pointer to the arc widget.
 * @param callback Callback function pointer.
 * @param parameter Optional parameter to pass to callback.
 */
void gui_arc_on_click(gui_arc_t *this, void *callback, void *parameter);

/**
 * @brief Apply rotation transformation to the arc widget.
 * @param this Pointer to the arc widget.
 * @param degrees Rotation angle in degrees (clockwise).
 */
void gui_arc_rotate(gui_arc_t *this, float degrees);

/**
 * @brief Apply scale transformation to the arc widget.
 * @param this Pointer to the arc widget.
 * @param scale_x Scale factor in X direction.
 * @param scale_y Scale factor in Y direction.
 */
void gui_arc_scale(gui_arc_t *this, float scale_x, float scale_y);

/**
 * @brief Apply translation transformation to the arc widget.
 * @param this Pointer to the arc widget.
 * @param tx Translation in X direction (pixels).
 * @param ty Translation in Y direction (pixels).
 */
void gui_arc_translate(gui_arc_t *this, float tx, float ty);

#ifdef __cplusplus
}
#endif

#endif /* __GUI_ARC_H__ */
