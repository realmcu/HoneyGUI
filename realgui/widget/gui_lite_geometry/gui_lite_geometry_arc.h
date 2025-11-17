#ifndef __GUI_LITE_GEOMETRY_ARC_H__
#define __GUI_LITE_GEOMETRY_ARC_H__

#include "guidef.h"
#include "gui_obj.h"
#include "draw_img.h"
#include "lite_geometry.h"

#define BLOCK_SIZE 100           /**< Tile size for block-based rendering */

/** Lite arc widget structure */
typedef struct
{
    gui_obj_t base;             /**< Base widget */

    // Drawing resources
    draw_img_t *draw_img;       /**< Drawing image object */
    uint8_t *block_buffer;      /**< Block buffer for tiled rendering */
    uint32_t block_buffer_size; /**< Block buffer size */
    bool use_block_drawing;     /**< Use block drawing mode */
    uint16_t block_cols;        /**< Number of block columns */
    uint16_t block_rows;        /**< Number of block rows */

    // Drawing context
    DrawContext draw_ctx;       /**< Drawing context */
    uint8_t opacity_value;      /**< Opacity value */

    // Arc geometry data
    int x;                      /**< Center X coordinate relative to widget */
    int y;                      /**< Center Y coordinate relative to widget */
    int radius;                 /**< Arc radius */
    float start_angle;          /**< Start angle in degrees */
    float end_angle;            /**< End angle in degrees */
    float line_width;           /**< Line width */
    uint32_t color;             /**< Arc color */
} gui_lite_arc_t;

// Public API
/**
 * @brief Create a new lite arc widget
 * @param parent Parent widget or NULL for top-level widget
 * @param name Name of the widget
 * @return Pointer to the created lite arc widget
 */
gui_lite_arc_t *gui_lite_arc_create(void *parent, const char *name);
/**
 * @brief Set style for lite arc widget
 * @param this Pointer to the arc widget
 * @param x Center X coordinate relative to widget
 * @param y Center Y coordinate relative to widget
 * @param radius Arc radius
 * @param start_angle Start angle in degrees
 * @param end_angle End angle in degrees
 * @param line_width Line width
 * @param color Arc color
 */
void gui_lite_arc_set_style(gui_lite_arc_t *this,
                            int x, int y, int radius,
                            float start_angle, float end_angle,
                            float line_width, uint32_t color);
/**
 * @brief Move arc geometry
 * @param this Pointer to the arc widget
 * @param x New center X coordinate relative to widget
 * @param y New center Y coordinate relative to widget
 */
void gui_lite_arc_set_position(gui_lite_arc_t *this, int x, int y);

/**
 * @brief Register click event callback for lite arc widget
 * @param this Pointer to the arc widget
 * @param callback Callback function pointer
 * @param parameter Optional parameter to pass to callback
 */
/**
 * @brief Set the radius of the lite Arc widget
 * @param this Pointer to the lite Arc widget
 * @param radius Arc radius
 */
void gui_lite_arc_set_radius(gui_lite_arc_t *this, int radius);

/**
 * @brief Set the color of the lite Arc widget
 * @param this Pointer to the lite Arc widget
 * @param color Arc color
 */
void gui_lite_arc_set_color(gui_lite_arc_t *this, uint32_t color);
/**
 * @brief Set the start angle of the lite Arc widget
 * @param this Pointer to the lite Arc widget
 * @param start_angle Start angle in degrees
 */
void gui_lite_arc_set_start_angle(gui_lite_arc_t *this, float start_angle);

/**
 * @brief Set the end angle of the lite Arc widget
 * @param this Pointer to the lite Arc widget
 * @param end_angle End angle in degrees
 */
void gui_lite_arc_set_end_angle(gui_lite_arc_t *this, float end_angle);
/**
 * @brief Set the line width of the lite Arc widget
 * @param this Pointer to the lite Arc widget
 * @param line_width Line width
 */
void gui_lite_arc_set_line_width(gui_lite_arc_t *this, float line_width);
/**
 * @brief Register click event callback for lite arc widget
 * @param this Pointer to the arc widget
 * @param callback Callback function pointer
 * @param parameter Optional parameter to pass to callback
 */
void gui_lite_arc_on_click(gui_lite_arc_t *this, void *callback, void *parameter);

#endif /* __GUI_LITE_GEOMETRY_ARC_H__ */