/**
 * @file lv_draw_rtk_ppe_utils_v2.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lvgl.h"
#if LV_USE_GPU_PPE_RTL8773E
#include "lv_draw_ppe_rtl8773e_utils.h"
#include "../../../core/lv_refr.h"
#include "section.h"


/*********************
 *      DEFINES
 *********************/
#define BUFFER_SIZE     (40 * 1024)
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static SHM_DATA_SECTION uint8_t internal_buffer[BUFFER_SIZE];
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
uint8_t *lv_ppe_get_buffer(uint32_t *size)
{
    *size = BUFFER_SIZE;
    return internal_buffer;
}

void lv_ppe_get_transformed_area(lv_area_t *res, lv_coord_t w, lv_coord_t h, ppe_matrix_t *matrix)
{
    ppe_buffer_t pesudo;
    pesudo.width = MY_DISP_HOR_RES;
    pesudo.height = MY_DISP_VER_RES;
    ppe_rect_t in = {.x = 0, .y = 0, .w = w, .h = h};
    ppe_rect_t out;
    ppe_get_area(&out, &in, matrix, &pesudo);
    res->x1 = out.x;
    res->x2 = out.x + out.w - 1;
    res->y1 = out.y;
    res->y2 = out.y + out.h - 1;
}

void lv_int_get_transformed_area(lv_area_t *res, lv_coord_t w, lv_coord_t h, int16_t angle,
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

void lv_ppe_get_matrix(ppe_matrix_t *matrix, const lv_area_t *coords, const lv_draw_img_dsc_t *dsc)
{
    ppe_get_identity(matrix);
    bool scale = (dsc->zoom != LV_IMG_ZOOM_NONE);
    bool rotate = (dsc->angle != 0);
    matrix->m[0][2] = coords->x1;
    matrix->m[1][2] = coords->y1;
    if (scale || rotate)
    {
        ppe_translate(dsc->pivot.x, dsc->pivot.y, matrix);
        if (rotate)
        {
            ppe_rotate(dsc->angle / 10.0f, matrix);    /* angle is 1/10 degree */
        }
        if (scale)
        {
            float scale_ratio = 1.0f * dsc->zoom / LV_IMG_ZOOM_NONE;
            ppe_scale(scale_ratio, scale_ratio, matrix);
        }
        ppe_translate(-dsc->pivot.x, -dsc->pivot.y, matrix);
    }
}

#include "trace.h"
lv_res_t lv_ppe_buffer_copy(void *dest_buf, lv_coord_t dest_stride, const lv_area_t *dest_area,
                            const void *src_buf, lv_coord_t src_stride, const lv_area_t *src_area, uint8_t unit_size)
{
//    SCB_InvalidateDCache();
    hal_idu_dma_info copy_info = {0};
    copy_info.dst_stride = dest_stride * unit_size;
    copy_info.src_stride = src_stride * unit_size;
    copy_info.length = lv_area_get_width(src_area) * unit_size;
    copy_info.height = lv_area_get_height(src_area);

    uint8_t *src = (uint8_t *)src_buf;
    src += (src_area->x1 + src_stride * src_area->y1) * unit_size;
    uint8_t *dst = (uint8_t *)dest_buf;
    dst += (dest_area->x1 + dest_stride * dest_area->y1) * unit_size;

    hal_dma_copy(&copy_info, src, dst);
    return LV_RES_OK;
}

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

#endif /*LV_USE_GPU_PPE_RTL8773E*/
