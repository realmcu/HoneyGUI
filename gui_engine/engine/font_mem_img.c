#include <string.h>
#include "font_mem_img.h"

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
//gui_inline uint32_t alphaBlendRGBA(gui_color_t fg, uint32_t bg, uint8_t alpha)
//{
//    uint32_t mix;
//    uint8_t back_a = 0xff - alpha;
//#if defined(_WIN32)
//    mix = 0xff000000;
//    mix += ((bg >> 16 & 0xff) * back_a + fg.color.rgba.r * alpha) / 0xff << 16;
//    mix += ((bg >>  8 & 0xff) * back_a + fg.color.rgba.g * alpha) / 0xff <<  8;
//    mix += ((bg >>  0 & 0xff) * back_a + fg.color.rgba.b * alpha) / 0xff <<  0;
//#else
//    mix = 0x000000ff;
//    mix += ((bg >> 24 & 0xff) * back_a + fg.color.rgba.r * alpha) / 0xff << 24;
//    mix += ((bg >> 16 & 0xff) * back_a + fg.color.rgba.g * alpha) / 0xff << 16;
//    mix += ((bg >>  8 & 0xff) * back_a + fg.color.rgba.b * alpha) / 0xff <<  8;
//#endif
//    return mix;
//}
gui_inline uint16_t rgba2565(gui_color_t rgba)
{
    uint16_t red = rgba.color.rgba.r * 0x1f / 0xff << 11;
    uint16_t gre = rgba.color.rgba.g * 0x3f / 0xff << 5;
    uint16_t blu = rgba.color.rgba.b * 0x1f / 0xff;
    return red + gre + blu;
}

static void gui_font_bmp2img_one_char(mem_char_t *chr, gui_color_t color, uint8_t rendor_mode,
                                      gui_text_rect_t *rect, uint8_t *buffer, int buf_width, uint8_t buffer_bytes, uint8_t crop)
{
    if (chr->dot_addr == NULL || buffer == NULL)
    {
        return;
    }
    uint8_t *dots = chr->dot_addr;
    gui_dispdev_t *dc = gui_get_dc();

    int font_x = chr->x;
    int font_y = chr->y + chr->char_y;
    int font_w = chr->w;
    // int font_h = chr->h;
    int x_start = font_x;
    int x_end = _UI_MIN(font_x + chr->char_w, rect->x2);
    int y_start = font_y;
    int y_end = _UI_MIN((font_y + chr->char_h), rect->y2);
    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }

    switch (rendor_mode)
    {
    case 2:
        {
            if (buffer_bytes == 2)
            {
                uint16_t *writebuf = (uint16_t *)buffer + chr->x;

                uint16_t color_output[4];
                //Normal is 0x00,0x40,0x80,0xFF, modify to 0x00,0x40,0xA0,0xFF.By Luke
                color_output[3] = rgba2565(color);
                color_output[2] = alphaBlendRGB565(color_output[3], 0x0, 0xA0);
                color_output[1] = alphaBlendRGB565(color_output[3], 0x0, 0x40);
                color_output[0] = 0;

                for (uint32_t i = y_start; i < y_end; i++)
                {
                    int write_off = buf_width * i;
                    int dots_off = (i - font_y) * (font_w / 4);
                    for (uint32_t j = x_start; j < x_end; j++)
                    {
                        uint8_t alpha = dots[dots_off + (j - font_x) / 4] >> ((j - font_x) % 4 * 2);
                        if (alpha != 0)
                        {
                            writebuf[write_off + j - x_start] = color_output[alpha & 0x03];
                        }
                    }
                }
            }
            else if (buffer_bytes == 3)
            {
                uint8_t *writebuf = (uint8_t *)buffer + chr->x * buffer_bytes;

                uint16_t write_color = rgba2565(color);
                //Normal is 0x00,0x40,0x80,0xFF, modify to 0x00,0x40,0xA0,0xFF.By Luke
                uint8_t alpha_output[4];
                alpha_output[3] = 0xFF;
                alpha_output[2] = 0xA0;
                alpha_output[1] = 0x40;
                alpha_output[0] = 0x00;

                for (uint32_t i = y_start; i < y_end; i++)
                {
                    int write_off = buf_width * i * 3;
                    int dots_off = (i - font_y) * (font_w / 4);
                    for (uint32_t j = x_start; j < x_end; j++)
                    {
                        uint8_t alpha = dots[dots_off + (j - font_x) / 4] >> ((j - font_x) % 4 * 2);
                        if (alpha != 0)
                        {
                            memcpy(writebuf + write_off + (j - x_start) * 3, &write_color, 2);
                            writebuf[write_off + (j - x_start) * 3 + 2] = alpha_output[alpha & 0x03];
                        }
                    }
                }
            }
            else if (buffer_bytes == 4)
            {
                uint32_t *writebuf = (uint32_t *)buffer + chr->x;

                uint32_t color_output[4];
                gui_color_t write_color = color;
                uint8_t temp_b = write_color.color.rgba.b;
                write_color.color.rgba.b = write_color.color.rgba.r;
                write_color.color.rgba.r = temp_b;
                int pre_alpha = 0x100 / ((1 << rendor_mode) - 1);
                for (int i = 0; i < 1 << rendor_mode; i++)
                {
                    write_color.color.rgba.a = pre_alpha * i;
                    color_output[i] = write_color.color.rgba_full;
                }
                for (uint32_t i = y_start; i < y_end; i++)
                {
                    int write_off = buf_width * i;
                    int dots_off = (i - font_y) * (font_w / 4);
                    for (uint32_t j = x_start; j < x_end; j++)
                    {
                        uint8_t alpha = dots[dots_off + (j - font_x) / 4] >> ((j - font_x) % 4 * 2);
                        if (alpha != 0)
                        {
                            writebuf[write_off + j - x_start] = color_output[alpha & 0x03];
                        }
                    }
                }
            }
        }
        break;
    case 4:
        {
            if (buffer_bytes == 2)
            {
                uint16_t *writebuf = (uint16_t *)buffer + chr->x;

                uint16_t color_output[16];
                int pre_alpha = 0x100 / ((1 << rendor_mode) - 1);

                color_output[0] = 0 ;
                color_output[15] = rgba2565(color);
                for (int i = 1; i + 1 < 1 << rendor_mode; i++)
                {
                    color_output[i] = alphaBlendRGB565(color_output[15], 0x0, pre_alpha * i);
                }

                for (uint32_t i = y_start; i < y_end; i++)
                {
                    int write_off = buf_width * i ;
                    int dots_off = (i - font_y) * (font_w / 2);
                    for (uint32_t j = x_start; j < x_end; j++)
                    {
                        uint8_t alpha = dots[dots_off + (j - font_x) / 2] >> ((j - font_x) % 2 * 4);
                        if (alpha != 0)
                        {
                            writebuf[write_off + j - x_start] = color_output[alpha & 0x0f];
                        }
                    }
                }
            }
            else if (buffer_bytes == 3)
            {
                uint8_t *writebuf = (uint8_t *)buffer + chr->x * buffer_bytes;

                uint16_t write_color = rgba2565(color);
                for (uint32_t i = y_start; i < y_end; i++)
                {
                    int write_off = buf_width * i * 3;
                    int dots_off = (i - font_y) * (font_w / 2);
                    for (uint32_t j = x_start; j < x_end; j++)
                    {
                        uint8_t alpha = dots[dots_off + (j - font_x) / 2] >> ((j - font_x) % 2 * 4);
                        if (alpha != 0)
                        {
                            memcpy(writebuf + write_off + (j - x_start) * 3, &write_color, 2);
                            writebuf[write_off + (j - x_start) * 3 + 2] = (alpha & 0x0f) * 17;
                        }
                    }
                }
            }
            else if (buffer_bytes == 4)
            {
                uint32_t *writebuf = (uint32_t *)buffer + chr->x;
                gui_color_t write_color = color;
                uint8_t temp_b = write_color.color.rgba.b;
                write_color.color.rgba.b = write_color.color.rgba.r;
                write_color.color.rgba.r = temp_b;
                for (uint32_t i = y_start; i < y_end; i++)
                {
                    int write_off = buf_width * i;
                    int dots_off = (i - font_y) * (font_w / 2);
                    for (uint32_t j = x_start; j < x_end; j++)
                    {
                        uint8_t alpha = dots[dots_off + (j - font_x) / 2] >> ((j - font_x) % 2 * 4);
                        if (alpha != 0)
                        {
                            write_color.color.rgba.a = (alpha & 0x0f) * 17;
                            writebuf[write_off + j - x_start] = write_color.color.rgba_full;
                        }
                    }
                }
            }
        }
        break;
    case 8:
        {
            if (buffer_bytes == 2)
            {
                gui_log("this type font scale no support !!! \n");
                GUI_ASSERT(NULL != NULL);
            }
            else if (buffer_bytes == 3)
            {
                uint8_t *writebuf = (uint8_t *)buffer + chr->x * buffer_bytes;

                uint16_t write_color = rgba2565(color);
                for (uint32_t i = y_start; i < y_end; i++)
                {
                    int write_off = buf_width * i * 3;
                    int dots_off = (i - font_y) * font_w;
                    for (uint32_t j = x_start; j < x_end; j++)
                    {
                        uint8_t alpha = dots[dots_off + j - font_x];
                        if (alpha != 0)
                        {
                            memcpy(writebuf + write_off + (j - x_start) * 3, &write_color, 2);
                            writebuf[write_off + (j - x_start) * 3 + 2] = alpha;
                        }
                    }
                }
            }
            else if (buffer_bytes == 4)
            {
                uint32_t *writebuf = (uint32_t *)buffer + chr->x;
                gui_color_t write_color = color;
                uint8_t temp_b = write_color.color.rgba.b;
                write_color.color.rgba.b = write_color.color.rgba.r;
                write_color.color.rgba.r = temp_b;
                for (uint32_t i = y_start; i < y_end; i++)
                {
                    int write_off = buf_width * i;
                    int dots_off = (i - font_y) * font_w;
                    for (uint32_t j = x_start; j < x_end; j++)
                    {
                        uint8_t alpha = dots[dots_off + j - font_x];
                        if (alpha != 0)
                        {
                            write_color.color.rgba.a = alpha;
                            writebuf[write_off + j - x_start] = write_color.color.rgba_full;
                        }
                    }
                }
            }
        }
        break;
    case 1:
        {
            if (buffer_bytes == 2)
            {
                uint16_t *writebuf = (uint16_t *)buffer + chr->x;
                uint16_t color_output = rgba2565(color);
                for (uint32_t i = y_start; i < y_end; i++)
                {
                    int write_off = buf_width * i;
                    int dots_off = (i - font_y) * (font_w / 8);

                    for (uint32_t j = x_start; j < x_end; j++)
                    {
                        if ((dots[dots_off + (j - font_x) / 8] >> ((j - font_x) % 8)) & 0x01)
                        {
                            writebuf[write_off + j - x_start] = color_output;
                        }
                    }
                }
            }
            else if (buffer_bytes == 3)
            {
                gui_log("this type font scale no support now!!! \n");
                GUI_ASSERT(NULL != NULL);
            }
            else if (buffer_bytes == 4)
            {
                uint32_t *writebuf = (uint32_t *)buffer + chr->x;
                gui_color_t write_color = color;
                uint8_t temp_b = write_color.color.rgba.b;
                write_color.color.rgba.b = write_color.color.rgba.r;
                write_color.color.rgba.r = temp_b;
                for (uint32_t i = y_start; i < y_end; i++)
                {
                    int write_off = buf_width * i ;
                    int dots_off = (i - font_y) * (font_w / 8);
                    for (uint32_t j = x_start; j < x_end; j++)
                    {
                        if ((dots[dots_off + (j - font_x) / 8] >> ((j - font_x) % 8)) & 0x01)
                        {
                            writebuf[write_off + j - x_start] = write_color.color.rgba_full;
                        }
                    }
                }
            }
        }
        break;
    default:
        break;
    }
}

void gui_font_scale_destory(gui_text_t *text)
{
    if (text->scale_img != NULL)
    {
        if (text->scale_img->data != NULL)
        {
            FONT_FREE_PSRAM(text->scale_img->data);
        }
    }
}

void *gui_text_bmp2img(gui_text_t *text, GUI_FormatType font_img_type, int16_t *img_x,
                       int16_t *img_y)
{
    int16_t buf_width = 0;
    int16_t buf_height = 0;
    uint8_t font_img_pixel_bytes;

    gui_text_rect_t rect = {0};
    rect.x1 = 0;
    rect.y1 = 0;
    rect.x2 = rect.x1 + text->base.w;
    rect.y2 = rect.y1 + text->base.h;
    TEXT_MODE mode = text->mode;
    if (text->mode == CENTER || text->mode == RIGHT)
    {
        text->mode = LEFT;
    }
    gui_font_mem_load(text, &rect);
    text->mode = mode;
    switch (font_img_type)
    {
    case RGB565:
        font_img_pixel_bytes = 2;
        break;
    case ARGB8565:
        // case RGB888:
        font_img_pixel_bytes = 3;
        break;
    case RGBA8888:
        font_img_pixel_bytes = 4;
        break;
    default:
        font_img_pixel_bytes = 0;
        break;
    }
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

    uint32_t size = buf_width * buf_height * (uint32_t)font_img_pixel_bytes + sizeof(
                        struct gui_rgb_data_head);
    void *img_buf = FONT_MALLOC_PSRAM(size);
    memset(img_buf, 0x00, size);

    struct gui_rgb_data_head head;
    memset(&head, 0x0, sizeof(head));
    head.w = buf_width;
    head.h = buf_height;
    head.type = font_img_type;
    memcpy(img_buf, &head, sizeof(head));

    mem_char_t *chr = text->data;
    GUI_FONT_HEAD *font = (GUI_FONT_HEAD *)text->path;
    uint8_t rendor_mode = font->rendor_mode;
    uint8_t *buffer_addr = (uint8_t *)img_buf + sizeof(struct gui_rgb_data_head);
    rect.x2 = rect.x1 + buf_width;
    rect.y2 = rect.y1 + buf_height;
    for (uint16_t i = 0; i < text->font_len; i++)
    {
        gui_font_bmp2img_one_char(chr + i, text->color, rendor_mode, &rect, buffer_addr, buf_width,
                                  font_img_pixel_bytes, font->font_mode_detail.detail.crop);
    }
    gui_font_mem_unload(text);
    return img_buf;
}

