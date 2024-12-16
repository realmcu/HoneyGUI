/**
 * @file lv_draw_rtk_ppe_utils_v2.h
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

#if LV_USE_GPU_RTK_PPEV2
#include "../../sw/lv_draw_sw.h"
#include "../../../misc/lv_log.h"
#include "rtl_ppe.h"
#include "hal_idu.h"
#include "rtl876x_rcc.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
uint8_t *lv_ppe_get_buffer(uint32_t *size);
void lv_ppe_get_transformed_area(lv_area_t *res, lv_coord_t w, lv_coord_t h, ppe_matrix_t *matrix);
void lv_ppe_get_matrix(ppe_matrix_t *matrix, const lv_area_t *coords, const lv_draw_img_dsc_t *dsc);
void lv_int_get_transformed_area(lv_area_t *res, lv_coord_t w, lv_coord_t h, int16_t angle,
                                 uint16_t zoom,
                                 const lv_point_t *pivot);
lv_color32_t lv_ppe_toABGR8888(lv_color_t color);
lv_res_t lv_ppe_buffer_copy(void *dest_buf, lv_coord_t dest_stride, const lv_area_t *dest_area,
                            const void *src_buf, lv_coord_t src_stride, const lv_area_t *src_area, uint8_t unit_size);
/**********************
 *      MACROS
 **********************/


#endif /*LV_USE_GPU_RTK_PPEV2*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_PPE_UTILS_H*/
