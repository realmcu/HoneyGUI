/**
 * @file acc_sw.h
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

#ifdef __cplusplus
extern "C" {
#endif

void no_rle(draw_img_t *image, struct gui_dispdev *dc,
            rtgui_rect_t *rect);

#ifdef __cplusplus
}
#endif

#endif

