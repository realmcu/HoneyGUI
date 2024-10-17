/**
 * @file lv_draw_rtk_ppe_rect.h
 *
 */

#ifndef LV_DRAW_PPE_RECT_H
#define LV_DRAW_PPE_RECT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_conf_internal.h"

#if LV_USE_GPU_RTK_PPE
#include "../../lv_draw_rect.h"
#include "../../sw/lv_draw_sw.h"
#include "lv_draw_rtk_ppe.h"
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_res_t lv_ppe_draw_bg(lv_draw_ctx_t *draw_ctx, const lv_area_t *coords,
                        const lv_area_t *clip_area,
                        const lv_draw_rect_dsc_t *dsc);


/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GPU_RTK_PPE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_PPE_RECT_H*/
