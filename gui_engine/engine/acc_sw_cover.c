/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file acc_sw_cover.c
  * @brief deal with no rle direct cover mode
  * @details input:rgba/rgb/rgb565;output:rgba/rgb565
  * @author wenjing_jiang@realsil.com.cn
  * @date 2023/12/23
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
// #include "acc_sw_blend.h"
// #include "acc_sw.h"
// #include "acc_sw_rle.h"
void cover_blit_rgb565_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                gui_rect_t *rect)
{
    int image_x = 0;
    int image_y = 0;
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;
//    int16_t source_h = image->img_h;
    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint8_t source_bytes_per_pixel = 2;

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);

    int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;
    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        int read_off = ((i - image_y) * source_w) * source_bytes_per_pixel + read_x_off -
                       source_bytes_per_pixel * x_start;

        uint16_t *writebuf = (uint16_t *)dc->frame_buf;

        for (uint32_t j = x_start; j < x_end; j++)
        {

            uint16_t pixel = (*((uint16_t *)read_off + j));
            writebuf[write_off + j] = pixel;
        }
    }
}
void cover_blit_rgb888_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                gui_rect_t *rect)
{
    int image_x = 0;
    int image_y = 0;
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;
//    int16_t source_h = image->img_h;
    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t source_bytes_per_pixel = 3;
    int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;
    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        int read_off = ((i - image_y) * source_w) * source_bytes_per_pixel + read_x_off -
                       source_bytes_per_pixel * x_start;

        uint16_t *writebuf = (uint16_t *)dc->frame_buf;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            uint8_t *pixel = (uint8_t *)(read_off + j * source_bytes_per_pixel);

            writebuf[write_off + j] = (uint16_t)(((pixel[2]) >> 3) << 11) + (uint16_t)(((pixel[1]) >> 2) << 5)
                                      + (uint16_t)(((pixel[0]) >> 3)); //RGB  565

        }
    }
    return;
}
void cover_blit_argb8888_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                  gui_rect_t *rect)
{
    int image_x = 0;
    int image_y = 0;
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;
//    int16_t source_h = image->img_h;
    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t source_bytes_per_pixel = 4;
    int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;

    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        int read_off = ((i - image_y) * source_w) * source_bytes_per_pixel + read_x_off -
                       source_bytes_per_pixel * x_start;

        uint16_t *writebuf = (uint16_t *)dc->frame_buf;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            uint8_t *pixel = (uint8_t *)(read_off + j * source_bytes_per_pixel);

            writebuf[write_off + j] = (uint16_t)(((pixel[2]) >> 3) << 11) + (uint16_t)(((pixel[1]) >> 2) << 5)
                                      + (uint16_t)(((pixel[0]) >> 3)); //RGB  565
        }
    }

}
void cover_blit_rgb565_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                                gui_rect_t *rect)
{
    int image_x = 0;
    int image_y = 0;
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;
//    int16_t source_h = image->img_h;
    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint8_t source_bytes_per_pixel = 2;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);

    int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;

    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        int read_off = ((i - image_y) * source_w) * source_bytes_per_pixel + read_x_off -
                       source_bytes_per_pixel * x_start;

        uint8_t *writebuf = dc->frame_buf;
        uint16_t pixel;
        for (uint32_t j = x_start; j < x_end; j++)
        {
            pixel = *((uint16_t *)read_off + j);
            writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = (pixel >> 11) << 3;
            writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = ((pixel & 0x07e0) >> 5) << 2;
            writebuf[(write_off + j) * dc_bytes_per_pixel] = (pixel & 0x001f) << 3;
        }
    }
    return;
}
void cover_blit_rgb888_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                                gui_rect_t *rect)
{
    int image_x = 0;
    int image_y = 0;
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;
//    int16_t source_h = image->img_h;
    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint8_t source_bytes_per_pixel = 3;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);

    int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;
    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        int read_off = ((i - image_y) * source_w) * source_bytes_per_pixel + read_x_off -
                       source_bytes_per_pixel * x_start;

        uint8_t *writebuf = dc->frame_buf;
        uint8_t *pixel;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            pixel = (uint8_t *)(read_off + j * source_bytes_per_pixel);
            writebuf[(write_off + j) * dc_bytes_per_pixel] = pixel[0];//b
            writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = pixel[1];//g
            writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = pixel[2]; //r

        }
    }
    return;
}
void cover_blit_argb8888_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                                  gui_rect_t *rect)
{
    int image_x = 0;
    int image_y = 0;
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;
//    int16_t source_h = image->img_h;
    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint8_t source_bytes_per_pixel = 4;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);

    int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;
    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        int read_off = ((i - image_y) * source_w) * source_bytes_per_pixel + read_x_off -
                       source_bytes_per_pixel * x_start;

        uint8_t *writebuf = dc->frame_buf;
        uint8_t *pixel;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            pixel = (uint8_t *)(read_off + j * source_bytes_per_pixel);
            writebuf[(write_off + j) * dc_bytes_per_pixel] = pixel[0];//b
            writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = pixel[1];//g
            writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = pixel[2]; //r

        }
    }
    return;
}
void cover_blit_rgb565_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                  gui_rect_t *rect)
{
    int image_x = 0;
    int image_y = 0;
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;
//    int16_t source_h = image->img_h;
    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint8_t source_bytes_per_pixel = 2;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);

    int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;
    uint8_t *writebuf = dc->frame_buf;
    uint16_t pixel;
    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        int read_off = ((i - image_y) * source_w) * source_bytes_per_pixel + read_x_off -
                       source_bytes_per_pixel * x_start;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            pixel = *((uint16_t *)read_off + j);
            writebuf[(write_off + j) * dc_bytes_per_pixel + 3] = 0xFF;
            writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = (pixel >> 11) << 3;
            writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = ((pixel & 0x07e0) >> 5) << 2;
            writebuf[(write_off + j) * dc_bytes_per_pixel] = (pixel & 0x001f) << 3;

        }
    }

}
void cover_blit_rgb888_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                  gui_rect_t *rect)
{
    int image_x = 0;
    int image_y = 0;
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;
//    int16_t source_h = image->img_h;
    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint8_t source_bytes_per_pixel = 3;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);

    int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;
    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        int read_off = ((i - image_y) * source_w) * source_bytes_per_pixel + read_x_off -
                       source_bytes_per_pixel * x_start;

        uint8_t *writebuf = dc->frame_buf;
        uint8_t *pixel;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            pixel = (uint8_t *)(read_off + j * source_bytes_per_pixel);
            writebuf[(write_off + j) * dc_bytes_per_pixel] = pixel[0];//B
            writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = pixel[1];//G
            writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = pixel[2]; //R
            writebuf[(write_off + j) * dc_bytes_per_pixel + 3] = 0xFF; //A
        }
    }
}
void cover_blit_argb8888_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                    gui_rect_t *rect)
{
    int image_x = 0;
    int image_y = 0;
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;
//    int16_t source_h = image->img_h;
    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint8_t source_bytes_per_pixel = 4;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);

    int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;
    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        int read_off = ((i - image_y) * source_w) * source_bytes_per_pixel + read_x_off -
                       source_bytes_per_pixel * x_start;

        uint8_t *writebuf = dc->frame_buf;
        uint8_t *pixel;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            pixel = (uint8_t *)(read_off + j * source_bytes_per_pixel);
            writebuf[(write_off + j) * dc_bytes_per_pixel    ] = pixel[0]; //B
            writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = pixel[1]; //G
            writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = pixel[2]; //R
            writebuf[(write_off + j) * dc_bytes_per_pixel + 3] = pixel[3]; //A
        }
    }
    return;
}
