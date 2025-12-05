/**
 \internal
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_lite3d.h
  * @brief Lite3D library encapsulation.
  * @details Lite3D library encapsulation.
  * @author sienna_shen@realsil.com.cn
  * @date 2025/7/10
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
#ifndef __GUI_LITE_3D_H__
#define __GUI_LITE_3D_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_api.h"
#include "gui_obj.h"
#include "l3.h"
#include "l3_obj.h"
#include "l3_gltf.h"

/*============================================================================*
 *                         Types
 *============================================================================*/
typedef struct gui_lite3d
{
    gui_obj_t base;

    l3_model_base_t *model;

} gui_lite3d_t;

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
 * @param x X-axis coordinate relative to parent widget.
 * @param y Y-axis coordinate relative to parent widget.
 * @param w Width.
 * @param h Height.
 * @return Widget object pointer.
 */
gui_lite3d_t *gui_lite3d_create(void                  *parent,
                                const char            *name,
                                l3_model_base_t       *model,
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
void gui_lite3d_on_click(gui_lite3d_t *this, void *callback, void *parameter);

#ifdef __cplusplus
}
#endif

#endif
