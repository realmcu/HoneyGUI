/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_IMG_H__
#define __GUI_IMG_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_api.h"
#include "draw_img.h"
#include "gui_obj.h"


/*============================================================================*
 *                         Types
 *============================================================================*/

/** @brief  Image widget structure. */
#ifdef  __CC_ARM
#pragma anon_unions
#endif

typedef struct gui_img
{
    gui_obj_t base;                    /* Base object structure. */
    draw_img_t draw;                   /* Per-frame drawing state, rebuilt in OBJ_PRE_PROCESS. */

    void *src_data;                    /* Image source pointer (type determined by storage_type). */


    uint32_t fg_color_set;  /* A8 image set color */
    uint32_t bg_color_fix;  /* bg color fix for A8 image */

    uint32_t opacity_value    : 8;        /* Opacity value (0-255). */
    uint32_t blend_mode       : 5;        /* Blend mode. */
    uint32_t storage_type     : 3;        /* Storage type: e.g., file system, flash, etc. */
    uint32_t need_clip        : 1;        /* Clipping flag. */
    uint32_t free_on_destroy  : 1;        /* Free src_data on destroy if true. */
    uint32_t draw_ready       : 1;        /* draw was rebuilt this frame. */
    uint8_t checksum;                  /* Checksum for change detection. */
    uint8_t animate_array_length;      /* Animation array length. */
} gui_img_t;


/*============================================================================*
 *                         Constants
 *============================================================================*/


/*============================================================================*
 *                         Macros
 *============================================================================*/


/*============================================================================*
 *                         Variables
 *============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Load the image to read its width.
 *
 * @param _this Image widget pointer.
 * @return Image's width.
 */
uint16_t gui_img_get_width(gui_img_t *_this);

/**
 * @brief Load the image to read its height.
 *
 * @param _this Image widget pointer.
 * @return Image's height.
 */
uint16_t gui_img_get_height(gui_img_t *_this);

/**
 * @brief Refresh the image size from source.
 *
 * @param _this Image widget pointer.
 */
void gui_img_refresh_size(gui_img_t *_this);

/**
 * @brief Refresh the image draw data.
 *
 * @param _this Image widget pointer.
 */
void gui_img_refresh_draw_data(gui_img_t  *_this);
/**
 * @brief Set the image's blend mode.
 *
 * @param _this Image widget pointer.
 * @param mode Enumeration value of the mode (BLEND_MODE_TYPE).
 */
void gui_img_set_mode(gui_img_t *_this, BLEND_MODE_TYPE mode);

/**
 * @brief Deprecated!
 * Use 'void gui_img_set_src(gui_img_t  *_this, const uint8_t *file_pointer, uint32_t storage_type)' instead.
 * Use 'void gui_img_set_pos(gui_img_t  *_this, int16_t x, int16_t y)' instead.
 */
void gui_img_set_attribute(gui_img_t  *_this,
                           const char *name,
                           void       *addr,
                           int16_t     x,
                           int16_t     y);

/**
 * @brief Set image position.
 *
 * @param this Image widget pointer.
 * @param x X-axis coordinate.
 * @param y Y-axis coordinate.
 */
void gui_img_set_pos(gui_img_t  *_this,
                     int16_t     x,
                     int16_t     y);

/**
 * @brief Set the opacity of the image.
 *
 * @param _this Image widget pointer.
 * @param opacity_value Opacity value (0-255, default 255).
 */
void gui_img_set_opacity(gui_img_t *_this, unsigned char opacity_value);

/**
 * @brief Create an image widget from memory address.
 * @note Create an image widget and set attribute.
 * @param parent Father widget it nested in.
 * @param name Widget name.
 * @param addr Bin file address.
 * @param x X-axis coordinate of the widget.
 * @param y Y-axis coordinate of the widget.
 * @param w Width of the widget.
 * @param h Height of the widget.
 * @return Widget object pointer.
 */
gui_img_t *gui_img_create_from_mem(void       *parent,
                                   const char *name,
                                   void       *addr,
                                   int16_t     x,
                                   int16_t     y,
                                   int16_t     w,
                                   int16_t     h);

/**
 * @brief Create an image widget from memory address.
 * @note Create an image widget and set attribute.
 * @param parent Father widget it nested in.
 * @param name Widget name.
 * @param ftl Not xip address, use ftl address.
 * @param x X-axis coordinate of the widget.
 * @param y Y-axis coordinate of the widget.
 * @param w Width of the widget.
 * @param h Height of the widget.
 * @return Widget object pointer.
 */
gui_img_t *gui_img_create_from_ftl(void       *parent,
                                   const char *name,
                                   void       *ftl,
                                   int16_t     x,
                                   int16_t     y,
                                   int16_t     w,
                                   int16_t     h);

/**
 * @brief Create an image widget from filesystem.
 *
 * @param parent Father widget it nested in.
 * @param name Image widget name.
 * @param file Image file path.
 * @param x X-axis coordinate of the widget.
 * @param y Y-axis coordinate of the widget.
 * @param w Width of the widget.
 * @param h Height of the widget.
 * @return Pointer to the created image widget.
 */
gui_img_t *gui_img_create_from_fs(void       *parent,
                                  const char *name,
                                  void       *file,
                                  int16_t     x,
                                  int16_t     y,
                                  int16_t     w,
                                  int16_t     h);

/**
 * @brief Deprecated!
 * Use 'void gui_img_set_src(gui_img_t  *_this, const uint8_t *file_pointer, uint32_t storage_type)' instead.
 */
void gui_img_set_image_data(gui_img_t *_this, const uint8_t *image_data_pointer);

/**
 * @brief Sets the image source for a specified image widget.
 *
 * This function updates the image source and storage type for the widget.
 * For filesystem sources (IMG_SRC_FILESYS), the path string is automatically
 * duplicated. The old source is properly cleaned up if it was previously
 * allocated.
 *
 * @param _this Pointer to the image widget.
 * @param file_pointer Pointer to the image data or file path string.
 * @param storage_type Storage type: IMG_SRC_MEMADDR, IMG_SRC_FTL, or IMG_SRC_FILESYS.
 */
void gui_img_set_src(gui_img_t  *_this, const uint8_t *file_pointer, uint32_t storage_type);

/**
 * @brief Gets the image data from a specified image widget.
 *
 * This function returns the current image data that is set in the specified image widget.
 *
 * @param _this Pointer to the image widget (`gui_img_t`) from which the image data should be retrieved.
 *
 * @return Pointer to the image data currently set in the widget. If no image data is set, the result may be `NULL`.
 */
const uint8_t *gui_img_get_image_data(gui_img_t *_this);

/**
 * @brief Sets the foreground color mixing value for the image widget.
 */

void gui_img_a8_recolor(gui_img_t *_this, uint32_t fg_color_mix);

/**
 * @brief Sets the background color mixing value for the image widget.
 */
void gui_img_a8_fix_bg(gui_img_t *_this, uint32_t bg_color_mix);

/**
 * @brief Sets the alpha mixing value for the A8 image widget.
 */
void gui_img_a8_mix_alpha(gui_img_t *_this, uint8_t alpha_mix);

/**
 * @brief Creates a solid-color RLE image buffer.
 * @param color Color of the solid-color image.
 * @param width Width of the image.
 * @param height Height of the image.
 * @return Pointer to the created image buffer.
 */
uint8_t *gui_rle_solid_color_create(uint16_t width, uint16_t height, gui_color_t color);

/**
 * @brief Sets the clipping state for the image widget.
 * @param _this Pointer to the image widget.
 * @param clip True to enable clipping, false to disable.
 */
void gui_img_clip(gui_img_t *_this, bool clip);

#ifdef __cplusplus
}
#endif

#endif
