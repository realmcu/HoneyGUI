/**
 * @file lv_draw_vglite_rect.h
 *
 */

#ifndef LV_DRAW_VGLITE_RECT_H
#define LV_DRAW_VGLITE_RECT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_conf_internal.h"

#if LV_USE_GPU_VG_LITE
#include "lv_vglite_utils.h"
#include "../../lv_draw_rect.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Draw rectangle background with effects (rounded corners, gradient)
 *
 * @param[in] coords Coordinates of the rectangle background (relative to dest buff)
 * @param[in] clip_area Clipping area with relative coordinates to dest buff
 * @param[in] dsc Description of the rectangle background
 *
 * @retval LV_RES_OK Draw completed
 * @retval LV_RES_INV Error occurred (\see LV_GPU_NXP_VG_LITE_LOG_ERRORS)
 *
 */
lv_res_t lv_gpu_vglite_draw_bg(const lv_area_t *coords, const lv_area_t *clip_area,
                               const lv_draw_rect_dsc_t *dsc);

/**
 * Draw rectangle border/outline shape with effects (rounded corners, opacity)
 *
 * @param[in] coords Coordinates of the rectangle border/outline (relative to dest buff)
 * @param[in] clip_area Clipping area with relative coordinates to dest buff
 * @param[in] dsc Description of the rectangle border/outline
 * @param[in] border True for border, False for outline
 *
 * @retval LV_RES_OK Draw completed
 * @retval LV_RES_INV Error occurred (\see LV_GPU_NXP_VG_LITE_LOG_ERRORS)
 *
 */
lv_res_t lv_gpu_vglite_draw_border_generic(const lv_area_t *coords, const lv_area_t *clip_area,
                                           const lv_draw_rect_dsc_t *dsc, bool border);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GPU_VG_LITE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_VGLITE_RECT_H*/
