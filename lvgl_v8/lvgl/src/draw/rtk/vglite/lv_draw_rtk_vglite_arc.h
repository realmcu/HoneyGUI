/**
 * @file lv_draw_vglite_arc.h
 *
 */


#ifndef LV_DRAW_VGLITE_ARC_H
#define LV_DRAW_VGLITE_ARC_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_conf_internal.h"

#if LV_USE_GPU_VG_LITE
#include "lv_vglite_utils.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_res_t lv_gpu_nxp_vglite_draw_arc(const lv_point_t *center, int32_t radius, int32_t start_angle,
                                    int32_t end_angle,
                                    const lv_area_t *clip_area, const lv_draw_arc_dsc_t *dsc);
lv_res_t lv_vglite_draw_arc(const lv_point_t *center, int32_t radius, int32_t start_angle,
                            int32_t end_angle,
                            const lv_area_t *clip_area, const lv_draw_arc_dsc_t *dsc);
/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GPU_NXP_VG_LITE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_VGLITE_ARC_H*/
