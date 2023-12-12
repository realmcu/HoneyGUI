/**
 * @file lv_draw_rtk_ppe_buffer.h
 *
 */

/**
 * MIT License
 *
 * Copyright 2020-2023 NXP
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next paragraph)
 * shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "../../../lv_conf_internal.h"
#ifndef LV_DRAW_PPE_BUFFER_H
#define LV_DRAW_PPE_BUFFER_H
#include "../../sw/lv_draw_sw.h"
#include "lv_area.h"
uint8_t *lv_draw_rtk_ppe_get_buffer(uint32_t *size);
void lv_draw_rtk_ppe_read_buffer(lv_draw_ctx_t *draw_ctx, const lv_area_t *area, uint8_t *dst);
void lv_draw_rtk_ppe_write_buffer(uint8_t *src, const lv_area_t *area, lv_draw_ctx_t *draw_ctx);
#endif