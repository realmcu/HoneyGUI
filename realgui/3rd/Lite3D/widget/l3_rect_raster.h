/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef L3_RECT_RASTER_H
#define L3_RECT_RASTER_H

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/

#include <stdbool.h>
#include <stdint.h>
#include "l3_common.h"


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
