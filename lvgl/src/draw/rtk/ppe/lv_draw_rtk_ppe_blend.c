/**
 * @file lv_draw_vglite_blend.c
 *
 */

/*********************
 *      INCLUDES
 *********************/


#if LV_USE_GPU_RTK_PPE
#include "lv_draw_rtk_ppe_blend.h"
#include "lv_draw_rtk_ppe_buffer.h"
#include "lv_draw_rtk_ppe_utils.h"
#include "rtl_ppe.h"
#include "rtl_gdma.h"
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
    ppe_rect_t rect = {.left = dest_area->x1, .right = dest_area->x2, .top = dest_area->y1, .bottom = dest_area->y2};
    PPE_ERR err = PPE_Clear_Rect(&target, &rect, bg_color.full);
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

    ppe_translate_t trans = {.x = coords->x1, .y = coords->y1};
    PPE_ERR err = PPE_blend(&source, &target, &trans, PPE_SRC_OVER_MODE);
    if (err == PPE_SUCCESS)
    {
        return LV_RES_OK;
    }
    else
    {
        return LV_RES_INV;
    }
}

#include "os_mem.h"
#include "trace.h"
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
    uint32_t source_width = coords->x2 - coords->x1 + 1;
    uint32_t source_height = coords->y2 - coords->y1 + 1;
    source.width = source_width;
    source.height = source_height;
    uint8_t pixel_byte = 2;
    PPE_BLEND_MODE mode = PPE_BYPASS_MODE;
    if (LV_COLOR_DEPTH == 16)
    {
        if (cf == LV_IMG_CF_TRUE_COLOR)
        {
            source.format = PPE_RGB565;
            pixel_byte = 2;
        }
        else if (cf == LV_IMG_CF_TRUE_COLOR_ALPHA)
        {
            source.format = PPE_ARGB8565;
            pixel_byte = 3;
            mode = PPE_SRC_OVER_MODE;
        }
        else if (cf == LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED)
        {
            source.format = PPE_RGB565;
            pixel_byte = 2;
            source.color_key_en = ENABLE;
            source.color_key_value = LV_COLOR_CHROMA_KEY.full;
            mode = PPE_SRC_OVER_MODE;
        }
        else
        {
            return LV_RES_INV;
        }
    }
    else if (LV_COLOR_DEPTH == 32)
    {
        source.format = PPE_ARGB8888;
        mode = PPE_SRC_OVER_MODE;
    }
    else if (cf == LV_IMG_CF_RGB888)
    {
        source.format = PPE_RGB888;
    }
    else
    {
        return LV_RES_INV;
    }
    ppe_translate_t trans = {.x = coords->x1, .y = coords->y1};

    if (dsc->zoom != LV_IMG_ZOOM_NONE)
    {
        memset(&zoom, 0, sizeof(ppe_buffer_t));
        if (dsc->opa != 0xFF)
        {
            zoom.global_alpha_en = ENABLE;
            zoom.global_alpha = dsc->opa;
            mode = PPE_SRC_OVER_MODE;
        }
        if (cf == LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED)
        {
            zoom.color_key_en = ENABLE;
            zoom.color_key_value = LV_COLOR_CHROMA_KEY.full;
        }
        float zoom_ratio = dsc->zoom * 1.0f / LV_IMG_ZOOM_NONE;
        uint32_t new_width = (uint32_t)(source_width * zoom_ratio);
        uint32_t new_height = (uint32_t)(source_height * zoom_ratio);
        uint32_t buffer_size = 0;
        uint8_t *internal_buf = lv_draw_rtk_ppe_get_buffer(&buffer_size);
        zoom.memory = (uint32_t *)internal_buf;
        uint32_t new_size = new_width * new_height * pixel_byte;

        zoom.address = (uint32_t)zoom.memory;
        zoom.color_key_en = DISABLE;
        zoom.color_key_value = 0;
        zoom.format = source.format;
        if (new_size <= buffer_size)
        {
            PPE_ERR err = PPE_Scale(&source, &zoom, zoom_ratio, zoom_ratio);
            if (err != PPE_SUCCESS)
            {
                return LV_RES_INV;
            }

            err = PPE_blend(&zoom, &target, &trans, mode);
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
            uint32_t extra_line = ceil(zoom_ratio);
            uint32_t zoom_line_num = buffer_size / (new_width * pixel_byte);
            for (int y = 0; y < new_height; y += (zoom_line_num - extra_line))
            {
                uint32_t start_line = y / zoom_ratio;
                uint32_t end_line = (y + zoom_line_num - extra_line) / zoom_ratio;
                if (end_line >= source_height)
                {
                    end_line = source_height - 1;
                }
                ppe_rect_t scale_rect = {.left = 0, .top = start_line, .bottom = end_line, .right = source.width - 1};
                PPE_ERR err = PPE_Scale_Rect(&source, &zoom, zoom_ratio, zoom_ratio, &scale_rect);
                if (err != PPE_SUCCESS)
                {
                    return LV_RES_INV;
                }
                trans.y = coords->y1 + y;
                err = PPE_blend(&zoom, &target, &trans, mode);
                if (err != PPE_SUCCESS)
                {
                    return LV_RES_INV;
                }
                if ((trans.y + zoom_line_num - extra_line) > draw_ctx->buf_area->y2)
                {
                    break;
                }
            }
            return LV_RES_OK;
        }
    }
    else
    {
        if (dsc->opa != 0xFF)
        {
            source.global_alpha_en = ENABLE;
            source.global_alpha = dsc->opa;
            mode = PPE_SRC_OVER_MODE;
        }

#if PPE_CACHE_BG
        uint32_t ppe_buffer_size;
        uint8_t *m_buf = lv_draw_rtk_ppe_get_buffer(&ppe_buffer_size);
        uint32_t block_size = source.width * source.height * LV_COLOR_DEPTH / 8;
        uint32_t block_num = block_size / ppe_buffer_size;

        if (block_size % ppe_buffer_size)
        {
            block_num ++;
        }
        lv_area_t block_area;
        if (block_num == 0)
        {
            //NOTE: for potential solution, will never be accessed
            lv_area_t blend_area;
            /*Let's get the blend area which is the intersection of the area to draw and the clip area*/
            if (!_lv_area_intersect(&blend_area, coords, draw_ctx->clip_area))
            {
                return LV_RES_INV;    /*Fully clipped, nothing to do*/
            }
            ppe_buffer_t bg;
            memset(&bg, 0, sizeof(ppe_buffer_t));
            bg.memory = (uint32_t *)m_buf;
            bg.address = (uint32_t)bg.memory;
            bg.width = blend_area.x2 - blend_area.x1;
            bg.height = blend_area.y2 - blend_area.y1;
            bg.format = target.format;
            lv_draw_rtk_ppe_read_buffer(draw_ctx, &blend_area, (uint8_t *)target.memory);
            ppe_translate_t block_trans = {.x = coords->x1, .y = coords->y1};
            ppe_translate_t blend_trans = {.x = blend_area.x1, .y = blend_area.y1};
            PPE_ERR err = PPE_blend_multi(&source, &bg, &target, &block_trans, &blend_trans);
            if (err != PPE_SUCCESS)
            {
                return LV_RES_INV;
            }
        }
        else
#endif
        {
            PPE_ERR err = PPE_blend(&source, &target, &trans, mode);
            if (err == PPE_SUCCESS)
            {
                return LV_RES_OK;
            }
            else
            {
                return LV_RES_INV;
            }
        }
    }
    return LV_RES_INV;
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

    uint32_t buffer_size = 0;
    uint8_t *internal_buf = lv_draw_rtk_ppe_get_buffer(&buffer_size);

    ppe_buffer_t recolor;
    memset(&recolor, 0, sizeof(ppe_buffer_t));
    recolor.memory = (uint32_t *)internal_buf;
    recolor.address = (uint32_t)recolor.memory;
    recolor.color_key_en = DISABLE;
    recolor.color_key_value = 0;
    recolor.format = source.format;
    recolor.width = source.width;
    recolor.height = source.height;
    lv_color32_t recolor_value = lv_ppe_toABGR8888(dsc->recolor);
    recolor_value.ch.alpha = dsc->recolor_opa;

    ppe_translate_t trans = {.x = coords->x1, .y = coords->y1};
    uint32_t image_size = source.width * source.height * LV_COLOR_DEPTH / 8;
    if (image_size <= buffer_size)
    {
        ppe_rect_t rect = {.left = 0, .right = source.width - 1, .top = 0, .bottom = source.height - 1};
        PPE_ERR err = lv_ppe_recolor(&source, &recolor, &rect, recolor_value.full);
        if (err != PPE_SUCCESS)
        {
            return LV_RES_INV;
        }
        recolor.global_alpha_en = ENABLE;
        recolor.global_alpha = dsc->opa;
        err = PPE_blend(&recolor, &target, &trans, PPE_SRC_OVER_MODE);
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
        uint32_t recolor_line_num = buffer_size / (source.width * LV_COLOR_DEPTH / 8);
        for (uint16_t y = 0; y < source.height; y += recolor_line_num)
        {
            ppe_rect_t rect = {.left = 0, .right = source.width - 1, .top = y, .bottom = y + recolor_line_num};
            if (y + recolor_line_num >= source.height)
            {
                rect.bottom = source.height - 1;
            }
            recolor.height = (rect.bottom - rect.top + 1);
            PPE_ERR err = lv_ppe_recolor(&source, &recolor, &rect, recolor_value.full);
            if (err != PPE_SUCCESS)
            {
                return LV_RES_INV;
            }
            trans.y = coords->y1 + y;
            err = PPE_blend(&recolor, &target, &trans, PPE_SRC_OVER_MODE);
            if (err != PPE_SUCCESS)
            {
                return LV_RES_INV;
            }
            if ((trans.y + recolor_line_num) > draw_ctx->buf_area->y2)
            {
                break;
            }
        }
        return LV_RES_OK;
    }
}

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

    ppe_translate_t trans = {.x = dsc->mask_area->x1, .y = dsc->mask_area->y1};
    PPE_ERR err = PPE_blend(&source, &target, &trans, PPE_SRC_OVER_MODE);
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
