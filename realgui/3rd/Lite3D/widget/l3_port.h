/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file l3_port.h
  * @brief
  * @details
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
#ifndef L3_PORT_H
#define L3_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/

#include <stdbool.h>
#include <stdint.h>
#include "l3_common.h"

/*============================================================================*
 *                            Custom Define
 *============================================================================*/

/**
 * @brief Allocates memory of the specified size using a custom allocator.
 * @param size The size of memory to allocate in bytes.
 * @return Pointer to the allocated memory block.
 */
void *l3_malloc(size_t size);

/**
 * @brief Frees memory allocated by l3_malloc.
 * @param ptr Pointer to the memory block to free.
 */
void l3_free(void *ptr);

/**
 * @brief Draws a rectangular image onto a canvas within the specified rectangle area.
 * @param image Pointer to the image data to draw.
 * @param dc Pointer to the canvas where the image will be drawn.
 * @param rect Pointer to the rectangle specifying the target area on the canvas.
 */
void l3_draw_rect_img_to_canvas(l3_draw_rect_img_t *image, l3_canvas_t *dc, l3_rect_t *rect);


#ifdef __cplusplus
}
#endif

#endif
