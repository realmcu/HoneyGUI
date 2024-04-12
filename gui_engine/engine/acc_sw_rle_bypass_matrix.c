/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file acc_sw_rle_bypass_matrix.c
  * @brief Picture software decoding library:bypass matrix mode
  * @details input:rgba/rgb/rgb565;output:rgba/rgb565
  * @author wenjing_jiang@realsil.com.cn
  * @date 2023/12/25
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
#include "acc_engine.h"
#include "acc_sw_rle.h"
#include "math.h"

void rle_bypass_matrix_blit_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                     struct gui_rect *rect)
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
    uint16_t *writebuf = (uint16_t *)dc->frame_buf;
    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t img_type = ((struct gui_rgb_data_head *)(image->data))->type;

    int32_t y1, y2, z1, z2;
//        x1 = round(inverse->m[0][0] * dc->section.x1 + inverse->m[0][1] * dc->section.y1 +
//                   inverse->m[0][2]);
    y1 = round(inverse->m[1][0] * dc->section.x1 + inverse->m[1][1] * dc->section.y1 +
               inverse->m[1][2]);
    z1 = round(inverse->m[2][0] * dc->section.x1 + inverse->m[2][1] * dc->section.y1 +
               inverse->m[2][2]);

//        x2 = round(inverse->m[0][0] * dc->section.x2 + inverse->m[0][1] * (dc->section.y2 - 1) +
//                   inverse->m[0][2]);
    y2 = round(inverse->m[1][0] * dc->section.x2 + inverse->m[1][1] * (dc->section.y2 - 1) +
               inverse->m[1][2]);
    z2 = round(inverse->m[2][0] * dc->section.x2 + inverse->m[2][1] * (dc->section.y2 - 1) +
               inverse->m[2][2]);

    y1 = y1 / z1;
    y2 = y2 / z2;

    uint32_t start_line = 0, end_line = image->img_h - 1;
    if (y1 < 0)
    {
        start_line = 0;
    }
    else if (y1 >= image->img_h)
    {
        return;
    }
    else
    {
        start_line = y1;
    }

    if (y2 < 0)
    {
        return;
    }
    else if (y2 + 4 >= image->img_h)
    {
        end_line = image->img_h - 1;
    }
    else
    {
        end_line = y2 + 4;
    }
    //TODO: y2 + ceil(1 / scale_y_ratio)
    if (img_type == RGB565)//rle_bypass_matrix_rgb565_2_rgb565
    {
        uint8_t source_bytes_per_pixel = 2;
        uint8_t line_buf[source_bytes_per_pixel * source_w * (end_line - start_line + 1)];
        imdc_file_t *file = (imdc_file_t *)image_off;
        for (uint32_t y_i = y_start; y_i <= y_end; y_i++)
        {
            int write_off = (y_i - dc->section.y1) * dc->fb_width ;
            for (int k = start_line; k <= end_line; k++)
            {
                uncompressed_rle_rgb565(file, k, line_buf + (k - start_line) * source_w * source_bytes_per_pixel);
            }
            uint32_t read_off = (uint32_t)line_buf ;
            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                float X = image->inverse->m[0][0] * x_j + image->inverse->m[0][1] * y_i + image->inverse->m[0][2];
                float Y = image->inverse->m[1][0] * x_j + image->inverse->m[1][1] * y_i + image->inverse->m[1][2];
                float Z = image->inverse->m[2][0] * x_j + image->inverse->m[2][1] * y_i + image->inverse->m[2][2];
                int x_matrix = round(X / Z);
                int y_matrix = round(Y / Z);

                if ((x_matrix > source_w - 1) || (x_matrix < 0) || (y_matrix < 0) || (y_matrix > source_h - 1))
                {
                    continue;
                }
                uint16_t pixel = *((uint16_t *)(read_off + ((y_matrix - start_line) * source_w + x_matrix) *
                                                source_bytes_per_pixel));
                uint8_t opacity_value = image->opacity_value;
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        writebuf[write_off + x_j] = pixel;
                    }
                    break;
                default:
                    {
                        if (opacity_value < 255)
                        {
                            writebuf[write_off + x_j] = ((((((pixel >> 11) << 3) * opacity_value + ((
                                                                                                        writebuf[write_off + x_j] >> 11) << 3) * (0xFF - opacity_value)) / 255) >> 3) << 11) +
                                                        ((((((((pixel & 0x07e0) >> 5) << 2) * opacity_value) + (((writebuf[write_off + x_j] & 0x07e0) >> 5)
                                                                << 2) * (0xFF - opacity_value)) / 0xFF) >> 2) << 5) +
                                                        ((((((pixel & 0x001f) << 3) * opacity_value) + ((writebuf[write_off + x_j]  & 0x001f) << 3) *
                                                           (0xFF - opacity_value)) / 0xFF) >> 3);
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
    else if (img_type == RGB888) //rle_bypass_matrix_rgb888_2_rgb565
    {
        uint8_t source_bytes_per_pixel = 3;

        uint8_t line_buf[source_bytes_per_pixel * source_w * (end_line - start_line + 1)];
        imdc_file_t *file = (imdc_file_t *)image_off;
        for (uint32_t y_i = y_start; y_i <= y_end; y_i++)
        {
            int write_off = (y_i - dc->section.y1) * dc->fb_width ;
//            int line = y_i - image_y;
            for (int k = start_line; k <= end_line; k++)
            {
                uncompressed_rle_rgb888(file, k, line_buf + (k - start_line) * source_w * source_bytes_per_pixel);
            }
            uint32_t read_off = (uint32_t)line_buf ;
            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                float X = image->inverse->m[0][0] * x_j + image->inverse->m[0][1] * y_i + image->inverse->m[0][2];
                float Y = image->inverse->m[1][0] * x_j + image->inverse->m[1][1] * y_i + image->inverse->m[1][2];
                float Z = image->inverse->m[2][0] * x_j + image->inverse->m[2][1] * y_i + image->inverse->m[2][2];
                int x_matrix = round(X / Z);
                int y_matrix = round(Y / Z);

                if ((x_matrix > source_w - 1) || (x_matrix < 0) || (y_matrix < 0) || (y_matrix > source_h - 1))
                {
                    continue;
                }
                uint8_t *pixel = (uint8_t *)(read_off + ((y_matrix - start_line) * source_w + x_matrix) *
                                             source_bytes_per_pixel);
                uint8_t opacity_value = image->opacity_value;
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        writebuf[write_off + x_j] = (uint16_t)(((pixel[2]) >> 3) << 11) + (uint16_t)(((pixel[1]) >> 2) << 5)
                                                    + (uint16_t)(((pixel[0]) >> 3)); //RGB  565
                    }
                    break;
                default:
                    {
                        if (opacity_value < 255)
                        {
                            writebuf[write_off + x_j] = (((((uint16_t)(pixel[0]) * opacity_value) + ((
                                                                                                         writebuf[write_off + x_j]  & 0x001f) << 3) * (0xFF - opacity_value)) / 0xFF) >> 3) +
                                                        ((((((uint16_t)(pixel[1]) * opacity_value) + (((writebuf[write_off + x_j] & 0x07e0) >> 5) << 2) *
                                                            (0xFF - opacity_value)) / 0xFF) >> 2) << 5) +
                                                        ((((((uint16_t)(pixel[2]) * (opacity_value)) + ((writebuf[write_off + x_j] >> 11) << 3) *
                                                            (0xFF - opacity_value)) / 0xFF) >> 3) << 11);
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
    else if (img_type == RGBA8888) //rle_bypass_matrix_argb8888_2_rgb565
    {
        uint8_t source_bytes_per_pixel = 4;

        uint8_t line_buf[source_bytes_per_pixel * source_w * (end_line - start_line + 1)];
        imdc_file_t *file = (imdc_file_t *)image_off;
        for (uint32_t y_i = y_start; y_i <= y_end; y_i++)
        {
            int write_off = (y_i - dc->section.y1) * dc->fb_width ;
//            int line = y_i - image_y;
            for (int k = start_line; k <= end_line; k++)
            {
                uncompressed_rle_argb8888(file, k, line_buf + (k - start_line) * source_w * source_bytes_per_pixel);
            }
            uint32_t read_off = (uint32_t)line_buf ;
            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                float X = image->inverse->m[0][0] * x_j + image->inverse->m[0][1] * y_i + image->inverse->m[0][2];
                float Y = image->inverse->m[1][0] * x_j + image->inverse->m[1][1] * y_i + image->inverse->m[1][2];
                float Z = image->inverse->m[2][0] * x_j + image->inverse->m[2][1] * y_i + image->inverse->m[2][2];
                int x_matrix = round(X / Z);
                int y_matrix = round(Y / Z);

                if ((x_matrix > source_w - 1) || (x_matrix < 0) || (y_matrix < 0) || (y_matrix > source_h - 1))
                {
                    continue;
                }
                uint8_t *pixel = (uint8_t *)(read_off + ((y_matrix - start_line) * source_w + x_matrix) *
                                             source_bytes_per_pixel);
                uint8_t opacity_value = image->opacity_value;
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        writebuf[write_off + x_j] = (uint16_t)(((pixel[2]) >> 3) << 11) + (uint16_t)(((pixel[1]) >> 2) << 5)
                                                    + (uint16_t)(((pixel[0]) >> 3)); //RGB  565
                    }
                    break;
                default:
                    {
                        if (opacity_value < 255)
                        {
                            uint8_t alpha = pixel[3] * opacity_value / 0xFF;
                            writebuf[write_off + x_j] = (((((uint16_t)(pixel[0]) * alpha) + ((writebuf[write_off + x_j]  &
                                                                                              0x001f)
                                                                                             << 3) * (0xFF - alpha)) / 0xFF) >> 3) +
                                                        ((((((uint16_t)(pixel[1]) * alpha) + (((writebuf[write_off + x_j] & 0x07e0) >> 5) << 2) *
                                                            (0xFF - alpha)) / 0xFF) >> 2) << 5) +
                                                        ((((((uint16_t)(pixel[2]) * (alpha)) + ((writebuf[write_off + x_j] >> 11) << 3) *
                                                            (0xFF - alpha)) / 0xFF) >> 3) << 11);
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
    return;
}
void rle_bypass_matrix_blit_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                       struct gui_rect *rect)
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
    uint8_t img_type = ((struct gui_rgb_data_head *)(image->data))->type;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    uint8_t opacity_value = image->opacity_value;
    int32_t  y1, y2, z1, z2;
//        x1 = round(inverse->m[0][0] * dc->section.x1 + inverse->m[0][1] * dc->section.y1 +
//                   inverse->m[0][2]);
    y1 = round(inverse->m[1][0] * dc->section.x1 + inverse->m[1][1] * dc->section.y1 +
               inverse->m[1][2]);
    z1 = round(inverse->m[2][0] * dc->section.x1 + inverse->m[2][1] * dc->section.y1 +
               inverse->m[2][2]);

//        x2 = round(inverse->m[0][0] * dc->section.x2 + inverse->m[0][1] * (dc->section.y2 - 1) +
//                   inverse->m[0][2]);
    y2 = round(inverse->m[1][0] * dc->section.x2 + inverse->m[1][1] * (dc->section.y2 - 1) +
               inverse->m[1][2]);
    z2 = round(inverse->m[2][0] * dc->section.x2 + inverse->m[2][1] * (dc->section.y2 - 1) +
               inverse->m[2][2]);

    y1 = y1 / z1;
    y2 = y2 / z2;

    uint32_t start_line = 0, end_line = image->img_h - 1;
    if (y1 < 0)
    {
        start_line = 0;
    }
    else if (y1 >= image->img_h)
    {
        return;
    }
    else
    {
        start_line = y1;
    }

    if (y2 < 0)
    {
        return;
    }
    else if (y2 + 4 >= image->img_h)
    {
        end_line = image->img_h - 1;
    }
    else
    {
        end_line = y2 + 4;
    }
    //TODO: y2 + ceil(1 / scale_y_ratio)
    if (img_type == RGBA8888)//rle_bypass_matrix_rgba8888_2_argb8888
    {
        uint8_t source_bytes_per_pixel = 4;
        uint8_t line_buf[source_bytes_per_pixel * source_w * (end_line - start_line + 1)];
        imdc_file_t *file = (imdc_file_t *)image_off;
        for (uint32_t y_i = y_start; y_i <= y_end; y_i++)
        {
            int write_off = (y_i - dc->section.y1) * dc->fb_width ;
//            int line = y_i - image_y;
            for (int k = start_line; k <= end_line; k++)
            {
                uncompressed_rle_argb8888(file, k, line_buf + (k - start_line) * source_w * source_bytes_per_pixel);
            }
            uint32_t read_off = (uint32_t)line_buf ;
            uint8_t *writebuf = dc->frame_buf;
            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                float X = image->inverse->m[0][0] * x_j + image->inverse->m[0][1] * y_i + image->inverse->m[0][2];
                float Y = image->inverse->m[1][0] * x_j + image->inverse->m[1][1] * y_i + image->inverse->m[1][2];
                float Z = image->inverse->m[2][0] * x_j + image->inverse->m[2][1] * y_i + image->inverse->m[2][2];
                int x_matrix = round(X / Z);
                int y_matrix = round(Y / Z);

                if ((x_matrix > source_w - 1) || (x_matrix < 0) || (y_matrix < 0) || (y_matrix > source_h - 1))
                {
                    continue;
                }
                uint8_t *pixel = (uint8_t *)(read_off + ((y_matrix - start_line) * source_w + x_matrix) *
                                             source_bytes_per_pixel);

                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel] = pixel[0];//b
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] = pixel[1];//g
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] = pixel[2]; //r
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel + 3] = pixel[3]; //a
                    }
                    break;
                default:
                    {
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel + 3] = (pixel[3] * opacity_value
                                                                                + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 3] * (255 - opacity_value))) / 255 ;
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] = (pixel[2] * opacity_value
                                                                                + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] * (255 - opacity_value))) / 255 ;
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] = ((pixel[1]) * opacity_value
                                                                                + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] * (255 - opacity_value))) / 255 ;
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel] = ((pixel[0]) * opacity_value
                                                                            + writebuf[(write_off + x_j) * dc_bytes_per_pixel] * (255 - opacity_value)) / 255 ;
                    }
                    break;
                }
            }
        }
    }
    else if (img_type == RGB888) //rle_bypass_matrix_rgb888_2_argb8888
    {

        uint8_t source_bytes_per_pixel = 3;
        uint8_t line_buf[source_bytes_per_pixel * source_w * (end_line - start_line + 1)];
        imdc_file_t *file = (imdc_file_t *)image_off;
        for (uint32_t y_i = y_start; y_i <= y_end; y_i++)
        {
            int write_off = (y_i - dc->section.y1) * dc->fb_width ;
//            int line = y_i - image_y;
            for (int k = start_line; k <= end_line; k++)
            {
                uncompressed_rle_rgb888(file, k, line_buf + (k - start_line) * source_w * source_bytes_per_pixel);
            }
            uint32_t read_off = (uint32_t)line_buf ;
            uint8_t *writebuf = dc->frame_buf;
            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                float X = image->inverse->m[0][0] * x_j + image->inverse->m[0][1] * y_i + image->inverse->m[0][2];
                float Y = image->inverse->m[1][0] * x_j + image->inverse->m[1][1] * y_i + image->inverse->m[1][2];
                float Z = image->inverse->m[2][0] * x_j + image->inverse->m[2][1] * y_i + image->inverse->m[2][2];
                int x_matrix = round(X / Z);
                int y_matrix = round(Y / Z);

                if ((x_matrix > source_w - 1) || (x_matrix < 0) || (y_matrix < 0) || (y_matrix > source_h - 1))
                {
                    continue;
                }
                uint8_t *pixel = (uint8_t *)(read_off + ((y_matrix - start_line) * source_w + x_matrix) *
                                             source_bytes_per_pixel);
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel] = pixel[0];//b
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] = pixel[1];//g
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] = pixel[2]; //r
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel + 3] = 0xFF; //a
                    }
                    break;
                default:
                    {
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel + 3] = (0xFF * opacity_value
                                                                                + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 3] * (255 - opacity_value))) / 255 ;
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] = (pixel[2] * opacity_value
                                                                                + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] * (255 - opacity_value))) / 255 ;
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] = ((pixel[1]) * opacity_value
                                                                                + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] * (255 - opacity_value))) / 255 ;
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel] = ((pixel[0]) * opacity_value
                                                                            + writebuf[(write_off + x_j) * dc_bytes_per_pixel] * (255 - opacity_value)) / 255 ;
                    }
                    break;
                }
            }
        }

    }
    else if (img_type == RGB565) //rle_bypass_matrix_rgb565_2_argb8888
    {
        uint8_t source_bytes_per_pixel = 2;
        uint8_t line_buf[source_bytes_per_pixel * source_w * (end_line - start_line + 1)];
        imdc_file_t *file = (imdc_file_t *)image_off;
        for (uint32_t y_i = y_start; y_i <= y_end; y_i++)
        {
            int write_off = (y_i - dc->section.y1) * dc->fb_width ;

            for (int k = start_line; k <= end_line; k++)
            {
                uncompressed_rle_rgb565(file, k, line_buf + (k - start_line) * source_w * source_bytes_per_pixel);
            }
            uint32_t read_off = (uint32_t)line_buf ;
            uint8_t *writebuf = dc->frame_buf;
            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                float X = image->inverse->m[0][0] * x_j + image->inverse->m[0][1] * y_i + image->inverse->m[0][2];
                float Y = image->inverse->m[1][0] * x_j + image->inverse->m[1][1] * y_i + image->inverse->m[1][2];
                float Z = image->inverse->m[2][0] * x_j + image->inverse->m[2][1] * y_i + image->inverse->m[2][2];
                int x_matrix = round(X / Z);
                int y_matrix = round(Y / Z);

                if ((x_matrix > source_w - 1) || (x_matrix < 0) || (y_matrix < 0) || (y_matrix > source_h - 1))
                {
                    continue;
                }
                uint16_t pixel = *(uint16_t *)(read_off + ((y_matrix - start_line) * source_w + x_matrix) *
                                               source_bytes_per_pixel);
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] = (pixel >> 11) << 3;
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] = ((pixel & 0x07e0) >> 5) << 2;
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel] = (pixel & 0x001f) << 3;
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel + 3] = 0xFF; //a
                    }
                    break;
                default:
                    {
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel + 3] = (0xFF * opacity_value
                                                                                + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 3] * (255 - opacity_value))) / 255 ;
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] = (((pixel >> 11) << 3) *
                                                                                opacity_value
                                                                                + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] * (255 - opacity_value))) / 255 ;
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] = ((((pixel & 0x07e0) >> 5) << 2) *
                                                                                opacity_value
                                                                                + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] * (255 - opacity_value))) / 255 ;
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel] = (((pixel & 0x001f) << 3) *
                                                                            opacity_value
                                                                            + writebuf[(write_off + x_j) * dc_bytes_per_pixel] * (255 - opacity_value)) / 255 ;
                    }
                    break;
                }
            }
        }

    }
    return;
}
