#ifndef __GUI_LITE_GEOMETRY_ROUND_RECT_H__
#define __GUI_LITE_GEOMETRY_ROUND_RECT_H__

#include "guidef.h"
#include "gui_obj.h"
#include "draw_img.h"
// #include "lite_geometry.h"

#define BLOCK_SIZE 100           /**< Tile size for block-based rendering */

/** Lite arc widget structure */
typedef struct
{
    gui_obj_t base;             /**< Base widget */
    draw_img_t *circle_00;
    draw_img_t *circle_01;
    draw_img_t *circle_10;
    draw_img_t *circle_11;
    uint8_t *circle_data;
    draw_img_t *rect_0;
    draw_img_t *rect_1;
    draw_img_t *rect_2;
    uint8_t *rect_data;
    uint8_t opacity_value;      /**< Opacity value */

    // Round rect geometry data
    int x;                      /**< Center X coordinate relative to widget */
    int y;                      /**< Center Y coordinate relative to widget */
    int w;                      /**< Round rect width */
    int h;                      /**< Round rect height */
    int radius;                 /**< Round rect radius */
    gui_color_t color;             /**< Round rect color */
} gui_lite_round_rect_t;
/**
 * @brief Create a new lite arc widget
 * @param parent Parent widget or NULL for root widget
 * @param name Widget name or NULL for default name
 * @return Pointer to the created lite arc widget
 */
gui_lite_round_rect_t *gui_lite_round_rect_create(void *parent, const char *name);

/**
 * @brief Set the style of the lite arc widget
 * @param this Pointer to the lite arc widget
 * @param x Center X coordinate relative to widget
 * @param y Center Y coordinate relative to widget
 * @param w Round rect width
 * @param h Round rect height
 * @param radius Arc radius
 * @param color Arc color
 */
void gui_lite_round_rect_set_style(gui_lite_round_rect_t *this,
                                   int x, int y, int w, int h,
                                   int radius, gui_color_t color);

/**
 * @brief Set the position of the lite round rect widget
 * @param this Pointer to the lite round rect widget
 * @param x Center X coordinate relative to widget
 * @param y Center Y coordinate relative to widget
 */
void gui_lite_round_rect_set_position(gui_lite_round_rect_t *this, int x, int y);
/**
 * @brief Set the size of the lite round rect widget
 * @param this Pointer to the lite round rect widget
 * @param w Round rect width
 * @param h Round rect height
 */
void gui_lite_round_rect_set_size(gui_lite_round_rect_t *this, int w, int h);
/**
 * @brief Set the radius of the lite round rect widget
 * @param this Pointer to the lite round rect widget
 * @param radius Round rect radius
 */
void gui_lite_round_rect_set_radius(gui_lite_round_rect_t *this, int radius);
/**
 * @brief Set the color of the lite round rect widget
 * @param this Pointer to the lite round rect widget
 * @param color Round rect color
 */
void gui_lite_round_rect_set_color(gui_lite_round_rect_t *this, gui_color_t color);
/**
 * @brief Register a click event callback for the lite round rect widget
 * @param this Pointer to the lite round rect widget
 * @param callback Callback function pointer
 * @param parameter Optional parameter to pass to the callback
 */
void gui_lite_round_rect_on_click(gui_lite_round_rect_t *this, void *callback, void *parameter);
#endif /* __GUI_LITE_GEOMETRY_ROUND_RECT_H__ */
