/**
 * @file acc_sw_bypass_matrix.h
 * @author wenjing_jiang (wenjing_jiang@realtek.com.cn)
 * @brief
 * @version 0.1
 * @date 2023-12-06
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __ACC_SW_BYPASS_MATRIX_H__
#define __ACC_SW_BYPASS_MATRIX_H__

#include <draw_img.h>
#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void bypass_matrix_blit_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                 gui_rect_t *rect);
void bypass_matrix_blit_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                                 gui_rect_t *rect);
void bypass_matrix_blit_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                   gui_rect_t *rect);

#ifdef __cplusplus
}
#endif

#endif
