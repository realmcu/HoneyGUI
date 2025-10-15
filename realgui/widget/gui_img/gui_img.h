/**
\internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_img.h
  * @brief Image widget header file.
  * @details Image widget is used to show image on the screen
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/25
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
\endinternal
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
#include "gui_matrix.h"
#include "gui_obj.h"
#include "gifdec.h"

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
    draw_img_t *draw_img;              /* Drawing image structure. */
    float degrees;                     /* Rotation angle in degrees. */
    float scale_x;                     /* Scale factor in X direction. */
    float scale_y;                     /* Scale factor in Y direction. */
    float f_x;                         /* Focus of image X coordinate. */
    float f_y;                         /* Focus of image Y coordinate. */

    float t_x;                         /* Translation in X direction. */
    float t_y;                         /* Translation in Y direction. */

    void *data;                        /* Image data (address or filesystem path). */
    union
    {
        void *filename;               /* Filepath for partial draw. */
        void *ftl;                    /* FTL address. */
    };
    gd_GIF *gif;                        /* GIF data */
    uint32_t fg_color_set;  //A8 image set color
    uint32_t bg_color_fix;  //bg color fix for A8 image
    uint8_t alpha_mix;      //alpha mix for A8 image

    uint32_t opacity_value : 8;        /* Opacity value (0-255). */
    uint32_t blend_mode    : 5;        /* Blend mode. */
    uint32_t storage_type  : 3;        /* Storage type: e.g., file system, flash, etc. */
    uint32_t high_quality  : 1;        /* High quality rendering flag. */
    uint32_t need_clip     : 1;        /* Clipping flag. */
    uint32_t gif_flag      : 1;        /* gif flag. */
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
 * @param this The image widget pointer.
 * @return uint16_t Image's width.
 */
uint16_t gui_img_get_width(gui_img_t *_this);

/**
 * @brief Load the image to read its height.
 *
 * @param this The image widget pointer.
 * @return uint16_t Image's height.
 */
uint16_t gui_img_get_height(gui_img_t *_this);

/**
 * @brief Refresh the image size from source.
 *
 * @param this The image widget pointer.
 */
void gui_img_refresh_size(gui_img_t *_this);

void gui_img_refresh_draw_data(gui_img_t  *_this);
/**
 * @brief Set the image's blend mode.
 *
 * @param this The image widget pointer.
 * @param mode The enumeration value of the mode (BLEND_MODE_TYPE).
 */
void gui_img_set_mode(gui_img_t *_this, BLEND_MODE_TYPE mode);

/**
 * @brief Set image attributes (name, path, position).
 *
 * @param this Image widget pointer.
 * @param name Widget name.
 * @param addr Image address/path.
 * @param x X-axis coordinate.
 * @param y Y-axis coordinate.
 */
void gui_img_set_attribute(gui_img_t  *_this,
                           const char *name,
                           void       *addr,
                           int16_t     x,
                           int16_t     y);

/**
 * @brief Rotate the image around its center.
 *
 * @param this The image widget pointer.
 * @param degrees Clockwise rotation absolute angle.
 */
void gui_img_rotation(gui_img_t *_this,
                      float      degrees);

/**
 * @brief Scale the image, taking (0,0) as the zoom center.
 *
 * @param this The image widget pointer.
 * @param scale_x Scale factor in X direction.
 * @param scale_y Scale factor in Y direction.
 */
void gui_img_scale(gui_img_t *_this, float scale_x, float scale_y);

/**
 * @brief Translate (move) the image.
 *
 * @param this The image widget pointer.
 * @param t_x New X-axis coordinate.
 * @param t_y New Y-axis coordinate.
 */
void gui_img_translate(gui_img_t *_this, float t_x, float t_y);

/**
 * @brief Skew the image on X-axis.
 *
 * @param this The image widget pointer.
 * @param degrees Skew angle.
 */
void gui_img_skew_x(gui_img_t *_this, float degrees);

/**
 * @brief Skew the image on Y-axis.
 *
 * @param this The image widget pointer.
 * @param degrees Skew angle.
 */
void gui_img_skew_y(gui_img_t *_this, float degrees);

/**
 * @brief Set the opacity of the image.
 *
 * @param this The image widget pointer.
 * @param opacity_value Opacity value (0-255, default 255).
 */
void gui_img_set_opacity(gui_img_t *_this, unsigned char opacity_value);

/**
 * @brief Set the focus point for image transformations.
 *
 * @param this The image widget pointer.
 * @param c_x Center X coordinate.
 * @param c_y Center Y coordinate.
 */
void gui_img_set_focus(gui_img_t *_this, float c_x, float c_y);

/**
 * @brief Create an image widget from memory address.
 * @note Create an image widget and set attribute.
 * @param parent The father widget it nested in.
 * @param name Widget name.
 * @param addr The bin file address.
 * @param x The X-axis coordinate of the widget.
 * @param y The Y-axis coordinate of the widget.
 * @param w The width of the widget.
 * @param h The height of the widget.
 * @return The widget object pointer.
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
 * @param parent The father widget it nested in.
 * @param name Widget name.
 * @param ftl Not xip address, use ftl address.
 * @param x The X-axis coordinate of the widget.
 * @param y The Y-axis coordinate of the widget.
 * @param w The width of the widget.
 * @param h The height of the widget.
 * @return Return the widget object pointer.
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
 * @param parent The father widget it nested in.
 * @param name Image widget name.
 * @param file Image file path.
 * @param x The X-axis coordinate of the widget.
 * @param y The Y-axis coordinate of the widget.
 * @param w The width of the widget.
 * @param h The height of the widget.
 * @return gui_img_t*.
 */
gui_img_t *gui_img_create_from_fs(void       *parent,
                                  const char *name,
                                  void       *file,
                                  int16_t     x,
                                  int16_t     y,
                                  int16_t     w,
                                  int16_t     h);

/**
 * @brief Set the image's quality.
 *
 * @param this The image widget pointer.
 * @param high_quality True for high quality rendering, false otherwise.
 */
void gui_img_set_quality(gui_img_t *_this, bool high_quality);

/**
 * @brief Convert a tree to an image data.
 *
 * @param obj Tree root.
 * @param matrix Null if no need to transform.
 * @param shot_buf Buffer for the screenshot.
 */
void gui_img_tree_convert_to_img(gui_obj_t *obj, gui_matrix_t *matrix, uint8_t *shot_buf);

/**
 * @brief Get the scale factor in X direction.
 *
 * @param this The image widget pointer.
 * @return Scale factor in X direction.
 */
float gui_img_get_scale_x(gui_img_t *_this);

/**
 * @brief Get the scale factor in Y direction.
 *
 * @param this The image widget pointer.
 * @return Scale factor in Y direction.
 */
float gui_img_get_scale_y(gui_img_t *_this);

/**
 * @brief Get the rotation angle in degrees.
 *
 * @param this The image widget pointer.
 * @return Rotation angle in degrees.
 */
float gui_img_get_degrees(gui_img_t *_this);

/**
 * @brief Get the center X coordinate for transformations.
 *
 * @param this The image widget pointer.
 * @return Center X coordinate.
 */
float gui_img_get_c_x(gui_img_t *_this);

/**
 * @brief Get the center Y coordinate for transformations.
 *
 * @param this The image widget pointer.
 * @return Center Y coordinate.
 */
float gui_img_get_c_y(gui_img_t *_this);

/**
 * @brief Get the translation in X direction.
 *
 * @param this The image widget pointer.
 * @return Translation in X direction.
 */
float gui_img_get_t_x(gui_img_t *_this);

/**
 * @brief Get the translation in Y direction.
 *
 * @param this The image widget pointer.
 * @return Translation in Y direction.
 */
float gui_img_get_t_y(gui_img_t *_this);


/**
 * @brief Sets the image data for a specified image widget.
 *
 * This function assigns the given image data to the specified image widget.
 * The image data might correspond to various formats, and the format
 * should be compatible with the handling of `gui_img_t`.
 *
 * @param widget          The pointer to the image widget (`gui_img_t`) for which the image data is to be set.
 * @param image_data_pointer  The pointer to the image data to be set to the widget.
 *                            The data should persist as long as the widget needs it or until it is explicitly updated.
 */
void gui_img_set_image_data(gui_img_t *_this, const uint8_t *image_data_pointer);

/**
 * @brief Gets the image data from a specified image widget.
 *
 * This function returns the current image data that is set in the specified image widget.
 *
 * @param widget The pointer to the image widget (`gui_img_t`) from which the image data should be retrieved.
 *
 * @return A pointer to the image data currently set in the widget. If no image data is set, the result may be `NULL`.
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
void gui_img_a8_mix_alpha(gui_img_t *_this, uint32_t alpha_mix);




#ifdef __cplusplus
}
#endif

#endif
