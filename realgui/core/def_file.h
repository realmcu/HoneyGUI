/**
 * @file def_file.h
 * @author wenjing_jiang(wenjing_jiang@realsil.com.cn)
 * @brief
 * @version 0.1
 * @date 2024-06-14
 *
 * @copyright Copyright (c) 2024
 *
 */
/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __DEF_FILE_H__
#define __DEF_FILE_H__



#ifdef __cplusplus
extern "C" {
#endif
/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
/*============================================================================*
 *                         Types
 *============================================================================*/

typedef struct gui_rgb_data_head
{
    unsigned char scan : 1;
    unsigned char align : 1;
    unsigned char resize: 2; //0-no resize;1-50%(x&y);2-70%;3-80%
    unsigned char compress: 1;
    unsigned char rsvd : 3;
    char type;
    short w;
    short h;
    char version;
    char rsvd2;
} gui_rgb_data_head_t;

typedef enum
{
    IMDC_SRC_1BIT = 0x00,
    IMDC_SRC_2BIT = 0x01,
    IMDC_SRC_4BIT = 0x02,
    IMDC_SRC_8BIT = 0x03,
    IMDC_SRC_16BIT = 0x04,
    IMDC_SRC_24BIT = 0x05,
    IMDC_SRC_32BIT = 0x06,
} imdc_src_type;

typedef struct imdc_file_header
{
    struct
    {
        uint8_t algorithm: 2;
        uint8_t feature_1: 2;
        uint8_t feature_2: 2;
        uint8_t pixel_bytes: 2;
    } algorithm_type;
    uint8_t reserved[3];
    uint32_t raw_pic_width;
    uint32_t raw_pic_height;
} imdc_file_header_t;

typedef struct imdc_file
{
    imdc_file_header_t header;
    uint32_t compressed_addr[1024];

} imdc_file_t;

typedef struct gui_img_file
{
    gui_rgb_data_head_t img_header;
    union
    {
        imdc_file_t imdc_file;
        uint32_t unzip_data[1024];
    } data;

} gui_img_file_t;

#pragma pack(1)

typedef struct imdc_rgb565_node
{
    uint8_t len;
    uint16_t pixel16;  //rgb565
} imdc_rgb565_node_t;
typedef struct imdc_argb8565_node
{
    uint8_t len;
    uint16_t pixel;  //argb8565
    uint8_t alpha;
} imdc_argb8565_node_t;
typedef struct imdc_rgb888_node
{
    uint8_t len;
    uint8_t pixel_b;  //rgb888
    uint8_t pixel_g;
    uint8_t pixel_r;
} imdc_rgb888_node_t;
typedef struct imdc_argb8888_node
{
    uint8_t len;
    uint32_t pixel32;    //argb8888
} imdc_argb8888_node_t;
#pragma pack()


#ifdef __cplusplus
}
#endif

#endif
