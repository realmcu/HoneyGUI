/**
 * @file lv_vglite_utils.h
 *
 */

#ifndef LV_VGLITE_UTILS_H
#define LV_VGLITE_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_conf_internal.h"

#if LV_USE_GPU_VG_LITE
#include "vg_lite.h"
#include "../../sw/lv_draw_sw.h"
#include "../../../misc/lv_log.h"

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

lv_res_t lv_vglite_premult_and_swizzle(vg_lite_color_t *vg_col32, lv_color32_t lv_col32,
                                       lv_opa_t opa,
                                       vg_lite_buffer_format_t vg_col_format);

vg_lite_blend_t lv_vglite_get_blend_mode(lv_blend_mode_t lv_blend_mode);

lv_res_t lv_vglite_run(void);

/**********************
 *      MACROS
 **********************/

#define VG_LITE_COND_STOP(cond, txt)          \
    do {                                      \
        if (cond) {                           \
            LV_LOG_ERROR("%s. STOP!", txt);   \
            for ( ; ; );                      \
        }                                     \
    } while(0)

#if LV_GPU_VG_LITE_LOG_ERRORS
#define VG_LITE_ERR_RETURN_INV(err, fmt, ...) \
    do {                                      \
        if(err != VG_LITE_SUCCESS) {          \
            LV_LOG_ERROR(fmt" (err = %d)",    \
                         err, ##__VA_ARGS__); \
            return LV_RES_INV;                \
        }                                     \
    } while (0)
#else
#define VG_LITE_ERR_RETURN_INV(err, fmt, ...) \
    do {                                      \
        if(err != VG_LITE_SUCCESS) {          \
            return LV_RES_INV;                \
        }                                     \
    }while(0)
#endif /*LV_GPU_VG_LITE_LOG_ERRORS*/

#if LV_GPU_VG_LITE_LOG_TRACES
#define VG_LITE_LOG_TRACE(fmt, ...)           \
    do {                                      \
        LV_LOG(fmt, ##__VA_ARGS__);     \
    } while (0)

#define VG_LITE_RETURN_INV(fmt, ...)          \
    do {                                      \
        LV_LOG_ERROR(fmt, ##__VA_ARGS__);     \
        return LV_RES_INV;                    \
    } while (0)
#else
#define VG_LITE_LOG_TRACE(fmt, ...)           \
    do {                                      \
    } while (0)
#define VG_LITE_RETURN_INV(fmt, ...)          \
    do {                                      \
        return LV_RES_INV;                    \
    }while(0)
#endif /*LV_GPU_VG_LITE_LOG_TRACES*/

#endif /*LV_USE_GPU_VG_LITE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_VGLITE_UTILS_H*/
