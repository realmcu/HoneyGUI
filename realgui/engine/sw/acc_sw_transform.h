/**
 * @file acc_sw_transform.h
 * @author howie_wang@realsil.com.cn
 * @brief
 * @version 0.1
 * @date 2023-12-06
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __ACC_SW_TRANSFORM_H__
#define __ACC_SW_TRANSFORM_H__

#include <draw_img.h>
#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void sw_transform_for_rgb565(draw_img_t *image, gui_dispdev_t *dc,
                             gui_rect_t *rect);

void sw_transform_for_argb8565(draw_img_t *image, gui_dispdev_t *dc,
                               gui_rect_t *rect);

void sw_transform_for_argb8565_aa(draw_img_t *image, gui_dispdev_t *dc,
                                  gui_rect_t *rect);
#ifdef __cplusplus
}
#endif

#endif
