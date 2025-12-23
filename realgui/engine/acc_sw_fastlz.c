/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file acc_sw_fastlz.c
  * @brief FastLZ compressed image software decoding library
  * @details Supports FastLZ compressed images with various color formats
  * @author triton_yu
  * @date 2024/12/19
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */
/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <draw_img.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "acc_sw_fastlz.h"
#include "acc_sw_raster.h"
#include "def_file.h"
#include "../3rd/fastlz/fastlz.h"

/*============================================================================*
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                           Constants
 *============================================================================*/


/*============================================================================*
 *                            Macros
 *============================================================================*/


/*============================================================================*
 *                            Variables
 *============================================================================*/


/*============================================================================*
 *                           Private Functions
 *============================================================================*/

/**
 * @brief Get the number of bytes per pixel
 *
 * @param header Compression header (imdc_file_header_t)
 * @return uint8_t Actual number of bytes per pixel
 */
uint8_t fastlz_get_pixel_bytes(imdc_file_header_t *header)
{
    /* According to the documentation: pixel_bytes field value + 2 equals the actual number of bytes occupied */
    return header->algorithm_type.pixel_bytes + 2;
}

/**
 * @brief Check if the image is in FastLZ compressed format
 *
 * @param image Image object
 * @return true Is FastLZ compressed
 * @return false Is not FastLZ compressed
 */
bool is_fastlz_compressed(draw_img_t *image)
{
    gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)image->data;

    if (head->compress == 0)
    {
        return false;
    }

    /* Get compression header, using imdc_file_header_t structure */
    imdc_file_header_t *compress_head = (imdc_file_header_t *)((uint8_t *)image->data +
                                                               sizeof(gui_rgb_data_head_t));

    return (compress_head->algorithm_type.algorithm == COMPRESS_ALGO_FASTLZ);
}

/**
 * @brief FastLZ decompress and draw image
 *
 * @param image Image object
 * @param dc Display device
 * @param rect Clipping region
 */
void fastlz_blit(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect)
{
    gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)image->data;

    /*
     * FastLZ compressed image layout is similar to RLE:
     * gui_rgb_data_head_t (8 bytes)
     * imdc_file_header_t (12 bytes)
     * compressed_addr[height] array: offset of compressed data for each row
     * Compressed data
     *
     * The difference is that each row's data is compressed with FastLZ, not RLE encoded
     */

    /* Use imdc_file_t structure (contains header and compressed_addr) */
    imdc_file_t *imdc_file = (imdc_file_t *)((uint8_t *)image->data + sizeof(gui_rgb_data_head_t));
    imdc_file_header_t *compress_head = &imdc_file->header;

    /* Check if it's really FastLZ compressed */
    if (compress_head->algorithm_type.algorithm != COMPRESS_ALGO_FASTLZ)
    {
        gui_log("[FastLZ] Not FastLZ algorithm, algo=%d\n", compress_head->algorithm_type.algorithm);
        return;
    }      /* Calculate parameters */
    uint8_t pixel_bytes = compress_head->algorithm_type.pixel_bytes + 2;
    uint32_t raw_width = compress_head->raw_pic_width;
    uint32_t raw_height = compress_head->raw_pic_height;
    uint32_t line_size = raw_width * pixel_bytes;  /* Size of each decompressed row */
    uint32_t decompressed_size = raw_width * raw_height * pixel_bytes;

    /* Input validity check */
    if (raw_width == 0 || raw_height == 0 || decompressed_size == 0)
    {
        gui_log("[FastLZ] Invalid image dimensions: %dx%d, pixel_bytes=%d\n",
                raw_width, raw_height, pixel_bytes);
        return;
    }
    /* Allocate decompression buffer (including new color header) */
    size_t buf_size = sizeof(gui_rgb_data_head_t) + decompressed_size;
    uint8_t *decompressed_buf = (uint8_t *)gui_malloc(buf_size);
    if (decompressed_buf == NULL)
    {
        gui_log("[FastLZ] Failed to allocate decompression buffer: %d bytes\n", buf_size);
        return;
    }
    /* Copy color header to new buffer */
    memcpy(decompressed_buf, head, sizeof(gui_rgb_data_head_t));
    gui_rgb_data_head_t *new_head = (gui_rgb_data_head_t *)decompressed_buf;

    /* Mark as uncompressed format */
    new_head->compress = 0;
    /* Update width and height to original size */
    new_head->w = (short)raw_width;
    new_head->h = (short)raw_height;

    /* Starting position of decompressed pixel data */
    uint8_t *decompressed_pixels = decompressed_buf + sizeof(gui_rgb_data_head_t);

    /* Decompress FastLZ data row by row */
    bool decompress_ok = true;
    for (uint32_t line = 0; line < raw_height; line++)
    {
        /* Get the starting offset of compressed data for current row (relative to imdc_file start) */
        uint32_t line_start = imdc_file->compressed_addr[line];
        uint32_t line_end = imdc_file->compressed_addr[line + 1];
        uint32_t compressed_line_size = line_end - line_start;

        /* Actual position of compressed data */
        uint8_t *compressed_line_data = (uint8_t *)imdc_file + line_start;

        /* Decompress current row to target position */
        uint8_t *dest_line = decompressed_pixels + line * line_size;

        int result = fastlz_decompress(compressed_line_data, compressed_line_size,
                                       dest_line, line_size);

        if (result <= 0)
        {
            gui_log("[FastLZ] Line %d decompression failed, result=%d, compressed_size=%d\n",
                    line, result, compressed_line_size);
            decompress_ok = false;
            break;
        }
    }      if (!decompress_ok)
    {
        gui_free(decompressed_buf);
        return;
    }
    /* Save original data pointer */
    void *original_data = image->data;
    uint16_t original_w = image->img_w;
    uint16_t original_h = image->img_h;

    /* Use decompressed data */
    image->data = decompressed_buf;
    image->img_w = raw_width;
    image->img_h = raw_height;

    /* Call rasterization processing for uncompressed image */
    do_raster_no_rle(image, dc, rect);

    /* Restore original data pointer */
    image->data = original_data;
    image->img_w = original_w;
    image->img_h = original_h;

    gui_free(decompressed_buf);
}
