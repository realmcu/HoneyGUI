/**
 * @file lv_draw_rtk_ppe_utils.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lvgl.h"
#if LV_USE_GPU_RTK_PPE
#include "lv_draw_rtk_ppe_utils.h"
#include "../../../core/lv_refr.h"
#include "rtl_ppe.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static uint8_t high_speed_dma_channel = 0xFF;
static uint8_t low_speed_dma_channel = 0xFF;
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_color32_t lv_ppe_toABGR8888(lv_color_t color)
{
    lv_color32_t ABGR8888_color;
#if LV_COLOR_SIZE == 16
    {
        ABGR8888_color.ch.alpha = 0xFF;
        ABGR8888_color.ch.blue = (color.ch.red << 3) | (color.ch.red >> 2);
        ABGR8888_color.ch.green = (color.ch.green << 2) | (color.ch.green >> 4);
        ABGR8888_color.ch.red = (color.ch.blue << 3) | (color.ch.blue >> 2);
    }
#elif LV_COLOR_SIZE == 32
    {
        ABGR8888_color.ch.alpha = color.ch.alpha;
        ABGR8888_color.ch.blue = color.ch.red;
        ABGR8888_color.ch.green = color.ch.green;
        ABGR8888_color.ch.red = color.ch.blue;
    }
#endif
    return ABGR8888_color;
}

void lv_ppe_get_transformed_area(lv_area_t *res, lv_coord_t w, lv_coord_t h, int16_t angle,
                                 uint16_t zoom,
                                 const lv_point_t *pivot)
{
#if LV_DRAW_COMPLEX
    if (angle == 0 && zoom == LV_IMG_ZOOM_NONE)
    {
        res->x1 = 0;
        res->y1 = 0;
        res->x2 = w - 1;
        res->y2 = h - 1;
        return;
    }

    lv_point_t p[4] =
    {
        {0, 0},
        {w, 0},
        {0, h},
        {w, h},
    };
    lv_point_transform(&p[0], angle, zoom, pivot);
    lv_point_transform(&p[1], angle, zoom, pivot);
    lv_point_transform(&p[2], angle, zoom, pivot);
    lv_point_transform(&p[3], angle, zoom, pivot);
    res->x1 = LV_MIN4(p[0].x, p[1].x, p[2].x, p[3].x);
    res->x2 = LV_MAX4(p[0].x, p[1].x, p[2].x, p[3].x);
    res->y1 = LV_MIN4(p[0].y, p[1].y, p[2].y, p[3].y);
    res->y2 = LV_MAX4(p[0].y, p[1].y, p[2].y, p[3].y);

#else
    LV_UNUSED(angle);
    LV_UNUSED(zoom);
    LV_UNUSED(pivot);
    res->x1 = 0;
    res->y1 = 0;
    res->x2 = w - 1;
    res->y2 = h - 1;
#endif
}

uint8_t lv_ppe_get_high_speed_dma(void)
{
    return high_speed_dma_channel;
}

uint8_t lv_ppe_get_low_speed_dma(void)
{
    return low_speed_dma_channel;
}

void lv_ppe_register_dma_channel(uint8_t high_speed, uint8_t low_speed)
{
    high_speed_dma_channel = high_speed;
    low_speed_dma_channel = low_speed;
}


#endif /*LV_USE_GPU_RTK_PPE*/
