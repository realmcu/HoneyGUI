/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __GUI_GIF_H__
#define __GUI_GIF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "guidef.h"
#include "gui_api.h"
#include "gui_obj.h"
#include "draw_img.h"
#include "gifdec.h"
#include "gui_api.h"
#include "gui_obj.h"
#include "gifdec.h"

/** @brief  GIF widget structure. */
#ifdef  __CC_ARM
#pragma anon_unions
#endif

typedef struct gui_gif
{
    gui_obj_t base;                    /* Base object structure. */
    draw_img_t *draw_img;              /* Drawing image structure. */
    float degrees;                     /* Rotation angle in degrees. */
    float scale_x;                     /* Scale factor in X direction. */
    float scale_y;                     /* Scale factor in Y direction. */
    float f_x;                         /* Focus of image X coordinate. */
    float f_y;                         /* Focus of image Y coordinate. */

    float t_x;                         /* Translation in X direction. */
    float t_y;                         /* Translation in Y direction. */

    void *src_data;                    /* Image source pointer (type determined by storage_type). */

    gd_GIF *gif;                        /* GIF decoder data */

    uint32_t opacity_value    : 8;        /* Opacity value (0-255). */
    uint32_t blend_mode       : 5;        /* Blend mode. */
    uint32_t storage_type     : 3;        /* Storage type: e.g., file system, flash, etc. */
    uint32_t high_quality     : 1;        /* High quality rendering flag. */
    uint32_t need_clip        : 1;        /* Clipping flag. */
    uint32_t free_on_destroy  : 1;        /* Free src_data on destroy if true. */
    uint8_t checksum;                  /* Checksum for change detection. */
} gui_gif_t;


/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Create a GIF widget from memory address.
 * @param parent Father widget it nested in.
 * @param name Widget name.
 * @param addr Bin file address.
 * @param x X-axis coordinate of the widget.
 * @param y Y-axis coordinate of the widget.
 * @param w Width of the widget.
 * @param h Height of the widget.
 * @return Widget object pointer.
 */
gui_gif_t *gui_gif_create_from_mem(void       *parent,
                                   const char *name,
                                   void       *addr,
                                   int16_t     x,
                                   int16_t     y,
                                   int16_t     w,
                                   int16_t     h);

/**
 * @brief Create a GIF widget from filesystem.
 * @param parent Father widget it nested in.
 * @param name GIF widget name.
 * @param file GIF file path.
 * @param x X-axis coordinate of the widget.
 * @param y Y-axis coordinate of the widget.
 * @param w Width of the widget.
 * @param h Height of the widget.
 * @return Pointer to the created GIF widget.
 */
gui_gif_t *gui_gif_create_from_fs(void       *parent,
                                  const char *name,
                                  void       *file,
                                  int16_t     x,
                                  int16_t     y,
                                  int16_t     w,
                                  int16_t     h);

/**
 * @brief Create a GIF widget from FTL address.
 * @param parent Father widget it nested in.
 * @param name Widget name.
 * @param ftl Not xip address, use ftl address.
 * @param x X-axis coordinate of the widget.
 * @param y Y-axis coordinate of the widget.
 * @param w Width of the widget.
 * @param h Height of the widget.
 * @return Widget object pointer.
 */
gui_gif_t *gui_gif_create_from_ftl(void       *parent,
                                   const char *name,
                                   void       *ftl,
                                   int16_t     x,
                                   int16_t     y,
                                   int16_t     w,
                                   int16_t     h);

/**
 * @brief Rotate the GIF around its center.
 * @param _this GIF widget pointer.
 * @param degrees Clockwise rotation absolute angle.
 */
void gui_gif_rotation(gui_gif_t *_this, float degrees);

/**
 * @brief Scale the GIF, taking (0,0) as the zoom center.
 * @param _this GIF widget pointer.
 * @param scale_x Scale factor in X direction.
 * @param scale_y Scale factor in Y direction.
 */
void gui_gif_scale(gui_gif_t *_this, float scale_x, float scale_y);

/**
 * @brief Translate (move) the GIF.
 * @param _this GIF widget pointer.
 * @param t_x New X-axis coordinate.
 * @param t_y New Y-axis coordinate.
 */
void gui_gif_translate(gui_gif_t *_this, float t_x, float t_y);

/**
 * @brief Set the opacity of the GIF.
 * @param _this GIF widget pointer.
 * @param opacity_value Opacity value (0-255, default 255).
 */
void gui_gif_set_opacity(gui_gif_t *_this, unsigned char opacity_value);

/**
 * @brief Set the focus point for GIF transformations.
 * @param _this GIF widget pointer.
 * @param c_x Center X coordinate.
 * @param c_y Center Y coordinate.
 */
void gui_gif_set_focus(gui_gif_t *_this, float c_x, float c_y);

/**
 * @brief Set GIF position.
 * @param _this GIF widget pointer.
 * @param x X-axis coordinate.
 * @param y Y-axis coordinate.
 */
void gui_gif_set_pos(gui_gif_t  *_this,
                     int16_t     x,
                     int16_t     y);

/**
 * @brief Set the GIF's blend mode.
 * @param _this GIF widget pointer.
 * @param mode Enumeration value of the mode (BLEND_MODE_TYPE).
 */
void gui_gif_set_mode(gui_gif_t *_this, BLEND_MODE_TYPE mode);

/**
 * @brief Set the GIF's quality.
 * @param _this GIF widget pointer.
 * @param high_quality True for high quality rendering, false otherwise.
 */
void gui_gif_set_quality(gui_gif_t *_this, bool high_quality);

/**
 * @brief Load the GIF to read its width.
 * @param _this GIF widget pointer.
 * @return GIF's width.
 */
uint16_t gui_gif_get_width(gui_gif_t *_this);

/**
 * @brief Load the GIF to read its height.
 * @param _this GIF widget pointer.
 * @return GIF's height.
 */
uint16_t gui_gif_get_height(gui_gif_t *_this);

#ifdef __cplusplus
}
#endif

#endif
