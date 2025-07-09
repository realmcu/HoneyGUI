/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_gray.h
  * @brief Gray scale widget
  * @details Widget that converts content in its area to grayscale
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
#ifndef __GUI_GRAY_H__
#define __GUI_GRAY_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_api.h"
#include "gui_obj.h"
#include "engine_gray.h"


/*============================================================================*
 *                         Types
 *============================================================================*/

/** @brief Gray scale widget structure */
typedef struct
{
    gui_obj_t base;                    /* Base object structure */
    void *data;                        /* User data pointer */
    engine_gray_t *gray;               /* gray scale image */
    uint8_t gray_level;                /* Gray level (0-255), 0 for black, 255 for white */
    uint8_t checksum;                  /* Checksum for change detection */
} gui_gray_t;

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
 * @brief Create a gray scale widget
 *
 * @param parent Parent object pointer
 * @param name Widget name
 * @param data User data pointer
 * @param x X coordinate
 * @param y Y coordinate
 * @param w Width
 * @param h Height
 * @return gui_gray_t* Pointer to the created gray scale widget
 */
gui_gray_t *gui_gray_create(void       *parent,
                            const char *name,
                            void       *data,
                            int16_t     x,
                            int16_t     y,
                            int16_t     w,
                            int16_t     h);

/**
 * @brief Set the gray level of the widget
 * @param this Gray scale widget pointer
 * @param name Widget name
 * @param data User data pointer
 * @param x X coordinate
 * @param y Y coordinate
 */
void gui_gray_set_attribute(gui_gray_t  *this,
                            const char *name,
                            void       *data,
                            int16_t     x,
                            int16_t     y);
/**
 * @brief Set the gray level of the widget
 *
 * @param gray Gray scale widget pointer
 * @param level Gray level (0-255), 0 for black, 255 for white
 */
void gui_gray_set_level(gui_gray_t *gray, uint8_t level);


#ifdef __cplusplus
}
#endif

#endif
