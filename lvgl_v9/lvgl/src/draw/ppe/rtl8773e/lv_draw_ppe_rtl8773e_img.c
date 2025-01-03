/**
 * @file lv_draw_ppe_img.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../lv_draw_private.h"
#if LV_USE_DRAW_PPE_RTL8773E
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
#include "lv_image_decoder.h"

#include "lv_ppe_rtl8773e_utils.h"
#include "hal_idu.h"
#include "string.h"
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
    bool scale = (draw_dsc->scale_x != LV_SCALE_NONE || draw_dsc->scale_y != LV_SCALE_NONE ||
                  draw_dsc->rotation != 0);
    ppe_buffer_t target, source;
    memset(&target, 0, sizeof(ppe_buffer_t));
    memset(&source, 0, sizeof(ppe_buffer_t));
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
    target.address = (uint32_t)draw_unit->target_layer->draw_buf->data;
    target.width = lv_area_get_width(&draw_unit->target_layer->buf_area);
    target.height = lv_area_get_height(&draw_unit->target_layer->buf_area);
    target.stride = target.width;
    target.win_x_min = 0;
    target.win_x_max = target.width - 1;
    target.win_y_min = 0;
    target.win_y_max = target.height - 1;

    PPE_BLEND_MODE mode = PPE_SRC_OVER_MODE;

    source.format = lv_ppe_get_format(img_dsc->header.cf, img_dsc->data);
    uint8_t pixel_byte = PPE_Get_Pixel_Size(source.format);
    source.address = (uint32_t)img_dsc->data;
    source.width = img_dsc->header.w;
    source.height = img_dsc->header.h;
    source.stride = img_dsc->header.stride / pixel_byte;
    source.opacity = draw_dsc->opa;
    source.win_x_min = target.win_x_min;
    source.win_x_max = target.win_x_max;
    source.win_y_min = target.win_y_min;
    source.win_y_max = target.win_y_max;

    if ((source.format == PPE_RGB565 || source.format == PPE_RGB888) && \
        draw_dsc->opa == 0xFF && draw_dsc->rotation == 0)
    {
        mode = PPE_BYPASS_MODE;
    }

    ppe_matrix_t matrix, inverse, pre_trans;

    area_rot = lv_ppe_get_matrix_area(&matrix, coords, draw_dsc);
    ppe_rect_t draw_rect;
    if (!lv_area_intersect(&constraint_area, &constraint_area, &area_rot))
    {
        return;
    }
    else
    {
        draw_rect.x = constraint_area.x1;
        draw_rect.y = constraint_area.y1;
        draw_rect.w = lv_area_get_width(&constraint_area);
        draw_rect.h = lv_area_get_height(&constraint_area);
    }
    ppe_rect_t image_area;
    memcpy(&inverse, &matrix, sizeof(ppe_matrix_t));
    ppe_matrix_inverse(&inverse);
    if (!ppe_get_area(&image_area, &draw_rect, &inverse, &source))
    {
        return;
    }

    uint8_t *pic_buffer = lv_ppe_get_buffer(0);
    if (image_area.w * image_area.h * pixel_byte <= LV_PPE_MAX_BUFFER_SIZE)
    {
        if (!compressed)
        {
            hal_idu_dma_info copy_info;
            copy_info.length = image_area.w * pixel_byte;
            copy_info.height = image_area.h;
            copy_info.src_stride = img_dsc->header.stride;
            copy_info.dst_stride = copy_info.length;
            uint32_t src_addr = source.address + (source.stride * image_area.y + image_area.x) * pixel_byte;
            hal_dma_copy(&copy_info, (uint8_t *)src_addr, pic_buffer);
        }
        else
        {
            hal_idu_decompress_info decode_info;
            decode_info.raw_data_address = (uint32_t)img_dsc->data + 8;
            decode_info.start_column = image_area.x;
            decode_info.end_column = image_area.x + image_area.w - 1;
            decode_info.start_line = image_area.y;
            decode_info.end_line = image_area.y + image_area.h - 1;
            hal_idu_decompress(&decode_info, pic_buffer);
        }
        source.width = image_area.w;
        source.height = image_area.h;
        source.stride = image_area.w;
        source.address = (uint32_t)pic_buffer;
        if (draw_dsc->recolor_opa >= LV_OPA_MIN)
        {
            uint32_t recolor_value = lv_ppe_get_color(draw_dsc->recolor, draw_dsc->recolor_opa);
            ppe_rect_t recolor_rect = {.x = 0, .y = 0, .w = source.width, .h = source.height};
            PPE_Mask(&source, recolor_value, &recolor_rect);
        }
        ppe_get_identity(&pre_trans);
        pre_trans.m[0][2] = image_area.x * -1.0f;
        pre_trans.m[1][2] = image_area.y * -1.0f;
        ppe_mat_multiply(&pre_trans, &inverse);
        ppe_translate(draw_unit->target_layer->buf_area.x1, draw_unit->target_layer->buf_area.y1,
                      &pre_trans);
        memcpy(&inverse, &pre_trans, sizeof(float) * 9);

    }
    else
    {
        ppe_translate(draw_unit->target_layer->buf_area.x1, draw_unit->target_layer->buf_area.y1, &inverse);
    }
    lv_area_move(&constraint_area, -draw_unit->target_layer->buf_area.x1,
                 -draw_unit->target_layer->buf_area.y1);
    draw_rect.x = constraint_area.x1;
    draw_rect.y = constraint_area.y1;
    if (draw_dsc->antialias)
    {
        source.high_quality = true;
    }
    PPE_err err = PPE_Blit_Inverse(&target, &source, &inverse, &draw_rect, mode);
    PPE_Finish();
    if (err == PPE_SUCCESS)
    {
        return;
    }
    return;
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
    uint8_t pixel_byte = PPE_Get_Pixel_Size(source.format);
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
    source.width = img_dsc->header.w;
    source.height = img_dsc->header.h;
    source.stride = img_dsc->header.stride / PPE_Get_Pixel_Size(source.format);
    source.opacity = draw_dsc->opa;
    target.address = (uint32_t)draw_unit->target_layer->draw_buf->data;
    target.width = lv_area_get_width(&draw_unit->target_layer->buf_area);
    target.height = lv_area_get_height(&draw_unit->target_layer->buf_area);
    target.stride = target.width;
    target.win_x_min = 0;
    target.win_x_max = target.width - 1;
    target.win_y_min = 0;
    target.win_y_max = target.height - 1;

    source.win_x_min = target.win_x_min;
    source.win_x_max = target.win_x_max;
    source.win_y_min = target.win_y_min;
    source.win_y_max = target.win_y_max;
    PPE_BLEND_MODE mode = PPE_SRC_OVER_MODE;
    if ((source.format == PPE_RGB565 || source.format == PPE_RGB888) && \
        draw_dsc->opa >= LV_OPA_MAX)
    {
        mode = PPE_BYPASS_MODE;
    }
    uint8_t *pic_buffer = lv_ppe_get_buffer(0);
    uint16_t last_image_x = 0;
    uint16_t last_image_y = 0;
    uint16_t last_image_w = 0;
    uint16_t last_image_h = 0;
    while (tile_area.y1 <= coords->y2)
    {
        while (tile_area.x1 <= coords->x2)
        {
            lv_area_t clipped_img_area;
            if (lv_area_intersect(&clipped_img_area, &tile_area, coords))
            {
                if (lv_area_intersect(&clipped_img_area, &clipped_img_area, &draw_unit->target_layer->buf_area))
                {
                    uint16_t draw_w = lv_area_get_width(&clipped_img_area);
                    uint16_t draw_h = lv_area_get_height(&clipped_img_area);
                    uint16_t image_x = clipped_img_area.x1 - tile_area.x1;
                    uint16_t image_y = clipped_img_area.y1 - tile_area.y1;
                    if (last_image_x != image_x || last_image_y != image_y || last_image_w != draw_w ||
                        last_image_h != draw_h)
                    {
                        if (!compressed)
                        {
                            hal_idu_dma_info copy_info;
                            copy_info.length = draw_w * pixel_byte;
                            copy_info.height = draw_h;
                            copy_info.src_stride = img_dsc->header.stride;
                            copy_info.dst_stride = copy_info.length;
                            uint32_t src_addr = (uint32_t)img_dsc->data + img_dsc->header.stride * image_y + image_x *
                                                pixel_byte;
                            hal_dma_copy(&copy_info, (uint8_t *)src_addr, pic_buffer);
                        }
                        else
                        {
                            hal_idu_decompress_info decode_info;
                            decode_info.raw_data_address = (uint32_t)img_dsc->data + 8;
                            decode_info.start_column = image_x;
                            decode_info.end_column = image_x + draw_w - 1;
                            decode_info.start_line = image_y;
                            decode_info.end_line = image_y + draw_h - 1;
                            hal_idu_decompress(&decode_info, pic_buffer);
                        }
                        source.width = draw_w;
                        source.height = draw_h;
                        source.stride = draw_w;
                        source.address = (uint32_t)pic_buffer;
                        last_image_x = image_x;
                        last_image_y = image_y;
                        last_image_w = draw_w;
                        last_image_h = draw_h;
                        if (draw_dsc->recolor_opa >= LV_OPA_MIN)
                        {
                            uint32_t recolor_value = lv_ppe_get_color(draw_dsc->recolor, draw_dsc->recolor_opa);
                            ppe_rect_t recolor_rect = {.x = 0, .y = 0, .w = source.width, .h = source.height};
                            PPE_Mask(&source, recolor_value, &recolor_rect);
                        }
                    }
                    else
                    {
                        PPE_Finish();
                    }
                    ppe_matrix_t inv_matrix;
                    ppe_get_identity(&inv_matrix);
                    inv_matrix.m[0][2] = -(clipped_img_area.x1 - draw_unit->target_layer->buf_area.x1);
                    inv_matrix.m[1][2] = -(clipped_img_area.y1 - draw_unit->target_layer->buf_area.y1);
                    ppe_rect_t draw_rect = {.x = clipped_img_area.x1 - draw_unit->target_layer->buf_area.x1,
                                            .y = clipped_img_area.y1 - draw_unit->target_layer->buf_area.y1,
                                            .w = draw_w,
                                            .h = draw_h
                                           };
                    if (draw_rect.x < 0)
                    {
                        draw_rect.x = 0;
                    }
                    if (draw_rect.y < 0)
                    {
                        draw_rect.y = 0;
                    }
                    PPE_Finish();
                    PPE_err err = PPE_Blit_Inverse(&target, &source, &inv_matrix, &draw_rect, mode);
                }
            }

            tile_area.x1 += img_w;
            tile_area.x2 += img_w;
        }

        tile_area.y1 += img_h;
        tile_area.y2 += img_h;
        tile_area.x1 = tile_x_start;
        tile_area.x2 = tile_x_start + img_w - 1;
    }
    PPE_Finish();
}
#endif /*LV_USE_PPE*/
