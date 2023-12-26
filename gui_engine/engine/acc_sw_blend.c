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


void do_blending_rgb565_2_rgb565_opacity(uint16_t *d, gui_color_t *s, uint8_t opacity)
{
    uint8_t Sa = opacity;
    uint16_t Sr = (uint16_t)s->channel.red;
    uint16_t Sg = (uint16_t)s->channel.green;
    uint16_t Sb = (uint16_t)s->channel.blue;

    uint16_t Dr = ((*d >> 11) << 3);
    uint16_t Dg = (((*d & 0x07e0) >> 5) << 2);
    uint16_t Db = ((*d & 0x001f) << 3);

    uint16_t red = (((((255 - Sa) * Dr + Sa * Sr) / 255) >> 3) << 11);
    uint16_t green = (((((255 - Sa) * Dg + Sa * Sg) / 255) >> 2) << 5);
    uint16_t blue = ((((255 - Sa) * Db + Sa * Sb) / 255) >> 3);

    *d = red + green + blue ;
}
void do_blending_rgb565_2_rgb565(uint16_t *d, gui_color_t *s)
{
    uint8_t Sa = 255;
    uint16_t Sr = (uint16_t)s->channel.red;
    uint16_t Sg = (uint16_t)s->channel.green;
    uint16_t Sb = (uint16_t)s->channel.blue;

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
    uint16_t Sr = (uint16_t)s->channel.red;
    uint16_t Sg = (uint16_t)s->channel.green;
    uint16_t Sb = (uint16_t)s->channel.blue;

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
    uint16_t Sr = (uint16_t)s->channel.red;
    uint16_t Sg = (uint16_t)s->channel.green;
    uint16_t Sb = (uint16_t)s->channel.blue;

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
    uint8_t Sa = s->channel.alpha;
    uint16_t Sr = (uint16_t)s->channel.red;
    uint16_t Sg = (uint16_t)s->channel.green;
    uint16_t Sb = (uint16_t)s->channel.blue;

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
    uint8_t Sa = (s->channel.alpha * opacity) / 255;
    uint16_t Sr = (uint16_t)s->channel.red;
    uint16_t Sg = (uint16_t)s->channel.green;
    uint16_t Sb = (uint16_t)s->channel.blue;

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
    uint8_t Sa = s->channel.alpha * opacity / 255;
    uint8_t Sr = s->channel.red;
    uint8_t Sg = s->channel.green;
    uint8_t Sb = s->channel.blue;

    uint8_t Da = 255;
    uint8_t Dr = d->channel.red;
    uint8_t Dg = d->channel.green;
    uint8_t Db = d->channel.blue;

    d->channel.alpha = ((255 - Sa) * Da + Sa * Sa) / 255;
    d->channel.red = ((255 - Sa) * Dr + Sa * Sr) / 255;
    d->channel.green = ((255 - Sa) * Dg + Sa * Sg) / 255;
    d->channel.blue = ((255 - Sa) * Db + Sa * Sb) / 255;
}
void do_blending_rgb565_2_rgb888(gui_color_t *d, gui_color_t *s)
{
    uint8_t Sa = s->channel.alpha;
    uint8_t Sr = s->channel.red;
    uint8_t Sg = s->channel.green;
    uint8_t Sb = s->channel.blue;

    uint8_t Da = 255;
    uint8_t Dr = d->channel.red;
    uint8_t Dg = d->channel.green;
    uint8_t Db = d->channel.blue;

    d->channel.alpha = ((255 - Sa) * Da + Sa * Sa) / 255;
    d->channel.red = ((255 - Sa) * Dr + Sa * Sr) / 255;
    d->channel.green = ((255 - Sa) * Dg + Sa * Sg) / 255;
    d->channel.blue = ((255 - Sa) * Db + Sa * Sb) / 255;
}
void do_blending_rgb888_2_rgb888_opacity(gui_color_t *d, gui_color_t *s, uint8_t opacity)
{
    uint8_t Sa = s->channel.alpha * opacity / 255;
    uint8_t Sr = s->channel.red;
    uint8_t Sg = s->channel.green;
    uint8_t Sb = s->channel.blue;

    uint8_t Da = 255;
    uint8_t Dr = d->channel.red;
    uint8_t Dg = d->channel.green;
    uint8_t Db = d->channel.blue;

    d->channel.alpha = ((255 - Sa) * Da + Sa * Sa) / 255;
    d->channel.red = ((255 - Sa) * Dr + Sa * Sr) / 255;
    d->channel.green = ((255 - Sa) * Dg + Sa * Sg) / 255;
    d->channel.blue = ((255 - Sa) * Db + Sa * Sb) / 255;
}
void do_blending_rgb888_2_rgb888(gui_color_t *d, gui_color_t *s)
{
    uint8_t Sa = s->channel.alpha;
    uint8_t Sr = s->channel.red;
    uint8_t Sg = s->channel.green;
    uint8_t Sb = s->channel.blue;

    uint8_t Da = 255;
    uint8_t Dr = d->channel.red;
    uint8_t Dg = d->channel.green;
    uint8_t Db = d->channel.blue;

    d->channel.alpha = ((255 - Sa) * Da + Sa * Sa) / 255;
    d->channel.red = ((255 - Sa) * Dr + Sa * Sr) / 255;
    d->channel.green = ((255 - Sa) * Dg + Sa * Sg) / 255;
    d->channel.blue = ((255 - Sa) * Db + Sa * Sb) / 255;
}
void do_blending_argb8888_2_rgb888(gui_color_t *d, gui_color_t *s)
{
    uint8_t Sa = s->channel.alpha;
    uint8_t Sr = s->channel.red;
    uint8_t Sg = s->channel.green;
    uint8_t Sb = s->channel.blue;

    uint8_t Da = 255;
    uint8_t Dr = d->channel.red;
    uint8_t Dg = d->channel.green;
    uint8_t Db = d->channel.blue;

    d->channel.alpha = ((255 - Sa) * Da + Sa * Sa) / 255;
    d->channel.red = ((255 - Sa) * Dr + Sa * Sr) / 255;
    d->channel.green = ((255 - Sa) * Dg + Sa * Sg) / 255;
    d->channel.blue = ((255 - Sa) * Db + Sa * Sb) / 255;
}
void do_blending_argb8888_2_rgb888_opacity(gui_color_t *d, gui_color_t *s, uint8_t opacity)
{
    uint8_t Sa = s->channel.alpha * opacity / 255;
    uint8_t Sr = s->channel.red;
    uint8_t Sg = s->channel.green;
    uint8_t Sb = s->channel.blue;

    uint8_t Da = 255;
    uint8_t Dr = d->channel.red;
    uint8_t Dg = d->channel.green;
    uint8_t Db = d->channel.blue;

    d->channel.alpha = ((255 - Sa) * Da + Sa * Sa) / 255;
    d->channel.red = ((255 - Sa) * Dr + Sa * Sr) / 255;
    d->channel.green = ((255 - Sa) * Dg + Sa * Sg) / 255;
    d->channel.blue = ((255 - Sa) * Db + Sa * Sb) / 255;
}
void do_blending_rgb888_2_argb8888(gui_color_t *d, gui_color_t *s)
{
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
void do_blending_rgb565_2_argb8888(gui_color_t *d, gui_color_t *s)
{
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
void do_blending_rgb888_2_argb8888_opacity(gui_color_t *d, gui_color_t *s, uint8_t opacity)
{
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
void do_blending_argb8888_2_argb8888(gui_color_t *d, gui_color_t *s)
{

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
void do_blending_argb8888_2_argb8888_opacity(gui_color_t *d, gui_color_t *s, uint8_t opacity)
{
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