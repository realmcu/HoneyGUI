/**
 * @file lv_draw_rtk_ppe_utils.c
 *
 */

/*********************
 *      INCLUDES
 *********************/


#if LV_USE_GPU_RTK_PPEV2
#include "lv_draw_rtk_ppe_utils.h"
#include "../../../core/lv_refr.h"
#include "section.h"


/*********************
 *      DEFINES
 *********************/
#define BUFFER_SIZE     (80 * 1024 - 454 * 40 * 2)
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

void lv_ppe_get_matrix(ppe_matrix_t *matrix, const lv_area_t *coords, const lv_draw_img_dsc_t *dsc)
{
    ppe_get_identity(matrix);
    bool scale = (dsc->zoom != LV_IMG_ZOOM_NONE);
    bool rotate = (dsc->angle != 0);
    ppe_translate(coords->x1, coords->y1, matrix);
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
extern void SCB_InvalidateDCache(void);
lv_res_t lv_ppe_buffer_copy(void *dest_buf, lv_coord_t dest_stride, const lv_area_t *dest_area,
                            const void *src_buf, lv_coord_t src_stride, const lv_area_t *src_area, uint8_t unit_size)
{
//    SCB_InvalidateDCache();
    hal_imdc_dma_info copy_info = {0};
    copy_info.dst_stride = dest_stride * unit_size;
    copy_info.src_stride = src_stride * unit_size;
    copy_info.length = lv_area_get_width(src_area) * unit_size;
    copy_info.height = lv_area_get_height(src_area);
//    DBG_DIRECT("src stride %d, dst stride %d, length %d, height %d", copy_info.src_stride, copy_info.dst_stride, copy_info.length, copy_info.height);
//    DBG_DIRECT("src (%d, %d), dst (%d, %d)", src_area->x1, src_area->y1, dest_area->x1, dest_area->y1);
    uint8_t *src = (uint8_t *)src_buf;
//    DBG_DIRECT("src addr 0x%08x dst 0x%08x", src, dest_buf);
    src += (src_area->x1 + src_stride * src_area->y1) * unit_size;
//    DBG_DIRECT("src addr after change 0x%08x", src);

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

#endif /*LV_USE_GPU_RTK_PPEV2*/
