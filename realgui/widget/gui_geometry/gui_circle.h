/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
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
#include "gui_geometry_span.h"

/*============================================================================*
 *                         Types
 *============================================================================*/
#ifndef M_SQRT1_2
#define M_SQRT1_2 0.707106781186547524401f /**< 1/sqrt(2) */
#endif

/** Circle gradient type */
typedef enum
{
    CIRCLE_GRADIENT_RADIAL = 0,   /**< From center to edge */
    CIRCLE_GRADIENT_ANGULAR,      /**< Along the circumference */
} gui_circle_gradient_type_t;

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
    draw_img_t *stroke_img;    /**< Optional inset stroke image. */
    gui_geometry_span_t *stroke_span; /**< Cached path scanline spans. */

    // Circle geometry data
    int x;                      /**< Center X coordinate relative to widget. */
    int y;                      /**< Center Y coordinate relative to widget. */
    int radius;                 /**< Circle radius. */
    gui_color_t color;          /**< Circle color. */
    float stroke_width;         /**< Inset stroke width. Zero disables stroke. */
    gui_color_t stroke_color;   /**< Stroke color. */
    uint8_t opacity_value;      /**< Opacity value. */
    uint32_t checksum;          /**< Checksum for change detection. */
    uint32_t stroke_path_checksum; /**< Geometry-only span checksum. */

    // Transformation parameters
    float degrees;              /**< Rotation angle in degrees. */
    float scale_x;              /**< Scale factor in X direction. */
    float scale_y;              /**< Scale factor in Y direction. */
    float offset_x;             /**< Translation offset in X direction. */
    float offset_y;             /**< Translation offset in Y direction. */

    // Gradient parameters
    Gradient *gradient;         /**< Optional gradient for circle fill. */
    bool use_gradient;          /**< Flag to enable gradient rendering. */
    gui_circle_gradient_type_t gradient_type; /**< Gradient type. */
    gui_matrix_t last_matrix;
} gui_circle_t;

/** Which rasterised part a cached payload holds. */
typedef enum
{
    CIRCLE_PART_FULL = 1,       /**< Whole circle in one ARGB8888 buffer. */
    CIRCLE_PART_SOLID_RECT,     /**< Solid inscribed square, used when transformed. */
    CIRCLE_PART_ARC_STRIP,      /**< Left arc strip; the other three mirror it. */
    CIRCLE_PART_STROKE,         /**< Whole inset stroke in one buffer. */
} gui_circle_part_t;

/**
 * Identity of a cached circle payload.
 *
 * Everything that changes a pixel goes in here and nothing else does; notably
 * position stays out, since it only ever reaches the draw_img matrix.  Gradient
 * parts pass the whole struct, solid ones stop short of the gradient -- see
 * circle_desc_len().
 */
typedef struct
{
    uint32_t part;              /**< gui_circle_part_t. */
    int32_t size_a;             /**< Radius, or width for CIRCLE_PART_SOLID_RECT. */
    int32_t size_b;             /**< Height for CIRCLE_PART_SOLID_RECT, else 0. */
    uint32_t color;             /**< ARGB baked into the pixels; 0 when A8. */
    uint32_t is_a8;             /**< Non-zero when the payload is a coverage mask. */
    uint32_t gradient_type;     /**< CIRCLE_GRADIENT_*, or UINT32_MAX for none. */
    float stroke_width;         /**< Inset width for full fill or stroke payload. */
    Gradient gradient;          /**< Only present when gradient_type is set. */
} circle_desc_t;
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
 * @param circle Circle widget pointer.
 * @param x X coordinate of circle center.
 * @param y Y coordinate of circle center.
 * @param radius Circle radius.
 * @param color Circle color.
 */
void gui_circle_set_style(gui_circle_t *circle,
                          int x, int y,
                          int radius, gui_color_t color);

/**
 * @brief Set circle position.
 *
 * @param circle Circle widget pointer.
 * @param x X coordinate of circle center.
 * @param y Y coordinate of circle center.
 */
void gui_circle_set_position(gui_circle_t *circle, int x, int y);

/**
 * @brief Set circle radius.
 *
 * @param circle Circle widget pointer.
 * @param radius Circle radius.
 */
void gui_circle_set_radius(gui_circle_t *circle, int radius);
/**
 * @brief Set the opacity of the circle widget.
 * @param circle Pointer to the circle widget.
 * @param opacity Opacity value (0-255).
 */
void gui_circle_set_opacity(gui_circle_t *circle, uint8_t opacity);
/**
 * @brief Set circle color.
 *
 * @param circle Circle widget pointer.
 * @param color Circle color.
 */
void gui_circle_set_color(gui_circle_t *circle, gui_color_t color);

/**
 * @brief Set an inset stroke for the circle.
 *
 * @param circle Circle widget pointer.
 * @param width Stroke width in pixels. A non-positive value disables stroke.
 * @param color Stroke color.
 */
void gui_circle_set_stroke(gui_circle_t *circle, float width, gui_color_t color);

/**
 * @brief Disable the circle stroke.
 *
 * @param circle Circle widget pointer.
 */
void gui_circle_clear_stroke(gui_circle_t *circle);

/**
 * @brief Set click callback for the circle.
 *
 * @param circle Circle widget pointer.
 * @param callback Callback function.
 * @param parameter Callback parameter.
 */
void gui_circle_on_click(gui_circle_t *circle, void *callback, void *parameter);

/**
 * @brief Apply rotation transformation to the circle widget.
 *
 * @param circle Circle widget pointer.
 * @param degrees Rotation angle in degrees (clockwise).
 */
void gui_circle_rotate(gui_circle_t *circle, float degrees);

/**
 * @brief Apply scale transformation to the circle widget.
 *
 * @param circle Circle widget pointer.
 * @param scale_x Scale factor in X direction.
 * @param scale_y Scale factor in Y direction.
 */
void gui_circle_scale(gui_circle_t *circle, float scale_x, float scale_y);

/**
 * @brief Apply translation transformation to the circle widget.
 *
 * @param circle Circle widget pointer.
 * @param tx Translation in X direction (pixels).
 * @param ty Translation in Y direction (pixels).
 */
void gui_circle_translate(gui_circle_t *circle, float tx, float ty);

/**
 * @brief Set radial gradient for circle widget (from center to edge).
 *
 * @param circle Circle widget pointer.
 */
void gui_circle_set_radial_gradient(gui_circle_t *circle);

/**
 * @brief Set angular gradient for circle widget (along circumference).
 *
 * @param circle Circle widget pointer.
 * @param start_angle Start angle in degrees.
 * @param end_angle End angle in degrees.
 */
void gui_circle_set_angular_gradient(gui_circle_t *circle, float start_angle, float end_angle);

/**
 * @brief Add color stop to circle gradient.
 *
 * @param circle Circle widget pointer.
 * @param position Position of color stop (0.0 to 1.0).
 * @param color Color at circle stop.
 */
void gui_circle_add_gradient_stop(gui_circle_t *circle, float position, gui_color_t color);

/**
 * @brief Clear gradient and use solid color.
 *
 * @param circle Circle widget pointer.
 */
void gui_circle_clear_gradient(gui_circle_t *circle);

#ifdef __cplusplus
}
#endif

#endif /* __GUI_CIRCLE_H__ */
