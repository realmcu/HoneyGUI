/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file
  * @brief
  * @details
  * @author
  * @date
  * @version
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

/** @brief  ... */

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
  * @brief  ...
  * @note
  * @param[in]  param1 ...
  * @param[in]  param2 ...
  * @return ...
  * @retval ...
  */
gui_img_t *gui_img_create_from_mem(void *parent,  const char *name, void *addr,
                                   int16_t x, int16_t y, int16_t w, int16_t h);


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
 * @brief set the image's mode.
 *
 * @param img the image widget pointer.
 * @param mode the enumeration value of the mode is IMAGE_MODE.
 * @return void
 *
 */
void gui_img_set_mode(gui_img_t *img, BLEND_MODE_TYPE mode);

/**
 * @brief
 *
 * @param img
 * @param degrees
 * @param c_x
 * @param c_y
 */
void gui_img_rotation(gui_img_t *img, float degrees, float c_x, float c_y);

/**
 * @brief
 *
 * @param img
 * @param scale_x
 * @param scale_y
 */
void gui_img_scale_add(gui_img_t *img, float scale_x, float scale_y);

/**
 * @brief
 *
 * @param img
 * @param scale_x
 * @param scale_y
 */
void gui_img_scale(gui_img_t *img, float scale_x, float scale_y);

/**
 * @brief
 *
 * @param img
 * @param t_x
 * @param t_y
 */
void gui_img_translate(gui_img_t *img, float t_x, float t_y);

/**
 * @brief
 *
 * @param this
 * @param opacity_value
 */
void gui_img_set_opacity(gui_img_t *this, unsigned char opacity_value);


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

