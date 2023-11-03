/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_img.h
  * @brief image widget header file
  * @details image widget is used to show image on the screen
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/25
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
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
#include <guidef.h>
#include <gui_api.h>
#include <draw_img.h>
#include <gui_matrix.h>

/** @defgroup WIDGET WIDGET
  * @brief
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup WIDGET_Exported_Types WIDGET Exported Types
  * @brief
  * @{
  */

/** @brief  image widget structure */
typedef struct gui_img
{
    gui_obj_t base;
    draw_img_t draw_img;

    float degrees;
    float c_x;//!< center of image x
    float c_y;//!< center of image y
    float scale_x;
    float scale_y;
    float t_x;//!< center of screen x
    float t_y;//!< center of screen y
    gui_animate_t *animate;
} gui_img_t;


/** End of WIDGET_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/
/** @defgroup WIDGET_Exported_Constants WIDGET Exported Constants
  * @brief
  * @{
  */


/** End of WIDGET_Exported_Constants
  * @}
  */

/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup WIDGET_Exported_Macros WIDGET Exported Macros
  * @brief
  * @{
  */



/** End of WIDGET_Exported_Macros
  * @}
  */

/*============================================================================*
 *                         Variables
 *============================================================================*/
/** @defgroup WIDGET_Exported_Variables WIDGET Exported Variables
  * @brief
  * @{
  */


/** End of WIDGET_Exported_Variables
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup WIDGET_Exported_Functions WIDGET Exported Functions
  * @brief
  * @{
  */

/**
 * @brief load the image to read it's width
 *
 * @param img the image widget pointer.
 * @return uint16_t image's width
 */
uint16_t gui_img_get_width(gui_img_t *img);

/**
 * @brief load the image to read it's hight
 *
 * @param img the image widget pointer.
 * @return uint16_t image's height
 */
uint16_t gui_img_get_height(gui_img_t *img);

/**
 * @brief set the image's location
 *
 * @param img the image widget pointer.
 * @param x the x coordinate
 * @param y the y coordinate
 */
void gui_img_set_location(gui_img_t *img, uint16_t x, uint16_t y);

/**
 * @brief set the image's blend mode.
 *
 * @param img the image widget pointer.
 * @param mode the enumeration value of the mode is BLEND_MODE_TYPE.
 *
 */
void gui_img_set_mode(gui_img_t *img, BLEND_MODE_TYPE mode);

/**
 * @brief set x,y and file path
 *
 * @param img image widget
 * @param filename change filename if using filesystem picture
 * @param addr change picture address
 * @param x X-axis coordinate
 * @param y Y-axis coordinate
 */
void gui_img_set_attribute(gui_img_t *img, const char *filename, void *addr, int16_t x,
                           int16_t y);

/**
 * @brief Rotate the iamge around the center of the circle
 *
 * @param img the image widget pointer.
 * @param degrees rotation angle
 * @param c_x The X-axis coordinates of the center of the circle
 * @param c_y The Y-axis coordinates of the center of the circle
 */
void gui_img_rotation(gui_img_t *img, float degrees, float c_x, float c_y);

/**
 * @brief Change the size of the image
 *
 * @param img the image widget pointer.
 * @param scale_x Scale in the x direction
 * @param scale_y Scale in the y direction
 */
void gui_img_scale(gui_img_t *img, float scale_x, float scale_y);

/**
 * @brief move image
 *
 * @param img the image widget pointer.
 * @param t_x New X-axis coordinate
 * @param t_y New Y-axis coordinate
 */
void gui_img_translate(gui_img_t *img, float t_x, float t_y);

/**
 * @brief skew inage on X-axis
 *
 * @param img the image widget pointer.
 * @param degrees skew angle
 */
void gui_img_skew_x(gui_img_t *img, float degrees);

/**
 * @brief skew inage on Y-axis
 *
 * @param img the image widget pointer.
 * @param degrees skew angle
 */
void gui_img_skew_x(gui_img_t *img, float degrees);

/**
 * @brief Add opacity value to the image
 *
 * @param this the image widget pointer.
 * @param opacity_value The opacity value ranges from 0 to 255.
 */
void gui_img_set_opacity(gui_img_t *this, unsigned char opacity_value);

/**
 * @brief creat an image widget
 * @note creat an image widget and set attribute
 * @param parent the father widget it nested in.
 * @param name widget name
 * @param addr bin file address
 * @param x the X-axis coordinate of the widget.
 * @param x the Y-axis coordinate of the widget.
 * @param w the width of the widget.
 * @param h the hight of the widget.
 * @return return the widget object pointer.
 */
gui_img_t *gui_img_create_from_mem(void *parent,  const char *name, void *addr,
                                   int16_t x, int16_t y, int16_t w, int16_t h);
/**
 * @brief creat an image widget using file path
 *
 * @param parent the father widget it nested in.
 * @param file image file path
 * @param x the X-axis coordinate of the widget.
 * @param x the Y-axis coordinate of the widget.
 * @return gui_magic_img_t*
 */
gui_img_t *gui_img_create_from_fs(void *parent, const char *file, int16_t x, int16_t y);
/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif

