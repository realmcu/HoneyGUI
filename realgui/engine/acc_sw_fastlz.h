/**
 * @file acc_sw_fastlz.h
 * @author triton_yu
 * @brief FastLZ compressed image decoding library
 * @version 0.1
 * @date 2024-12-19
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __ACC_SW_FASTLZ_H__
#define __ACC_SW_FASTLZ_H__

#include <guidef.h>
#include <draw_img.h>
#include <draw_path.h>
#include "def_file.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Compression algorithm type definitions */
#define COMPRESS_ALGO_RLE               0
#define COMPRESS_ALGO_FASTLZ            1
#define COMPRESS_ALGO_YUV_SAMPLE_FASTLZ 2
#define COMPRESS_ALGO_YUV_SAMPLE        3

/**
 * @brief FastLZ decompress and draw image
 *
 * @param image Image object
 * @param dc Display device
 * @param rect Clipping region
 */
void fastlz_blit(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect);

/**
 * @brief Check if the image is in FastLZ compressed format
 *
 * @param image Image object
 * @return true Is FastLZ compressed
 * @return false Is not FastLZ compressed
 */
bool is_fastlz_compressed(draw_img_t *image);

/**
 * @brief Get the number of bytes per pixel
 *
 * @param header Compression header (imdc_file_header_t)
 * @return uint8_t Actual number of bytes per pixel
 */
uint8_t fastlz_get_pixel_bytes(imdc_file_header_t *header);

#ifdef __cplusplus
}
#endif

#endif /* __ACC_SW_FASTLZ_H__ */
