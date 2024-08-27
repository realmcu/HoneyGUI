/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_image_array.h
  * @brief image array widget
  * @details show a image array
  * @author howie_wang@realsil.com.cn
  * @date 2024/08/05
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
#ifndef __GUI_IMAGE_ARRAY_H__
#define __GUI_IMAGE_ARRAY_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include <gui_api.h>
#include "gui_img.h"

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



/** @brief  widget structure */
#ifdef  __CC_ARM
#pragma anon_unions
#endif
struct gui_image_array_scope
{
    uint16_t scope_x1;
    uint16_t scope_x2;
    uint16_t scope_y1;
    uint16_t scope_y2;
    int16_t ax, ay, w;
};

typedef struct gui_image_array
{
    gui_obj_t base;
    draw_img_t **draw_img;
    struct gui_image_array_scope **scope;
    void **image_data;
    int array_count;
    uint8_t checksum;
    bool parent_scope, scope_flag;
} gui_image_array_t;

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
 * @brief set image array
 *
 * @param obj the widget pointer
 * @param image_data_array the image file data
 * @param array_length array_length
 */
void gui_image_array_write(gui_image_array_t *obj, void **image_data_array, int array_length);

/**
 * @brief image array create
 *
 * @param parent parent widget
 * @param x left
 * @param y top
 * @param image_data_array  image_data_array
 * @param array_lenght  array_lenght
 * @param name  widget name
 * @return gui_image_array_t* widget pointer
 */
gui_image_array_t *gui_image_array_create(void *parent,
                                          int16_t x,
                                          int16_t y,
                                          void **image_data_array,
                                          int array_lenght,
                                          const char *widget_name);


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
