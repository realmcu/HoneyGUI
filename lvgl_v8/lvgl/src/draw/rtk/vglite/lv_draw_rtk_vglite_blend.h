/**
 * @file lv_draw_vglite_blend.h
 *
 */

#ifndef LV_DRAW_VGLITE_BLEND_H
#define LV_DRAW_VGLITE_BLEND_H

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

lv_res_t lv_gpu_vglite_fill(const lv_area_t *dest_area, lv_color_t color, lv_opa_t opa);

lv_res_t lv_gpu_vglite_blit(lv_color_t *dest_buf, lv_area_t *dest_area, lv_coord_t dest_stride,
                            const lv_color_t *src_buf, lv_area_t *src_area, lv_coord_t src_stride,
                            lv_opa_t opa);

lv_res_t lv_gpu_vglite_blit_transform(lv_color_t *dest_buf, lv_area_t *dest_area,
                                      lv_coord_t dest_stride,
                                      const lv_color_t *src_buf, lv_area_t *src_area, lv_coord_t src_stride,
                                      const lv_draw_img_dsc_t *dsc);

lv_res_t lv_gpu_vglite_index(const lv_img_dsc_t *img, lv_area_t *dest_area,
                             const lv_draw_img_dsc_t *dsc);
lv_res_t lv_gpu_vglite_alpha_only(const lv_img_dsc_t *img, lv_area_t *dest_area,
                                  const lv_draw_img_dsc_t *dsc);
lv_res_t lv_gpu_vglite_mask(const lv_draw_sw_blend_dsc_t *dsc);
/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GPU_VG_LITE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_VGLITE_BLEND_H*/
