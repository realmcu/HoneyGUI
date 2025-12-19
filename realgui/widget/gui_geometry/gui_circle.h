/**
 \internal
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_circle.h
  * @brief Circle widget encapsulation.
  * @details Circle widget encapsulation.
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
#ifndef __GUI_CIRCLE_H__
#define __GUI_CIRCLE_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_obj.h"
#include "draw_img.h"
#include <string.h>
#include "gui_fb.h"
#include "acc_api.h"
#include "tp_algo.h"
#include "lite_geometry.h"

/*============================================================================*
 *                         Types
 *============================================================================*/
/** Circle widget structure. */
typedef struct
{
    gui_obj_t base;             /**< Base widget. */

    // Image parts
    draw_img_t *center_rect;    /**< Center rectangle (inscribed square). */
    draw_img_t *arc_left;    /**< Left arc. */
    draw_img_t *arc_right;    /**< Right arc. */
    draw_img_t *arc_top;    /**< Top arc. */
    draw_img_t *arc_bottom;    /**< Bottom arc. */

    // Circle geometry data
    int x;                      /**< Center X coordinate relative to widget. */
    int y;                      /**< Center Y coordinate relative to widget. */
    int radius;                 /**< Circle radius. */
    gui_color_t color;          /**< Circle color. */
    uint8_t opacity_value;      /**< Opacity value. */
    uint8_t checksum;           /**< Checksum for change detection. */

    // Transformation parameters
    float degrees;              /**< Rotation angle in degrees. */
    float scale_x;              /**< Scale factor in X direction. */
    float scale_y;              /**< Scale factor in Y direction. */
    float offset_x;             /**< Translation offset in X direction. */
    float offset_y;             /**< Translation offset in Y direction. */
} gui_circle_t;

/*============================================================================*
 *                         Macros
 *============================================================================*/
#define BLOCK_SIZE 100           /**< Tile size for block-based rendering. */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/**
 * @brief Create a circle widget.
 *
 * @param parent Parent widget.
 * @param name Widget name.
 * @param x X coordinate of circle center.
 * @param y Y coordinate of circle center.
 * @param radius Circle radius.
 * @param color Circle color.
 * @return Pointer to the created circle widget.
 */
gui_circle_t *gui_circle_create(void *parent, const char *name, int x, int y,
                                int radius, gui_color_t color);

/**
 * @brief Set circle style.
 *
 * @param this Circle widget pointer.
 * @param x X coordinate of circle center.
 * @param y Y coordinate of circle center.
 * @param radius Circle radius.
 * @param color Circle color.
 */
void gui_circle_set_style(gui_circle_t *this,
                          int x, int y,
                          int radius, gui_color_t color);

/**
 * @brief Set circle position.
 *
 * @param this Circle widget pointer.
 * @param x X coordinate of circle center.
 * @param y Y coordinate of circle center.
 */
void gui_circle_set_position(gui_circle_t *this, int x, int y);

/**
 * @brief Set circle radius.
 *
 * @param this Circle widget pointer.
 * @param radius Circle radius.
 */
void gui_circle_set_radius(gui_circle_t *this, int radius);

/**
 * @brief Set circle color.
 *
 * @param this Circle widget pointer.
 * @param color Circle color.
 */
void gui_circle_set_color(gui_circle_t *this, gui_color_t color);

/**
 * @brief Set click callback for the circle.
 *
 * @param this Circle widget pointer.
 * @param callback Callback function.
 * @param parameter Callback parameter.
 */
void gui_circle_on_click(gui_circle_t *this, void *callback, void *parameter);

/**
 * @brief Apply rotation transformation to the circle widget.
 *
 * @param this Circle widget pointer.
 * @param degrees Rotation angle in degrees (clockwise).
 */
void gui_circle_rotate(gui_circle_t *this, float degrees);

/**
 * @brief Apply scale transformation to the circle widget.
 *
 * @param this Circle widget pointer.
 * @param scale_x Scale factor in X direction.
 * @param scale_y Scale factor in Y direction.
 */
void gui_circle_scale(gui_circle_t *this, float scale_x, float scale_y);

/**
 * @brief Apply translation transformation to the circle widget.
 *
 * @param this Circle widget pointer.
 * @param tx Translation in X direction (pixels).
 * @param ty Translation in Y direction (pixels).
 */
void gui_circle_translate(gui_circle_t *this, float tx, float ty);

#ifdef __cplusplus
}
#endif

#endif /* __GUI_CIRCLE_H__ */
