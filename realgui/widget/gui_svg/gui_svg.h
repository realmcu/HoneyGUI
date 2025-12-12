/**
*****************************************************************************************
*     Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_svg.h
  * @brief SVG widget header file.
  * @details SVG widget is used to display SVG images on the screen.
  * @author
  * @date 2024/12/09
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2023 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

#ifndef __GUI_SVG_H__
#define __GUI_SVG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "guidef.h"
#include "gui_api.h"
#include "gui_obj.h"

typedef struct gui_svg
{
    gui_obj_t base;
    void *svg_data;
    float scale;
    uint8_t opacity_value;
    uint8_t from_file : 1;
} gui_svg_t;

/**
 * @brief Create an SVG widget from memory.
 * @param parent Parent widget.
 * @param name Widget name.
 * @param addr SVG data address.
 * @param size SVG data size.
 * @param x X coordinate.
 * @param y Y coordinate.
 * @return Pointer to the created SVG widget.
 */
gui_svg_t *gui_svg_create_from_mem(void       *parent,
                                   const char *name,
                                   void       *addr,
                                   uint32_t    size,
                                   int16_t     x,
                                   int16_t     y);

/**
 * @brief Create an SVG widget from file.
 * @param parent Parent widget.
 * @param name Widget name.
 * @param filename SVG file path.
 * @param x X coordinate.
 * @param y Y coordinate.
 * @return Pointer to the created SVG widget.
 */
gui_svg_t *gui_svg_create_from_file(void       *parent,
                                    const char *name,
                                    const char *filename,
                                    int16_t     x,
                                    int16_t     y);

/**
 * @brief Set the scale of the SVG widget.
 * @param _this SVG widget pointer.
 * @param scale Scale factor.
 */
void gui_svg_set_scale(gui_svg_t *_this, float scale);

/**
 * @brief Set the opacity of the SVG widget.
 * @param _this SVG widget pointer.
 * @param opacity Opacity value (0-255).
 */
void gui_svg_set_opacity(gui_svg_t *_this, uint8_t opacity);

/**
 * @brief Get the width of the SVG image.
 * @param _this SVG widget pointer.
 * @return Width of the SVG image.
 */
uint16_t gui_svg_get_width(gui_svg_t *_this);

/**
 * @brief Get the height of the SVG image.
 * @param _this SVG widget pointer.
 * @return Height of the SVG image.
 */
uint16_t gui_svg_get_height(gui_svg_t *_this);

#ifdef __cplusplus
}
#endif

#endif
