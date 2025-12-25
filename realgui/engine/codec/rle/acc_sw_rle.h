/**
 * @file acc_sw_rle.h
 * @author wenjing_jiang@realsil.com.cn
 * @brief
 * @version 0.1
 * @date 2023-12-06
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __ACC_SW_RLE_H__
#define __ACC_SW_RLE_H__

#include "guidef.h"
#include "draw_img.h"
#include "gui_api_color.h"



#ifdef __cplusplus
extern "C" {
#endif


void blit_compressed(draw_img_t *image, gui_dispdev_t *dc,
                     gui_rect_t *rect);

void uncompressed_rle_rgb565(imdc_file_t *file, uint32_t line,  uint8_t *buf);
void uncompressed_rle_argb8565(imdc_file_t *file, uint32_t line,  uint8_t *buf);
void uncompressed_rle_rgb888(imdc_file_t *file, uint32_t line,  uint8_t *buf);
void uncompressed_rle_argb8888(imdc_file_t *file, uint32_t line,  uint8_t *buf);

void uncompressed_rle_line(imdc_file_t *file, uint32_t line, int16_t x, int16_t w, uint8_t *buf);
void uncompressed_rle_rect(imdc_file_t *file, int16_t x, int16_t y, int16_t w, int16_t h,
                           uint8_t *buf);

#ifdef __cplusplus
}
#endif

#endif
