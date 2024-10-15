/**
 * @file lv_draw_rtk_ppe_blend.h
 *
 */


#ifndef LV_DRAW_PPE_BLEND_H
#define LV_DRAW_PPE_BLEND_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

#if LV_USE_GPU_RTK_PPE
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

lv_res_t lv_ppe_fill(const lv_area_t *dest_area, lv_draw_ctx_t *draw_ctx,
                     const lv_draw_sw_blend_dsc_t *dsc);

lv_res_t lv_ppe_blit(lv_color_t *dest_buf, lv_area_t *dest_area, lv_coord_t dest_stride,
                     const lv_color_t *src_buf, lv_area_t *src_area, lv_coord_t src_stride,
                     lv_opa_t opa);

lv_res_t lv_ppe_blit_transform(lv_draw_ctx_t *draw_ctx, const lv_draw_img_dsc_t *dsc,
                               lv_res_t lv_ppe_blit_transform(lv_draw_ctx_t *draw_ctx, const lv_draw_img_dsc_t *dsc,
                                                              lv_point_t *base,
                                                              const lv_area_t *coords, const uint8_t *map_p, lv_img_cf_t cf);

                               lv_res_t lv_ppe_alpha_only(const lv_img_dsc_t *img, lv_draw_ctx_t *draw_ctx,
                                                          const lv_draw_img_dsc_t *dsc, const lv_area_t *coords);

                               lv_res_t lv_ppe_blit_recolor(lv_draw_ctx_t *draw_ctx, const lv_draw_img_dsc_t *dsc,
                                                            const lv_area_t *coords, const uint8_t *map_p, lv_img_cf_t cf);

                               lv_res_t lv_ppe_mask(lv_draw_ctx_t *draw_ctx, const lv_draw_sw_blend_dsc_t *dsc);
                               /**********************
                                *      MACROS
                                **********************/

#endif /*LV_USE_GPU_RTK_PPE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_PPE_BLEND_H*/
