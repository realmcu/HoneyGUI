/**
 * @file lv_draw_ppe.h
 *
 */

#ifndef LV_DRAW_PPE_H
#define LV_DRAW_PPE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_draw.h"
#if LV_USE_DRAW_PPE_RTL8773E

#include "../../misc/lv_area.h"
#include "../../misc/lv_color.h"
#include "../../display/lv_display.h"
#include "../../osal/lv_os.h"

#include "../lv_draw_rect.h"
#include "../lv_draw_image.h"
#include "lv_ppe_rtl8773e_utils.h"
#include "lv_draw_sw_utils.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef lv_draw_sw_unit_t lv_draw_ppe_unit_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_ppe_init(void);

void lv_draw_ppe_deinit(void);

void lv_draw_ppe_fill(lv_draw_unit_t *draw_unit, lv_draw_fill_dsc_t *dsc, const lv_area_t *coords);

void lv_draw_ppe_image(lv_draw_unit_t *draw_unit, const lv_draw_image_dsc_t *draw_dsc,
                       const lv_area_t *coords);

void lv_draw_ppe_layer(lv_draw_unit_t *draw_unit, const lv_draw_image_dsc_t *draw_dsc,
                       const lv_area_t *coords);
/***********************
 * GLOBAL VARIABLES
 ***********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_PPE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_PPE_H*/
