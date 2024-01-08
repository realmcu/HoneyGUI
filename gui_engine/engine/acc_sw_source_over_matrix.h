/**
 * @file acc_sw_source_over_matrix.h
 * @author howie_wang (howie_wang@realtek.com.cn)
 * @brief
 * @version 0.1
 * @date 2023-12-26
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __ACC_SW_BLEND_MATRIX_H__
#define __ACC_SW_BLEND_MATRIX_H__

#include <guidef.h>
#include <draw_img.h>
#include <draw_path.h>



#ifdef __cplusplus
extern "C" {
#endif


void alpha_matrix_blit_argb8888_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                           gui_rect_t *rect);
void alpha_matrix_blit_rgb888_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                         gui_rect_t *rect);
void alpha_matrix_blit_rgb565_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                         gui_rect_t *rect);
void alpha_matrix_blit_rgb565_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                       gui_rect_t *rect);
void alpha_matrix_blit_argb8565_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                         gui_rect_t *rect);
void alpha_matrix_blit_rgb888_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                       gui_rect_t *rect);
void alpha_matrix_blit_rgba8888_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                         gui_rect_t *rect);
void alpha_matrix_blit_rgb565_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                                       gui_rect_t *rect);
void alpha_matrix_blit_rgb888_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                                       gui_rect_t *rect);
void alpha_matrix_blit_rgba8888_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                                         gui_rect_t *rect);
#ifdef __cplusplus
}
#endif

#endif
