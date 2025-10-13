/**
 * @file acc_sw_blend.h
 * @author howie_wang (howie_wang@realtek.com.cn)
 * @brief
 * @version 0.1
 * @date 2023-12-06
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __ACC_SW_BLEND_H__
#define __ACC_SW_BLEND_H__

#include <guidef.h>
#include <draw_img.h>
#include <draw_path.h>



#ifdef __cplusplus
extern "C" {
#endif

void src_over_blit_2_rgb565(draw_img_t *image, gui_dispdev_t *dc,
                            gui_rect_t *rect);////S * Sa + (1 - Sa) * D

void preconfig_a8_fg(draw_img_t *image, gui_dispdev_t *dc,
                     gui_rect_t *rect);////fixed fg

void preconfig_a8_fgbg(draw_img_t *image, gui_dispdev_t *dc,
                       gui_rect_t *rect);////fixed fg and bg

#ifdef __cplusplus
}
#endif

#endif
