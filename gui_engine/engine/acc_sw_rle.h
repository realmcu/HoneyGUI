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

#include <guidef.h>
#include <draw_img.h>
#include <draw_path.h>



#ifdef __cplusplus
extern "C" {
#endif

typedef struct rtzip_file_header
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
} rtzip_file_header_t;

typedef struct rtzip_file
{
    rtzip_file_header_t header;
    uint32_t compressed_addr[1024];

} rtzip_file_t;
#pragma pack(1)

typedef struct rtzip_rgb565_node
{
    uint8_t len;
    uint16_t pixel16;  //rgb565
} rtzip_rgb565_node_t;
typedef struct rtzip_rgb888_node
{
    uint8_t len;
    uint8_t pixel_b;  //rgb888
    uint8_t pixel_g;
    uint8_t pixel_r;
} rtzip_rgb888_node_t;
typedef struct rtzip_argb8888_node
{
    uint8_t len;
    uint32_t pixel32;    //argb8888
} rtzip_argb8888_node_t;
#pragma pack()

void rle_bypass_blit_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                              struct rtgui_rect *rect);
void rle_bypass_blit_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                              struct rtgui_rect *rect);
void rle_bypass_blit_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                struct rtgui_rect *rect);
void rle_bypass_matrix_blit_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                     struct rtgui_rect *rect);
void rle_bypass_matrix_blit_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                                     struct rtgui_rect *rect);
void rle_bypass_matrix_blit_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                       struct rtgui_rect *rect);
void rle_filter_blit_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                              struct rtgui_rect *rect);
void rle_filter_blit_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                              struct rtgui_rect *rect);
void rle_filter_blit_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                struct rtgui_rect *rect);

void rle_filter_matrix_blit_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                     struct rtgui_rect *rect);
void rle_filter_matrix_blit_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                                     struct rtgui_rect *rect);
void rle_filter_matrix_blit_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                       struct rtgui_rect *rect);
void rle_alpha_blend_blit_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                     struct rtgui_rect *rect);
void rle_alpha_matrix_blit_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                      struct rtgui_rect *rect);
void rle(draw_img_t *image, struct gui_dispdev *dc,
         struct rtgui_rect *rect);

static void gui_memset16(uint16_t *addr, uint16_t pixel, uint32_t len);
static void gui_memset32(uint32_t *addr, uint32_t pixel, uint32_t len);
static void uncompressed_rle_rgb565(rtzip_file_t *file, uint32_t line,  uint8_t *buf);
void uncompressed_rle_rgb888(rtzip_file_t *file, uint32_t line,  uint8_t *buf);
void uncompressed_rle_argb8888(rtzip_file_t *file, uint32_t line,  uint8_t *buf);


#ifdef __cplusplus
}
#endif

#endif

