/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file acc_sw_blend.c
  * @brief Picture software decoding library blend
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

uint16_t do_blending_rgb565_2_rgb565_opacity(uint32_t fg, uint32_t bg, uint8_t alpha)
{
    // Alpha converted from [0..255] to [0..31]
    // Converts  0000000000000000rrrrrggggggbbbbb
    //     into  00000gggggg00000rrrrr000000bbbbb
    // with mask 00000111111000001111100000011111
    // This is useful because it makes space for a parallel fixed-point multiply
    alpha = (alpha + 4) >> 3;
    bg = ((bg | (bg << 16)) & 0x07C5F81F);
    fg = ((fg | (fg << 16)) & 0x07C5F81F);
    uint32_t result = (((((fg - bg) * alpha) >> 5) + bg) & 0x07C5F81F);
    return (uint16_t)((result >> 16) | result);
}
void  do_blending_argb8565_2_rgb565_opacity(uint16_t *d, gui_color_t *s, uint8_t alpha)
{
    uint8_t Sa = alpha;
    uint16_t Sr = (uint16_t)s->color.rgba.r;
    uint16_t Sg = (uint16_t)s->color.rgba.g;
    uint16_t Sb = (uint16_t)s->color.rgba.b;

    uint16_t Dr = ((*d >> 11) << 3);
    uint16_t Dg = (((*d & 0x07e0) >> 5) << 2);
    uint16_t Db = ((*d & 0x001f) << 3);

    uint16_t red = (((((255 - Sa) * Dr + Sa * Sr) / 255) >> 3) << 11);
    uint16_t green = (((((255 - Sa) * Dg + Sa * Sg) / 255) >> 2) << 5);
    uint16_t blue = ((((255 - Sa) * Db + Sa * Sb) / 255) >> 3);

    *d = red + green + blue ;
}

void do_blending_rgb888_2_rgb565(uint16_t *d, gui_color_t *s)
{
    uint8_t Sa = 255;
    uint16_t Sr = (uint16_t)s->color.rgba.r;
    uint16_t Sg = (uint16_t)s->color.rgba.g;
    uint16_t Sb = (uint16_t)s->color.rgba.b;

    uint16_t Dr = ((*d >> 11) << 3);
    uint16_t Dg = (((*d & 0x07e0) >> 5) << 2);
    uint16_t Db = ((*d & 0x001f) << 3);

    uint16_t red = (((((255 - Sa) * Dr + Sa * Sr) / 255) >> 3) << 11);
    uint16_t green = (((((255 - Sa) * Dg + Sa * Sg) / 255) >> 2) << 5);
    uint16_t blue = ((((255 - Sa) * Db + Sa * Sb) / 255) >> 3);

    *d = red + green + blue ;
}
void do_blending_rgb888_2_rgb565_opacity(uint16_t *d, gui_color_t *s, uint8_t opacity)
{
    uint8_t Sa = opacity;
    uint16_t Sr = (uint16_t)s->color.rgba.r;
    uint16_t Sg = (uint16_t)s->color.rgba.g;
    uint16_t Sb = (uint16_t)s->color.rgba.b;

    uint16_t Dr = ((*d >> 11) << 3);
    uint16_t Dg = (((*d & 0x07e0) >> 5) << 2);
    uint16_t Db = ((*d & 0x001f) << 3);

    uint16_t red = (((((255 - Sa) * Dr + Sa * Sr) / 255) >> 3) << 11);
    uint16_t green = (((((255 - Sa) * Dg + Sa * Sg) / 255) >> 2) << 5);
    uint16_t blue = ((((255 - Sa) * Db + Sa * Sb) / 255) >> 3);

    *d = red + green + blue ;
}
void do_blending_argb8888_2_rgb565(uint16_t *d, gui_color_t *s)
{
    uint8_t Sa = s->color.rgba.a;
    uint16_t Sr = (uint16_t)s->color.rgba.r;
    uint16_t Sg = (uint16_t)s->color.rgba.g;
    uint16_t Sb = (uint16_t)s->color.rgba.b;

    uint16_t Dr = ((*d >> 11) << 3);
    uint16_t Dg = (((*d & 0x07e0) >> 5) << 2);
    uint16_t Db = ((*d & 0x001f) << 3);

    uint16_t red = (((((255 - Sa) * Dr + Sa * Sr) / 255) >> 3) << 11);
    uint16_t green = (((((255 - Sa) * Dg + Sa * Sg) / 255) >> 2) << 5);
    uint16_t blue = ((((255 - Sa) * Db + Sa * Sb) / 255) >> 3);

    *d = red + green + blue ;
}
void do_blending_argb8888_2_rgb565_opacity(uint16_t *d, gui_color_t *s, uint8_t opacity)
{
    uint8_t Sa = (s->color.rgba.a * opacity) / 255;
    uint16_t Sr = (uint16_t)s->color.rgba.r;
    uint16_t Sg = (uint16_t)s->color.rgba.g;
    uint16_t Sb = (uint16_t)s->color.rgba.b;

    uint16_t Dr = ((*d >> 11) << 3);
    uint16_t Dg = (((*d & 0x07e0) >> 5) << 2);
    uint16_t Db = ((*d & 0x001f) << 3);

    uint16_t red = (((((255 - Sa) * Dr + Sa * Sr) / 255) >> 3) << 11);
    uint16_t green = (((((255 - Sa) * Dg + Sa * Sg) / 255) >> 2) << 5);
    uint16_t blue = ((((255 - Sa) * Db + Sa * Sb) / 255) >> 3);

    *d = red + green + blue ;
}
void do_blending_rgb565_2_rgb888_opacity(gui_color_t *d, gui_color_t *s, uint8_t opacity)
{
    uint8_t Sa = s->color.rgba.a * opacity / 255;
    uint8_t Sr = s->color.rgba.r;
    uint8_t Sg = s->color.rgba.g;
    uint8_t Sb = s->color.rgba.b;

    uint8_t Da = 255;
    uint8_t Dr = d->color.rgba.r;
    uint8_t Dg = d->color.rgba.g;
    uint8_t Db = d->color.rgba.b;

    d->color.rgba.a = ((255 - Sa) * Da + Sa * Sa) / 255;
    d->color.rgba.r = ((255 - Sa) * Dr + Sa * Sr) / 255;
    d->color.rgba.g = ((255 - Sa) * Dg + Sa * Sg) / 255;
    d->color.rgba.b = ((255 - Sa) * Db + Sa * Sb) / 255;
}
void do_blending_rgb565_2_rgb888(gui_color_t *d, gui_color_t *s)
{
    uint8_t Sa = s->color.rgba.a;
    uint8_t Sr = s->color.rgba.r;
    uint8_t Sg = s->color.rgba.g;
    uint8_t Sb = s->color.rgba.b;

    uint8_t Da = 255;
    uint8_t Dr = d->color.rgba.r;
    uint8_t Dg = d->color.rgba.g;
    uint8_t Db = d->color.rgba.b;

    d->color.rgba.a = ((255 - Sa) * Da + Sa * Sa) / 255;
    d->color.rgba.r = ((255 - Sa) * Dr + Sa * Sr) / 255;
    d->color.rgba.g = ((255 - Sa) * Dg + Sa * Sg) / 255;
    d->color.rgba.b = ((255 - Sa) * Db + Sa * Sb) / 255;
}
void do_blending_rgb888_2_rgb888_opacity(gui_color_t *d, gui_color_t *s, uint8_t opacity)
{
    uint8_t Sa = s->color.rgba.a * opacity / 255;
    uint8_t Sr = s->color.rgba.r;
    uint8_t Sg = s->color.rgba.g;
    uint8_t Sb = s->color.rgba.b;

    uint8_t Da = 255;
    uint8_t Dr = d->color.rgba.r;
    uint8_t Dg = d->color.rgba.g;
    uint8_t Db = d->color.rgba.b;

    d->color.rgba.a = ((255 - Sa) * Da + Sa * Sa) / 255;
    d->color.rgba.r = ((255 - Sa) * Dr + Sa * Sr) / 255;
    d->color.rgba.g = ((255 - Sa) * Dg + Sa * Sg) / 255;
    d->color.rgba.b = ((255 - Sa) * Db + Sa * Sb) / 255;
}
void do_blending_rgb888_2_rgb888(gui_color_t *d, gui_color_t *s)
{
    uint8_t Sa = s->color.rgba.a;
    uint8_t Sr = s->color.rgba.r;
    uint8_t Sg = s->color.rgba.g;
    uint8_t Sb = s->color.rgba.b;

    uint8_t Da = 255;
    uint8_t Dr = d->color.rgba.r;
    uint8_t Dg = d->color.rgba.g;
    uint8_t Db = d->color.rgba.b;

    d->color.rgba.a = ((255 - Sa) * Da + Sa * Sa) / 255;
    d->color.rgba.r = ((255 - Sa) * Dr + Sa * Sr) / 255;
    d->color.rgba.g = ((255 - Sa) * Dg + Sa * Sg) / 255;
    d->color.rgba.b = ((255 - Sa) * Db + Sa * Sb) / 255;
}
void do_blending_argb8888_2_rgb888(gui_color_t *d, gui_color_t *s)
{
    uint8_t Sa = s->color.rgba.a;
    uint8_t Sr = s->color.rgba.r;
    uint8_t Sg = s->color.rgba.g;
    uint8_t Sb = s->color.rgba.b;

    uint8_t Da = 255;
    uint8_t Dr = d->color.rgba.r;
    uint8_t Dg = d->color.rgba.g;
    uint8_t Db = d->color.rgba.b;

    d->color.rgba.a = ((255 - Sa) * Da + Sa * Sa) / 255;
    d->color.rgba.r = ((255 - Sa) * Dr + Sa * Sr) / 255;
    d->color.rgba.g = ((255 - Sa) * Dg + Sa * Sg) / 255;
    d->color.rgba.b = ((255 - Sa) * Db + Sa * Sb) / 255;
}
void do_blending_argb8888_2_rgb888_opacity(gui_color_t *d, gui_color_t *s, uint8_t opacity)
{
    uint8_t Sa = s->color.rgba.a * opacity / 255;
    uint8_t Sr = s->color.rgba.r;
    uint8_t Sg = s->color.rgba.g;
    uint8_t Sb = s->color.rgba.b;

    uint8_t Da = 255;
    uint8_t Dr = d->color.rgba.r;
    uint8_t Dg = d->color.rgba.g;
    uint8_t Db = d->color.rgba.b;

    d->color.rgba.a = ((255 - Sa) * Da + Sa * Sa) / 255;
    d->color.rgba.r = ((255 - Sa) * Dr + Sa * Sr) / 255;
    d->color.rgba.g = ((255 - Sa) * Dg + Sa * Sg) / 255;
    d->color.rgba.b = ((255 - Sa) * Db + Sa * Sb) / 255;
}
void do_blending_rgb888_2_argb8888(gui_color_t *d, gui_color_t *s)
{
    uint8_t Sa = s->color.rgba.a;
    uint8_t Sr = s->color.rgba.r;
    uint8_t Sg = s->color.rgba.g;
    uint8_t Sb = s->color.rgba.b;

    uint8_t Da = d->color.rgba.a;
    uint8_t Dr = d->color.rgba.r;
    uint8_t Dg = d->color.rgba.g;
    uint8_t Db = d->color.rgba.b;

    d->color.rgba.a = ((255 - Sa) * Da + Sa * Sa) / 255;
    d->color.rgba.r = ((255 - Sa) * Dr + Sa * Sr) / 255;
    d->color.rgba.g = ((255 - Sa) * Dg + Sa * Sg) / 255;
    d->color.rgba.b = ((255 - Sa) * Db + Sa * Sb) / 255;
}
void do_blending_rgb565_2_argb8888(gui_color_t *d, gui_color_t *s)
{
    uint8_t Sa = s->color.rgba.a;
    uint8_t Sr = s->color.rgba.r;
    uint8_t Sg = s->color.rgba.g;
    uint8_t Sb = s->color.rgba.b;

    uint8_t Da = d->color.rgba.a;
    uint8_t Dr = d->color.rgba.r;
    uint8_t Dg = d->color.rgba.g;
    uint8_t Db = d->color.rgba.b;

    d->color.rgba.a = ((255 - Sa) * Da + Sa * Sa) / 255;
    d->color.rgba.r = ((255 - Sa) * Dr + Sa * Sr) / 255;
    d->color.rgba.g = ((255 - Sa) * Dg + Sa * Sg) / 255;
    d->color.rgba.b = ((255 - Sa) * Db + Sa * Sb) / 255;
}
void do_blending_rgb888_2_argb8888_opacity(gui_color_t *d, gui_color_t *s, uint8_t opacity)
{
    uint8_t Sa = s->color.rgba.a * opacity / 255;
    uint8_t Sr = s->color.rgba.r;
    uint8_t Sg = s->color.rgba.g;
    uint8_t Sb = s->color.rgba.b;

    uint8_t Da = d->color.rgba.a;
    uint8_t Dr = d->color.rgba.r;
    uint8_t Dg = d->color.rgba.g;
    uint8_t Db = d->color.rgba.b;

    d->color.rgba.a = ((255 - Sa) * Da + Sa * Sa) / 255;
    d->color.rgba.r = ((255 - Sa) * Dr + Sa * Sr) / 255;
    d->color.rgba.g = ((255 - Sa) * Dg + Sa * Sg) / 255;
    d->color.rgba.b = ((255 - Sa) * Db + Sa * Sb) / 255;
}
void do_blending_rgb565_2_argb8888_opacity(gui_color_t *d, gui_color_t *s, uint8_t opacity)
{
    uint8_t Sa = s->color.rgba.a * opacity / 255;
    uint8_t Sr = s->color.rgba.r;
    uint8_t Sg = s->color.rgba.g;
    uint8_t Sb = s->color.rgba.b;

    uint8_t Da = d->color.rgba.a;
    uint8_t Dr = d->color.rgba.r;
    uint8_t Dg = d->color.rgba.g;
    uint8_t Db = d->color.rgba.b;

    d->color.rgba.a = ((255 - Sa) * Da + Sa * Sa) / 255;
    d->color.rgba.r = ((255 - Sa) * Dr + Sa * Sr) / 255;
    d->color.rgba.g = ((255 - Sa) * Dg + Sa * Sg) / 255;
    d->color.rgba.b = ((255 - Sa) * Db + Sa * Sb) / 255;
}
void do_blending_argb8888_2_argb8888(gui_color_t *d, gui_color_t *s)
{

    uint8_t Sa = s->color.rgba.a;
    uint8_t Sr = s->color.rgba.r;
    uint8_t Sg = s->color.rgba.g;
    uint8_t Sb = s->color.rgba.b;

    uint8_t Da = d->color.rgba.a;
    uint8_t Dr = d->color.rgba.r;
    uint8_t Dg = d->color.rgba.g;
    uint8_t Db = d->color.rgba.b;

    d->color.rgba.a = ((255 - Sa) * Da + Sa * Sa) / 255;
    d->color.rgba.r = ((255 - Sa) * Dr + Sa * Sr) / 255;
    d->color.rgba.g = ((255 - Sa) * Dg + Sa * Sg) / 255;
    d->color.rgba.b = ((255 - Sa) * Db + Sa * Sb) / 255;
}
void do_blending_argb8888_2_argb8888_opacity(gui_color_t *d, gui_color_t *s, uint8_t opacity)
{
    uint8_t Sa = s->color.rgba.a * opacity / 255;
    uint8_t Sr = s->color.rgba.r;
    uint8_t Sg = s->color.rgba.g;
    uint8_t Sb = s->color.rgba.b;

    uint8_t Da = d->color.rgba.a;
    uint8_t Dr = d->color.rgba.r;
    uint8_t Dg = d->color.rgba.g;
    uint8_t Db = d->color.rgba.b;

    d->color.rgba.a = ((255 - Sa) * Da + Sa * Sa) / 255;
    d->color.rgba.r = ((255 - Sa) * Dr + Sa * Sr) / 255;
    d->color.rgba.g = ((255 - Sa) * Dg + Sa * Sg) / 255;
    d->color.rgba.b = ((255 - Sa) * Db + Sa * Sb) / 255;
}
