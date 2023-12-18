/**
 * @file lv_vglite_utils.h
 *
 */

#ifndef LV_PPE_UTILS_H
#define LV_PPE_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_conf_internal.h"

#if LV_USE_GPU_RTK_PPE
#include "../../sw/lv_draw_sw.h"
#include "../../../misc/lv_log.h"
#include "rtl_ppe.h"
#include "rtl_rcc.h"

/*********************
 *      DEFINES
 *********************/

#ifndef LV_GPU_VG_LITE_LOG_ERRORS
/** Enable logging of VG-Lite errors (\see LV_LOG_ERROR)*/
#define LV_GPU_VG_LITE_LOG_ERRORS 1
#endif

#ifndef LV_GPU_VG_LITE_LOG_TRACES
/** Enable logging of VG-Lite traces (\see LV_LOG_ERROR)*/
#define LV_GPU_VG_LITE_LOG_TRACES 0
#endif


/* The optimal Bezier control point offset for radial unit
 * see: https://spencermortensen.com/articles/bezier-circle/
 **/
#define BEZIER_OPTIM_CIRCLE 0.552284749831f

/* Draw lines for control points of Bezier curves */
#define BEZIER_DBG_CONTROL_POINTS   0

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_color32_t lv_ppe_toABGR8888(lv_color_t color);

PPE_ERR lv_ppe_recolor(ppe_buffer_t *image, ppe_buffer_t *buffer, PPE_rect_t *rect, uint32_t color);
/**********************
 *      MACROS
 **********************/


#endif /*LV_USE_GPU_RTK_PPE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_PPE_UTILS_H*/
