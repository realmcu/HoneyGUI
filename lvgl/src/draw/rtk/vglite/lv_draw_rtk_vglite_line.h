/**
 * @file lv_draw_vglite_line.h
 *
 */


#ifndef LV_DRAW_VGLITE_LINE_H
#define LV_DRAW_VGLITE_LINE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_conf_internal.h"

#if LV_USE_GPU_VG_LITE
#include "lv_vglite_utils.h"
#include "../../lv_draw_line.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_res_t lv_gpu_vglite_draw_line(const lv_point_t *point1, const lv_point_t *point2,
                                 const lv_area_t *clip_area, const lv_draw_line_dsc_t *dsc);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GPU_VG_LITE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_VGLITE_RECT_H*/
