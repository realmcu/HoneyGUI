/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file
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
