/**
\internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_simple_img.h
  * @brief image widget header file
  * @details image widget is used to show image on the screen
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

/** @brief  image widget structure */
#ifdef  __CC_ARM
#pragma anon_unions
#endif

typedef struct gui_simple_img
{
    gui_obj_t base;
    draw_img_t *draw_img;

    void *data;                         // this means address or filesystem path

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
 * @brief load the image to read it's width
 *
 * @param img the image widget pointer.
 * @return uint16_t image's width
 */
uint16_t gui_simple_img_get_width(gui_simple_img_t *this);

/**
 * @brief load the image to read it's hight
 *
 * @param img the image widget pointer.
 * @return uint16_t image's height
 */
uint16_t gui_simple_img_get_height(gui_simple_img_t *this);

/**
 * @brief set the image's location
 *
 * @param img the image widget pointer.
 * @param x the x coordinate
 * @param y the y coordinate
 */
void gui_simple_img_set_location(gui_simple_img_t *this, uint16_t x, uint16_t y);

/**
 * @brief set the image's blend mode.
 *
 * @param img the image widget pointer.
 * @param mode the enumeration value of the mode is BLEND_MODE_TYPE.
 *
 */
void gui_simple_img_set_mode(gui_simple_img_t *this, BLEND_MODE_TYPE mode);

/**
 * @brief set x,y and file path
 *
 * @param img image widget
 * @param name change widget name
 * @param addr change picture address
 * @param x X-axis coordinate
 * @param y Y-axis coordinate
 */
void gui_simple_img_set_attribute(gui_simple_img_t *this,
                                  const char       *name,
                                  void             *addr,
                                  int16_t           x,
                                  int16_t           y);

/**
 * @brief Add opacity value to the image
 *
 * @param this the image widget pointer.
 * @param opacity_value The opacity value ranges from 0 to 255, default 255.
 */
void gui_simple_img_set_opacity(gui_simple_img_t *this, unsigned char opacity_value);

/**
 * @brief set the image's quality.
 *
 * @param img the image widget pointer.
 * @param high_quality image drawn in high quality or not.
 *
 */
void gui_simple_img_set_quality(gui_simple_img_t *this, bool high_quality);

/**
* @brief creat an simple image widget
* @note creat an simple image widget and set attribute
* @param parent the father widget it nested in.
* @param name widget name.
* @param addr bin file address in IMG_SRC_MEMADDR, or image file path in IMG_SRC_FILESYS mode.
* @param x the X-axis coordinate of the widget.
* @param y the Y-axis coordinate of the widget.
* @param w the width of the widget.
* @param h the hight of the widget.
* @param src_mode_type the type of image source.
* @return return the widget object pointer.
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
