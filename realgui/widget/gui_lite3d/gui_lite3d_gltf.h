/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_lite3d_gltf.h
  * @brief Lite3D library encapsulation.
  * @details Lite3D library encapsulation.
  * @author sienna_shen@realsil.com.cn
  * @date 2025/10/28
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
#ifndef __GUI_LITE_3D_GLTF_H__
#define __GUI_LITE_3D_GLTF_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_api.h"
#include "gui_obj.h"
#include "l3_gltf.h"

/*============================================================================*
 *                         Types
 *============================================================================*/
typedef struct gui_lite3d_gltf
{
    gui_obj_t base;

    l3_gltf_model_t *model;

} gui_lite3d_gltf_t;

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
 * @brief Lite3D Widget Create.
 *
 * @param parent Parent widget.
 * @param name Widget name.
 * @param model Lite3D model.
 * @param x The X-axis coordinate relative to parent widget.
 * @param y The Y-axis coordinate relative to parent widget.
 * @param w Width.
 * @param h Height.
 * @return The widget object pointer.
 */
gui_lite3d_gltf_t *gui_lite3d_gltf_create(void                  *parent,
                                          const char            *name,
                                          l3_gltf_model_t       *model,
                                          int16_t                x,
                                          int16_t                y,
                                          int16_t                w,
                                          int16_t                h);

/**
 * @brief Lite3D Widget On Click.
 *
 * @param this Widget object pointer.
 * @param callback Callback function.
 * @param parameter Parameter.
 */
void gui_lite3d_gltf_on_click(gui_lite3d_gltf_t *this, void *callback, void *parameter);

#ifdef __cplusplus
}
#endif

#endif
