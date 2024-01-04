/**
 * @file acc_sw_rle_filter_matrix.h
 * @author wenjing_jiang@realsil.com.cn
 * @brief
 * @version 0.1
 * @date 2023-12-25
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __ACC_SW_RLE_FILTER_MATRIX_H__
#define __ACC_SW_RLE_FILTER_MATRIX_H__

#include <guidef.h>
#include <draw_img.h>
#include <draw_path.h>


#ifdef __cplusplus
extern "C" {
#endif

void rle_filter_matrix_blit_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                     struct gui_rect *rect);
void rle_filter_matrix_blit_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                                     struct gui_rect *rect);
void rle_filter_matrix_blit_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                       struct gui_rect *rect);

#ifdef __cplusplus
}
#endif

#endif
