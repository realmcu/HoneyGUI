/**
\internal
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
#ifdef  __CC_ARM
#pragma anon_unions
#endif

typedef struct gui_img
{
    gui_obj_t base;
    draw_img_t *draw_img;

    float degrees;
    float c_x;                          //!< center of image x
    float c_y;                          //!< center of image y
    float scale_x;
    float scale_y;
    float t_x;                          //!< translate of screen x
    float t_y;                          //!< translate of screen y
    void *data;                         // this means address or filesystem path
    union
    {
        void *filename;                         // keep the filepath for partial draw
        void *ftl;
    };
    gui_animate_t *animate;
    uint16_t scope_x1;
    uint16_t scope_x2;
    uint16_t scope_y1;
    uint16_t scope_y2;
    int16_t ax, ay;
    uint32_t opacity_value : 8;
    uint32_t blend_mode    : 3;
    uint32_t src_mode      : 3;
    uint32_t high_quality  : 1;
    uint32_t press_flag    : 1;         //!< press to change picture to the highlighted
    uint32_t release_flag  : 1;
    uint32_t scope         : 1;
    uint32_t scope_flag         : 1;
    uint8_t checksum;
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
/** @defgroup WIDGET_Exported_GUI_Functions WIDGET Exported Functions
  * @brief
  * @{
  */

/**
 * @brief load the image to read it's width
 *
 * @param this the image widget pointer.
 * @return uint16_t image's width
 */
uint16_t gui_img_get_width(gui_img_t *this);

/**
 * @brief load the image to read it's hight
 *
 * @param this the image widget pointer.
 * @return uint16_t image's height
 */
uint16_t gui_img_get_height(gui_img_t *this);

/**
 * @brief refresh the image size from src
 *
 * @param this the image widget pointer.
 */
void gui_img_refresh_size(gui_img_t *this);

/**
 * @brief set the image's location
 *
 * @param this the image widget pointer.
 * @param x the x coordinate
 * @param y the y coordinate
 */
void gui_img_set_location(gui_img_t *this, uint16_t x, uint16_t y);

/**
 * @brief set the image's blend mode.
 *
 * @param this the image widget pointer.
 * @param mode the enumeration value of the mode is BLEND_MODE_TYPE.
 *
 */
void gui_img_set_mode(gui_img_t *this, BLEND_MODE_TYPE mode);

/**
 * @brief set x,y and file path
 *
 * @param this image widget
 * @param name change widget name
 * @param addr change picture address
 * @param x X-axis coordinate
 * @param y Y-axis coordinate
 */
void gui_img_set_attribute(gui_img_t  *this,
                           const char *name,
                           void       *addr,
                           int16_t     x,
                           int16_t     y);

/**
 * @brief Rotate the image around the center of the circle
 *
 * @param this the image widget pointer.
 * @param degrees clockwise rotation absolute angle
 * @param c_x The X-axis coordinates of the center of the circle
 * @param c_y The Y-axis coordinates of the center of the circle
 */
void gui_img_rotation(gui_img_t *this,
                      float      degrees,
                      float      c_x,
                      float      c_y);

/**
 * @brief Change the size of the image, take (0, 0) as the zoom center
 *
 * @param this the image widget pointer.
 * @param scale_x Scale in the x direction
 * @param scale_y Scale in the y direction
 */
void gui_img_scale(gui_img_t *this, float scale_x, float scale_y);

/**
 * @brief move image
 *
 * @param this the image widget pointer.
 * @param t_x New X-axis coordinate
 * @param t_y New Y-axis coordinate
 */
void gui_img_translate(gui_img_t *this, float t_x, float t_y);

/**
 * @brief skew image on X-axis
 *
 * @param this the image widget pointer.
 * @param degrees skew angle
 */
void gui_img_skew_x(gui_img_t *this, float degrees);

/**
 * @brief skew image on Y-axis
 *
 * @param this the image widget pointer.
 * @param degrees skew angle
 */
void gui_img_skew_y(gui_img_t *this, float degrees);

/**
 * @brief Add opacity value to the image
 *
 * @param this the image widget pointer.
 * @param opacity_value The opacity value ranges from 0 to 255, default 255.
 */
void gui_img_set_opacity(gui_img_t *this, unsigned char opacity_value);

/**
 * @brief creat an image widget from memory address
 * @note creat an image widget and set attribute
 * @param parent the father widget it nested in.
 * @param name widget name.
 * @param addr bin file address.
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @param w the width of the widget.
 * @param h the hight of the widget.
 * @return return the widget object pointer.
 */
gui_img_t *gui_img_create_from_mem(void       *parent,
                                   const char *name,
                                   void       *addr,
                                   int16_t     x,
                                   int16_t     y,
                                   int16_t     w,
                                   int16_t     h);

/**
 * @brief creat an image widget from memory address
 * @note creat an image widget and set attribute
 * @param parent the father widget it nested in.
 * @param name widget name.
 * @param ftl not xip address, use ftl address
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @param w the width of the widget.
 * @param h the hight of the widget.
 * @return return the widget object pointer.
 */
gui_img_t *gui_img_create_from_ftl(void       *parent,
                                   const char *name,
                                   void       *ftl,
                                   int16_t     x,
                                   int16_t     y,
                                   int16_t     w,
                                   int16_t     h);
/**
 * @brief creat an image widget from filesystem
 *
 * @param parent the father widget it nested in.
 * @param name image widget name
 * @param file image file path
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @param w the width of the widget.
 * @param h the hight of the widget.
 * @return gui_img_t*
 */
gui_img_t *gui_img_create_from_fs(void       *parent,
                                  const char *name,
                                  void       *file,
                                  int16_t     x,
                                  int16_t     y,
                                  int16_t     w,
                                  int16_t     h);

/**
 * @brief
 *
 * @param this
 * @param dur
 * @param repeat_count
 * @param callback
 * @param p
 */
void gui_img_set_animate(gui_img_t *this,
                         uint32_t   dur,
                         int        repeat_count,
                         void      *callback,
                         void      *p);

/**
 * @brief set the image's quality.
 *
 * @param this the image widget pointer.
 * @param high_quality image drawn in high quality or not.
 *
 */
void gui_img_set_quality(gui_img_t *this, bool high_quality);

/**
 * @brief
 *
 * @param obj
 * @param matrix
 * @return gui_img_t*
 */
void gui_img_tree_convert_to_img(gui_obj_t *obj, gui_matrix_t *matrix, uint8_t *shot_buf);

/** End of WIDGET_Exported_GUI_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif
