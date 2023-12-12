/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file acc_sw.c
  * @brief Picture software decoding library
  * @details input:rgba/rgb/rgb565;output:rgba/rgb565
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/19
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
#include "acc_sw_blend.h"
#include "acc_sw.h"
#include "acc_sw_rle.h"

void do_blending_argb8888_2_argb8888(gui_color_t *d, gui_color_t *s)
{
    //gui_log("[GUI] TODO line = %d", __LINE__);

    uint8_t Sa = s->channel.alpha;
    uint8_t Sr = s->channel.red;
    uint8_t Sg = s->channel.green;
    uint8_t Sb = s->channel.blue;

    uint8_t Da = d->channel.alpha;
    uint8_t Dr = d->channel.red;
    uint8_t Dg = d->channel.green;
    uint8_t Db = d->channel.blue;

    d->channel.alpha = ((255 - Sa) * Da + Sa * Sa) / 255;
    d->channel.red = ((255 - Sa) * Dr + Sa * Sr) / 255;
    d->channel.green = ((255 - Sa) * Dg + Sa * Sg) / 255;
    d->channel.blue = ((255 - Sa) * Db + Sa * Sb) / 255;
}


void do_blending_argb8888_2_rgb565(gui_color_t *target, gui_color_t *color)
{
    gui_log("TODO:%d", __LINE__);
    while (1);
}
void do_blending_rgb888_2_argb8888(gui_color_t *d, gui_color_t *s)
{
    uint8_t Sa = 255;
    uint8_t Sr = s->channel.red;
    uint8_t Sg = s->channel.green;
    uint8_t Sb = s->channel.blue;

    uint8_t Da = d->channel.alpha;
    uint8_t Dr = d->channel.red;
    uint8_t Dg = d->channel.green;
    uint8_t Db = d->channel.blue;

    d->channel.alpha = ((255 - Sa) * Da + Sa * Sa) / 255;
    d->channel.red = ((255 - Sa) * Dr + Sa * Sr) / 255;
    d->channel.green = ((255 - Sa) * Dg + Sa * Sg) / 255;
    d->channel.blue = ((255 - Sa) * Db + Sa * Sb) / 255;
}
void do_blending_rgb565_2_argb8888(gui_color_t *d, gui_color_t *s)
{
    gui_log("TODO:%d", __LINE__);
    while (1);
}
void do_blending_argb8888_2_argb8888_opacity(gui_color_t *d, gui_color_t *s, uint8_t opacity)
{
    //gui_log("[GUI] TODO line = %d", __LINE__);

    uint8_t Sa = s->channel.alpha * opacity / 255;
    uint8_t Sr = s->channel.red;
    uint8_t Sg = s->channel.green;
    uint8_t Sb = s->channel.blue;

    uint8_t Da = d->channel.alpha;
    uint8_t Dr = d->channel.red;
    uint8_t Dg = d->channel.green;
    uint8_t Db = d->channel.blue;

    d->channel.alpha = ((255 - Sa) * Da + Sa * Sa) / 255;
    d->channel.red = ((255 - Sa) * Dr + Sa * Sr) / 255;
    d->channel.green = ((255 - Sa) * Dg + Sa * Sg) / 255;
    d->channel.blue = ((255 - Sa) * Db + Sa * Sb) / 255;
}

void do_blending_argb8888_2_rgb565_opacity(gui_color_t *d, gui_color_t *s, uint8_t opacity)
{
    //gui_log("[GUI] TODO line = %d", __LINE__);

    uint8_t Sa = s->channel.alpha * opacity / 255;
    uint8_t Sr = s->channel.red;
    uint8_t Sg = s->channel.green;
    uint8_t Sb = s->channel.blue;

    //uint8_t Da = d->channel.alpha;
    uint8_t Dr = d->channel.red;
    uint8_t Dg = d->channel.green;
    uint8_t Db = d->channel.blue;

    //d->channel.alpha = ((255 - Sa) * Da + Sa * Sa) / 255;
    d->channel.red = ((255 - Sa) * Dr + Sa * Sr) / 255;
    d->channel.green = ((255 - Sa) * Dg + Sa * Sg) / 255;
    d->channel.blue = ((255 - Sa) * Db + Sa * Sb) / 255;
}
void do_blending_argb8888_2_rgb888_opacity(gui_color_t *d, gui_color_t *s, uint8_t opacity)
{
    //gui_log("[GUI] TODO line = %d", __LINE__);

    uint8_t Sa = s->channel.alpha * opacity / 255;
    uint8_t Sr = s->channel.red;
    uint8_t Sg = s->channel.green;
    uint8_t Sb = s->channel.blue;

    //uint8_t Da = d->channel.alpha;
    uint8_t Dr = d->channel.red;
    uint8_t Dg = d->channel.green;
    uint8_t Db = d->channel.blue;

    //d->channel.alpha = ((255 - Sa) * Da + Sa * Sa) / 255;
    d->channel.red = ((255 - Sa) * Dr + Sa * Sr) / 255;
    d->channel.green = ((255 - Sa) * Dg + Sa * Sg) / 255;
    d->channel.blue = ((255 - Sa) * Db + Sa * Sb) / 255;
}
void do_blending_rgb888_2_argb8888_opacity(gui_color_t *d, gui_color_t *s, uint8_t opacity)
{
    //gui_log("[GUI] TODO line = %d", __LINE__);

    uint8_t Sa = s->channel.alpha * opacity / 255;
    uint8_t Sr = s->channel.red;
    uint8_t Sg = s->channel.green;
    uint8_t Sb = s->channel.blue;

    uint8_t Da = d->channel.alpha;
    uint8_t Dr = d->channel.red;
    uint8_t Dg = d->channel.green;
    uint8_t Db = d->channel.blue;

    d->channel.alpha = ((255 - Sa) * Da + Sa * Sa) / 255;
    d->channel.red = ((255 - Sa) * Dr + Sa * Sr) / 255;
    d->channel.green = ((255 - Sa) * Dg + Sa * Sg) / 255;
    d->channel.blue = ((255 - Sa) * Db + Sa * Sb) / 255;
}
void do_blending_rgb565_2_argb8888_opacity(gui_color_t *d, gui_color_t *s, uint8_t opacity)
{
    //gui_log("[GUI] TODO line = %d", __LINE__);

    uint8_t Sa = s->channel.alpha * opacity / 255;
    uint8_t Sr = s->channel.red;
    uint8_t Sg = s->channel.green;
    uint8_t Sb = s->channel.blue;

    uint8_t Da = d->channel.alpha;
    uint8_t Dr = d->channel.red;
    uint8_t Dg = d->channel.green;
    uint8_t Db = d->channel.blue;

    d->channel.alpha = ((255 - Sa) * Da + Sa * Sa) / 255;
    d->channel.red = ((255 - Sa) * Dr + Sa * Sr) / 255;
    d->channel.green = ((255 - Sa) * Dg + Sa * Sg) / 255;
    d->channel.blue = ((255 - Sa) * Db + Sa * Sb) / 255;
}
void do_blending_rgb888_2_rgb565_opacity(gui_color_t *d, gui_color_t *s, uint8_t opacity)
{
    //gui_log("[GUI] TODO line = %d", __LINE__);

    uint8_t Sa = s->channel.alpha * opacity / 255;
    uint8_t Sr = s->channel.red;
    uint8_t Sg = s->channel.green;
    uint8_t Sb = s->channel.blue;

    //uint8_t Da = d->channel.alpha;
    uint8_t Dr = d->channel.red;
    uint8_t Dg = d->channel.green;
    uint8_t Db = d->channel.blue;

    //d->channel.alpha = ((255 - Sa) * Da + Sa * Sa) / 255;
    d->channel.red = ((255 - Sa) * Dr + Sa * Sr) / 255;
    d->channel.green = ((255 - Sa) * Dg + Sa * Sg) / 255;
    d->channel.blue = ((255 - Sa) * Db + Sa * Sb) / 255;
}
void alpha_blend_blit_argb8888_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                          struct rtgui_rect *rect)
{
    int image_x = rect->x1;
    int image_y = rect->y1;
    int image_w = image->img_w;
    int image_h = image->img_h;

    int x_start = _UI_MAX(image_x, 0);
    int x_end = _UI_MIN(image_x + image_w, dc->fb_width);
    int y_start = _UI_MAX(dc->section.y1, image_y);
    int y_end = _UI_MIN(dc->section.y2, image_y + image_h);

    if ((x_start >= x_end) || (y_start >= y_end))
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

        int read_off = ((i - image_y) * image_w) * source_bytes_per_pixel + read_x_off -
                       source_bytes_per_pixel * x_start;

        uint8_t *writebuf = dc->frame_buf;
        uint8_t *pixel;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            pixel = (uint8_t *)(read_off + j * source_bytes_per_pixel);
            gui_color_t color = {.channel.blue = pixel[0],
                                 .channel.green = pixel[1],
                                 .channel.red = pixel[2],
                                 .channel.alpha = pixel[3],
                                };
            uint8_t opacity_value = image->opacity_value;
            switch (opacity_value)
            {
            case 0:
                break;
            case 255:
                {
                    gui_color_t *d = (gui_color_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);
                    do_blending_argb8888_2_argb8888(d, &color);
                }
                break;
            default:
                {
                    gui_color_t *d = (gui_color_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);
                    do_blending_argb8888_2_argb8888(d, &color);
                }
                break;
            }
        }
    }
}
void alpha_blend_blit_rgb888_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                        struct rtgui_rect *rect)
{
    int image_x = rect->x1;
    int image_y = rect->y1;
    int image_w = image->img_w;
    int image_h = image->img_h;

    int x_start = _UI_MAX(image_x, 0);
    int x_end = _UI_MIN(image_x + image_w, dc->fb_width);
    int y_start = _UI_MAX(dc->section.y1, image_y);
    int y_end = _UI_MIN(dc->section.y2, image_y + image_h);

    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }

    uint8_t source_bytes_per_pixel = 3;
//    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);

    int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;

    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        int read_off = ((i - image_y) * image_w) * source_bytes_per_pixel + read_x_off -
                       source_bytes_per_pixel * x_start;

        uint8_t *writebuf = dc->frame_buf;
        uint8_t *pixel;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            pixel = (uint8_t *)(read_off + j * source_bytes_per_pixel);
            gui_color_t color = {.channel.blue = pixel[0],
                                 .channel.green = pixel[1],
                                 .channel.red = pixel[2],
                                 .channel.alpha = 255,
                                };
            do_blending_rgb888_2_argb8888((gui_color_t *)(writebuf + write_off + j), &color);
        }
    }
}
void alpha_blend_blit_rgb565_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                        struct rtgui_rect *rect)
{
    int image_x = rect->x1;
    int image_y = rect->y1;
    int image_w = image->img_w;
    int image_h = image->img_h;

    int x_start = _UI_MAX(image_x, 0);
    int x_end = _UI_MIN(image_x + image_w, dc->fb_width);
    int y_start = _UI_MAX(dc->section.y1, image_y);
    int y_end = _UI_MIN(dc->section.y2, image_y + image_h);

    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }

    uint8_t source_bytes_per_pixel = 2;
//    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);

    int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;
    uint8_t *writebuf = dc->frame_buf;
    uint16_t pixel;

    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        int read_off = ((i - image_y) * image_w) * source_bytes_per_pixel + read_x_off -
                       source_bytes_per_pixel * x_start;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            pixel = *((uint16_t *)read_off + j);
            gui_color_t color = {.channel.blue = (pixel & 0x001f) << 3,
                                 .channel.green = ((pixel & 0x07e0) >> 5) << 2,
                                 .channel.red = (pixel >> 11) << 3,
                                 .channel.alpha = 255,
                                };
            do_blending_rgb565_2_argb8888((gui_color_t *)(writebuf + write_off + j), &color);
        }
    }
}
void alpha_blend_blit_argb8888_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                        struct rtgui_rect *rect)
{
    int image_x = rect->x1;
    int image_y = rect->y1;
    int image_w = image->img_w;
    int image_h = image->img_h;

    int x_start = _UI_MAX(image_x, 0);
    int x_end = _UI_MIN(image_x + image_w, dc->fb_width);
    int y_start = _UI_MAX(dc->section.y1, image_y);
    int y_end = _UI_MIN(dc->section.y2, image_y + image_h);

    uint8_t source_bytes_per_pixel = 4;
//    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);

    int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;

    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        int read_off = ((i - image_y) * image_w) * source_bytes_per_pixel + read_x_off -
                       source_bytes_per_pixel * x_start;

        uint16_t *writebuf = (uint16_t *)dc->frame_buf;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            uint8_t *pixel = (uint8_t *)read_off;
            gui_color_t color = {.channel.alpha = pixel[3],
                                 .channel.blue = pixel[0],
                                 .channel.green = pixel[1],
                                 .channel.red = pixel[2],
                                };
            do_blending_argb8888_2_rgb565((gui_color_t *)(writebuf + write_off + j), &color);
        }
    }

}
