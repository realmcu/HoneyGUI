/**
 * @file lv_ppe_utils.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../../core/lv_refr.h"
#include "../lv_draw_image_private.h"
#if LV_USE_DRAW_PPE_RTL8773E
#include "lv_ppe_rtl8773e_utils.h"
#include "section.h"

SHM_DATA_SECTION uint8_t cache_buffer[40 * 1024];
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

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

uint32_t lv_ppe_get_color(lv_color_t color, uint8_t opa)
{
    lv_color32_t ABGR8888_color;

    ABGR8888_color.alpha = opa;
    ABGR8888_color.blue = color.red;
    ABGR8888_color.green = color.green;
    ABGR8888_color.red = color.blue;

    return *(uint32_t *)&ABGR8888_color;
}

PPE_PIXEL_FORMAT lv_ppe_get_format(lv_color_format_t cf, const uint8_t *content)
{
    switch (cf)
    {
    case LV_COLOR_FORMAT_RGB565:
        return PPE_RGB565;
    case LV_COLOR_FORMAT_ARGB8888:
        return PPE_ARGB8888;
    case LV_COLOR_FORMAT_RGB888:
        return PPE_RGB888;
    case LV_COLOR_FORMAT_XRGB8888:
        return PPE_XRGB8888;
    case LV_COLOR_FORMAT_ARGB8565:
        return PPE_ARGB8565;
    case LV_COLOR_FORMAT_ARGB1555:
        return PPE_ARGB1555;
    case LV_COLOR_FORMAT_ARGB4444:
        return PPE_ARGB4444;
    case LV_COLOR_FORMAT_ARGB2222:
        return PPE_ARGB2222;
    case LV_COLOR_FORMAT_A8:
        return PPE_A8;
    case LV_COLOR_FORMAT_RAW:
        {
            uint8_t type = *(content + 1);
            switch (type)
            {
            case 0:
                return PPE_RGB565;
            case 1:
                return PPE_ARGB8565;
            case 3:
                return PPE_RGB888;
            case 4:
                return PPE_ARGB8888;
            case 5:
                return PPE_XRGB8888;
            default:
                return PPE_FORMAT_NOT_SUPPORT;
            }
        }
    default:
        return PPE_FORMAT_NOT_SUPPORT;
    }
}

lv_area_t lv_ppe_get_matrix_area(ppe_matrix_t *matrix, const lv_area_t *coords,
                                 const lv_draw_image_dsc_t *draw_dsc)
{
    lv_area_t transform_area;
    ppe_get_identity(matrix);
    bool scale = (draw_dsc->scale_x != LV_SCALE_NONE || draw_dsc->scale_y != LV_SCALE_NONE);
    bool rotate = (draw_dsc->rotation != 0);
    bool skew = (draw_dsc->skew_x != 0 || draw_dsc->skew_y != 0);
    matrix->m[0][2] = coords->x1;
    matrix->m[1][2] = coords->y1;
    if (scale || rotate)
    {
        ppe_translate(draw_dsc->pivot.x, draw_dsc->pivot.y, matrix);
        if (rotate)
        {
            ppe_rotate(draw_dsc->rotation / 10.0f, matrix);    /* angle is 1/10 degree */
        }
        if (scale)
        {
            float scale_ratio_x = 1.0f * draw_dsc->scale_x / LV_SCALE_NONE;
            float scale_ratio_y = 1.0f * draw_dsc->scale_y / LV_SCALE_NONE;
            ppe_scale(scale_ratio_x, scale_ratio_y, matrix);
        }
        if (skew)
        {
            ppe_skew(draw_dsc->skew_x, draw_dsc->skew_y, matrix);
        }
        ppe_translate(-draw_dsc->pivot.x, -draw_dsc->pivot.y, matrix);
    }

    int32_t w = lv_area_get_width(coords);
    int32_t h = lv_area_get_height(coords);

    lv_image_buf_get_transformed_area(&transform_area, w, h, draw_dsc->rotation, draw_dsc->scale_x,
                                      draw_dsc->scale_y,
                                      &draw_dsc->pivot);

    transform_area.x1 += coords->x1;
    transform_area.y1 += coords->y1;
    transform_area.x2 += coords->x1;
    transform_area.y2 += coords->y1;
    return transform_area;
}

uint8_t *lv_ppe_get_buffer(uint32_t size)
{
    return cache_buffer;
}
#endif /*LV_USE_PPE*/
