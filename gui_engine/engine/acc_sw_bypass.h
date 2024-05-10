/**
 * @file acc_sw_bypass.h
 * @author wenjing_jiang (wenjing_jiang@realtek.com.cn)
 * @brief
 * @version 0.1
 * @date 2023-12-06
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __ACC_SW_BYPASS_H__
#define __ACC_SW_BYPASS_H__

#include <draw_img.h>
#include <stdio.h>
#include <stdint.h>
#include "acc_engine.h"
// #include "acc_sw_rle.h"
// #include "acc_sw_blend.h"

#ifdef __cplusplus
extern "C" {
#endif

void bypass_blit_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                          gui_rect_t *rect);

#ifdef __cplusplus
}
#endif

#endif
