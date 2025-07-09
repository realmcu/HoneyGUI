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

void sw_acc_blit(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect);

#ifdef __cplusplus
}
#endif

#endif

