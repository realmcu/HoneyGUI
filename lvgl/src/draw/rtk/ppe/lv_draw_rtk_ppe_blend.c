/**
 * @file lv_draw_vglite_blend.c
 *
 */

/*********************
 *      INCLUDES
 *********************/


#if LV_USE_GPU_RTK_PPE
#include "lv_draw_rtk_ppe_blend.h"
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

/**
 * Blit single image, with optional opacity.
 *
 * @param[in] dest_area Area with relative coordinates of destination buffer
 * @param[in] src_area Source area with relative coordinates of source buffer
 * @param[in] opa Opacity
 *
 * @retval LV_RES_OK Transfer complete
 * @retval LV_RES_INV Error occurred
 */
static lv_res_t lv_ppe_blit_single(const lv_area_t *dest_area, const lv_area_t *src_area,
                                   lv_opa_t opa);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_res_t lv_ppe_fill(const lv_area_t *dest_area, lv_draw_ctx_t *draw_ctx,
                     const lv_draw_sw_blend_dsc_t *dsc)
{
    ppe_buffer_t target;
    memset(&target, 0, sizeof(ppe_buffer_t));
    target.address = (uint32_t)draw_ctx->buf;
    target.memory = draw_ctx->buf;
    target.width = lv_area_get_width(draw_ctx->buf_area);
    target.height = lv_area_get_height(draw_ctx->buf_area);
    target.format = sizeof(lv_color_t) == 2 ? PPE_RGB565 : PPE_ARGB8888;
    lv_color32_t bg_color = lv_ppe_toABGR8888(dsc->color);

    if (dsc->opa == 0xFF)
    {
        target.global_alpha_en = DISABLE;
    }
    else
    {
        target.global_alpha_en = ENABLE;
        target.global_alpha = dsc->opa;
    }
    PPE_rect_t rect = {.left = dest_area->x1, .right = dest_area->x2, .top = dest_area->y1, .bottom = dest_area->y2};
    PPE_ERR err = PPE_Clear_Rect_lvgl(&target, &rect, bg_color.full);
    if (err == PPE_SUCCESS)
    {
        return LV_RES_OK;
    }
    else
    {
        return LV_RES_INV;
    }
}

lv_res_t lv_ppe_alpha_only(const lv_img_dsc_t *img, lv_draw_ctx_t *draw_ctx,
                           const lv_draw_img_dsc_t *dsc, const lv_area_t *coords)
{
    ppe_buffer_t target, source;
    memset(&target, 0, sizeof(ppe_buffer_t));
    memset(&source, 0, sizeof(ppe_buffer_t));
    target.address = (uint32_t)draw_ctx->buf;
    target.memory = (uint32_t *)target.address;
    target.width = lv_area_get_width(draw_ctx->buf_area);
    target.height = lv_area_get_height(draw_ctx->buf_area);
    target.format = sizeof(lv_color_t) == 2 ? PPE_RGB565 : PPE_ARGB8888;

    source.address = (uint32_t)img->data;
    source.memory = (uint32_t *)source.address;
    source.width = coords->x2 - coords->x1 + 1;
    source.height = coords->y2 - coords->y1 + 1;
    source.format = PPE_A8;

    PPE_translate_t trans = {.x = coords->x1, .y = coords->y1};
    PPE_ERR err = PPE_blend(&source, &target, &trans);
    if (err == PPE_SUCCESS)
    {
        return LV_RES_OK;
    }
    else
    {
        return LV_RES_INV;
    }
}

#include "trace.h"
#include "os_mem.h"
lv_res_t lv_ppe_blit_transform(lv_draw_ctx_t *draw_ctx, const lv_draw_img_dsc_t *dsc,
                               const lv_area_t *coords, const uint8_t *map_p, lv_img_cf_t cf)
{
    ppe_buffer_t zoom;
    ppe_buffer_t target, source;
    memset(&target, 0, sizeof(ppe_buffer_t));
    memset(&source, 0, sizeof(ppe_buffer_t));
    target.address = (uint32_t)draw_ctx->buf;
    target.memory = (uint32_t *)target.address;
    target.width = lv_area_get_width(draw_ctx->buf_area);
    target.height = lv_area_get_height(draw_ctx->buf_area);
    target.format = sizeof(lv_color_t) == 2 ? PPE_RGB565 : PPE_ARGB8888;

    source.address = (uint32_t)map_p;
    source.memory = (uint32_t *)source.address;
    source.width = coords->x2 - coords->x1 + 1;
    source.height = coords->y2 - coords->y1 + 1;
    if (LV_COLOR_DEPTH == 16)
    {
        if (cf == LV_IMG_CF_TRUE_COLOR)
        {
            source.format = PPE_BGR565;
        }
        else if (cf == LV_IMG_CF_TRUE_COLOR_ALPHA)
        {
            source.format = PPE_ABGR8565;
        }
        else if (cf == LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED)
        {
            source.format = PPE_BGR565;
            source.color_key_en = ENABLE;
            source.color_key_value = LV_COLOR_CHROMA_KEY.full;
        }
        else
        {
            return LV_RES_INV;
        }
    }
    else if (LV_COLOR_DEPTH == 32)
    {
        source.format = PPE_ABGR8888;
    }
    else if (cf == LV_IMG_CF_RGB888)
    {
        source.format = PPE_BGR888;
    }
    else
    {
        //DBG_DIRECT("format return %d", cf);
        return LV_RES_INV;
    }
    PPE_translate_t trans = {.x = coords->x1, .y = coords->y1};

    if (dsc->zoom != LV_IMG_ZOOM_NONE)
    {
        memset(&zoom, 0, sizeof(ppe_buffer_t));
        float zoom_ratio = dsc->zoom * 1.0f / LV_IMG_ZOOM_NONE;
        uint32_t new_width = (uint32_t)((coords->x2 - coords->x1 + 1) * zoom_ratio);
        uint32_t new_height = (uint32_t)((coords->y2 - coords->y1 + 1) * zoom_ratio);
        if (source.format == PPE_BGR565)
        {
            zoom.memory = (uint32_t *)os_mem_alloc(RAM_TYPE_DATA_ON, new_width * new_height * 2);
        }
        else if (source.format == PPE_ABGR8565)
        {
            zoom.memory = (uint32_t *)os_mem_alloc(RAM_TYPE_DATA_ON, new_width * new_height * 3);
        }
        zoom.address = (uint32_t)zoom.memory;
        zoom.color_key_en = DISABLE;
        zoom.color_key_value = 0;
        zoom.format = source.format;
        PPE_ERR err = PPE_Scale(&source, &zoom, zoom_ratio, zoom_ratio);
        if (err != PPE_SUCCESS)
        {
            os_mem_free(zoom.memory);
            return LV_RES_INV;
        }
        if (dsc->opa != 0xFF)
        {
            zoom.global_alpha_en = ENABLE;
            zoom.global_alpha = dsc->opa;
        }
        if (cf == LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED)
        {
            zoom.color_key_en = ENABLE;
            zoom.color_key_value = LV_COLOR_CHROMA_KEY.full;
        }
        err = PPE_blend(&zoom, &target, &trans);
        os_mem_free(zoom.memory);
        if (err == PPE_SUCCESS)
        {
            return LV_RES_OK;
        }
        else
        {
            return LV_RES_INV;
        }
    }
    else
    {
        if (dsc->opa != 0xFF)
        {
            source.global_alpha_en = ENABLE;
            source.global_alpha = dsc->opa;
        }
        PPE_ERR err = PPE_blend(&source, &target, &trans);
        if (err == PPE_SUCCESS)
        {
            return LV_RES_OK;
        }
        else
        {
            return LV_RES_INV;
        }
    }
//    DBG_DIRECT("src w %d, h %d, x1 %d, y1 %d", source.width, source.height, trans.x, trans.y);
}

lv_res_t lv_ppe_blit_recolor(lv_draw_ctx_t *draw_ctx, const lv_draw_img_dsc_t *dsc,
                             const lv_area_t *coords, const uint8_t *map_p, lv_img_cf_t cf)
{
    ppe_buffer_t target, source;
    memset(&target, 0, sizeof(ppe_buffer_t));
    memset(&source, 0, sizeof(ppe_buffer_t));
    target.address = (uint32_t)draw_ctx->buf;
    target.memory = (uint32_t *)target.address;
    target.width = lv_area_get_width(draw_ctx->buf_area);
    target.height = lv_area_get_height(draw_ctx->buf_area);
    target.format = sizeof(lv_color_t) == 2 ? PPE_RGB565 : PPE_ARGB8888;

    source.address = (uint32_t)map_p;
    source.memory = (uint32_t *)source.address;
    source.width = coords->x2 - coords->x1 + 1;
    source.height = coords->y2 - coords->y1 + 1;
    if (LV_COLOR_DEPTH == 16)
    {
        if (cf == LV_IMG_CF_TRUE_COLOR)
        {
            source.format = PPE_BGR565;
        }
        else
        {
            return LV_RES_INV;
        }
    }
    source.global_alpha_en = ENABLE;
    source.global_alpha = 0xFF - dsc->recolor_opa;

    ppe_buffer_t recolor;
    memset(&recolor, 0, sizeof(ppe_buffer_t));
    if (source.format == PPE_BGR565)
    {
        recolor.memory = (uint32_t *)os_mem_alloc(RAM_TYPE_DATA_ON, source.width * source.height * 2);
    }
    recolor.address = (uint32_t)recolor.memory;
    recolor.color_key_en = DISABLE;
    recolor.color_key_value = 0;
    recolor.format = source.format;
    recolor.width = source.width;
    recolor.height = source.height;
    lv_color32_t recolor_value = lv_ppe_toABGR8888(dsc->recolor);
    recolor_value.ch.alpha = dsc->recolor_opa;
    PPE_ERR err = PPE_recolor_lvgl(&source, &recolor, recolor_value.full);
    if (err != PPE_SUCCESS)
    {
        os_mem_free(recolor.memory);
        return LV_RES_INV;
    }
    PPE_translate_t trans = {.x = coords->x1, .y = coords->y1};
    recolor.global_alpha_en = ENABLE;
    recolor.global_alpha = dsc->opa;
    err = PPE_blend(&recolor, &target, &trans);
    os_mem_free(recolor.memory);
    if (err == PPE_SUCCESS)
    {
        return LV_RES_OK;
    }
    else
    {
        return LV_RES_INV;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
lv_res_t lv_ppe_mask(lv_draw_ctx_t *draw_ctx, const lv_draw_sw_blend_dsc_t *dsc)
{
    ppe_buffer_t target, source;
    memset(&target, 0, sizeof(ppe_buffer_t));
    memset(&source, 0, sizeof(ppe_buffer_t));
    target.address = (uint32_t)draw_ctx->buf;
    target.memory = (uint32_t *)target.address;
    target.width = lv_area_get_width(draw_ctx->buf_area);
    target.height = lv_area_get_height(draw_ctx->buf_area);
    target.format = sizeof(lv_color_t) == 2 ? PPE_RGB565 : PPE_ARGB8888;

    source.address = (uint32_t)dsc->mask_buf;
    source.memory = (uint32_t *)source.address;
    source.width = dsc->mask_area->x2 - dsc->mask_area->x1 + 1;
    source.height = dsc->mask_area->y2 - dsc->mask_area->y1 + 1;
    source.format = PPE_A8;

    PPE_translate_t trans = {.x = dsc->mask_area->x1, .y = dsc->mask_area->y1};
    PPE_ERR err = PPE_blend(&source, &target, &trans);
    if (err == PPE_SUCCESS)
    {
        return LV_RES_OK;
    }
    else
    {
        return LV_RES_INV;
    }
}


#endif /*LV_USE_GPU_RTK_VG_LITE*/
