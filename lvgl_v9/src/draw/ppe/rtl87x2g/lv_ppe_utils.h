/**
 * @file lv_draw_rtk_ppe_utils.h
 *
 */

#ifndef LV_PPE_UTILS_H
#define LV_PPE_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_conf_internal.h"

#if LV_USE_PPE
#include "../../sw/lv_draw_sw.h"
#include "../../../misc/lv_log.h"
#include "rtl_ppe.h"
#include "rtl_rcc.h"

/*********************
 *      DEFINES
 *********************/
#define LV_PPE_MAX_BUFFER_SIZE  (20 * 1024)
#if LV_MEM_SIZE < LV_PPE_MAX_BUFFER_SIZE
#error "PPE buffer size cannot be larger than LV heap size"
#endif
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
uint32_t lv_ppe_get_color(lv_color_t color, uint8_t opa);

PPE_ERR lv_ppe_recolor(ppe_buffer_t *image, ppe_buffer_t *buffer, ppe_rect_t *rect, uint32_t color);

uint8_t *lv_ppe_get_buffer(uint32_t size);

PPE_PIXEL_FORMAT lv_ppe_get_format(lv_color_format_t cf, const uint8_t *content);
/**********************
 *      MACROS
 **********************/


#endif /*LV_USE_PPE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_PPE_UTILS_H*/
