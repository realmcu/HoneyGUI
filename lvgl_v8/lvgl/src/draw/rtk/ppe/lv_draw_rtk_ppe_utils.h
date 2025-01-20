/**
 * @file lv_draw_rtk_ppe_utils.h
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

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_color32_t lv_ppe_toABGR8888(lv_color_t color);

PPE_ERR lv_ppe_recolor(ppe_buffer_t *image, ppe_buffer_t *buffer, ppe_rect_t *rect, uint32_t color);

void lv_ppe_get_transformed_area(lv_area_t *res, lv_coord_t w, lv_coord_t h, int16_t angle,
                                 uint16_t zoom,
                                 const lv_point_t *pivot);
/**********************
 *      MACROS
 **********************/


#endif /*LV_USE_GPU_RTK_PPE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_PPE_UTILS_H*/
