/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file l3_rect.h
  * @brief 3D widget for objects composed of rectangle mesh
  * @details 3D widget for objects composed of rectangle mesh
  * @author sienna_shen@realsil.com.cn
  * @date 2025/3/7
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
#ifndef __L3_RECT_H__
#define __L3_RECT_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/

#include "l3_common.h"


/*============================================================================*
 *                         Functions
 *============================================================================*/


void l3_rect_draw(l3_obj_model_t *_this);

void l3_rect_push(l3_obj_model_t *_this);

void l3_set_rect_face_image(l3_obj_model_t *_this, uint8_t face_index, void *image_addr);

void l3_rect_free_model(l3_obj_model_t *_this);

#ifdef __cplusplus
}
#endif

#endif
