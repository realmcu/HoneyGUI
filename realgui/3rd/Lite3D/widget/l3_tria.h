/**
 \internal
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file l3_tria.h
  * @brief 3D widget for objects composed of triangle mesh
  * @details 3D widget for objects composed of triangle mesh
  * @author sienna_shen@realsil.com.cn
  * @date 2025/3/7
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
 \endinternal
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __L3_TRIA_H__
#define __L3_TRIA_H__
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


void l3_tria_push(l3_obj_model_t *_this);

void l3_tria_draw(l3_obj_model_t *_this);


void l3_tria_free_model(l3_obj_model_t *_this);

#ifdef __cplusplus
}
#endif

#endif
