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

uint16_t do_blending_acc_2_rgb565_opacity(uint32_t fg, uint32_t bg, uint8_t alpha);
void  do_blending_argb8565_2_rgb565_opacity(uint16_t *d, gui_color_t *s, uint8_t alpha);

void do_blending_2_rgb565(uint16_t *d, gui_color_t *s);
void do_blending_2_rgb565_opacity(uint16_t *d, gui_color_t *s, uint8_t opacity);
void do_blending_2_rgb888(gui_color_t *d, gui_color_t *s);
void do_blending_2_rgb888_opacity(gui_color_t *d, gui_color_t *s, uint8_t opacity);
void do_blending_2_argb8888(gui_color_t *d, gui_color_t *s);
void do_blending_2_argb8888_opacity(gui_color_t *d, gui_color_t *s, uint8_t opacity);


#ifdef __cplusplus
}
#endif

#endif
