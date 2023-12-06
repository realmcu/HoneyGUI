/**
 * @file acc_sw_rle.h
 * @author howie_wang (howie_wang@realtek.com.cn)
 * @brief
 * @version 0.1
 * @date 2023-12-06
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __ACC_SW_RLE_H__
#define __ACC_SW_RLE_H__

#include <guidef.h>
#include <draw_img.h>
#include <draw_path.h>



#ifdef __cplusplus
extern "C" {
#endif

void rle_blit_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                       struct rtgui_rect *rect);

void rle_blit_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                         struct rtgui_rect *rect);

void rle_matrix_blit_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                              struct rtgui_rect *rect);

void rle_matrix_blit_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                struct rtgui_rect *rect);



#ifdef __cplusplus
}
#endif

#endif

