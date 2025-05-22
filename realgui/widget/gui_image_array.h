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

/*============================================================================*
 *                         Types
 *============================================================================*/

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
    const char *text;
} gui_image_array_t;

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
 * @brief Set the image array for the widget.
 * @param obj The widget pointer.
 * @param image_data_array The image file data.
 * @param array_length The length of the array.
 */
void gui_image_array_write(gui_image_array_t *obj, void **image_data_array, int array_length);

/**
 * @brief Create an image array widget.
 * @param parent Parent widget.
 * @param x Left coordinate.
 * @param y Top coordinate.
 * @param image_data_array The image data array.
 * @param array_lenght The length of the array.
 * @param name Widget name.
 * @return Return the image array widget pointer.
 */
gui_image_array_t *gui_image_array_create(void *parent,
                                          int16_t x,
                                          int16_t y,
                                          void **image_data_array,
                                          int array_lenght,
                                          const char *widget_name);

/**
 * @brief Create an image array widget in FTL address mode.
 * @param parent Parent widget.
 * @param x Left coordinate.
 * @param y Top coordinate.
 * @param image_data_array The image data array (FTL address mode).
 * @param array_lenght The length of the array.
 * @param name Widget name.
 * @return Return the image array widget pointer.
 */
gui_image_array_t *gui_image_array_create_ftl(void *parent,
                                              int16_t x,
                                              int16_t y,
                                              void **image_data_array,
                                              int array_lenght,
                                              const char *widget_name);

#ifdef __cplusplus
}
#endif

#endif
