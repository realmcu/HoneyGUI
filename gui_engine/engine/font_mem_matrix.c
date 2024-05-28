#include <string.h>
#include "draw_font.h"
#include "font_mem.h"
#include "font_mem_matrix.h"

gui_inline uint16_t alphaBlendRGB565(uint32_t fg, uint32_t bg, uint8_t alpha)
{
    alpha = (alpha + 4) >> 3;
    bg = (bg | (bg << 16)) & 0x7e0f81f;
    fg = (fg | (fg << 16)) & 0x7e0f81f;
    uint32_t result = (fg - bg) * alpha; // parallel fixed-point multiply of all components
    result >>= 5;
    result += bg;
    result &= 0x7e0f81f;
    return (uint16_t)((result >> 16) | result); // contract result
}
gui_inline uint32_t alphaBlendRGBA(gui_color_t fg, uint32_t bg, uint8_t alpha)
{
    uint32_t mix;
    uint8_t back_a = 0xff - alpha;
#if defined(_WIN32)
    mix = 0xff000000;
    mix += ((bg >> 16 & 0xff) * back_a + fg.color.rgba.r * alpha) / 0xff << 16;
    mix += ((bg >>  8 & 0xff) * back_a + fg.color.rgba.g * alpha) / 0xff <<  8;
    mix += ((bg >>  0 & 0xff) * back_a + fg.color.rgba.b * alpha) / 0xff <<  0;
#else
    mix = 0x000000ff;
    mix += ((bg >> 24 & 0xff) * back_a + fg.color.rgba.r * alpha) / 0xff << 24;
    mix += ((bg >> 16 & 0xff) * back_a + fg.color.rgba.g * alpha) / 0xff << 16;
    mix += ((bg >>  8 & 0xff) * back_a + fg.color.rgba.b * alpha) / 0xff <<  8;
#endif
    return mix;
}
gui_inline uint16_t rgba2565(gui_color_t rgba)
{
    uint16_t red = rgba.color.rgba.r * 0x1f / 0xff << 11;
    uint16_t gre = rgba.color.rgba.g * 0x3f / 0xff << 5;
    uint16_t blu = rgba.color.rgba.b * 0x1f / 0xff;
    return red + gre + blu;
}
static void rtk_draw_unicode_matrix(mem_char_t *chr, gui_color_t color, uint8_t rendor_mode,
                                    gui_text_rect_t *rect, bool crop, float scale)
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
    int x_start = _UI_MAX(_UI_MAX(font_x, rect->xboundleft), 0);
    int x_end;
    if (rect->xboundright != 0)
    {
        x_end = _UI_MIN(_UI_MIN(font_x + chr->char_w, dc->fb_width), rect->xboundright);
    }
    else
    {
        x_end = _UI_MIN(font_x + chr->char_w, dc->fb_width);
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
            for (uint32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * dc->fb_width ;
                for (uint32_t j = x_start; j < x_end; j++)
                {
                    uint8_t old_y = (i - font_y) / scale;
                    uint8_t old_x = (j - font_x) / scale;
                    uint8_t alpha = dots[old_y * (font_w / ppb) + old_x / ppb] >> (old_x % ppb * 2);
                    if (alpha != 0)
                    {
                        alpha = alpha & 0x03;
                        alpha = alpha_list[alpha];
                        alpha = color.color.rgba.a * alpha / 0xff;
                        color_back = writebuf[write_off + j];
                        writebuf[write_off + j] = alphaBlendRGB565(color_output, color_back, alpha);
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
            for (uint32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * dc->fb_width ;
                for (uint32_t j = x_start; j < x_end; j++)
                {
                    uint8_t old_y = (i - font_y) / scale;
                    uint8_t old_x = (j - font_x) / scale;
                    uint8_t alpha = dots[old_y * (font_w / ppb) + old_x / ppb] >> (old_x % ppb * 2);
                    if (alpha != 0)
                    {
                        alpha = alpha & 0x03;
                        alpha = alpha_list[alpha];
                        alpha = color.color.rgba.a * alpha / 0xff;
                        color_back[0] = writebuf[write_off * 3 + j * 3 + 2];
                        color_back[1] = writebuf[write_off * 3 + j * 3 + 1];
                        color_back[2] = writebuf[write_off * 3 + j * 3 + 0];
                        writebuf[write_off * 3 + j * 3 + 0] = (color.color.rgba.b * alpha + color_back[2] *
                                                               (0xff - alpha)) / 0xff;
                        writebuf[write_off * 3 + j * 3 + 1] = (color.color.rgba.g * alpha + color_back[1] *
                                                               (0xff - alpha)) / 0xff;
                        writebuf[write_off * 3 + j * 3 + 2] = (color.color.rgba.r * alpha + color_back[0] *
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
            for (uint32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * dc->fb_width ;
                for (uint32_t j = x_start; j < x_end; j++)
                {
                    uint8_t old_y = (i - font_y) / scale;
                    uint8_t old_x = (j - font_x) / scale;
                    uint8_t alpha = dots[old_y * (font_w / ppb) + old_x / ppb] >> (old_x % ppb * 2);
                    if (alpha != 0)
                    {
                        alpha = alpha & 0x03;
                        alpha = alpha_list[alpha];
                        alpha = color.color.rgba.a * alpha / 0xff;
                        color_back = writebuf[write_off + j];
                        writebuf[write_off + j] = alphaBlendRGBA(color, color_back, alpha);
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
            for (uint32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * dc->fb_width ;
                for (uint32_t j = x_start; j < x_end; j++)
                {
                    uint8_t old_y = (i - font_y) / scale;
                    uint8_t old_x = (j - font_x) / scale;
                    uint8_t alpha = (dots[old_y * (font_w / ppb) + old_x / ppb] >> (old_x % ppb * 4));
                    if (alpha != 0)
                    {
                        alpha = alpha & 0x0f;
                        alpha = alpha_list[alpha];
                        alpha = color.color.rgba.a * alpha / 0xff;
                        color_back = writebuf[write_off + j];
                        writebuf[write_off + j] = alphaBlendRGB565(color_output, color_back, alpha);
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
            for (uint32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * dc->fb_width ;
                for (uint32_t j = x_start; j < x_end; j++)
                {
                    uint8_t old_y = (i - font_y) / scale;
                    uint8_t old_x = (j - font_x) / scale;
                    uint8_t alpha = (dots[old_y * (font_w / ppb) + old_x / ppb] >> (old_x % ppb * 4));
                    if (alpha != 0)
                    {
                        alpha = alpha & 0x0f;
                        alpha = alpha_list[alpha];
                        alpha = color.color.rgba.a * alpha / 0xff;
                        color_back[0] = writebuf[write_off * 3 + j * 3 + 2];
                        color_back[1] = writebuf[write_off * 3 + j * 3 + 1];
                        color_back[2] = writebuf[write_off * 3 + j * 3 + 0];
                        writebuf[write_off * 3 + j * 3 + 0] = (color.color.rgba.b * alpha + color_back[2] *
                                                               (0xff - alpha)) / 0xff;
                        writebuf[write_off * 3 + j * 3 + 1] = (color.color.rgba.g * alpha + color_back[1] *
                                                               (0xff - alpha)) / 0xff;
                        writebuf[write_off * 3 + j * 3 + 2] = (color.color.rgba.r * alpha + color_back[0] *
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
            for (uint32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * dc->fb_width ;
                for (uint32_t j = x_start; j < x_end; j++)
                {
                    uint8_t old_y = (i - font_y) / scale;
                    uint8_t old_x = (j - font_x) / scale;
                    uint8_t alpha = (dots[old_y * (font_w / ppb) + old_x / ppb] >> (old_x % ppb * 4));
                    if (alpha != 0)
                    {
                        alpha = alpha & 0x0f;
                        alpha = alpha_list[alpha];
                        alpha = color.color.rgba.a * alpha / 0xff;
                        color_back = writebuf[write_off + j];
                        writebuf[write_off + j] = alphaBlendRGBA(color, color_back, alpha);
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
            for (uint32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * dc->fb_width ;
                for (uint32_t j = x_start; j < x_end; j++)
                {
                    uint8_t old_y = (i - font_y) / scale;
                    uint8_t old_x = (j - font_x) / scale;
                    uint8_t alpha = dots[old_y * font_w + old_x];
                    if (alpha != 0)
                    {
                        alpha = color.color.rgba.a * alpha / 0xff;
                        color_back = writebuf[write_off + j];
                        writebuf[write_off + j] = alphaBlendRGB565(color_output, color_back, alpha);
                    }
                }
            }
        }
        else if (dc_bytes_per_pixel == 3)
        {
            uint8_t *writebuf = (uint8_t *)dc->frame_buf;
            uint8_t color_back[3];
            for (uint32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * dc->fb_width ;
                for (uint32_t j = x_start; j < x_end; j++)
                {
                    uint8_t old_y = (i - font_y) / scale;
                    uint8_t old_x = (j - font_x) / scale;
                    uint8_t alpha = dots[old_y * font_w + old_x];
                    if (alpha != 0)
                    {
                        alpha = color.color.rgba.a * alpha / 0xff;
                        color_back[0] = writebuf[write_off * 3 + j * 3 + 2];
                        color_back[1] = writebuf[write_off * 3 + j * 3 + 1];
                        color_back[2] = writebuf[write_off * 3 + j * 3 + 0];
                        writebuf[write_off * 3 + j * 3 + 0] = (color.color.rgba.b * alpha + color_back[2] *
                                                               (0xff - alpha)) / 0xff;
                        writebuf[write_off * 3 + j * 3 + 1] = (color.color.rgba.g * alpha + color_back[1] *
                                                               (0xff - alpha)) / 0xff;
                        writebuf[write_off * 3 + j * 3 + 2] = (color.color.rgba.r * alpha + color_back[0] *
                                                               (0xff - alpha)) / 0xff;
                    }
                }
            }
        }
        else if (dc_bytes_per_pixel == 4)
        {
            uint32_t *writebuf = (uint32_t *)dc->frame_buf;
            uint32_t color_back;
            gui_log("font scale %f , x %d %d , y %d %d \n", scale, x_start, x_end, y_start, y_end);
            gui_log(" \n");
            for (uint32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * dc->fb_width ;
                for (uint32_t j = x_start; j < x_end; j++)
                {
                    uint8_t old_y = (i - font_y) / scale;
                    uint8_t old_x = (j - font_x) / scale;
                    uint8_t alpha = dots[old_y * font_w + old_x];
                    if (alpha != 0)
                    {
                        color_back = writebuf[write_off + j];
                        alpha = color.color.rgba.a * alpha / 0xff;
                        writebuf[write_off + j] = alphaBlendRGBA(color, color_back, alpha);
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
            for (uint32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * dc->fb_width;
                for (uint32_t j = x_start; j < x_end; j++)
                {
                    uint8_t old_y = (i - font_y) / scale;
                    uint8_t old_x = (j - font_x) / scale;
                    if ((dots[old_y * (font_w / ppb) + old_x / ppb] >> (old_x % ppb)) & 0x01)
                    {
                        writebuf[write_off + j] = color_output;
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
            for (uint32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * dc->fb_width ;
                for (uint32_t j = x_start; j < x_end; j++)
                {
                    uint8_t old_y = (i - font_y) / scale;
                    uint8_t old_x = (j - font_x) / scale;
                    if ((dots[old_y * (font_w / ppb) + old_x / ppb] >> (old_x % ppb)) & 0x01)
                    {
                        color_back[0] = writebuf[write_off * 3 + j * 3 + 2];
                        color_back[1] = writebuf[write_off * 3 + j * 3 + 1];
                        color_back[2] = writebuf[write_off * 3 + j * 3 + 0];
                        writebuf[write_off * 3 + j * 3 + 0] = (color.color.rgba.b * alpha + color_back[2] *
                                                               (0xff - alpha)) / 0xff;
                        writebuf[write_off * 3 + j * 3 + 1] = (color.color.rgba.g * alpha + color_back[1] *
                                                               (0xff - alpha)) / 0xff;
                        writebuf[write_off * 3 + j * 3 + 2] = (color.color.rgba.r * alpha + color_back[0] *
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
            for (uint32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * dc->fb_width ;
                for (uint32_t j = x_start; j < x_end; j++)
                {
                    uint8_t old_y = (i - font_y) / scale;
                    uint8_t old_x = (j - font_x) / scale;
                    if ((dots[old_y * (font_w / ppb) + old_x / ppb] >> (old_x % ppb)) & 0x01)
                    {
                        color_back = writebuf[write_off + j];
                        writebuf[write_off + j] = alphaBlendRGBA(color, color_back, color.color.rgba.a);
                    }
                }
            }
        }
        break;
    default:
        break;
    }
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
    mem_char_t *chr = text->data;
    float scale_x = text->base.matrix->m[0][0];
    float scale_y = text->base.matrix->m[1][1];

    for (int i = 0; i < text->font_len; i++)
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
        gui_free(text->data);
        text->data = NULL;
    }
    return;
}

void gui_font_mat_load(gui_text_t *text, gui_text_rect_t *rect)
{
    gui_font_get_dot_info(text);
    gui_font_char_mat(text);
    gui_font_matrix_adapt_rect(text, rect);
    gui_font_mem_layout(text, rect);
}

void gui_font_mat_draw(gui_text_t *text, gui_text_rect_t *rect)
{
    mem_char_t *chr = text->data;
    GUI_FONT_HEAD *font = (GUI_FONT_HEAD *)text->path;
    uint8_t rendor_mode = font->rendor_mode;
    for (uint16_t i = 0; i < text->font_len; i++)
    {
        rtk_draw_unicode_matrix(chr + i, text->color, rendor_mode, rect, font->font_mode_detail.detail.crop,
                                text->base.matrix->m[0][0]);
    }
}






