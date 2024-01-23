/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file acc_sw_filter_matrix.c
  * @brief deal with no rle filter mode and matrix transformation
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
#include "math.h"
// #include "acc_sw_blend.h"
// #include "acc_sw.h"
// #include "acc_sw_rle.h"

void cpu_filter_matrix_blit_rgb565_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                            gui_rect_t *rect)
{
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;
    int16_t source_h = image->img_h;
    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint16_t *writebuf = (uint16_t *)dc->frame_buf;

    struct gui_matrix *inverse = image->inverse;

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t opacity_value = image->opacity_value;
    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            float X = inverse->m[0][0] * j + inverse->m[0][1] * i + inverse->m[0][2];
            float Y = inverse->m[1][0] * j + inverse->m[1][1] * i + inverse->m[1][2];
            float Z = inverse->m[2][0] * j + inverse->m[2][1] * i + inverse->m[2][2];
            int x = X / Z;
            int y = Y / Z;

            if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h))
            {
                continue;
            }
            uint16_t pixel = *((uint16_t *)image_off + y * source_w + x);
            if (pixel != 0)
            {
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        writebuf[write_off + j] = pixel;
                    }
                    break;
                default:
                    {
                        if (opacity_value < 255)
                        {
                            writebuf[write_off + j] = ((((((pixel >> 11) << 3) * opacity_value + ((
                                                                                                      writebuf[write_off + j] >> 11) << 3) * (0xFF - opacity_value)) / 255) >> 3) << 11) +
                                                      ((((((((pixel & 0x07e0) >> 5) << 2) * opacity_value) + (((writebuf[write_off + j] & 0x07e0) >> 5) <<
                                                              2) * (0xFF - opacity_value)) / 0xFF) >> 2) << 5) +
                                                      ((((((pixel & 0x001f) << 3) * opacity_value) + ((writebuf[write_off + j]  & 0x001f) << 3) *
                                                         (0xFF - opacity_value)) / 0xFF) >> 3);
                        }
                    }
                    break;
                }
            }
        }
    }
    return;
}
void cpu_filter_matrix_blit_rgb888_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                            gui_rect_t *rect)
{
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;
    int16_t source_h = image->img_h;
    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint16_t *writebuf = (uint16_t *)dc->frame_buf;


    uint8_t source_bytes_per_pixel = 3;
    struct gui_matrix *inverse = image->inverse;
    uint8_t opacity_value = image->opacity_value;

    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        for (uint32_t j = x_start; j < x_end; j++)
        {

            float X = inverse->m[0][0] * j + inverse->m[0][1] * i + inverse->m[0][2];
            float Y = inverse->m[1][0] * j + inverse->m[1][1] * i + inverse->m[1][2];
            float Z = inverse->m[2][0] * j + inverse->m[2][1] * i + inverse->m[2][2];
            int x = X / Z;
            int y = Y / Z;

            if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h))
            {
                continue;
            }

            uint8_t *pixel = (uint8_t *)(image_off + (y * source_w + x) * source_bytes_per_pixel);
            if (pixel[0] != 0 || pixel[1] != 0 || pixel[2] != 0)
            {
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        writebuf[write_off + j] = (uint16_t)(((pixel[2]) >> 3) << 11) + (uint16_t)(((pixel[1]) >> 2) << 5)
                                                  + (uint16_t)(((pixel[0]) >> 3)); //RGB  565
                    }
                    break;
                default:
                    {
                        if (opacity_value < 255)
                        {
                            writebuf[write_off + j] = (((((uint16_t)(pixel[0]) * opacity_value) + ((writebuf[write_off + j]  &
                                                                                                    0x001f)
                                                                                                   << 3) * (0xFF - opacity_value)) / 0xFF) >> 3) +
                                                      ((((((uint16_t)(pixel[1]) * opacity_value) + (((writebuf[write_off + j] & 0x07e0) >> 5) << 2) *
                                                          (0xFF - opacity_value)) / 0xFF) >> 2) << 5) +
                                                      ((((((uint16_t)(pixel[2]) * (opacity_value)) + ((writebuf[write_off + j] >> 11) << 3) *
                                                          (0xFF - opacity_value)) / 0xFF) >> 3) << 11);
                        }
                    }
                    break;
                }
            }
        }
    }
    return;
}
void cpu_filter_matrix_blit_argb8888_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                              gui_rect_t *rect)
{
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;
    int16_t source_h = image->img_h;
    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint16_t *writebuf = (uint16_t *)dc->frame_buf;


    uint8_t source_bytes_per_pixel = 4;
    //uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    struct gui_matrix *inverse = image->inverse;
    uint8_t opacity_value = image->opacity_value;

    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        for (uint32_t j = x_start; j < x_end; j++)
        {

            float X = inverse->m[0][0] * j + inverse->m[0][1] * i + inverse->m[0][2];
            float Y = inverse->m[1][0] * j + inverse->m[1][1] * i + inverse->m[1][2];
            float Z = inverse->m[2][0] * j + inverse->m[2][1] * i + inverse->m[2][2];
            int x = X / Z;
            int y = Y / Z;

            if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h))
            {
                continue;
            }

            uint8_t *pixel = (uint8_t *)(image_off + (y * source_w + x) * source_bytes_per_pixel);

            if (pixel[0] != 0 || pixel[1] != 0 || pixel[2] != 0)
            {
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        writebuf[write_off + j] = (uint16_t)(((pixel[2]) >> 3) << 11) + (uint16_t)(((pixel[1]) >> 2) << 5)
                                                  + (uint16_t)(((pixel[0]) >> 3)); //RGB  565
                    }
                    break;
                default:
                    {
                        if (opacity_value < 255)
                        {
                            uint8_t alpha = pixel[3] * opacity_value / 0xFF;
                            writebuf[write_off + j] = (((((uint16_t)(pixel[0]) * alpha) + ((writebuf[write_off + j]  &
                                                                                            0x001f)
                                                                                           << 3) * (0xFF - alpha)) / 0xFF) >> 3) +
                                                      ((((((uint16_t)(pixel[1]) * alpha) + (((writebuf[write_off + j] & 0x07e0) >> 5) << 2) *
                                                          (0xFF - alpha)) / 0xFF) >> 2) << 5) +
                                                      ((((((uint16_t)(pixel[2]) * (alpha)) + ((writebuf[write_off + j] >> 11) << 3) *
                                                          (0xFF - alpha)) / 0xFF) >> 3) << 11);
                        }
                    }
                    break;
                }
            }
        }
    }
    return;
}

void cpu_filter_matrix_blit_rgb565_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                                            gui_rect_t *rect)
{
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;
    int16_t source_h = image->img_h;
    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t *writebuf = dc->frame_buf;


    uint8_t source_bytes_per_pixel = 2;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    struct gui_matrix *inverse = image->inverse;
    uint8_t opacity_value = image->opacity_value;
    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            float X = inverse->m[0][0] * j + inverse->m[0][1] * i + inverse->m[0][2];
            float Y = inverse->m[1][0] * j + inverse->m[1][1] * i + inverse->m[1][2];
            float Z = inverse->m[2][0] * j + inverse->m[2][1] * i + inverse->m[2][2];
            int x = X / Z;
            int y = Y / Z;

            if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h))
            {
                continue;
            }
            uint16_t pixel = *(uint16_t *)(image_off + (y * source_w + x) * source_bytes_per_pixel);

            if (pixel != 0)
            {
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = (pixel >> 11) << 3;
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = ((pixel & 0x07e0) >> 5) << 2;
                        writebuf[(write_off + j) * dc_bytes_per_pixel] = (pixel & 0x001f) << 3;
                    }
                    break;
                default:
                    {
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = (((pixel >> 11) << 3) * opacity_value
                                                                              + (writebuf[(write_off + j) * dc_bytes_per_pixel + 2] * (255 - opacity_value))) / 255 ;
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = ((((pixel & 0x07e0) >> 5) << 2) * opacity_value
                                                                              + (writebuf[(write_off + j) * dc_bytes_per_pixel + 1] * (255 - opacity_value))) / 255 ;
                        writebuf[(write_off + j) * dc_bytes_per_pixel] = (((pixel & 0x001f) << 3) * opacity_value
                                                                          + (writebuf[(write_off + j) * dc_bytes_per_pixel] * (255 - opacity_value))) / 255 ;
                    }
                    break;
                }
            }
        }
    }
    return;
}
void cpu_filter_matrix_blit_rgb888_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                                            gui_rect_t *rect)
{
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;
    int16_t source_h = image->img_h;
    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    struct gui_matrix *inverse = image->inverse;

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t *writebuf = dc->frame_buf;


    uint8_t source_bytes_per_pixel = 3;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    uint8_t opacity_value = image->opacity_value;

    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        for (uint32_t j = x_start; j < x_end; j++)
        {

            float X = inverse->m[0][0] * j + inverse->m[0][1] * i + inverse->m[0][2];
            float Y = inverse->m[1][0] * j + inverse->m[1][1] * i + inverse->m[1][2];
            float Z = inverse->m[2][0] * j + inverse->m[2][1] * i + inverse->m[2][2];
            int x = X / Z;
            int y = Y / Z;

            if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h))
            {
                continue;
            }

            uint8_t *pixel = (uint8_t *)(image_off + (y * source_w + x) * source_bytes_per_pixel);
            if (pixel[0] != 0 || pixel[1] != 0 || pixel[2] != 0)
            {
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        writebuf[(write_off + j) * dc_bytes_per_pixel] = pixel[0];//B
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = pixel[1];//G
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = pixel[2]; //R
                    }
                    break;
                default:
                    {
                        if (opacity_value < 255)
                        {
                            writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = (pixel[2] * (opacity_value)
                                                                                  + (writebuf[(write_off + j) * dc_bytes_per_pixel + 2] * (255 - opacity_value))) / 255 ;
                            writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = ((pixel[1]) * (opacity_value)
                                                                                  + (writebuf[(write_off + j) * dc_bytes_per_pixel + 1] * (255 - opacity_value))) / 255 ;
                            writebuf[(write_off + j) * dc_bytes_per_pixel] = ((pixel[0]) * (opacity_value)
                                                                              + writebuf[(write_off + j) * dc_bytes_per_pixel] * (255 - opacity_value)) / 255 ;
                        }
                    }
                    break;
                }

            }
        }
    }
}
void cpu_filter_matrix_blit_argb8888_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                                              gui_rect_t *rect)
{
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;
    int16_t source_h = image->img_h;
    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    struct gui_matrix *inverse = image->inverse;

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t *writebuf = dc->frame_buf;


    uint8_t source_bytes_per_pixel = 4;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    uint8_t opacity_value = image->opacity_value;

    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        for (uint32_t j = x_start; j < x_end; j++)
        {

            int x = inverse->m[0][0] * j + inverse->m[0][1] * i + inverse->m[0][2];
            int y = inverse->m[1][0] * j + inverse->m[1][1] * i + inverse->m[1][2];

            if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h))
            {
                continue;
            }

            uint8_t *pixel = (uint8_t *)(image_off + (y * source_w + x) * source_bytes_per_pixel);
            if (pixel[0] != 0 || pixel[1] != 0 || pixel[2] != 0)
            {
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        writebuf[(write_off + j) * dc_bytes_per_pixel] = pixel[0];//b
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = pixel[1];//g
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = pixel[2]; //r
                    }
                    break;
                default:
                    {
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = (pixel[2] * (opacity_value)
                                                                              + (writebuf[(write_off + j) * dc_bytes_per_pixel + 2] * (255 - opacity_value))) / 255 ;
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = ((pixel[1]) * (opacity_value)
                                                                              + (writebuf[(write_off + j) * dc_bytes_per_pixel + 1] * (255 - opacity_value))) / 255 ;
                        writebuf[(write_off + j) * dc_bytes_per_pixel] = ((pixel[0]) * (opacity_value)
                                                                          + writebuf[(write_off + j) * dc_bytes_per_pixel] * (255 - opacity_value)) / 255 ;
                    }
                    break;
                }

            }
        }
    }
}
void cpu_filter_matrix_blit_rgb565_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                              gui_rect_t *rect)
{
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;
    int16_t source_h = image->img_h;
    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint8_t *writebuf = dc->frame_buf;

    struct gui_matrix *inverse = image->inverse;

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    uint8_t opacity_value = image->opacity_value;

    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            float X = inverse->m[0][0] * j + inverse->m[0][1] * i + inverse->m[0][2];
            float Y = inverse->m[1][0] * j + inverse->m[1][1] * i + inverse->m[1][2];
            float Z = inverse->m[2][0] * j + inverse->m[2][1] * i + inverse->m[2][2];
            int x = X / Z;
            int y = Y / Z;

            if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h))
            {
                continue;
            }
            uint16_t pixel = *((uint16_t *)image_off + y * source_w + x);

            if (pixel  != 0)
            {
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 3] = 0xFF;
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = (pixel >> 11) << 3;
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = ((pixel & 0x07e0) >> 5) << 2;
                        writebuf[(write_off + j) * dc_bytes_per_pixel] = (pixel & 0x001f) << 3;
                    }
                    break;
                default:
                    {
                        if (opacity_value < 255)
                        {
                            writebuf[(write_off + j) * dc_bytes_per_pixel + 3] = (0xFF * opacity_value
                                                                                  + (writebuf[(write_off + j) * dc_bytes_per_pixel + 3] * (255 - opacity_value))) / 255 ;
                            writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = (((pixel >> 11) << 3) * opacity_value
                                                                                  + (writebuf[(write_off + j) * dc_bytes_per_pixel + 2] * (255 - opacity_value))) / 255 ;
                            writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = ((((pixel & 0x07e0) >> 5) << 2) * opacity_value
                                                                                  + (writebuf[(write_off + j) * dc_bytes_per_pixel + 1] * (255 - opacity_value))) / 255 ;
                            writebuf[(write_off + j) * dc_bytes_per_pixel] = (((pixel & 0x001f) << 3) * opacity_value
                                                                              + writebuf[(write_off + j) * dc_bytes_per_pixel] * (255 - opacity_value)) / 255 ;
                        }
                        else
                        {
                            break;
                        }
                    }
                    break;
                }

            }

        }

    }
}
void cpu_filter_matrix_blit_rgb888_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                              gui_rect_t *rect)
{
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;
    int16_t source_h = image->img_h;
    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    struct gui_matrix *inverse = image->inverse;

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t *writebuf = dc->frame_buf;


    uint8_t source_bytes_per_pixel = 3;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    uint8_t opacity_value = image->opacity_value;

    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        for (uint32_t j = x_start; j < x_end; j++)
        {

            float X = inverse->m[0][0] * j + inverse->m[0][1] * i + inverse->m[0][2];
            float Y = inverse->m[1][0] * j + inverse->m[1][1] * i + inverse->m[1][2];
            float Z = inverse->m[2][0] * j + inverse->m[2][1] * i + inverse->m[2][2];
            int x = X / Z;
            int y = Y / Z;

            if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h))
            {
                continue;
            }

            uint8_t *pixel = (uint8_t *)(image_off + (y * source_w + x) * source_bytes_per_pixel);
            if (pixel[0] != 0 || pixel[1] != 0 || pixel[2] != 0)
            {
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        writebuf[(write_off + j) * dc_bytes_per_pixel] = pixel[0];//b
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = pixel[1];//g
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = pixel[2]; //r
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 3] = 0xFF; //a
                    }
                    break;
                default:
                    {
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 3] = (0xFF * (opacity_value)
                                                                              + (writebuf[(write_off + j) * dc_bytes_per_pixel + 3] * (255 - opacity_value))) / 255 ;
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = (pixel[2] * (opacity_value)
                                                                              + (writebuf[(write_off + j) * dc_bytes_per_pixel + 2] * (255 - opacity_value))) / 255 ;
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = ((pixel[1]) * (opacity_value)
                                                                              + (writebuf[(write_off + j) * dc_bytes_per_pixel + 1] * (255 - opacity_value))) / 255 ;
                        writebuf[(write_off + j) * dc_bytes_per_pixel] = ((pixel[0]) * (opacity_value)
                                                                          + writebuf[(write_off + j) * dc_bytes_per_pixel] * (255 - opacity_value)) / 255 ;
                    }
                    break;
                }

            }
        }
    }
}
void cpu_filter_matrix_blit_argb8888_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                                gui_rect_t *rect)
{
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;
    int16_t source_h = image->img_h;
    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    struct gui_matrix *inverse = image->inverse;

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t *writebuf = dc->frame_buf;
    uint8_t source_bytes_per_pixel = 4;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    uint8_t opacity_value = image->opacity_value;

    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        for (uint32_t j = x_start; j < x_end; j++)
        {

            float X = inverse->m[0][0] * j + inverse->m[0][1] * i + inverse->m[0][2];
            float Y = inverse->m[1][0] * j + inverse->m[1][1] * i + inverse->m[1][2];
            float Z = inverse->m[2][0] * j + inverse->m[2][1] * i + inverse->m[2][2];
            int x = X / Z;
            int y = Y / Z;

            if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h))
            {
                continue;
            }

            uint8_t *pixel = (uint8_t *)(image_off + (y * source_w + x) * source_bytes_per_pixel);

            if (pixel[0] != 0 || pixel[1] != 0 || pixel[2] != 0)
            {
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        writebuf[(write_off + j) * dc_bytes_per_pixel] = pixel[0];//b
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = pixel[1];//g
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = pixel[2]; //r
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 3] = pixel[3]; //a
                    }
                    break;
                default:
                    {
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 3] = (pixel[3] * (opacity_value)
                                                                              + (writebuf[(write_off + j) * dc_bytes_per_pixel + 3] * (255 - opacity_value))) / 255 ;
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = (pixel[2] * (opacity_value)
                                                                              + (writebuf[(write_off + j) * dc_bytes_per_pixel + 2] * (255 - opacity_value))) / 255 ;
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = ((pixel[1]) * (opacity_value)
                                                                              + (writebuf[(write_off + j) * dc_bytes_per_pixel + 1] * (255 - opacity_value))) / 255 ;
                        writebuf[(write_off + j) * dc_bytes_per_pixel] = ((pixel[0]) * (opacity_value)
                                                                          + writebuf[(write_off + j) * dc_bytes_per_pixel] * (255 - opacity_value)) / 255 ;
                    }
                    break;
                }

            }
        }
    }
}
