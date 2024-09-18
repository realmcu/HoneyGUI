/**
 * @file lv_draw_rtk_ppe_buffer.h
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