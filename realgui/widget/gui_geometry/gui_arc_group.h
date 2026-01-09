/**
 \internal
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_arc_group.h
  * @brief Arc group widget for batch rendering multiple background arcs.
  * @details Optimizes rendering by combining multiple static arcs into one buffer.
  * @author
  * @date 2025/01/09
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
 \endinternal
  */

#ifndef __GUI_ARC_GROUP_H__
#define __GUI_ARC_GROUP_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "guidef.h"
#include "gui_obj.h"
#include "draw_img.h"
#include "lite_geometry.h"

#define MAX_ARCS_IN_GROUP 8

/** Arc definition for group rendering */
typedef struct
{
    float cx;                   /**< Center X relative to group. */
    float cy;                   /**< Center Y relative to group. */
    float radius;               /**< Arc radius. */
    float start_angle;          /**< Start angle in degrees. */
    float end_angle;            /**< End angle in degrees. */
    float line_width;           /**< Line width. */
    Gradient *gradient;         /**< Optional gradient. */
    bool use_gradient;          /**< Use gradient flag. */
    gui_color_t color;          /**< Solid color (if no gradient). */
} arc_def_t;

/** Arc group widget structure */
typedef struct
{
    gui_obj_t base;             /**< Base widget. */

    draw_img_t *draw_img;       /**< Drawing image object. */
    uint8_t *pixel_buffer;      /**< Cached pixel buffer. */
    uint32_t buffer_size;       /**< Buffer size. */
    bool buffer_valid;          /**< Buffer cache valid flag. */

    DrawContext draw_ctx;       /**< Drawing context. */
    uint8_t opacity_value;      /**< Opacity value. */

    arc_def_t arcs[MAX_ARCS_IN_GROUP];  /**< Arc definitions. */
    int arc_count;              /**< Number of arcs in group. */

    int buffer_w;               /**< Buffer width. */
    int buffer_h;               /**< Buffer height. */
} gui_arc_group_t;

/**
 * @brief Create arc group widget.
 * @param parent Parent widget.
 * @param name Widget name.
 * @param x X position.
 * @param y Y position.
 * @param w Width.
 * @param h Height.
 * @return Pointer to created arc group.
 */
gui_arc_group_t *gui_arc_group_create(void *parent, const char *name,
                                      int x, int y, int w, int h);

/**
 * @brief Add arc to group.
 * @param this Arc group widget.
 * @param cx Center X relative to group.
 * @param cy Center Y relative to group.
 * @param radius Arc radius.
 * @param start_angle Start angle.
 * @param end_angle End angle.
 * @param line_width Line width.
 * @param color Arc color.
 * @return Index of added arc, or -1 if group is full.
 */
int gui_arc_group_add_arc(gui_arc_group_t *this, float cx, float cy,
                          float radius, float start_angle, float end_angle,
                          float line_width, gui_color_t color);

/**
 * @brief Set gradient for arc in group.
 * @param this Arc group widget.
 * @param arc_index Index of arc (returned by add_arc).
 * @param start_angle Gradient start angle.
 * @param end_angle Gradient end angle.
 */
void gui_arc_group_set_gradient(gui_arc_group_t *this, int arc_index,
                                float start_angle, float end_angle);

/**
 * @brief Add gradient stop to arc in group.
 * @param this Arc group widget.
 * @param arc_index Index of arc.
 * @param position Stop position (0.0-1.0).
 * @param color Stop color.
 */
void gui_arc_group_add_gradient_stop(gui_arc_group_t *this, int arc_index,
                                     float position, gui_color_t color);

#ifdef __cplusplus
}
#endif

#endif /* __GUI_ARC_GROUP_H__ */
