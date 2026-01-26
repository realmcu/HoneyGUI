/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
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

void l3_rect_push(l3_obj_model_t *_this, l3_3x3_matrix_t *parent_matrix);

void l3_set_rect_face_image(l3_obj_model_t *_this, uint8_t face_index, void *image_addr);

void l3_rect_free_model(l3_obj_model_t *_this);

#ifdef __cplusplus
}
#endif

#endif
