/**
 * @file acc_sw_filter_matrix.h
 * @author wenjing_jiang (wenjing_jiang@realtek.com.cn)
 * @brief
 * @version 0.1
 * @date 2023-12-23
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __ACC_SW_FILTER_MATRIX_H__
#define __ACC_SW_FILTER_MATRIX_H__

#include <draw_img.h>
#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void filter_matrix_blit_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                 gui_rect_t *rect);
void filter_matrix_blit_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                                 gui_rect_t *rect);
void filter_matrix_blit_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                   gui_rect_t *rect);

#ifdef __cplusplus
}
#endif

#endif
