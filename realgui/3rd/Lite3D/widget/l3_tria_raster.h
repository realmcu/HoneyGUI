/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef L3_TRIA_RASTER_H
#define L3_TRIA_RASTER_H

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/

#include <stdbool.h>
#include <stdint.h>
#include "l3_common.h"
#include "l3_sw_raster.h"




void l3_draw_tria_to_canvas(l3_draw_tria_img_t *image, l3_draw_rect_img_t *combined_image,
                            float *zbuffer);

#ifdef __cplusplus
}
#endif

#endif
