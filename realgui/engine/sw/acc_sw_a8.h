/**
 * @file acc_sw_a8.h
 * @author howie_wang (howie_wang@realtek.com.cn)
 * @brief
 * @version 0.1
 * @date 2025-10-20
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __ACC_SW_A8_H__
#define __ACC_SW_A8_H__

#include <draw_img.h>
#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void a8_2_rgb565(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect);

#ifdef __cplusplus
}
#endif

#endif
