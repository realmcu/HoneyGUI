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
#ifndef __ACC_SW_H__
#define __ACC_SW_H__

#include <draw_img.h>
#include <stdio.h>
#include <stdint.h>
#include "acc_engine.h"
#include "acc_sw_rle.h"
#include "acc_sw_blend.h"

#ifdef __cplusplus
extern "C" {
#endif

void filter_blit_rgb565_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                 struct rtgui_rect *rect);
void filter_blit_rgb888_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                 struct rtgui_rect *rect);
void filter_blit_rgb565_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                                 struct rtgui_rect *rect);
void filter_blit_rgb888_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                                 struct rtgui_rect *rect);
void filter_blit_argb8888_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                                   struct rtgui_rect *rect);

void filter_blit_rgb888_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                   struct rtgui_rect *rect);
void filter_blit_rgba8888_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                     struct rtgui_rect *rect);
void filter_blit_rgba8888_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                   struct rtgui_rect *rect);
void filter_blit_rgb565_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                   struct rtgui_rect *rect);
void bypass_blit_rgb565_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                 struct rtgui_rect *rect);
void bypass_blit_rgb565_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                   struct rtgui_rect *rect);
void bypass_blit_rgb888_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                 struct rtgui_rect *rect);
void bypass_blit_rgb565_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                                 struct rtgui_rect *rect);
void bypass_blit_rgb888_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                   struct rtgui_rect *rect);
void bypass_blit_argb8888_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                   struct rtgui_rect *rect);
void bypass_blit_argb8888_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                                   struct rtgui_rect *rect);
void bypass_blit_argb8888_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                     struct rtgui_rect *rect);
void bypass_blit_rgb888_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                                 struct rtgui_rect *rect);
void bypass_matrix_blit_rgb565_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                        struct rtgui_rect *rect);
void bypass_matrix_blit_rgb565_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                          struct rtgui_rect *rect);
void bypass_matrix_blit_rgb888_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                        struct rtgui_rect *rect);
void bypass_matrix_blit_rgb565_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                                        struct rtgui_rect *rect);
void bypass_matrix_blit_rgb888_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                          struct rtgui_rect *rect);
void bypass_matrix_blit_argb8888_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                          struct rtgui_rect *rect);
void bypass_matrix_blit_argb8888_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                                          struct rtgui_rect *rect);
void bypass_matrix_blit_argb8888_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                            struct rtgui_rect *rect);
void bypass_matrix_blit_rgb888_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                                        struct rtgui_rect *rect);
void cpu_filter_matrix_blit_argb8888_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                                struct rtgui_rect *rect);
void cpu_filter_matrix_blit_rgb888_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                              struct rtgui_rect *rect);
void cpu_filter_matrix_blit_rgb565_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                              struct rtgui_rect *rect);
void cpu_filter_matrix_blit_rgb565_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                            struct rtgui_rect *rect);
void cpu_filter_matrix_blit_rgb888_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                            struct rtgui_rect *rect);
void cpu_filter_matrix_blit_argb8888_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                              struct rtgui_rect *rect);
void cpu_filter_matrix_blit_rgb565_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                                            struct rtgui_rect *rect);
void cpu_filter_matrix_blit_rgb888_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                                            struct rtgui_rect *rect);
void cpu_filter_matrix_blit_argb8888_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                                              struct rtgui_rect *rect);
void no_rle(draw_img_t *image, struct gui_dispdev *dc,
            struct rtgui_rect *rect);

#ifdef __cplusplus
}
#endif

#endif

