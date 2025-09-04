#include <string.h>
#include "draw_font.h"
#include "font_mem.h"
#include "font_mem_matrix.h"
#include "acc_api.h"
#include "font_rendering_utils.h"

static void rtk_draw_unicode_matrix(mem_char_t *chr, gui_color_t color, uint8_t rendor_mode,
                                    gui_text_rect_t *rect, float scale)
{
    if (chr->dot_addr == NULL)
    {
        return;
    }
    uint8_t *dots = chr->dot_addr;
    gui_dispdev_t *dc = gui_get_dc();
    int font_x = chr->x;
    int font_y = chr->y + chr->char_y;
    int font_w = chr->w;
    int x_start = _UI_MAX(_UI_MAX(font_x, rect->xboundleft), dc->section.x1);
    int x_end;
    if (rect->xboundright != 0)
    {
        x_end = _UI_MIN(_UI_MIN(font_x + chr->char_w, dc->section.x2 + 1), rect->xboundright);
    }
    else
    {
        x_end = _UI_MIN(font_x + chr->char_w, dc->section.x2 + 1);
    }
    int y_start = _UI_MAX(dc->section.y1, _UI_MAX(font_y, rect->yboundtop));
    int y_end;
    if (rect->yboundbottom != 0)
    {
        y_end = _UI_MIN(_UI_MIN(dc->section.y2 + 1, font_y + chr->char_h), rect->yboundbottom);
    }
    else
    {
        y_end = _UI_MIN(dc->section.y2 + 1, font_y + chr->char_h);
    }
    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    switch (rendor_mode)
    {
    case 2:
        if (dc_bytes_per_pixel == 2)
        {
            uint16_t *writebuf = (uint16_t *)dc->frame_buf;
            uint16_t color_output = rgba2565(color);
            uint16_t color_back;
            uint8_t alpha_list[1 << rendor_mode];
            uint8_t ppb = 4;//pixel_per_byte = 8 / rendor_mode
            int pre_alpha = 0x100 / ((1 << rendor_mode) - 1);
            for (int i = 0; i < 1 << rendor_mode; i++)
            {
                alpha_list[i] = i * pre_alpha;
            }
            for (int32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) ;
                for (int32_t j = x_start; j < x_end; j++)
                {
                    uint8_t old_y = (i - font_y) / scale;
                    uint8_t old_x = (j - font_x) / scale;
                    uint8_t alpha = dots[old_y * (font_w / ppb) + old_x / ppb] >> (old_x % ppb * 2);
                    if (alpha != 0)
                    {
                        alpha = alpha & 0x03;
                        alpha = alpha_list[alpha];
                        alpha = color.color.rgba.a * alpha / 0xff;
                        color_back = writebuf[write_off + j - dc->section.x1];
                        writebuf[write_off + j - dc->section.x1] = alphaBlendRGB565(color_output, color_back, alpha);
                    }
                }
            }
        }
        else if (dc_bytes_per_pixel == 3)
        {
            uint8_t *writebuf = (uint8_t *)dc->frame_buf;
            uint8_t color_back[3];
            uint8_t alpha_list[1 << rendor_mode];
            uint8_t ppb = 4;//pixel_per_byte = 8 / rendor_mode
            int pre_alpha = 0x100 / ((1 << rendor_mode) - 1);
            for (int i = 0; i < 1 << rendor_mode; i++)
            {
                alpha_list[i] = i * pre_alpha;
            }
            for (int32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) ;
                for (int32_t j = x_start; j < x_end; j++)
                {
                    uint8_t old_y = (i - font_y) / scale;
                    uint8_t old_x = (j - font_x) / scale;
                    uint8_t alpha = dots[old_y * (font_w / ppb) + old_x / ppb] >> (old_x % ppb * 2);
                    if (alpha != 0)
                    {
                        alpha = alpha & 0x03;
                        alpha = alpha_list[alpha];
                        alpha = color.color.rgba.a * alpha / 0xff;
                        color_back[0] = writebuf[write_off * 3 + j * 3 + 2 - dc->section.x1];
                        color_back[1] = writebuf[write_off * 3 + j * 3 + 1 - dc->section.x1];
                        color_back[2] = writebuf[write_off * 3 + j * 3 + 0 - dc->section.x1];
                        writebuf[write_off * 3 + j * 3 + 0 - dc->section.x1] = (color.color.rgba.b * alpha + color_back[2] *
                                                                                (0xff - alpha)) / 0xff;
                        writebuf[write_off * 3 + j * 3 + 1 - dc->section.x1] = (color.color.rgba.g * alpha + color_back[1] *
                                                                                (0xff - alpha)) / 0xff;
                        writebuf[write_off * 3 + j * 3 + 2 - dc->section.x1] = (color.color.rgba.r * alpha + color_back[0] *
                                                                                (0xff - alpha)) / 0xff;
                    }
                }
            }
        }
        else if (dc_bytes_per_pixel == 4)
        {
            uint32_t *writebuf = (uint32_t *)dc->frame_buf;
            uint32_t color_back;
            uint8_t alpha_list[1 << rendor_mode];
            uint8_t ppb = 4;//pixel_per_byte = 8 / rendor_mode
            int pre_alpha = 0x100 / ((1 << rendor_mode) - 1);
            for (int i = 0; i < 1 << rendor_mode; i++)
            {
                alpha_list[i] = i * pre_alpha;
            }
            for (int32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) ;
                for (int32_t j = x_start; j < x_end; j++)
                {
                    uint8_t old_y = (i - font_y) / scale;
                    uint8_t old_x = (j - font_x) / scale;
                    uint8_t alpha = dots[old_y * (font_w / ppb) + old_x / ppb] >> (old_x % ppb * 2);
                    if (alpha != 0)
                    {
                        alpha = alpha & 0x03;
                        alpha = alpha_list[alpha];
                        alpha = color.color.rgba.a * alpha / 0xff;
                        color_back = writebuf[write_off + j - dc->section.x1];
                        writebuf[write_off + j - dc->section.x1] = alphaBlendRGBA(color, color_back, alpha);
                    }
                }
            }
        }
        break;
    case 4:
        if (dc_bytes_per_pixel == 2)
        {
            uint16_t *writebuf = (uint16_t *)dc->frame_buf;
            uint16_t color_back;
            uint16_t color_output = rgba2565(color);
            uint8_t alpha_list[1 << rendor_mode];
            uint8_t ppb = 2;//pixel_per_byte = 8 / rendor_mode
            int pre_alpha = 0x100 / ((1 << rendor_mode) - 1);
            for (int i = 0; i < 1 << rendor_mode; i++)
            {
                alpha_list[i] = i * pre_alpha;
            }
            for (int32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) ;
                for (int32_t j = x_start; j < x_end; j++)
                {
                    uint8_t old_y = (i - font_y) / scale;
                    uint8_t old_x = (j - font_x) / scale;
                    uint8_t alpha = (dots[old_y * (font_w / ppb) + old_x / ppb] >> (old_x % ppb * 4));
                    if (alpha != 0)
                    {
                        alpha = alpha & 0x0f;
                        alpha = alpha_list[alpha];
                        alpha = color.color.rgba.a * alpha / 0xff;
                        color_back = writebuf[write_off + j - dc->section.x1];
                        writebuf[write_off + j - dc->section.x1] = alphaBlendRGB565(color_output, color_back, alpha);
                    }
                }
            }
        }
        else if (dc_bytes_per_pixel == 3)
        {
            uint8_t *writebuf = (uint8_t *)dc->frame_buf;
            uint8_t color_back[3];
            uint8_t alpha_list[1 << rendor_mode];
            uint8_t ppb = 2;//pixel_per_byte = 8 / rendor_mode
            int pre_alpha = 0x100 / ((1 << rendor_mode) - 1);
            for (int i = 0; i < 1 << rendor_mode; i++)
            {
                alpha_list[i] = i * pre_alpha;
            }
            for (int32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) ;
                for (int32_t j = x_start; j < x_end; j++)
                {
                    uint8_t old_y = (i - font_y) / scale;
                    uint8_t old_x = (j - font_x) / scale;
                    uint8_t alpha = (dots[old_y * (font_w / ppb) + old_x / ppb] >> (old_x % ppb * 4));
                    if (alpha != 0)
                    {
                        alpha = alpha & 0x0f;
                        alpha = alpha_list[alpha];
                        alpha = color.color.rgba.a * alpha / 0xff;
                        color_back[0] = writebuf[write_off * 3 + j * 3 + 2 - dc->section.x1];
                        color_back[1] = writebuf[write_off * 3 + j * 3 + 1 - dc->section.x1];
                        color_back[2] = writebuf[write_off * 3 + j * 3 + 0 - dc->section.x1];
                        writebuf[write_off * 3 + j * 3 + 0 - dc->section.x1] = (color.color.rgba.b * alpha + color_back[2] *
                                                                                (0xff - alpha)) / 0xff;
                        writebuf[write_off * 3 + j * 3 + 1 - dc->section.x1] = (color.color.rgba.g * alpha + color_back[1] *
                                                                                (0xff - alpha)) / 0xff;
                        writebuf[write_off * 3 + j * 3 + 2 - dc->section.x1] = (color.color.rgba.r * alpha + color_back[0] *
                                                                                (0xff - alpha)) / 0xff;
                    }
                }
            }
        }
        else if (dc_bytes_per_pixel == 4)
        {
            uint32_t *writebuf = (uint32_t *)dc->frame_buf;
            uint32_t color_back;
            uint8_t alpha_list[1 << rendor_mode];
            uint8_t ppb = 2;//pixel_per_byte = 8 / rendor_mode
            int pre_alpha = 0x100 / ((1 << rendor_mode) - 1);
            for (int i = 0; i < 1 << rendor_mode; i++)
            {
                alpha_list[i] = i * pre_alpha;
            }
            for (int32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) ;
                for (int32_t j = x_start; j < x_end; j++)
                {
                    uint8_t old_y = (i - font_y) / scale;
                    uint8_t old_x = (j - font_x) / scale;
                    uint8_t alpha = (dots[old_y * (font_w / ppb) + old_x / ppb] >> (old_x % ppb * 4));
                    if (alpha != 0)
                    {
                        alpha = alpha & 0x0f;
                        alpha = alpha_list[alpha];
                        alpha = color.color.rgba.a * alpha / 0xff;
                        color_back = writebuf[write_off + j - dc->section.x1];
                        writebuf[write_off + j - dc->section.x1] = alphaBlendRGBA(color, color_back, alpha);
                    }
                }
            }
        }
        break;
    case 8:
        if (dc_bytes_per_pixel == 2)
        {
            uint16_t *writebuf = (uint16_t *)dc->frame_buf;
            uint16_t color_back;
            uint16_t color_output = rgba2565(color);
            for (int32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) ;
                for (int32_t j = x_start; j < x_end; j++)
                {
                    uint8_t old_y = (i - font_y) / scale;
                    uint8_t old_x = (j - font_x) / scale;
                    uint8_t alpha = dots[old_y * font_w + old_x];
                    if (alpha != 0)
                    {
                        alpha = color.color.rgba.a * alpha / 0xff;
                        color_back = writebuf[write_off + j - dc->section.x1];
                        writebuf[write_off + j - dc->section.x1] = alphaBlendRGB565(color_output, color_back, alpha);
                    }
                }
            }
        }
        else if (dc_bytes_per_pixel == 3)
        {
            uint8_t *writebuf = (uint8_t *)dc->frame_buf;
            uint8_t color_back[3];
            for (int32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) ;
                for (int32_t j = x_start; j < x_end; j++)
                {
                    uint8_t old_y = (i - font_y) / scale;
                    uint8_t old_x = (j - font_x) / scale;
                    uint8_t alpha = dots[old_y * font_w + old_x];
                    if (alpha != 0)
                    {
                        alpha = color.color.rgba.a * alpha / 0xff;
                        color_back[0] = writebuf[write_off * 3 + j * 3 + 2 - dc->section.x1];
                        color_back[1] = writebuf[write_off * 3 + j * 3 + 1 - dc->section.x1];
                        color_back[2] = writebuf[write_off * 3 + j * 3 + 0 - dc->section.x1];
                        writebuf[write_off * 3 + j * 3 + 0 - dc->section.x1] = (color.color.rgba.b * alpha + color_back[2] *
                                                                                (0xff - alpha)) / 0xff;
                        writebuf[write_off * 3 + j * 3 + 1 - dc->section.x1] = (color.color.rgba.g * alpha + color_back[1] *
                                                                                (0xff - alpha)) / 0xff;
                        writebuf[write_off * 3 + j * 3 + 2 - dc->section.x1] = (color.color.rgba.r * alpha + color_back[0] *
                                                                                (0xff - alpha)) / 0xff;
                    }
                }
            }
        }
        else if (dc_bytes_per_pixel == 4)
        {
            uint32_t *writebuf = (uint32_t *)dc->frame_buf;
            uint32_t color_back;
            gui_log("font scale %f , x %d %d , y %d %d \n", (double)scale, x_start, x_end, y_start, y_end);
            gui_log(" \n");
            for (int32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) ;
                for (int32_t j = x_start; j < x_end; j++)
                {
                    uint8_t old_y = (i - font_y) / scale;
                    uint8_t old_x = (j - font_x) / scale;
                    uint8_t alpha = dots[old_y * font_w + old_x];
                    if (alpha != 0)
                    {
                        color_back = writebuf[write_off + j - dc->section.x1];
                        alpha = color.color.rgba.a * alpha / 0xff;
                        writebuf[write_off + j - dc->section.x1] = alphaBlendRGBA(color, color_back, alpha);
                    }
                }
            }
        }
        break;
    case 1:
        if (dc_bytes_per_pixel == 2)
        {
            uint16_t *writebuf = (uint16_t *)dc->frame_buf;
            uint16_t color_output = rgba2565(color);
            uint8_t ppb = 8;//pixel_per_byte = 8 / rendor_mode
            for (int32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1);
                for (int32_t j = x_start; j < x_end; j++)
                {
                    uint8_t old_y = (i - font_y) / scale;
                    uint8_t old_x = (j - font_x) / scale;
                    if ((dots[old_y * (font_w / ppb) + old_x / ppb] >> (old_x % ppb)) & 0x01)
                    {
                        writebuf[write_off + j - dc->section.x1] = color_output;
                    }
                }
            }
        }
        else if (dc_bytes_per_pixel == 3)
        {
            uint8_t *writebuf = (uint8_t *)dc->frame_buf;
            uint8_t color_back[3];
            uint8_t alpha = color.color.rgba.a;
            uint8_t ppb = 8;//pixel_per_byte = 8 / rendor_mode
            for (int32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) ;
                for (int32_t j = x_start; j < x_end; j++)
                {
                    uint8_t old_y = (i - font_y) / scale;
                    uint8_t old_x = (j - font_x) / scale;
                    if ((dots[old_y * (font_w / ppb) + old_x / ppb] >> (old_x % ppb)) & 0x01)
                    {
                        color_back[0] = writebuf[write_off * 3 + j * 3 + 2 - dc->section.x1];
                        color_back[1] = writebuf[write_off * 3 + j * 3 + 1 - dc->section.x1];
                        color_back[2] = writebuf[write_off * 3 + j * 3 + 0 - dc->section.x1];
                        writebuf[write_off * 3 + j * 3 + 0 - dc->section.x1] = (color.color.rgba.b * alpha + color_back[2] *
                                                                                (0xff - alpha)) / 0xff;
                        writebuf[write_off * 3 + j * 3 + 1 - dc->section.x1] = (color.color.rgba.g * alpha + color_back[1] *
                                                                                (0xff - alpha)) / 0xff;
                        writebuf[write_off * 3 + j * 3 + 2 - dc->section.x1] = (color.color.rgba.r * alpha + color_back[0] *
                                                                                (0xff - alpha)) / 0xff;
                    }
                }
            }
        }
        else if (dc_bytes_per_pixel == 4)
        {
            uint32_t *writebuf = (uint32_t *)dc->frame_buf;
            uint32_t color_back;
            uint8_t ppb = 8;//pixel_per_byte = 8 / rendor_mode
            for (int32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) ;
                for (int32_t j = x_start; j < x_end; j++)
                {
                    uint8_t old_y = (i - font_y) / scale;
                    uint8_t old_x = (j - font_x) / scale;
                    if ((dots[old_y * (font_w / ppb) + old_x / ppb] >> (old_x % ppb)) & 0x01)
                    {
                        color_back = writebuf[write_off + j - dc->section.x1];
                        writebuf[write_off + j - dc->section.x1] = alphaBlendRGBA(color, color_back, color.color.rgba.a);
                    }
                }
            }
        }
        break;
    default:
        break;
    }
}
static bool rtk_draw_unicode_matrix_by_img(mem_char_t *chr, gui_color_t color, uint8_t rendor_mode,
                                           gui_text_rect_t *rect, uint8_t *img_buf, draw_img_t *draw_img)
{
    if (chr->dot_addr == NULL) { return false; }

    const int font_x1 = chr->x;
    const int font_y1 = chr->y + chr->char_y;
    const int font_x2 = chr->x + chr->char_w - 1;
    const int font_y2 = chr->y + chr->char_y + chr->char_h - 1;

    const int x_start = _UI_MAX3(font_x1, rect->xboundleft, 0);
    const int x_end = rect->xboundright ? _UI_MIN(font_x2, rect->xboundright) : font_x2;

    const int y_start = _UI_MAX3(font_y1, rect->yboundtop, 0);
    const int y_end = rect->yboundbottom ? _UI_MIN(font_y2, rect->yboundbottom) : font_y2;

    if (x_start >= x_end || y_start >= y_end) { return false; }

    uint16_t img_w = x_end - x_start + 1;
    uint16_t img_h = y_end - y_start + 1;

    gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)img_buf;
    head->w = img_w;
    head->h = img_h;

    draw_img->img_target_x = x_start;
    draw_img->img_target_y = y_start;

    uint8_t *font_buf = img_buf + sizeof(gui_rgb_data_head_t);

    uint8_t target_bit_depth = font_get_bitdepth_by_cf((GUI_FormatType)head->type);

    draw_font_t df =
    {
        .color = color,
        .render_mode = rendor_mode,
        .target_buf = font_buf,
        .target_buf_stride = img_w * target_bit_depth / 8,
        .target_format = (GUI_FormatType)head->type,
        .clip_rect = {
            .x1 = x_start,
            .y1 = y_start,
            .x2 = x_end,
            .y2 = y_end,
        },
        .target_rect = {
            .x1 = x_start,
            .y1 = y_start,
            .x2 = x_end,
            .y2 = y_end,
        }
    };

    font_glyph_t glyph =
    {
        .data = chr->dot_addr,
        .pos_x = chr->x,
        .pos_y = chr->y + chr->char_y,
        .width = chr->char_w,
        .height = chr->char_h,
        .stride = chr->w,
    };

    font_glyph_render(&df, &glyph);
    return true;
}

static void gui_font_matrix_adapt_rect(gui_text_t *text, gui_text_rect_t *rect)
{
    float scale_x = text->base.matrix->m[0][0];
    float scale_y = text->base.matrix->m[1][1];

    rect->x2 = (rect->x2 - rect->x1) * scale_x + rect->x1;
    rect->y2 = (rect->y2 - rect->y1) * scale_y + rect->y1;

    rect->xboundright = (rect->xboundright - rect->xboundleft) * scale_x + rect->xboundleft;
    rect->yboundbottom = (rect->yboundbottom - rect->yboundtop) * scale_y + rect->yboundtop;
}

static void gui_font_char_mat(gui_text_t *text)
{
    GUI_ASSERT(text->data != NULL)
    mem_char_t *chr = text->data;
    float scale_x = text->base.matrix->m[0][0];
    float scale_y = text->base.matrix->m[1][1];

    for (int i = 0; i < text->active_font_len; i++)
    {
        chr[i].char_w = chr[i].char_w * scale_x;

        chr[i].h = chr[i].h * scale_y;
        chr[i].char_y = chr[i].char_y * scale_y;
        chr[i].char_h = chr[i].char_h * scale_y;
    }
    text->char_width_sum = text->char_width_sum * scale_x;
}

void gui_font_mat_unload(gui_text_t *text)
{
    if (text->data)
    {
        mem_char_t *chr = text->data;
        for (int i = 0; i < text->active_font_len; i++)
        {
            if (chr[i].buf != NULL)
            {
                gui_free(chr[i].buf);
                chr[i].buf = NULL;
            }
        }
    }
    return;
}

void gui_font_mat_destroy(gui_text_t *text)
{
    if (text->data)
    {
        if (text->font_mode == FONT_SRC_FTL)
        {
            mem_char_t *chr = text->data;
            for (int i = 0; i < text->font_len; i++)
            {
                gui_free(chr[i].dot_addr);
            }
        }
        gui_free(text->data);
        text->data = NULL;
    }
    return;
}

void gui_font_mat_load(gui_text_t *text, gui_text_rect_t *rect)
{
    GUI_ASSERT(text != NULL)
    if (text->data == NULL)
    {
        gui_font_get_dot_info(text);
    }
    else
    {
        gui_font_mem_unload(text);
        gui_font_get_dot_info(text);
    }
    if (!text->use_img_blit)
    {
        gui_font_char_mat(text);
        gui_font_matrix_adapt_rect(text, rect);
    }
    gui_font_mem_layout(text, rect);
    text->layout_refresh = true;
    text->content_refresh = true;
}

void gui_font_mat_draw(gui_text_t *text, gui_text_rect_t *rect)
{
    mem_char_t *chr = text->data;
    GUI_FONT_HEAD_BMP *font = (GUI_FONT_HEAD_BMP *)text->path;
    gui_dispdev_t *dc = gui_get_dc();
    uint8_t rendor_mode = font->rendor_mode;

    if (text->use_img_blit)
    {
        uint8_t buffer_bytes = 2;
        GUI_FormatType font_img_type;

        switch (rendor_mode)
        {
        case 1:
            buffer_bytes = 2;
            font_img_type = RGB565;
            break;
        case 2:
            buffer_bytes = 3;
            font_img_type = ARGB8565;
            break;
        case 4:
            buffer_bytes = 3;
            font_img_type = ARGB8565;
            break;
        case 8:
            gui_log("gui_font_mat_draw not support 8 bit font, to do by luke \n");
            GUI_ASSERT(0)
            break;
        default:
            return;
        }

        uint32_t size = text->font_height * text->font_height * buffer_bytes;
        uint8_t *img_buf = gui_malloc(size + sizeof(gui_rgb_data_head_t));
        memset(img_buf, 0x00, size + sizeof(gui_rgb_data_head_t));
        uint8_t *font_buf = img_buf + sizeof(gui_rgb_data_head_t);

        gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)img_buf;
        head->type = font_img_type;

        draw_img_t draw_img = {0};
        draw_img.data = img_buf;
        switch (font_img_type)
        {
        case RGB565:
            draw_img.blend_mode = IMG_BYPASS_MODE;
            break;
        case ARGB8565:
            draw_img.blend_mode = IMG_SRC_OVER_MODE;
            break;
        case ARGB8888:
            draw_img.blend_mode = IMG_SRC_OVER_MODE;
            break;
        default:
            break;
        }
        draw_img.opacity_value = text->color.color.rgba.a;
        draw_img.high_quality = true;

        for (uint16_t i = 0; i < text->active_font_len; i++)
        {
            bool res = rtk_draw_unicode_matrix_by_img(chr + i, text->color, rendor_mode, rect, img_buf,
                                                      &draw_img);
            if (res == false)
            {
                continue;
            }
            if (chr[i].buf == NULL)
            {
                gui_matrix_t *font_matrix = gui_malloc(sizeof(gui_matrix_t) * 2);

                memcpy(&draw_img.matrix, text->base.matrix, sizeof(gui_matrix_t));
                matrix_translate(draw_img.img_target_x - text->offset_x, draw_img.img_target_y - text->offset_y,
                                 &draw_img.matrix);

                memcpy(&draw_img.inverse, &draw_img.matrix, sizeof(gui_matrix_t));
                matrix_inverse(&draw_img.inverse);

                memcpy(&font_matrix[0], &draw_img.matrix, sizeof(gui_matrix_t) * 2);

                chr[i].buf = (uint8_t *)font_matrix;
            }
            else
            {
                gui_matrix_t *font_matrix = (gui_matrix_t *)chr[i].buf;
                memcpy(&draw_img.matrix, &font_matrix[0], sizeof(gui_matrix_t));
                memcpy(&draw_img.inverse, &font_matrix[1], sizeof(gui_matrix_t));
            }

            draw_img.img_h = head->h;
            draw_img.img_w = head->w;

            draw_img_new_area(&draw_img, NULL);

            gui_acc_blit_to_dc(&draw_img, dc, NULL);

            if (draw_img_acc_end_cb != NULL)
            {
                draw_img_acc_end_cb(&draw_img);
            }

            memset(font_buf, 0x00, size);
        }
        gui_free(img_buf);
    }
    else
    {
        for (uint16_t i = 0; i < text->active_font_len; i++)
        {
            rtk_draw_unicode_matrix(chr + i, text->color, rendor_mode, rect, text->base.matrix->m[0][0]);
        }
    }
}
