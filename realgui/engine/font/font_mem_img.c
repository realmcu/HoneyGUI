/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <string.h>
#include "font_mem_img.h"
#include "font_rendering_utils.h"

static void gui_font_bmp2img_one_char(mem_char_t *chr, gui_color_t color, uint8_t render_mode,
                                      gui_text_rect_t *rect, uint8_t *buffer, int buf_width, GUI_FormatType type)
{
    if (chr->dot_addr == NULL || buffer == NULL) { return; }

    int font_x = chr->x;
    int font_y = chr->y + chr->char_y;
    int x_start = font_x;
    int x_end = _UI_MIN(font_x + chr->char_w - 1, rect->x2);
    int y_start = font_y;
    int y_end = _UI_MIN(font_y + chr->char_h - 1, rect->y2);

    if ((x_start >= x_end) || (y_start >= y_end)) { return; }

    uint8_t target_bit_depth = font_get_bitdepth_by_cf(type);

    draw_font_t df =
    {
        .color = color,
        .render_mode = render_mode,
        .target_buf = buffer,
        .target_buf_stride = buf_width * target_bit_depth / 8,
        .target_format = type,
        .clip_rect = {
            .x1 = x_start,
            .y1 = y_start,
            .x2 = x_end,
            .y2 = y_end,
        },
        .target_rect = {
            .x1 = rect->x1,
            .y1 = rect->y1,
            .x2 = rect->x2,
            .y2 = rect->y2,
        }
    };

    font_glyph_t glyph =
    {
        .data = chr->dot_addr,
        .pos_x = font_x,
        .pos_y = font_y,
        .width = chr->char_w,
        .height = chr->char_h,
        .stride = chr->w,
    };

    font_glyph_render(&df, &glyph);
}

void gui_font_scale_destroy(gui_text_t *text)
{
    if (text->scale_img != NULL)
    {
        if (text->scale_img->src.data != NULL)
        {
            FONT_FREE_PSRAM(text->scale_img->src.data);
        }
    }
}

void *gui_text_bmp2img(gui_text_t *text, GUI_FormatType font_img_type, int16_t *img_x,
                       int16_t *img_y)
{
    (void)img_y;
    int16_t buf_width = 0;
    int16_t buf_height = 0;
    uint8_t font_img_pixel_bytes;

    gui_text_rect_t rect = {0};
    rect.x1 = 0;
    rect.y1 = 0;
    rect.x2 = rect.x1 + text->base.w - 1;
    rect.y2 = rect.y1 + text->base.h - 1;
    TEXT_MODE mode = text->mode;
    if (text->mode == CENTER || text->mode == RIGHT)
    {
        text->mode = LEFT;
    }
    gui_font_mem_load(text, &rect);
    text->mode = mode;
    font_img_pixel_bytes = font_get_bitdepth_by_cf(font_img_type) / 8;

    if (font_img_pixel_bytes == 0)
    {
        return NULL;
    }
    switch (text->mode)
    {
    case LEFT:
    case CENTER:
    case RIGHT:
        {
            int offset = _UI_MAX((text->base.w - text->char_width_sum) / 2, 0);
            buf_width = text->base.w - offset * 2;
            buf_height = text->font_height;
            *img_x = offset * text->mode;
            break;
        }
    case MULTI_LEFT:
    case MULTI_CENTER:
    case MULTI_RIGHT:
        {
            buf_width = text->base.w;
            buf_height = text->font_height * text->char_line_sum;
            break;
        }
    // case SCROLL_X:
    // case SCROLL_Y:
    default:
        break;
    }

    uint32_t size = buf_width * buf_height * (uint32_t)font_img_pixel_bytes +
                    sizeof(struct gui_rgb_data_head);
    void *img_buf = FONT_MALLOC_PSRAM(size);
    memset(img_buf, 0x00, size);

    struct gui_rgb_data_head head;
    memset(&head, 0x0, sizeof(head));
    head.w = buf_width;
    head.h = buf_height;
    head.type = font_img_type;
    memcpy(img_buf, &head, sizeof(head));

    mem_char_t *chr = text->data;
    GUI_FONT_HEAD_BMP *font;
    if (text->font_mode == FONT_SRC_FTL || text->font_mode == FONT_SRC_FILESYS)
    {
        FONT_LIB_NODE *node = get_fontlib_by_name(text->path);
        font = (node != NULL) ? (GUI_FONT_HEAD_BMP *)node->cached_data : NULL;
    }
    else
    {
        font = (GUI_FONT_HEAD_BMP *)text->path;
    }
    if (font == NULL)
    {
        gui_free(img_buf);
        return NULL;
    }
    uint8_t render_mode = font->render_mode;
    uint8_t *buffer_addr = (uint8_t *)img_buf + sizeof(struct gui_rgb_data_head);
    rect.x2 = rect.x1 + buf_width - 1;
    rect.y2 = rect.y1 + buf_height - 1;
    for (uint16_t i = 0; i < text->active_font_len; i++)
    {
        gui_font_bmp2img_one_char(chr + i, text->color, render_mode, &rect, buffer_addr, buf_width,
                                  font_img_type);
    }
    gui_font_mem_unload(text);
    return img_buf;
}

