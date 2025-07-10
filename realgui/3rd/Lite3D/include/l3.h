/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file Lite3D.h
  * @brief Lite 3D widget
  * @details Lite 3D widget
  * @author sienna_shen@realsil.com.cn
  * @date 2025/4/11
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
#ifndef __L3_H__
#define __L3_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "l3_common.h"

/*============================================================================*
 *                         Types
 *============================================================================*/



typedef void (*l3_global_transform_cb)(l3_model_t *__this);

typedef void (*l3_face_transform_cb)(l3_model_t *__this, size_t face_index/*face offset*/);

/*============================================================================*
 *                         Functions
 *============================================================================*/

// Memory management. Default malloc/free functions.
void *l3_malloc(size_t size);
void l3_free(void *ptr);


l3_model_t *l3_create_model(void *desc_addr, L3_DRAW_TYPE draw_type, int16_t view_w,
                            int16_t view_h);



void l3_set_target_canvas(l3_model_t *_this, \
                          uint16_t x, uint16_t y, \
                          uint16_t w, uint16_t h, \
                          uint16_t bit_depth, \
                          uint8_t *canvas);

void l3_set_face_image(l3_model_t *this, uint8_t face_index, void *image_addr);

/**
 * @brief Set global transform callback.
 *
 * @param _this the 3D model pointer
 * @param cb Set callback functions for the world coordinate system, camera coordinate system,
 *           and light source for all faces
 */
void l3_set_global_transform(l3_model_t *_this, l3_global_transform_cb cb);

/**
 * @brief Set face transform callback.
 *
 * @param _this the 3D model pointer
 * @param cb Set callback functions for the world coordinate system, camera coordinate system,
 *           and light source for the specified face
 */
void l3_set_face_transform(l3_model_t *_this, l3_face_transform_cb cb);

void l3_push(l3_model_t *_this);

void l3_draw(l3_model_t *_this);

void l3_push_and_draw(l3_model_t *_this);

/**
 * @brief 3D model memory free.
 *
 * @param _this the 3D model pointer
 */
void l3_free_model(l3_model_t *_this);


char *Lite_3D_Version(void);


#ifdef __cplusplus
}
#endif

#endif
