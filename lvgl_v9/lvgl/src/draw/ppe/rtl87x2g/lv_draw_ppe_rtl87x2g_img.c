/**
 * @file lv_draw_ppe_img.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_draw_private.h"
#if LV_USE_DRAW_PPE_RTL872xG
#include "../../misc/lv_area_private.h"
#include "blend/lv_draw_sw_blend_private.h"
#include "../lv_image_decoder_private.h"
#include "../lv_draw_image_private.h"
#include "../../display/lv_display.h"
#include "../../display/lv_display_private.h"
#include "../../misc/lv_log.h"
#include "../../core/lv_refr_private.h"
#include "../../stdlib/lv_mem.h"
#include "../../misc/lv_math.h"
#include "../../misc/lv_color.h"
#include "../../stdlib/lv_string.h"
#include "../../core/lv_global.h"

#include "lv_ppe_rtl87x2g_utils.h"
#include "hal_idu.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_draw_ppe_normal(lv_draw_unit_t *draw_unit, const lv_draw_image_dsc_t *draw_dsc,
                               const lv_area_t *coords);
static void lv_draw_ppe_tile(lv_draw_unit_t *draw_unit, const lv_draw_image_dsc_t *draw_dsc,
                             const lv_area_t *coords);
/**********************
 *  STATIC VARIABLES
 **********************/
#define _draw_info LV_GLOBAL_DEFAULT()->draw_info

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_draw_ppe_image(lv_draw_unit_t *draw_unit, const lv_draw_image_dsc_t *draw_dsc,
                       const lv_area_t *coords)
{
    if (draw_dsc->opa <= (lv_opa_t)LV_OPA_MIN)
    {
        return;
    }

    if (!draw_dsc->tile)
    {
        lv_draw_ppe_normal(draw_unit, draw_dsc, coords);
    }
    else
    {
        lv_draw_ppe_tile(draw_unit, draw_dsc, coords);
    }
}

void lv_draw_ppe_layer(lv_draw_unit_t *draw_unit, const lv_draw_image_dsc_t *draw_dsc,
                       const lv_area_t *coords)
{
    lv_layer_t *layer_to_draw = (lv_layer_t *)draw_dsc->src;

    /*It can happen that nothing was draw on a layer and therefore its buffer is not allocated.
     *In this case just return. */
    if (layer_to_draw->draw_buf == NULL) { return; }

    lv_draw_image_dsc_t new_draw_dsc = *draw_dsc;
    new_draw_dsc.src = layer_to_draw->draw_buf;
    lv_draw_ppe_image(draw_unit, &new_draw_dsc, coords);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_draw_ppe_normal(lv_draw_unit_t *draw_unit, const lv_draw_image_dsc_t *draw_dsc,
                               const lv_area_t *coords)
{
    lv_layer_t *layer = draw_unit->target_layer;
    const lv_image_dsc_t *img_dsc = draw_dsc->src;
    lv_area_t area_rot;
    lv_area_copy(&area_rot, coords);
    lv_area_t constraint_area;
    bool compressed = false;
    if (!lv_area_intersect(&constraint_area, &draw_unit->target_layer->buf_area, draw_unit->clip_area))
    {
        return;
    }
    bool scale = (draw_dsc->scale_x != LV_SCALE_NONE || draw_dsc->scale_y != LV_SCALE_NONE);

    ppe_buffer_t target, source;
    memset(&target, 0, sizeof(ppe_buffer_t));
    memset(&source, 0, sizeof(ppe_buffer_t));
    source.format = lv_ppe_get_format(img_dsc->header.cf, img_dsc->data);
    uint8_t pixel_byte = ppe_get_format_data_len(source.format);
    if (img_dsc->header.cf == LV_COLOR_FORMAT_RAW)
    {
        compressed = true;
    }
    target.format = PPE_ABGR8888;
    switch (draw_unit->target_layer->color_format)
    {
    case LV_COLOR_FORMAT_RGB565:
        target.format = PPE_RGB565;
        break;
    case LV_COLOR_FORMAT_ARGB8888:
        target.format = PPE_ARGB8888;
        break;
    case LV_COLOR_FORMAT_RGB888:
        target.format = PPE_RGB888;
        break;
    case LV_COLOR_FORMAT_XRGB8888:
        target.format = PPE_XRGB8888;
        break;
    default:
        lv_draw_sw_image(draw_unit, draw_dsc, coords);
        return;
    }
    source.address = (uint32_t)img_dsc->data;
    source.memory = (uint32_t *)img_dsc->data;
    source.width = img_dsc->header.w;
    source.height = img_dsc->header.h;
    source.stride = img_dsc->header.stride / pixel_byte;
    if (draw_dsc->opa < LV_OPA_MAX)
    {
        source.global_alpha_en = true;
        source.global_alpha = draw_dsc->opa;
    }
    target.address = (uint32_t)draw_unit->target_layer->draw_buf->data;
    target.memory = (uint32_t *)draw_unit->target_layer->draw_buf->data;
    target.width = lv_area_get_width(&draw_unit->target_layer->buf_area);
    target.height = lv_area_get_height(&draw_unit->target_layer->buf_area);
    target.global_alpha_en = false;
    PPE_BLEND_MODE mode = PPE_SRC_OVER_MODE;
    if ((source.format == PPE_RGB565 || source.format == PPE_RGB888) && \
        !source.global_alpha_en)
    {
        mode = PPE_BYPASS_MODE;
    }

    if (scale)
    {
        int32_t w = lv_area_get_width(coords);
        int32_t h = lv_area_get_height(coords);

        lv_image_buf_get_transformed_area(&area_rot, w, h, 0, draw_dsc->scale_x, draw_dsc->scale_y,
                                          &draw_dsc->pivot);

        area_rot.x1 += coords->x1;
        area_rot.y1 += coords->y1;
        area_rot.x2 += coords->x1;
        area_rot.y2 += coords->y1;

        if (!lv_area_intersect(&constraint_area, &constraint_area, &area_rot))
        {
            return;
        }

        ppe_buffer_t zoom;
        memset(&zoom, 0, sizeof(ppe_buffer_t));
        float scale_x = draw_dsc->scale_x * 1.0f / LV_SCALE_NONE;
        float scale_y = draw_dsc->scale_y * 1.0f / LV_SCALE_NONE;
        int32_t start_column = (constraint_area.x1 - area_rot.x1) / scale_x;
        int32_t end_column = ceil((constraint_area.x2 - area_rot.x1) / scale_x);
        if (end_column >= source.width)
        {
            end_column = source.width - 1;
        }
        int32_t start_line = (constraint_area.y1 - area_rot.y1) / scale_y;
        int32_t end_line = ceil((constraint_area.y2 - area_rot.y1) / scale_y);
        uint32_t scaled_w = ceil((end_column - start_column + 1) * scale_x);
        uint32_t scaled_h = ceil((end_line - start_line + 1) * scale_y);
        uint32_t scale_image_size = scaled_w * scaled_h * pixel_byte;
        zoom.format = source.format;
        if (scale_image_size < LV_PPE_MAX_BUFFER_SIZE)
        {
            zoom.memory = (uint32_t *)lv_ppe_get_buffer(scale_image_size);
            zoom.address = (uint32_t)zoom.memory;
            ppe_rect_t scale_rect = {.x1 = start_column, .x2 = end_column,
                                     .y1 = start_line, .y2 = end_line
                                    };
            if (compressed)
            {
                hal_idu_decompress_info info;
                info.start_column = scale_rect.left;
                info.end_column = scale_rect.right;
                info.start_line = scale_rect.top;
                info.end_line = scale_rect.bottom;
                info.raw_data_address = (uint32_t)img_dsc->data + 8;
                source.width = info.end_column - info.start_column + 1;
                source.height = info.end_line - info.start_line + 1;
                source.memory = (uint32_t *)lv_malloc(source.width * source.height * pixel_byte);
                source.address = (uint32_t)source.memory;
                source.stride = source.width;
                bool ret = hal_idu_decompress(&info, (uint8_t *)source.memory);
                scale_rect.top = 0;
                scale_rect.left = 0;
                scale_rect.right = source.width - 1;
                scale_rect.bottom = source.height - 1;
            }
            PPE_ERR err = PPE_Scale_Rect(&source, &zoom, scale_x, scale_y, &scale_rect);
            if (err != PPE_SUCCESS)
            {
                if (compressed)
                {
                    lv_free(source.memory);
                }
                lv_free(zoom.memory);
                return;
            }
            zoom.stride = zoom.width;
            if (draw_dsc->recolor_opa > LV_OPA_MIN && draw_dsc->recolor_opa < LV_OPA_MAX)
            {
                ppe_buffer_t recolor;
                memset(&recolor, 0, sizeof(ppe_buffer_t));
                recolor.memory = zoom.memory;
                recolor.address = (uint32_t)recolor.memory;
                recolor.color_key_en = DISABLE;
                recolor.color_key_value = 0;
                recolor.format = zoom.format;
                recolor.width = zoom.width;
                recolor.height = zoom.height;
                recolor.stride = zoom.stride;
                uint32_t recolor_value = lv_ppe_get_color(draw_dsc->recolor, draw_dsc->recolor_opa);
                ppe_rect_t recolor_rect = {.x1 = 0, .x2 = zoom.width - 1,
                                           .y1 = 0, .y2 = zoom.height - 1
                                          };
                lv_ppe_recolor(&zoom, &recolor, &recolor_rect, recolor_value);
            }
            ppe_rect_t draw_rect = {.x1 = constraint_area.x1 - draw_unit->target_layer->buf_area.x1,
                                    .x2 = constraint_area.x2 - draw_unit->target_layer->buf_area.x1,
                                    .y1 = constraint_area.y1 - draw_unit->target_layer->buf_area.y1,
                                    .y2 = constraint_area.y2 - draw_unit->target_layer->buf_area.y1
                                   };
            ppe_translate_t trans = {.x = draw_rect.x1, .y = draw_rect.y1};
            if (draw_dsc->opa < LV_OPA_MAX)
            {
                zoom.global_alpha_en = true;
                zoom.global_alpha = draw_dsc->opa;
            }
            PPE_blend_rect(&zoom, &target, &trans, &draw_rect, mode);
            lv_free(zoom.memory);
        }
        else
        {
            zoom.memory = (uint32_t *)lv_ppe_get_buffer(LV_PPE_MAX_BUFFER_SIZE);
            zoom.address = (uint32_t)zoom.memory;
            uint16_t line_num = LV_PPE_MAX_BUFFER_SIZE / (scaled_w * pixel_byte) - 1;
            uint16_t actual_inc = 0;
            uint32_t t = 0;
            for (int y = constraint_area.y1; y <= constraint_area.y2; y += actual_inc)
            {
                t++;
                start_line = (y - area_rot.y1);
                end_line = (start_line + line_num) / scale_y;
                if (end_line >= img_dsc->header.h)
                {
                    end_line = img_dsc->header.h - 1;
                }
                start_line = start_line / scale_y;
                ppe_rect_t scale_rect = {.x1 = start_column, .x2 = end_column,
                                         .y1 = start_line, .y2 = end_line
                                        };
                zoom.width = (uint32_t)((end_column - start_column + 1) * scale_x);
                zoom.height = (uint32_t)((end_line - start_line + 1) * scale_y);

                if (zoom.width == 0 || zoom.height == 0)
                {
                    break;
                }

                if (compressed)
                {
                    hal_idu_decompress_info info;
                    info.start_column = scale_rect.left;
                    info.end_column = scale_rect.right;
                    info.start_line = scale_rect.top;
                    info.end_line = scale_rect.bottom;
                    info.raw_data_address = (uint32_t)img_dsc->data + 8;
                    source.width = info.end_column - info.start_column + 1;
                    source.height = info.end_line - info.start_line + 1;
                    source.memory = (uint32_t *)lv_malloc(source.width * source.height * pixel_byte);
                    source.address = (uint32_t)source.memory;
                    source.stride = source.width;
                    bool ret = hal_idu_decompress(&info, (uint8_t *)source.memory);
                    scale_rect.top = 0;
                    scale_rect.left = 0;
                    scale_rect.right = source.width - 1;
                    scale_rect.bottom = source.height - 1;
                }

                PPE_ERR err = PPE_Scale_Rect(&source, &zoom, scale_x, scale_y, &scale_rect);
                zoom.stride = zoom.width;
                if (err != PPE_SUCCESS)
                {
                    if (compressed)
                    {
                        lv_free(source.memory);
                    }
                    lv_free(zoom.memory);
                    return;
                }
                ppe_translate_t trans = {.x = area_rot.x1 - draw_unit->target_layer->buf_area.x1,
                                         .y = y - draw_unit->target_layer->buf_area.y1
                                        };
                if (trans.x < 0)
                {
                    trans.x = 0;
                }
                actual_inc = zoom.height - 1;
                zoom.height = actual_inc;
                if (draw_dsc->recolor_opa > LV_OPA_MIN && draw_dsc->recolor_opa < LV_OPA_MAX)
                {
                    ppe_buffer_t recolor;
                    memset(&recolor, 0, sizeof(ppe_buffer_t));
                    recolor.memory = zoom.memory;
                    recolor.address = (uint32_t)recolor.memory;
                    recolor.color_key_en = DISABLE;
                    recolor.color_key_value = 0;
                    recolor.format = zoom.format;
                    recolor.width = zoom.width;
                    recolor.height = zoom.height;
                    uint32_t recolor_value = lv_ppe_get_color(draw_dsc->recolor, draw_dsc->recolor_opa);
                    ppe_rect_t recolor_rect = {.x1 = 0, .x2 = zoom.width - 1,
                                               .y1 = 0, .y2 = zoom.height - 1
                                              };
                    lv_ppe_recolor(&zoom, &recolor, &recolor_rect, recolor_value);
                }

                ppe_rect_t draw_rect = {.x1 = constraint_area.x1 - draw_unit->target_layer->buf_area.x1,
                                        .x2 = constraint_area.x2 - draw_unit->target_layer->buf_area.x1,
                                        .y1 = y - draw_unit->target_layer->buf_area.y1,
                                        .y2 = y + actual_inc - draw_unit->target_layer->buf_area.y1 - 1
                                       };
                err = PPE_blend_rect(&zoom, &target, &trans, &draw_rect, mode);
                if (compressed)
                {
                    lv_free(source.memory);
                }
                if (err != PPE_SUCCESS)
                {
                    lv_free(zoom.memory);
                    return;
                }
                if ((int32_t)(y + actual_inc) >= (int32_t)draw_unit->target_layer->buf_area.y2)
                {
                    break;
                }
            }
            lv_free(zoom.memory);
        }
    }
    else
    {
        if (!lv_area_intersect(&constraint_area, &constraint_area, &area_rot))
        {
            return;
        }
        ppe_rect_t draw_rect = {.x1 = constraint_area.x1 - draw_unit->target_layer->buf_area.x1,
                                .x2 = constraint_area.x2 - draw_unit->target_layer->buf_area.x1,
                                .y1 = constraint_area.y1 - draw_unit->target_layer->buf_area.y1,
                                .y2 = constraint_area.y2 - draw_unit->target_layer->buf_area.y1
                               };
        ppe_translate_t trans = {.x = area_rot.x1 - draw_unit->target_layer->buf_area.x1,
                                 .y = area_rot.y1 - draw_unit->target_layer->buf_area.y1
                                };
        if (draw_dsc->recolor_opa >= LV_OPA_MIN)
        {
            ppe_buffer_t recolor;
            memset(&recolor, 0, sizeof(ppe_buffer_t));
            uint16_t line_num = LV_PPE_MAX_BUFFER_SIZE / ((draw_rect.x2 - draw_rect.x1 + 1) * pixel_byte);
            ppe_rect_t source_rect = {.left = trans.x, .top = trans.y, .right = trans.x + source.width - 1, .bottom = trans.y + source.height - 1};
            if (!ppe_rect_intersect(&source_rect, &source_rect, &draw_rect))
            {
                return;
            }
            ppe_translate_t recolor_pos = {.x = source_rect.x1, .y = source_rect.y1};
            source_rect.left = source_rect.x1 - trans.x;
            source_rect.top = source_rect.y1 - trans.y;
            source_rect.right = source_rect.x2 - trans.x;
            source_rect.bottom = source_rect.y2 - trans.y;

            recolor.color_key_en = DISABLE;
            recolor.color_key_value = 0;
            recolor.format = source.format;
            recolor.width = (draw_rect.x2 - draw_rect.x1 + 1);
            recolor.stride = recolor.width;
            recolor.memory = lv_malloc(line_num * recolor.width * pixel_byte);
            recolor.address = (uint32_t)recolor.memory;
            for (int y = source_rect.y1; y <= source_rect.y2; y += line_num)
            {
                if ((y + line_num) > source_rect.y2)
                {
                    recolor.height = source_rect.y2 - y + 1;
                }
                else
                {
                    recolor.height = line_num;
                }
                uint32_t recolor_value = lv_ppe_get_color(draw_dsc->recolor, draw_dsc->recolor_opa);
                ppe_rect_t recolor_rect = {.x1 = source_rect.x1, .x2 = source_rect.x2,
                                           .y1 = y, .y2 = y + recolor.height - 1
                                          };
                if (compressed)
                {
                    hal_idu_decompress_info info;
                    info.start_column = recolor_rect.x1;
                    info.end_column = recolor_rect.x2;
                    info.start_line = recolor_rect.y1;
                    info.end_line = recolor_rect.y2;
                    info.raw_data_address = (uint32_t)img_dsc->data + 8;
                    source.width = info.end_column - info.start_column + 1;
                    source.height = info.end_line - info.start_line + 1;
                    source.stride = source.width;
                    uint16_t malloc_size = source.width * source.height * pixel_byte;
                    source.memory = (uint32_t *)lv_malloc(malloc_size);
                    source.address = (uint32_t)source.memory;
                    hal_idu_decompress(&info, (uint8_t *)source.memory);
                }
                lv_ppe_recolor(&source, &recolor, &recolor_rect, recolor_value);
                draw_rect.y2 = draw_rect.y1 + recolor.height - 1;
                PPE_blend_rect(&recolor, &target, &recolor_pos, &draw_rect, mode);
                recolor_pos.y += recolor.height;
                draw_rect.y1 = draw_rect.y2 + 1;
                if (compressed)
                {
                    lv_free(source.memory);
                }
            }
            lv_free(recolor.memory);
        }
        else
        {
            if (compressed)
            {
                uint16_t line_num = LV_PPE_MAX_BUFFER_SIZE / ((draw_rect.x2 - draw_rect.x1 + 1) * pixel_byte);
                ppe_rect_t source_rect = {.left = trans.x, .top = trans.y, .right = trans.x + source.width - 1, .bottom = trans.y + source.height - 1};
                if (!ppe_rect_intersect(&source_rect, &source_rect, &draw_rect))
                {
                    return;
                }
                ppe_translate_t recolor_pos = {.x = source_rect.x1, .y = source_rect.y1};
                source_rect.left = source_rect.x1 - trans.x;
                source_rect.top = source_rect.y1 - trans.y;
                source_rect.right = source_rect.x2 - trans.x;
                source_rect.bottom = source_rect.y2 - trans.y;
                source.width = (draw_rect.x2 - draw_rect.x1 + 1);
                source.memory = lv_malloc(line_num * source.width * pixel_byte);
                source.address = (uint32_t)source.memory;
                for (int y = source_rect.y1; y <= source_rect.y2; y += line_num)
                {
                    if ((y + line_num) > source_rect.y2)
                    {
                        source.height = source_rect.y2 - y + 1;
                    }
                    else
                    {
                        source.height = line_num;
                    }
                    ppe_rect_t decompress_rect = {.x1 = source_rect.x1, .x2 = source_rect.x2,
                                                  .y1 = y, .y2 = y + source.height - 1
                                                 };
                    if (compressed)
                    {
                        hal_idu_decompress_info info;
                        info.start_column = decompress_rect.x1;
                        info.end_column = decompress_rect.x2;
                        info.start_line = decompress_rect.y1;
                        info.end_line = decompress_rect.y2;
                        info.raw_data_address = (uint32_t)img_dsc->data + 8;
                        source.width = info.end_column - info.start_column + 1;
                        source.height = info.end_line - info.start_line + 1;
                        source.stride = source.width;
                        uint16_t malloc_size = source.width * source.height * pixel_byte;
                        source.memory = (uint32_t *)lv_malloc(malloc_size);
                        source.address = (uint32_t)source.memory;
                        hal_idu_decompress(&info, (uint8_t *)source.memory);
                    }
                    draw_rect.y2 = draw_rect.y1 + source.height - 1;
                    PPE_blend_rect(&source, &target, &recolor_pos, &draw_rect, mode);
                    recolor_pos.y += source.height;
                    draw_rect.y1 = draw_rect.y2 + 1;
                    if (compressed)
                    {
                        lv_free(source.memory);
                    }
                }
            }
            else
            {
                PPE_blend_rect(&source, &target, &trans, &draw_rect, mode);
            }
        }
    }
}

static void lv_draw_ppe_tile(lv_draw_unit_t *draw_unit, const lv_draw_image_dsc_t *draw_dsc,
                             const lv_area_t *coords)
{
    const lv_image_dsc_t *img_dsc = draw_dsc->src;
    bool compressed = false;
    int32_t img_w = img_dsc->header.w;
    int32_t img_h = img_dsc->header.h;

    lv_area_t tile_area;
    if (lv_area_get_width(&draw_dsc->image_area) >= 0)
    {
        tile_area = draw_dsc->image_area;
    }
    else
    {
        tile_area = *coords;
    }
    lv_area_set_width(&tile_area, img_w);
    lv_area_set_height(&tile_area, img_h);

    int32_t tile_x_start = tile_area.x1;

    lv_area_t relative_decoded_area =
    {
        .x1 = LV_COORD_MIN,
        .y1 = LV_COORD_MIN,
        .x2 = LV_COORD_MIN,
        .y2 = LV_COORD_MIN,
    };

    uint32_t recolor = lv_ppe_get_color(draw_dsc->recolor, 0);
    ppe_buffer_t target, source;
    memset(&target, 0, sizeof(ppe_buffer_t));
    memset(&source, 0, sizeof(ppe_buffer_t));
    source.format = lv_ppe_get_format(img_dsc->header.cf, img_dsc->data);
    if (img_dsc->header.cf == LV_COLOR_FORMAT_RAW)
    {
        compressed = true;
    }
    if (source.format == PPE_FORMAT_NOT_SUPPORT || (source.format == PPE_A8 && recolor != 0))
    {
        lv_draw_sw_image(draw_unit, draw_dsc, coords);
        return;
    }
    target.format = PPE_ABGR8888;
    switch (draw_unit->target_layer->color_format)
    {
    case LV_COLOR_FORMAT_RGB565:
        target.format = PPE_RGB565;
        break;
    case LV_COLOR_FORMAT_ARGB8888:
        target.format = PPE_ARGB8888;
        break;
    case LV_COLOR_FORMAT_RGB888:
        target.format = PPE_RGB888;
        break;
    case LV_COLOR_FORMAT_XRGB8888:
        target.format = PPE_XRGB8888;
        break;
    default:
        lv_draw_sw_image(draw_unit, draw_dsc, coords);
        return;
    }
    source.address = (uint32_t)img_dsc->data;
    source.memory = (uint32_t *)img_dsc->data;
    source.width = img_dsc->header.w;
    source.height = img_dsc->header.h;
    source.stride = img_dsc->header.stride / ppe_get_format_data_len(source.format);
    if (draw_dsc->opa < LV_OPA_MAX)
    {
        source.global_alpha_en = true;
        source.global_alpha = draw_dsc->opa;
    }

    target.address = (uint32_t)draw_unit->target_layer->draw_buf->data;
    target.memory = (uint32_t *)draw_unit->target_layer->draw_buf->data;
    target.width = lv_area_get_width(&draw_unit->target_layer->buf_area);
    target.height = lv_area_get_height(&draw_unit->target_layer->buf_area);
    target.global_alpha_en = false;
    PPE_BLEND_MODE mode = PPE_SRC_OVER_MODE;
    if ((source.format == PPE_RGB565 || source.format == PPE_RGB888) && \
        !source.global_alpha_en)
    {
        mode = PPE_BYPASS_MODE;
    }
    while (tile_area.y1 <= coords->y2)
    {
        while (tile_area.x1 <= coords->x2)
        {

            lv_area_t clipped_img_area;
            if (lv_area_intersect(&clipped_img_area, &tile_area, coords))
            {
                ppe_rect_t draw_rect = {.x1 = clipped_img_area.x1 - draw_unit->target_layer->buf_area.x1,
                                        .x2 = clipped_img_area.x2 - draw_unit->target_layer->buf_area.x1,
                                        .y1 = clipped_img_area.y1 - draw_unit->target_layer->buf_area.y1,
                                        .y2 = clipped_img_area.y2 - draw_unit->target_layer->buf_area.y1
                                       };
                ppe_translate_t trans_pos = {.x = tile_area.x1 - draw_unit->target_layer->buf_area.x1,
                                             .y = tile_area.y1 - draw_unit->target_layer->buf_area.y1
                                            };
                PPE_blend_rect(&source, &target, &trans_pos, &draw_rect, mode);
            }

            tile_area.x1 += img_w;
            tile_area.x2 += img_w;
        }

        tile_area.y1 += img_h;
        tile_area.y2 += img_h;
        tile_area.x1 = tile_x_start;
        tile_area.x2 = tile_x_start + img_w - 1;
    }
}
#endif /*LV_USE_PPE*/
