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
#include "hal_imdc.h"
#include "rtl_imdc.h"
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
    if (dsc->opa != 0xFF)
    {
        bg_color.ch.alpha = dsc->opa * bg_color.ch.alpha / 255;
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

lv_res_t lv_ppe_blit_transform(lv_draw_ctx_t *draw_ctx, const lv_draw_img_dsc_t *dsc,
                               lv_point_t *base,
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
    PPE_BLEND_MODE mode = PPE_BYPASS_MODE;
    bool compressed = false;
    switch (cf)
    {
    case LV_IMG_CF_ALPHA_8BIT:
        source.format = PPE_A8;
        break;
    case LV_IMG_CF_TRUE_COLOR:
        if (LV_COLOR_DEPTH == 16)
        {
            source.format = PPE_RGB565;
        }
        if (LV_COLOR_DEPTH == 32)
        {
            source.format = PPE_ARGB8888;
            mode = PPE_SRC_OVER_MODE;
        }
        break;
    case LV_IMG_CF_TRUE_COLOR_ALPHA:
        if (LV_COLOR_DEPTH == 16)
        {
            source.format = PPE_ARGB8565;
        }
        if (LV_COLOR_DEPTH == 32)
        {
            source.format = PPE_ARGB8888;
        }
        mode = PPE_SRC_OVER_MODE;
        break;
    case LV_IMG_CF_RGB888:
        source.format = PPE_RGB888;
        break;
    case LV_IMG_CF_RGBA8888:
        source.format = PPE_RGBA8888;
        mode = PPE_SRC_OVER_MODE;
        break;
    case LV_IMG_CF_RGBX8888:
        source.format = PPE_RGBX8888;
        mode = PPE_SRC_OVER_MODE;
        break;
    case LV_IMG_CF_RGB565:
        source.format = PPE_RGB565;
        break;
    case LV_IMG_CF_RGBA5658:
        source.format = PPE_RGBA5658;
        mode = PPE_SRC_OVER_MODE;
        break;
    case LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED:
        {
            if (LV_COLOR_DEPTH == 16)
            {
                source.format = PPE_RGB565;
            }
            if (LV_COLOR_DEPTH == 32)
            {
                source.format = PPE_ARGB8888;
            }
            source.color_key_en = ENABLE;
            source.color_key_value = LV_COLOR_CHROMA_KEY.full;
            mode = PPE_SRC_OVER_MODE;
        }
        break;
    case LV_IMG_CF_RAW:
        {
            IMDC_file_header *header = (IMDC_file_header *)(map_p + 8);
            if (header->algorithm_type.pixel_bytes == IMDC_PIXEL_16BIT)
            {
                source.format = PPE_RGB565;
            }
            else if (header->algorithm_type.pixel_bytes == IMDC_PIXEL_24BIT)
            {
                source.format = PPE_RGB888;
            }
            else
            {
                source.format = PPE_ARGB8888;
            }
            compressed = true;
        }
        break;
    default:
        return LV_RES_INV;
    }

    uint8_t pixel_byte = ppe_get_format_data_len(source.format);
    ppe_translate_t trans = {.x = coords->x1 - draw_ctx->buf_area->x1, .y = coords->y1 - draw_ctx->buf_area->y1};

    lv_area_t constraint_area;
    ppe_rect_t blend_rect;
    if (_lv_area_intersect(&constraint_area, draw_ctx->buf_area, draw_ctx->clip_area))
    {
        blend_rect.x1 = constraint_area.x1 - draw_ctx->buf_area->x1;
        blend_rect.y1 = constraint_area.y1 - draw_ctx->buf_area->y1;
        blend_rect.x2 = constraint_area.x2 - draw_ctx->buf_area->x1;
        blend_rect.y2 = constraint_area.y2 - draw_ctx->buf_area->y1;
    }
    else
    {
        return LV_RES_OK;
    }
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
        lv_area_t new_area;
        trans.x = base->x - draw_ctx->buf_area->x1;
        trans.y = base->y - draw_ctx->buf_area->y1;
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
            if (compressed)
            {
                ppe_rect_t scale_rect = {.x1 = 0, .x2 = source.width - 1, \
                                         .y1 = 0, .y2 = source.height - 1
                                        };
                hal_imdc_decompress_info info;
                info.start_column = scale_rect.left;
                info.end_column = scale_rect.right;
                info.start_line = scale_rect.top;
                info.end_line = scale_rect.bottom;
                info.raw_data_address = (uint32_t)(map_p + 8);
                source.width = info.end_column - info.start_column + 1;
                source.height = info.end_line - info.start_line + 1;
                source.memory = lv_mem_alloc(source.width * source.height * pixel_byte);
                source.address = (uint32_t)source.memory;
                bool ret = hal_imdc_decompress(&info, (uint8_t *)source.memory);
                scale_rect.top = 0;
                scale_rect.left = 0;
                scale_rect.right = source.width - 1;
                scale_rect.bottom = source.height - 1;
            }

            PPE_ERR err = PPE_Scale(&source, &zoom, zoom_ratio, zoom_ratio);
            if (err != PPE_SUCCESS)
            {
                if (compressed)
                {
                    lv_mem_free(source.memory);
                }
                return LV_RES_INV;
            }

            trans.x = base->x - draw_ctx->buf_area->x1;
            trans.y = base->y - draw_ctx->buf_area->y1;
            err = PPE_blend_rect(&zoom, &target, &trans, &blend_rect, mode);
            if (compressed)
            {
                lv_mem_free(source.memory);
            }
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
            uint32_t actual_inc = 0;
            int32_t start_column = (constraint_area.x1 - base->x) / zoom_ratio;
            int32_t end_column = ceil((constraint_area.x2 - base->x) / zoom_ratio);

            if (start_column < 0)
            {
                start_column = 0;
            }
            if (end_column >= source.width)
            {
                end_column = source.width - 1;
            }
            for (int y = constraint_area.y1; y < constraint_area.y2; y += actual_inc)
            {
                uint32_t start_line = (y - base->y) / zoom_ratio;
                uint32_t end_line = (y  - base->y + zoom_line_num - extra_line) / zoom_ratio;
                if (end_line >= source_height)
                {
                    end_line = source_height - 1;
                }
                ppe_rect_t scale_rect = {.left = start_column, .top = start_line, .bottom = end_line, .right = end_column};
                zoom.width = (uint32_t)((end_column - start_column + 1) * zoom_ratio);
                zoom.height = (uint32_t)((end_line - start_line + 1) * zoom_ratio);
                if (zoom.width == 0 || zoom.height == 0)
                {
                    break;
                }
                if (compressed)
                {
                    hal_imdc_decompress_info info;
                    info.start_column = scale_rect.left;
                    info.end_column = scale_rect.right;
                    info.start_line = scale_rect.top;
                    info.end_line = scale_rect.bottom;
                    info.raw_data_address = (uint32_t)(map_p + 8);
                    source.width = info.end_column - info.start_column + 1;
                    source.height = info.end_line - info.start_line + 1;
                    source.memory = lv_mem_alloc(source.width * source.height * pixel_byte);
                    source.address = (uint32_t)source.memory;
                    bool ret = hal_imdc_decompress(&info, (uint8_t *)source.memory);
                    scale_rect.top = 0;
                    scale_rect.left = 0;
                    scale_rect.right = source.width - 1;
                    scale_rect.bottom = source.height - 1;
                }
                PPE_ERR err = PPE_Scale_Rect(&source, &zoom, zoom_ratio, zoom_ratio, &scale_rect);
                if (err != PPE_SUCCESS)
                {
                    if (compressed)
                    {
                        lv_mem_free(source.memory);
                    }
                    return LV_RES_INV;
                }
                trans.y = y - draw_ctx->buf_area->y1;
                if (trans.x < 0)
                {
                    trans.x = 0;
                }
                if (y + zoom.height - base->y < new_height)
                {
                    actual_inc = zoom.height - extra_line;
                }
                else
                {
                    actual_inc = zoom.height;
                }
                zoom.height = actual_inc;
                err = PPE_blend_rect(&zoom, &target, &trans, &blend_rect, mode);
                if (compressed)
                {
                    lv_mem_free(source.memory);
                }
                if (err != PPE_SUCCESS)
                {
                    return LV_RES_INV;
                }
                if ((int32_t)(y + actual_inc) >= (int32_t)draw_ctx->buf_area->y2)
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
        if (compressed)
        {
            hal_imdc_decompress_info info;
            info.start_column = constraint_area.x1 - base->x;
            info.end_column = constraint_area.x2 - base->x;
            info.start_line = constraint_area.y1 - base->y;
            info.end_line = constraint_area.y2 - base->y;
            info.raw_data_address = (uint32_t)(map_p + 8);
            source.width = info.end_column - info.start_column + 1;
            source.height = info.end_line - info.start_line + 1;
            source.memory = (uint32_t *)lv_mem_alloc(source.width * source.height * pixel_byte);
            bool ret = hal_imdc_decompress(&info, (uint8_t *)source.memory);
            trans.x = blend_rect.x1;
            trans.y = blend_rect.y1;
        }
        {
            PPE_ERR err = PPE_blend_rect(&source, &target, &trans, &blend_rect, mode);
            if (compressed)
            {
                lv_mem_free(source.memory);
            }
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
    target.format = sizeof(lv_color_t) == 2 ? PPE_RGB565 : PPE_ABGR8888;

    source.address = (uint32_t)map_p;
    source.memory = (uint32_t *)source.address;
    source.width = coords->x2 - coords->x1 + 1;
    source.height = coords->y2 - coords->y1 + 1;
    if (LV_COLOR_DEPTH == 16)
    {
        source.format = PPE_RGB565;
    }
    if (LV_COLOR_DEPTH == 32)
    {
        source.format = PPE_ARGB8888;
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

    if (cf == LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED)
    {
        recolor.color_key_en = ENABLE;
        lv_color32_t chroma = lv_ppe_toABGR8888(LV_COLOR_CHROMA_KEY);
        lv_color32_t combined_chroma;
        combined_chroma.ch.alpha = (recolor_value.ch.alpha * recolor_value.ch.alpha +
                                    (0xFF - recolor_value.ch.alpha) * chroma.ch.alpha) / 0xFF;
        combined_chroma.ch.blue = (recolor_value.ch.red * recolor_value.ch.alpha +
                                   (0xFF - recolor_value.ch.alpha) * chroma.ch.red) / 0xFF;
        combined_chroma.ch.green = (recolor_value.ch.green * recolor_value.ch.alpha +
                                    (0xFF - recolor_value.ch.alpha) * chroma.ch.green) / 0xFF;
        combined_chroma.ch.red = (recolor_value.ch.blue * recolor_value.ch.alpha +
                                  (0xFF - recolor_value.ch.alpha) * chroma.ch.blue) / 0xFF;
        lv_color_t chroma_16 = lv_color_hex(combined_chroma.full);
        recolor.color_key_value = chroma_16.full;
    }

    ppe_translate_t trans = {.x = coords->x1 - draw_ctx->buf_area->x1, .y = coords->y1 - draw_ctx->buf_area->y1};
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
        for (uint16_t y = 0; y < source.height; y += recolor_line_num - 1)
        {
            ppe_rect_t rect = {.left = 0, .right = source.width - 1, .top = y, .bottom = y + recolor_line_num - 1};
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
            trans.y = coords->y1 + y - draw_ctx->buf_area->y1;
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

lv_res_t lv_ppe_map(const lv_area_t *dest_area, lv_draw_ctx_t *draw_ctx,
                    const lv_draw_sw_blend_dsc_t *dsc)
{
    PPE_BLEND_MODE mode = PPE_BYPASS_MODE;

    ppe_buffer_t target, source;
    memset(&target, 0, sizeof(ppe_buffer_t));
    memset(&source, 0, sizeof(ppe_buffer_t));
    target.address = (uint32_t)draw_ctx->buf;
    target.memory = (uint32_t *)target.address;
    target.width = lv_area_get_width(draw_ctx->buf_area);
    target.height = lv_area_get_height(draw_ctx->buf_area);
    target.format = sizeof(lv_color_t) == 2 ? PPE_RGB565 : PPE_ARGB8888;

    source.address = (uint32_t)dsc->src_buf;
    source.memory = (uint32_t *)source.address;
    source.width = dsc->blend_area->x2 - dsc->blend_area->x1 + 1;
    source.height = dsc->blend_area->y2 - dsc->blend_area->y1 + 1;
    source.format = target.format;
    if (dsc->opa < LV_OPA_MIN)
    {
        source.global_alpha_en = true;
        source.global_alpha = dsc->opa;
        mode = PPE_SRC_OVER_MODE;
    }

    ppe_rect_t clip_rect = {.x1 = dsc->blend_area->x1, .x2 = dsc->blend_area->x2,
                            .y1 = dsc->blend_area->y1, .y2 = dsc->blend_area->y2
                           };

    ppe_translate_t trans = {.x = dest_area->x1 - dsc->blend_area->x1, .y = dest_area->y1 - dsc->blend_area->y1};
    PPE_ERR err = PPE_blend_rect(&source, &target, &trans, &clip_rect, mode);
    if (err == PPE_SUCCESS)
    {
        return LV_RES_OK;
    }
    else
    {
        return LV_RES_INV;
    }
}

#endif /*LV_USE_PPE*/
