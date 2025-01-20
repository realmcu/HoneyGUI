/**
 * @file lv_draw_rtk_ppe.h
 *
 */


#ifndef LV_DRAW_PPE_H
#define LV_DRAW_PPE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

#if LV_USE_GPU_PPE_RTL8773E
#include "../../sw/lv_draw_sw.h"
#include "lv_draw_ppe_rtl8773e_utils.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef lv_draw_sw_ctx_t lv_draw_ppe_ctx_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_ppe_ctx_init(struct _lv_disp_drv_t *drv, lv_draw_ctx_t *draw_ctx);

void lv_draw_ppe_ctx_deinit(struct _lv_disp_drv_t *drv, lv_draw_ctx_t *draw_ctx);

/**********************
 *      MACROS
 **********************/
#endif /*LV_USE_RTK_PPE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_PPE_H*/
