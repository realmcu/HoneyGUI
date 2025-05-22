/**
\internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_simple_img.h
  * @brief Image widget header file.
  * @details Image widget is used to show image on the screen.
  * @author
  * @date
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
#ifndef __GUI_SIMPLE_IMG_H__
#define __GUI_SIMPLE_IMG_H__
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

/*============================================================================*
 *                         Types
 *============================================================================*/

/** @brief  Image widget structure. */
#ifdef  __CC_ARM
#pragma anon_unions
#endif

typedef struct gui_simple_img
{
    gui_obj_t base;
    draw_img_t *draw_img;

    void *data;                         // this means address or filesystem path
    union
    {
        void *filename;                  // keep the filepath for partial draw
        void *ftl;
    };
    uint32_t opacity_value : 8;
    uint32_t blend_mode    : 3;
    uint32_t src_mode      : 3;
    uint32_t high_quality  : 1;
    uint32_t press_flag    : 1;         //!< press to change picture to the highlighted
    uint32_t release_flag  : 1;

    uint8_t checksum;
} gui_simple_img_t;

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
 * @param img The image widget pointer.
 * @return uint16_t Image's width.
 */
uint16_t gui_simple_img_get_width(gui_simple_img_t *this);

/**
 * @brief Load the image to read its height.
 *
 * @param img The image widget pointer.
 * @return uint16_t Image's height.
 */
uint16_t gui_simple_img_get_height(gui_simple_img_t *this);

/**
 * @brief Set the image's location.
 *
 * @param img The image widget pointer.
 * @param x The x coordinate.
 * @param y The y coordinate.
 */
void gui_simple_img_set_location(gui_simple_img_t *this, uint16_t x, uint16_t y);

/**
 * @brief Set the image's blend mode.
 *
 * @param img The image widget pointer.
 * @param mode The enumeration value of the mode is BLEND_MODE_TYPE.
 *
 */
void gui_simple_img_set_mode(gui_simple_img_t *this, BLEND_MODE_TYPE mode);

/**
 * @brief Set x, y and file path.
 *
 * @param img Image widget.
 * @param name Change widget name.
 * @param addr Change picture address.
 * @param x X-axis coordinate.
 * @param y Y-axis coordinate.
 */
void gui_simple_img_set_attribute(gui_simple_img_t *this,
                                  const char       *name,
                                  void             *addr,
                                  int16_t           x,
                                  int16_t           y);

/**
 * @brief Add opacity value to the image.
 *
 * @param this The image widget pointer.
 * @param opacity_value The opacity value ranges from 0 to 255, default 255.
 */
void gui_simple_img_set_opacity(gui_simple_img_t *this, unsigned char opacity_value);

/**
 * @brief Set the image's quality.
 *
 * @param img The image widget pointer.
 * @param high_quality Image drawn in high quality or not.
 *
 */
void gui_simple_img_set_quality(gui_simple_img_t *this, bool high_quality);

/**
* @brief Create a simple image widget.
* @note Create a simple image widget and set attribute.
* @param parent The father widget it nested in.
* @param name Widget name.
* @param addr Bin file address in IMG_SRC_MEMADDR, or image file path in IMG_SRC_FILESYS mode.
* @param x The X-axis coordinate of the widget.
* @param y The Y-axis coordinate of the widget.
* @param w The width of the widget.
* @param h The height of the widget.
* @param src_mode_type The type of image source.
* @return Return the widget object pointer.
*/
gui_simple_img_t *gui_simple_img_create(void                 *parent,
                                        const char           *name,
                                        void                 *addr,
                                        int16_t               x,
                                        int16_t               y,
                                        int16_t               w,
                                        int16_t               h,
                                        IMG_SOURCE_MODE_TYPE  src_mode_type);

#ifdef __cplusplus
}
#endif

#endif
