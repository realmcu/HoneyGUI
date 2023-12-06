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

void do_blending_argb8888_2_argb8888(gui_color_t *d, gui_color_t *s);

void do_blending_argb8888_2_rgb565(uint8_t *target, gui_color_t color);



#ifdef __cplusplus
}
#endif

#endif

