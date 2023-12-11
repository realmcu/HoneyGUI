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


void do_blending_argb8888_2_rgb565(uint8_t *target, gui_color_t color)
{
    gui_log("[GUI] TODO line = %d", __LINE__);
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