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

typedef enum
{
    GUI_SW_IDU_RLE = 0x00,                         /*!< The compressed image uses Run Length Encode algorithm. */
    GUI_SW_IDU_FASTLZ = 0x01,                      /*!< The compressed image uses FastLZ compression algorithm. */
    GUI_SW_IDU_YUV_SAMPLE_BLUR_FASTLZ = 0x02,      /*!< The image is sampled in the YUV color space with a 0-3 bit blur, and then compressed using the FastLZ compression algorithm. */
    GUI_SW_IDU_YUV_SAMPLE_BLUR = 0x03,             /*!< The image is sampled in the YUV color space with a 0-3 bit blur. */
} GUI_SW_GUI_ALGORITHM;

typedef enum
{
    GUI_SW_IDU_RUN_LENGTH_SIZE_0BYTE = 0x00,       /*!< The input data is not RLE compressed. */
    GUI_SW_IDU_RUN_LENGTH_SIZE_1BYTE = 0x01,       /*!< Run length is 1 byte. */
    GUI_SW_IDU_RUN_LENGTH_SIZE_2BYTE = 0x02,       /*!< Run length is 2 bytes. */
} GUI_SW_IDU_RLE_RUNLENGTH_SIZE;

typedef enum
{
    GUI_SW_IDU_YUV444_SAMPLE = 0x00,       /*!< YUV444 sampling, data storing format: [Y0 U0 V0 Y1 U1 V1]. */
    GUI_SW_IDU_YUV422_SAMPLE = 0x01,       /*!< YUV422 sampling, data storing format: [Y0 U0 Y1 V1]. */
    GUI_SW_IDU_YUV411_SAMPLE = 0x02,       /*!< YUV411 sampling, data storing format: [Y0 U0 Y1 Y2 V2 Y3]. */
} GUI_SW_IDU_YUV_SAMPLE_TYPE;

typedef enum
{
    GUI_SW_IDU_YUV_BLUR_0BIT = 0x00,       /*!< The compressed image keeps all data after YUV sampling. */
    GUI_SW_IDU_YUV_BLUR_1BIT = 0x01,       /*!< The compressed image throws away 1 least significant bit of each byte after YUV sampling. */
    GUI_SW_IDU_YUV_BLUR_2BIT = 0x02,       /*!< The compressed image throws away 2 least significant bits of each byte after YUV sampling. */
    GUI_SW_IDU_YUV_BLUR_4BIT = 0x03,       /*!< The compressed image throws away 4 least significant bits of each byte after YUV sampling. */
} GUI_SW_IDU_YUV_BLUR_BIT;

typedef struct
{
    struct
    {
        uint8_t algorithm: 2;       /*!< Compression algorithm of input file,
                                        this parameter can be a value of @ref GUI_SW_IDU_ALGORITHM. */
uint8_t feature_1:
        2;       /*!< For RLE: Run length size, value of @ref GUI_SW_IDU_RLE_RUNLENGTH_SIZE.
                                        For YUV: Sample type, value of @ref GUI_SW_IDU_YUV_SAMPLE_TYPE. */
uint8_t feature_2:
        2;       /*!< For RLE: Run length size, value of @ref GUI_SW_IDU_RLE_RUNLENGTH_SIZE.
                                        For YUV: Blur bit, value of @ref GUI_SW_IDU_YUV_BLUR_BIT. */
        uint8_t pixel_bytes: 2;     /*!< Size of pixel, can be 1-4 bytes. */
    } algorithm_type;               /*!< Compressing information. */
    uint8_t reserved[3];            /*!< Reserved for future use. */
    uint32_t raw_pic_width;         /*!< Width of compressed picture in pixel. */
    uint32_t raw_pic_height;        /*!< Height of compressed picture in pixel. */
} gui_sw_idu_head_t;


typedef struct
{
    uint32_t start_line;            /*!< Start line of target decode range. */
    uint32_t end_line;              /*!< End line of target decode range. */
    uint32_t start_column;          /*!< Start column of target decode range. */
    uint32_t end_column;            /*!< End column of target decode range. */
    uint32_t target_stride;         /*!< Data length between 2 contiguous target lines.*/
} gui_sw_idu_range_t;

void gui_sw_idu_decode(void *in, gui_rect_t *rect, void *out, uint32_t stride);

// Wrapper functions for acc_engine integration
void *gui_sw_idu_load(void *input);
void gui_sw_idu_free(void *decoded);
void gui_sw_idu_register(struct acc_engine *acc);

#ifdef __cplusplus
}
#endif

#endif
