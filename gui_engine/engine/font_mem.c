#include <string.h>
#include "draw_font.h"
#include "font_mem.h"

MEM_FONT_LIB font_lib_tab[10];
uint8_t get_fontlib_name(uint8_t font_size)
{
    uint8_t tab_size = sizeof(font_lib_tab) / sizeof(MEM_FONT_LIB);
    for (size_t i = 0; i < tab_size; i++)
    {
        if (font_lib_tab[i].font_size == font_size)
        {
            return i;
        }
    }
    gui_log("Can not match font file, use default \n");
    if (font_lib_tab[0].font_size == 0)
    {
        gui_log("There is no font file \n");
        GUI_ASSERT(font_lib_tab[0].font_size != 0)
    }
    return 0;
}

void gui_font_get_dot_info(gui_text_t *text)
{
    GUI_FONT_HEAD *font;
    uint8_t font_index;
    uint32_t table_offset;
    uint32_t dot_offset;
    if (text->path == NULL)
    {
        font_index = get_fontlib_name(text->font_height);
        if (font_lib_tab[font_index].type == FONT_SRC_MEMADDR)
        {
            text->font_mode = FONT_SRC_MEMADDR;
            text->path = font_lib_tab[font_index].font_file;
        }
        else if (font_lib_tab[font_index].type == FONT_SRC_FTL)
        {
            text->font_mode = FONT_SRC_FTL;
            text->path = font_lib_tab[font_index].font_file;
        }
    }
    font = (GUI_FONT_HEAD *)text->path;
    if (text->font_mode == FONT_SRC_MEMADDR)
    {
        font = (GUI_FONT_HEAD *)text->path;
        table_offset = (uint32_t)(uintptr_t)((uint8_t *)font + font->head_length);
        dot_offset = table_offset + font->index_area_size;
    }
    else if (text->font_mode == FONT_SRC_FTL)
    {
        font = (GUI_FONT_HEAD *)font_lib_tab[font_index].data;
        table_offset = (uint32_t)(uintptr_t)((uint8_t *)font + font->head_length);
        dot_offset = (uintptr_t)text->path + font->head_length + font->index_area_size;
    }


    uint8_t rendor_mode = font->rendor_mode;
    if (rendor_mode == 0)
    {
        rendor_mode = 1;
        gui_log("font file error ! rendor_mode is 0 ! \n");
    }
    uint8_t index_method = font->font_mode_detail.detail.index_method;
    uint8_t index_unit_length = 4; //now set to 4 , todo
    bool crop = font->font_mode_detail.detail.crop;

    if (text->data != NULL)
    {
        if (text->refresh)
        {
            gui_font_mem_unload(text);
        }
        else
        {
            return;
        }
    }
    mem_char_t *chr = gui_malloc(sizeof(mem_char_t) * text->len);
    if (chr == NULL)
    {
        GUI_ASSERT(NULL != NULL);
        return;
    }
    if (chr)
    {
        memset(chr, 0, sizeof(mem_char_t) * text->len);
    }
    text->data = chr;
    text->refresh = false;
    uint16_t *p_buf = NULL;
    uint16_t unicode_len = 0;
    switch (text->charset)
    {
    case UTF_8_CHARSET:
        p_buf = gui_malloc(text->len * sizeof(uint16_t));
        if (p_buf == NULL)
        {
            GUI_ASSERT(NULL != NULL);
            return;
        }
        else
        {
            unicode_len = utf8_to_unicode(text->content, text->len, p_buf, text->len);
        }
        break;
    case UTF_16_CHARSET:
        unicode_len = text->len / 2;
        p_buf = (uint16_t *)text->content;
        break;
    default:
        break;
    }
    int32_t all_char_w = 0;
    int32_t all_char_h = 0;
    uint32_t line_flag = 0;
    int32_t line_byte = 0;

    if (crop)
    {
        switch (index_method)
        {
        case 0: //address
            for (uint32_t i = 0; i < unicode_len; i++)
            {
                chr[i].unicode = p_buf[i];
                chr[i].w = 0;
                chr[i].h = text->font_height;
                uint32_t offset = 0;
                if (chr[i].unicode == 0x20 || chr[i].unicode == 0x0D)
                {
                    chr[i].char_w = text->font_height / 2;
                    chr[i].char_h = text->font_height / 2;
                }
                else if (chr[i].unicode == 0x0A)
                {
                    line_flag ++;
                    chr[i].char_w = 0;
                    chr[i].char_h = 0;
                }
                else
                {
                    uint32_t *offset_addr = (uint32_t *)((uint8_t *)(uintptr_t)table_offset + chr[i].unicode *
                                                         index_unit_length);
                    offset = *offset_addr;
                    if (offset == 0xFFFFFFFF) { continue; }
                    chr[i].dot_addr = (uint8_t *)text->path + offset + 4;
                    if (text->font_mode == FONT_SRC_MEMADDR)
                    {
                        chr[i].char_w = (uint8_t)(*(chr[i].dot_addr - 2));
                        chr[i].char_y = (uint8_t)(*(chr[i].dot_addr - 4));
                        chr[i].char_h = (uint8_t)(*(chr[i].dot_addr - 1)) - chr[i].char_y;
                        line_byte = (chr[i].char_w * rendor_mode + 8 - 1) / 8;
                        chr[i].w = line_byte * 8 / rendor_mode;
                    }
                    else if (text->font_mode == FONT_SRC_FTL)
                    {
                        gui_ftl_read((uintptr_t)chr[i].dot_addr - 2, &chr[i].char_w, 1);
                        gui_ftl_read((uintptr_t)chr[i].dot_addr - 4, &chr[i].char_y, 1);
                        gui_ftl_read((uintptr_t)chr[i].dot_addr - 1, &chr[i].char_h, 1);
                        chr[i].char_h = chr[i].char_h - chr[i].char_y;
                        line_byte = (chr[i].char_w * rendor_mode + 8 - 1) / 8;
                        chr[i].w = line_byte * 8 / rendor_mode;

                        uint8_t dot_size = chr[i].w * chr[i].char_h;
                        uint8_t *dot_buf = gui_malloc(dot_size);
                        gui_ftl_read((uintptr_t)chr[i].dot_addr, dot_buf, dot_size);
                        chr[i].dot_addr = dot_buf;
                    }
                }
                all_char_w += chr[i].char_w;
            }
            break;
        case 1: //offset
            {
                //todo by luke
            }
            break;
        default:
            break;
        }
    }
    else
    {
        uint8_t aliened_font_size;
        aliened_font_size = text->font_height;
        if (text->font_height % 8 != 0)
        {
            aliened_font_size = 8 - text->font_height % 8 + text->font_height;
        }
        uint32_t font_area = aliened_font_size * text->font_height / 8 * rendor_mode + 4;
        switch (index_method)
        {
        case 0: //address
            for (uint32_t i = 0; i < unicode_len; i++)
            {
                chr[i].unicode = p_buf[i];
                chr[i].w = aliened_font_size;
                chr[i].h = text->font_height;
                uint16_t offset = 0;
                if (chr[i].unicode == 0x20 || chr[i].unicode == 0x0D)
                {
                    chr[i].char_w = text->font_height / 2;
                    chr[i].char_h = text->font_height / 2;
                }
                else if (chr[i].unicode == 0x0A)
                {
                    line_flag ++;
                    chr[i].char_w = 0;
                    chr[i].char_h = 0;
                }
                else
                {
                    offset = *(uint16_t *)(uintptr_t)(chr[i].unicode * 2 + table_offset);
                    if (offset == 0xFFFF) { continue; }
                    chr[i].dot_addr = (uint8_t *)(uintptr_t)((uintptr_t)offset * font_area + dot_offset + 4);
                    if (text->font_mode == FONT_SRC_MEMADDR)
                    {
                        chr[i].char_w = (int16_t)(*(chr[i].dot_addr - 2));
                        chr[i].char_h = (int16_t)(*(chr[i].dot_addr - 1));
                    }
                    else if (text->font_mode == FONT_SRC_FTL)
                    {
                        gui_ftl_read((uintptr_t)chr[i].dot_addr - 2, &chr[i].char_w, 1);
                        gui_ftl_read((uintptr_t)chr[i].dot_addr - 1, &chr[i].char_h, 1);
                        uint8_t *dot_buf = gui_malloc(text->font_height * text->font_height);
                        gui_ftl_read((uintptr_t)chr[i].dot_addr, dot_buf, text->font_height * text->font_height);
                        chr[i].dot_addr = dot_buf;
                    }
                }
                all_char_w += chr[i].char_w;
                all_char_h = all_char_h + chr[i].char_h + 2;
            }
            break;
        case 1: //offset
            {
                uint32_t index_area_size = font->index_area_size;
                for (uint32_t i = 0; i < unicode_len; i++)
                {
                    chr[i].unicode = p_buf[i];
                    chr[i].w = aliened_font_size;
                    chr[i].h = text->font_height;
                    chr[i].char_w = text->font_height / 2;
                    chr[i].char_h = text->font_height;
                    uint32_t index = 0;
                    if (chr[i].unicode == 0x0A)
                    {
                        line_flag ++;
                        chr[i].char_w = 0;
                        chr[i].char_h = 0;
                    }
                    else if (chr[i].unicode == 0x20)
                    {
                        chr[i].char_w = text->font_height / 2;
                        chr[i].char_h = text->font_height / 2;
                    }
                    else
                    {
                        for (; index < index_area_size / 2; index ++)
                        {
                            if (chr[i].unicode == *(uint16_t *)(uintptr_t)(table_offset + index * 2))
                            {
                                chr[i].dot_addr = (uint8_t *)(uintptr_t)((uintptr_t)index * font_area + dot_offset + 4);
                                if (text->font_mode == FONT_SRC_MEMADDR)
                                {
                                    chr[i].char_w = (int16_t)(*(chr[i].dot_addr - 2));
                                    chr[i].char_h = (int16_t)(*(chr[i].dot_addr - 1));
                                    break;
                                }
                                else if (text->font_mode == FONT_SRC_FTL)
                                {
                                    gui_ftl_read((uintptr_t)chr[i].dot_addr - 2, &chr[i].char_w, 1);
                                    gui_ftl_read((uintptr_t)chr[i].dot_addr - 1, &chr[i].char_h, 1);
                                    uint8_t *dot_buf = gui_malloc(text->font_height * text->font_height);
                                    gui_ftl_read((uintptr_t)chr[i].dot_addr, dot_buf, text->font_height * text->font_height);
                                    chr[i].dot_addr = dot_buf;
                                    break;
                                }
                            }
                        }
                    }
                    all_char_w += chr[i].char_w;
                }
            }
            break;
        default:
            break;
        }
    }
    text->char_width_sum = all_char_w;
    text->char_height_sum = all_char_h;
    text->char_line_sum = line_flag;
    text->font_len = unicode_len;
    switch (text->charset)
    {
    case UTF_8_CHARSET:
        gui_free(p_buf);
        break;
    case UTF_16_CHARSET:
        break;
    default:
        break;
    }
}

void gui_font_mem_load(gui_text_t *text, gui_text_rect_t *rect)
{
    gui_font_get_dot_info(text);
    gui_font_mem_layout(text, rect);
}

void gui_font_mem_layout(gui_text_t *text, gui_text_rect_t *rect)
{
    mem_char_t *chr = text->data;
    int rect_w = rect->x2 - rect->x1;
    switch (text->mode)
    {
    case LEFT:
    case CENTER:
    case RIGHT:
        {
            text->char_line_sum = 1;
            int offset = _UI_MAX((rect_w - text->char_width_sum) / 2, 0);
            for (uint16_t i = 0; i < text->font_len; i++)
            {
                chr[i].y = rect->y1;
                if (i == 0)
                {
                    chr[i].x = rect->x1 + offset * text->mode;
                }
                else
                {
                    chr[i].x = chr[i - 1].x + chr[i - 1].char_w;
                }
                if ((chr[i].x + chr[i].char_w) >= rect->x2)
                {
                    text->font_len = i;
                    break;
                }
            }
            break;
        }
    case MULTI_LEFT:
    case MULTI_CENTER:
    case MULTI_RIGHT:
        {
            gui_text_line_t *line_buf;
            uint32_t line = 0;
            text->char_line_sum = text->char_width_sum / rect_w + 1 + text->char_line_sum;
            line_buf = gui_malloc((text->char_line_sum + 1) * sizeof(gui_text_line_t));
            memset(line_buf, 0, (text->char_line_sum + 1) * sizeof(gui_text_line_t));
            for (uint16_t i = 0; i < text->font_len; i++)
            {
                // gui_log("chr[i].y_bound is %d, chr[i].h_bound is %d\n",chr[i].y_bound,chr[i].h_bound);
                if (i == 0)
                {
                    chr[i].x = rect->x1;
                }
                else
                {
                    chr[i].x = chr[i - 1].x + chr[i - 1].char_w;
                }
                if ((chr[i].x + chr[i].char_w) > rect->x2 || chr[i - 1].unicode == 0x0A)
                {
                    line_buf[line].line_char = i - 1;
                    line_buf[line].line_dx = (rect_w - chr[i].x + rect->x1) / 2 * (text->mode - 3);
                    line++;
                    chr[i].x = rect->x1;
                }
                chr[i].y = rect->y1 + line * chr[i].h;
                if (chr[i].y >= rect->y2)
                {
                    text->font_len = i;
                    break;
                }
            }
            line_buf[line].line_char = text->font_len - 1;
            line_buf[line].line_dx = (rect_w - chr[text->font_len - 1].x + rect->x1 -
                                      chr[text->font_len - 1].char_w) / 2 * (text->mode - 3);
            line = 0;
            for (uint16_t i = 0; i < text->font_len; i++)
            {
                chr[i].x += line_buf[line].line_dx;
                if (i >= line_buf[line].line_char)
                {
                    line++;
                }
            }
            text->char_line_sum = line;
            gui_free(line_buf);
            break;
        }
    case SCROLL_X:
        {
            text->char_line_sum = 1;
            for (uint16_t i = 0; i < text->font_len; i++)
            {
                chr[i].y = rect->y1;
                if (i == 0)
                {
                    chr[i].x = rect->x1;
                }
                else
                {
                    chr[i].x = chr[i - 1].x + chr[i - 1].char_w;
                }
                if (chr[i].x > rect->xboundright)
                {
                    text->font_len = i;
                    break;
                }
            }
            break;
        }
    case SCROLL_Y:
        {
            uint32_t line = 0;
            for (uint16_t i = 0; i < text->font_len; i++)
            {
                // gui_log("chr[i].y_bound is %d, chr[i].h_bound is %d\n",chr[i].y_bound,chr[i].h_bound);
                if (i == 0)
                {
                    chr[i].x = rect->x1;
                }
                else
                {
                    chr[i].x = chr[i - 1].x + chr[i - 1].char_w;
                }
                if ((chr[i].x + chr[i].char_w) > rect->x2 || chr[i - 1].unicode == 0x0A)
                {
                    line++;
                    chr[i].x = rect->x1;
                }
                chr[i].y = rect->y1 + line * chr[i].h;
                if (text->char_line_sum != 0)
                {
                    if (chr[i].y >= rect->yboundbottom)
                    {
                        text->font_len = i;
                        break;
                    }
                }
            }
            if (text->char_line_sum == 0)
            {
                text->char_line_sum = line;
            }
            break;
        }
    case SCROLL_Y_REVERSE:
        {
            for (uint16_t i = 0; i < text->font_len; i++)
            {
                chr[i].x = rect->x1;
                if (i == 0)
                {
                    chr[i].y = rect->y2 - chr[i].char_h;
                }
                else
                {
                    chr[i].y = chr[i - 1].y - chr[i].char_h - 2;
                }
            }
            break;
        }
    case VERTICAL_LEFT:
        {
            uint32_t line = 0;
            for (uint16_t i = 0; i < text->font_len; i++)
            {
                if (i == 0)
                {
                    chr[i].y = rect->y1;
                }
                else
                {
                    chr[i].y = chr[i - 1].y + chr[i - 1].char_h + 2;
                }
                if ((chr[i].y + chr[i].char_y + chr[i].char_h) >= rect->y2 || chr[i - 1].unicode == 0x0A)
                {
                    line++;
                    chr[i].y = rect->y1;
                }
                chr[i].x = rect->x1 - line * chr[i].w;
                if (chr[i].x < 0)
                {
                    text->font_len = i;
                    break;
                }
            }
            break;
        }
    case VERTICAL_RIGHT:
        {
            uint32_t line = 0;
            for (uint16_t i = 0; i < text->font_len; i++)
            {
                if (i == 0)
                {
                    chr[i].y = rect->y2 - chr[i].char_h;
                }
                else
                {
                    chr[i].y = chr[i - 1].y - chr[i].char_h - 2;
                }
                if (chr[i].y  <= rect->y1 || chr[i - 1].unicode == 0x0A)
                {
                    line++;
                    chr[i].y = rect->y2 - chr[i].char_h;
                }
                chr[i].x = rect->x1 + line * chr[i].w;
                if (chr[i].x >= rect->x2)
                {
                    text->font_len = i;
                    break;
                }
            }
            break;
        }
    default:
        break;
    }
}

void gui_font_mem_unload(gui_text_t *text)
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

// Fast RGB565 pixel blending
// Found in a pull request for the Adafruit framebuffer library. Clever!
// https://github.com/tricorderproject/arducordermini/pull/1/files#diff-d22a481ade4dbb4e41acc4d7c77f683d
gui_inline uint16_t alphaBlendRGB565(uint32_t fg, uint32_t bg, uint8_t alpha)
{
    // Alpha converted from [0..255] to [0..31]
    alpha = (alpha + 4) >> 3;
    // Converts  0000000000000000rrrrrggggggbbbbb
    //     into  00000gggggg00000rrrrr000000bbbbb
    // with mask 00000111111000001111100000011111
    // This is useful because it makes space for a parallel fixed-point multiply
    // bg = (bg | (bg << 16)) & 0b00000111111000001111100000011111;
    // fg = (fg | (fg << 16)) & 0b00000111111000001111100000011111;
    bg = (bg | (bg << 16)) & 0x7e0f81f;
    fg = (fg | (fg << 16)) & 0x7e0f81f;
    // This implements the linear interpolation formula: result = bg * (1.0 - alpha) + fg * alpha
    // This can be factorized into: result = bg + (fg - bg) * alpha
    // alpha is in Q1.5 format, so 0.0 is represented by 0, and 1.0 is represented by 32
    uint32_t result = (fg - bg) * alpha; // parallel fixed-point multiply of all components
    result >>= 5;
    result += bg;
    // result &= 0b00000111111000001111100000011111; // mask out fractional parts
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
static void rtk_draw_unicode(mem_char_t *chr, gui_color_t color, uint8_t rendor_mode,
                             gui_text_rect_t *rect, bool crop)
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
#if 1 //0 - Stable slow processing , 1 - Fast processing in tests
            uint16_t *writebuf = (uint16_t *)dc->frame_buf;
            uint16_t color_output = rgba2565(color);
            uint8_t ppb = 4;//pixel_per_byte = 8 / rendor_mode
            int write_off = (y_start - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1);
            int dots_off = (y_start - font_y) * (font_w / ppb);
            int left_offset = 0, right_offset = 0, byte = 0;
            uint16_t color_back;
            uint8_t alpha_2bit;
            if (font_x + chr->char_w > x_end)
            {
                while (font_x + ppb * byte < x_end)
                {
                    byte++;
                }
                right_offset = x_end - font_x - ppb * byte + ppb;
                byte = 0;
            }
            if (font_x < x_start)
            {
                byte = 0;
                while (font_x + ppb * byte < x_start)
                {
                    byte++;
                }
                left_offset = font_x + ppb * byte - x_start;
            }
            for (uint32_t i = y_start; i < y_end; i++)
            {
                uint8_t *temp_p = &dots[dots_off + byte];

                for (uint32_t j = x_start; j < x_start + left_offset; j++)
                {
                    uint8_t alpha = dots[(i - font_y) * (font_w / ppb) + (j - font_x) / ppb] >>
                                    ((j - font_x) % ppb * 2);
                    if (alpha != 0)
                    {
                        alpha = alpha & 0x03;
                        alpha = alpha * 85;
                        alpha = color.color.rgba.a * alpha / 0xff;
                        color_back = writebuf[write_off + j - dc->section.x1];
                        writebuf[write_off + j - dc->section.x1] = alphaBlendRGB565(color_output, color_back, alpha);
                    }
                }
                for (uint32_t j = x_start + left_offset; j < x_end - right_offset;)
                {
                    uint8_t alpha = *temp_p;
                    if (alpha != 0)
                    {
                        if (alpha & 0x03)
                        {
                            alpha_2bit = alpha & 0x03;
                            if (alpha_2bit == 0x03)
                            {
                                writebuf[write_off + j - dc->section.x1] = color_output;
                            }
                            else
                            {
                                alpha_2bit *= 85;
//                              alpha_2bit = color.color.rgba.a * alpha_2bit / 0xff;
                                color_back = writebuf[write_off + j - dc->section.x1];
                                writebuf[write_off + j - dc->section.x1] = alphaBlendRGB565(color_output, color_back, alpha_2bit);
                            }
                        }
                        if (alpha & 0x0C)
                        {
                            alpha_2bit = (alpha >> 2) & 0x03;
                            if (alpha_2bit == 0x03)
                            {
                                writebuf[write_off + j + 1 - dc->section.x1] = color_output;
                            }
                            else
                            {
                                alpha_2bit *= 85;
//                              alpha_2bit = color.color.rgba.a * alpha_2bit / 0xff;
                                color_back = writebuf[write_off + j + 1 - dc->section.x1];
                                writebuf[write_off + j + 1 - dc->section.x1] = alphaBlendRGB565(color_output, color_back,
                                                                                                alpha_2bit);
                            }
                        }
                        if (alpha & 0x30)
                        {
                            alpha_2bit = (alpha >> ppb) & 0x03;
                            if (alpha_2bit == 0x03)
                            {
                                writebuf[write_off + j + 2 - dc->section.x1] = color_output;
                            }
                            else
                            {
                                alpha_2bit *= 85;
//                              alpha_2bit = color.color.rgba.a * alpha_2bit / 0xff;
                                color_back = writebuf[write_off + j + 2 - dc->section.x1];
                                writebuf[write_off + j + 2 - dc->section.x1] = alphaBlendRGB565(color_output, color_back,
                                                                                                alpha_2bit);
                            }
                        }
                        if (alpha & 0xC0)
                        {
                            alpha_2bit = (alpha >> 6) & 0x03;
                            if (alpha_2bit == 0x03)
                            {
                                writebuf[write_off + j + 3 - dc->section.x1] = color_output;
                            }
                            else
                            {
                                alpha_2bit *= 85;
//                              alpha_2bit = color.color.rgba.a * alpha_2bit / 0xff;
                                color_back = writebuf[write_off + j + 3 - dc->section.x1];
                                writebuf[write_off + j + 3 - dc->section.x1] = alphaBlendRGB565(color_output, color_back,
                                                                                                alpha_2bit);
                            }
                        }
                    }
                    temp_p++;
                    j += ppb;
                }
                for (uint32_t j = x_end - right_offset; j < x_end; j++)
                {
                    uint8_t alpha = dots[(i - font_y) * (font_w / ppb) + (j - font_x) / ppb] >>
                                    ((j - font_x) % ppb * 2);
                    if (alpha != 0)
                    {
                        alpha = alpha & 0x03;
                        alpha = alpha * 85;
                        alpha = color.color.rgba.a * alpha / 0xff;
                        color_back = writebuf[write_off + j - dc->section.x1];
                        writebuf[write_off + j - dc->section.x1] = alphaBlendRGB565(color_output, color_back, alpha);
                    }
                }
                write_off += dc->section.x2 - dc->section.x1 + 1;
                dots_off += (font_w / ppb);
            }
#else
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
                int write_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) ;
                for (uint32_t j = x_start; j < x_end; j++)
                {
                    uint8_t alpha = dots[(i - font_y) * (font_w / ppb) + (j - font_x) / ppb] >>
                                    ((j - font_x) % ppb * 2);
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
#endif
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
                int write_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) ;
                for (uint32_t j = x_start; j < x_end; j++)
                {
                    uint8_t alpha = dots[(i - font_y) * (font_w / ppb) + (j - font_x) / ppb] >>
                                    ((j - font_x) % ppb * 2);
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
            for (uint32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) ;
                for (uint32_t j = x_start; j < x_end; j++)
                {
                    uint8_t alpha = dots[(i - font_y) * (font_w / ppb) + (j - font_x) / ppb] >>
                                    ((j - font_x) % ppb * 2);
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
            for (uint32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) ;
                for (uint32_t j = x_start; j < x_end; j++)
                {
                    uint8_t alpha = dots[(i - font_y) * (font_w / ppb) + (j - font_x) / ppb] >>
                                    ((j - font_x) % ppb * 4);
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
            for (uint32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) ;
                for (uint32_t j = x_start; j < x_end; j++)
                {
                    uint8_t alpha = (dots[(i - font_y) * (font_w / ppb) + (j - font_x) / ppb] >>
                                     ((j - font_x) % ppb * 4));
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
            for (uint32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) ;
                for (uint32_t j = x_start; j < x_end; j++)
                {
                    uint8_t alpha = (dots[(i - font_y) * (font_w / ppb) + (j - font_x) / ppb] >>
                                     ((j - font_x) % ppb * 4));
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
            for (uint32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1);
                int dots_off = (i - font_y) * font_w - font_x;
                for (uint32_t j = x_start; j < x_end; j++)
                {
                    uint8_t alpha = dots[dots_off + j];
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
            for (uint32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) ;
                int dots_off = (i - font_y) * font_w - font_x;
                for (uint32_t j = x_start; j < x_end; j++)
                {
                    uint8_t alpha = dots[dots_off + j];
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
            for (uint32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) ;
                int dots_off = (i - font_y) * font_w - font_x;
                for (uint32_t j = x_start; j < x_end; j++)
                {
                    uint8_t alpha = dots[dots_off + j];
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
#if 1 //0 - Stable slow processing , 1 - Fast processing in tests
            uint16_t *writebuf = (uint16_t *)dc->frame_buf;
            uint16_t color_output = rgba2565(color);
            uint8_t ppb = 8;//pixel_per_byte = 8 / rendor_mode
            int write_off = (y_start - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1);
            int dots_off = (y_start - font_y) * (font_w / ppb);
            int left_offset = 0, right_offset = 0, byte = 0;
            uint32_t x_start_right = x_start;
            if (font_x + chr->char_w > x_end)
            {
                while (font_x + ppb * byte < x_end)
                {
                    byte++;
                }
                right_offset = x_end - font_x - ppb * byte + ppb;
                byte = 0;
            }
            if (font_x < x_start)
            {
                while (font_x + ppb * byte < x_start)
                {
                    byte++;
                }
                left_offset = font_x + ppb * byte - x_start;
                x_start_right = _UI_MIN((x_start + left_offset), x_end);
            }
            for (uint32_t i = y_start; i < y_end; i++)
            {
                uint8_t *temp_p = &dots[dots_off + byte];

                for (uint32_t j = x_start; j < x_start_right; j++)
                {
                    if ((dots[dots_off + (j - font_x) / ppb] >> ((j - font_x) % ppb)) & 0x01)
                    {
                        writebuf[write_off + j - dc->section.x1] = color_output;
                    }
                }
                for (uint32_t j = x_start + left_offset; j < x_end - right_offset;)
                {
                    uint8_t temp = *temp_p;
                    if (temp)
                    {
                        uint16_t offset = write_off - dc->section.x1 + font_x + (j - font_x) / ppb * ppb;
                        if (temp & 0x0F)
                        {
                            if (temp & 0x01)
                            {
                                writebuf[offset + 0] = color_output;
                            }
                            if (temp & 0x02)
                            {
                                writebuf[offset + 1] = color_output;
                            }
                            if (temp & 0x04)
                            {
                                writebuf[offset + 2] = color_output;
                            }
                            if (temp & 0x08)
                            {
                                writebuf[offset + 3] = color_output;
                            }
                        }
                        if (temp & 0xF0)
                        {
                            if (temp & 0x10)
                            {
                                writebuf[offset + 4] = color_output;
                            }
                            if (temp & 0x20)
                            {
                                writebuf[offset + 5] = color_output;
                            }
                            if (temp & 0x40)
                            {
                                writebuf[offset + 6] = color_output;
                            }
                            if (temp & 0x80)
                            {
                                writebuf[offset + 7] = color_output;
                            }
                        }
                    }
                    temp_p++;
                    j += ppb;
                }
                for (uint32_t j = x_end - right_offset; j < x_end; j++)
                {
                    if ((dots[dots_off + (j - font_x) / ppb] >> ((j - font_x) % ppb)) & 0x01)
                    {
                        writebuf[write_off + j - dc->section.x1] = color_output;
                    }
                }
                write_off += dc->section.x2 - dc->section.x1 + 1;
                dots_off += (font_w / ppb);
            }
#else
            uint16_t *writebuf = (uint16_t *)dc->frame_buf;
            uint16_t color_output = rgba2565(color);
            uint8_t ppb = 8;//pixel_per_byte = 8 / rendor_mode
            for (uint32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1);
                int dots_off = (i - font_y) * (font_w / ppb);
                for (uint32_t j = x_start; j < x_end; j++)
                {
                    if ((dots[dots_off + (j - font_x) / ppb] >> ((j - font_x) % ppb)) & 0x01)
                    {
                        writebuf[write_off + j - dc->section.x1] = color_output;
                    }
                }
            }
#endif
        }
        else if (dc_bytes_per_pixel == 3)
        {
            uint8_t *writebuf = (uint8_t *)dc->frame_buf;
            uint8_t color_back[3];
            uint8_t alpha = color.color.rgba.a;
            uint8_t ppb = 8;//pixel_per_byte = 8 / rendor_mode
            for (uint32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) ;
                int dots_off = (i - font_y) * (font_w / ppb);
                for (uint32_t j = x_start; j < x_end; j++)
                {
                    if ((dots[dots_off + (j - font_x) / ppb] >> ((j - font_x) % ppb)) & 0x01)
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
            for (uint32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) ;
                int dots_off = (i - font_y) * (font_w / ppb);
                for (uint32_t j = x_start; j < x_end; j++)
                {
                    if ((dots[dots_off + (j - font_x) / ppb] >> ((j - font_x) % ppb)) & 0x01)
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
void gui_font_mem_draw(gui_text_t *text, gui_text_rect_t *rect)
{
    mem_char_t *chr = text->data;
    GUI_FONT_HEAD *font = (GUI_FONT_HEAD *)text->path;
    uint8_t rendor_mode = font->rendor_mode;
    for (uint16_t i = 0; i < text->font_len; i++)
    {
        rtk_draw_unicode(chr + i, text->color, rendor_mode, rect, font->font_mode_detail.detail.crop);
    }
}


uint8_t gui_font_mem_init_ftl(uint8_t *font_bin_addr)
{
    if (!font_bin_addr)
    {
        return UINT8_MAX;
    }
    uint8_t *data = gui_malloc(sizeof(GUI_FONT_HEAD));
    gui_ftl_read((uintptr_t)font_bin_addr, data, sizeof(GUI_FONT_HEAD));

    GUI_FONT_HEAD *font = (GUI_FONT_HEAD *)data;
    if (font->file_type != FONT_FILE_FLAG)
    {
        gui_log("this font file is not valid \n");
        gui_free(data);
        return UINT8_MAX;
    }
    int i = 0;
    for (; i < sizeof(font_lib_tab) / sizeof(MEM_FONT_LIB); i++)
    {
        if (font_lib_tab[i].font_file == NULL)
        {
            break;
        }
        if (font_lib_tab[i].font_file == font_bin_addr)
        {
            if (font_lib_tab[i].type == FONT_SRC_FTL)
            {
                gui_log("this font file has been created \n");
                gui_free(data);
                return i;
            }
            break;
        }
    }
    if (i >= sizeof(font_lib_tab) / sizeof(MEM_FONT_LIB))
    {
        gui_free(data);
        return UINT8_MAX;
    }

    uint32_t head_index_len = font->head_length + font->index_area_size;
    data = gui_realloc(data, head_index_len);
    gui_ftl_read((uintptr_t)font_bin_addr, data, head_index_len);

    font_lib_tab[i].font_file = font_bin_addr;
    font_lib_tab[i].font_size = font->font_size;
    font_lib_tab[i].type = FONT_SRC_FTL;
    font_lib_tab[i].data = data;
    gui_log("font_lib_tab[%d].data has been created, need to free, size : %d \n", i, head_index_len);
    return i;
}

uint8_t gui_font_mem_init_fs(uint8_t *font_bin_addr)
{
    return gui_font_mem_init(font_bin_addr);
}

uint8_t gui_font_mem_init_mem(uint8_t *font_bin_addr)
{
    return gui_font_mem_init(font_bin_addr);
}

uint8_t gui_font_mem_init(uint8_t *font_bin_addr)
{
    if (!font_bin_addr)
    {
        return UINT8_MAX;
    }
    GUI_FONT_HEAD *font = (GUI_FONT_HEAD *)font_bin_addr;
    if (font->file_type != 0x01)
    {
        gui_log("this font file is not valid \n");
        return UINT8_MAX;
    }
    int i = 0;
    for (; i < sizeof(font_lib_tab) / sizeof(MEM_FONT_LIB); i++)
    {
        if (font_lib_tab[i].font_file == NULL)
        {
            break;
        }
        if (font_lib_tab[i].font_file == font_bin_addr)
        {
            break;
        }
    }
    if (i >= sizeof(font_lib_tab) / sizeof(MEM_FONT_LIB))
    {
        return UINT8_MAX;
    }

    font_lib_tab[i].font_file = font_bin_addr;
    font_lib_tab[i].font_size = font->font_size;
    font_lib_tab[i].type = FONT_SRC_MEMADDR;
    return i;
}

uint8_t gui_font_mem_destroy(uint8_t *font_bin_addr)
{
    if (!font_bin_addr)
    {
        return UINT8_MAX;
    }
    int i = 0;
    for (; i < sizeof(font_lib_tab) / sizeof(MEM_FONT_LIB); i++)
    {
        if (font_lib_tab[i].font_file == font_bin_addr)
        {
            if (font_lib_tab[i].data)
            {
                gui_free(font_lib_tab[i].data);
            }
            memset(&font_lib_tab[i], 0, sizeof(MEM_FONT_LIB));
            gui_log("font_lib_tab[%d].data has been free \n", i);
            return i;
        }
    }
    return UINT8_MAX;
}

uint32_t gui_get_mem_utf8_char_width(void *content, void *font_bin_addr)
{
    return gui_get_mem_char_width(content, font_bin_addr, UTF_8_CHARSET);
}

uint32_t gui_get_mem_char_width(void *content, void *font_bin_addr, TEXT_CHARSET charset)
{
    GUI_FONT_HEAD *font = (GUI_FONT_HEAD *)font_bin_addr;
    uint32_t string_len = strlen(content);
    uint32_t table_offset = (uint32_t)(uintptr_t)((uint8_t *)font_bin_addr + font->head_length);
    uint32_t dot_offset = table_offset + font->index_area_size;

    uint8_t aliened_font_size = font->font_size;
    if (font->font_size % 8 != 0)
    {
        aliened_font_size = 8 - font->font_size % 8 + font->font_size;
    }
    uint32_t font_area = aliened_font_size * font->font_size / 8 * font->rendor_mode + 4;
    uint8_t index_unit_length = 4; //now set to 4 , todo
    uint16_t *unicode_buffer = NULL;
    uint16_t unicode_len = 0;
    switch (charset)
    {
    case UTF_8_CHARSET:
        unicode_buffer = gui_malloc(string_len * sizeof(uint16_t));
        if (unicode_buffer == NULL)
        {
            GUI_ASSERT(NULL != NULL);
            return 0;
        }
        else
        {
            unicode_len = utf8_to_unicode(content, string_len, unicode_buffer, string_len);
        }
        break;
    case UTF_16_CHARSET:
        unicode_len = string_len / 2;
        unicode_buffer = (uint16_t *)content;
        break;
    default:
        break;
    }
    uint32_t all_char_w = 0;
    uint32_t line_flag = 0;

    if (font->font_mode_detail.detail.crop)
    {
        switch (font->font_mode_detail.detail.index_method)
        {
        case 0: //address
            for (uint32_t i = 0; i < unicode_len; i++)
            {
                uint32_t offset = 0;
                uint16_t char_w = 0;
                if (unicode_buffer[i] == 0x20 || unicode_buffer[i] == 0x0D)
                {
                    char_w = font->font_size / 2;
                }
                else if (unicode_buffer[i] == 0x0A)
                {
                    line_flag ++;
                    char_w = 0;
                }
                else
                {
                    uint32_t *offset_addr = (uint32_t *)((uint8_t *)(uintptr_t)table_offset + unicode_buffer[i] *
                                                         index_unit_length);
                    offset = *offset_addr;
                    if (offset == 0xFFFFFFFF) { continue; }
                    uint8_t *dot_addr = (uint8_t *)font_bin_addr + offset + 4;
                    char_w = (uint8_t)(*(dot_addr - 2));
                }
                all_char_w += char_w;
            }
            break;
        case 1: //offset
            for (uint32_t i = 0; i < unicode_len; i++)
            {
                //todo by luke
            }
            break;
        default:
            break;
        }
    }
    else
    {
        switch (font->font_mode_detail.detail.index_method)
        {
        case 0: //address
            for (uint32_t i = 0; i < unicode_len; i++)
            {
                uint16_t offset = 0;
                uint16_t char_w = 0;
                if (unicode_buffer[i] == 0x20 || unicode_buffer[i] == 0x0D)
                {
                    char_w = font->font_size / 2;
                }
                else if (unicode_buffer[i] == 0x0A)
                {
                    line_flag ++;
                    char_w = 0;
                }
                else
                {
                    offset = *(uint16_t *)(uintptr_t)(unicode_buffer[i] * 2 + table_offset);
                    if (offset == 0xFFFF) { continue; }
                    uint8_t *dot_addr = (uint8_t *)(uintptr_t)((uintptr_t)offset * font_area + dot_offset + 4);
                    char_w = (int16_t)(*(dot_addr - 2));
                }
                all_char_w += char_w;
            }
            break;
        case 1: //offset
            for (uint32_t i = 0; i < unicode_len; i++)
            {
                uint16_t char_w = 0;
                uint32_t index = 0;
                if (unicode_buffer[i] == 0x0A)
                {
                    line_flag ++;
                    char_w = 0;
                }
                else if (unicode_buffer[i] == 0x20)
                {
                    char_w = font->font_size / 2;
                }
                else
                {
                    for (; index < font->index_area_size / 2; index ++)
                    {
                        if (unicode_buffer[i] == *(uint16_t *)(uintptr_t)(table_offset + index * 2))
                        {
                            uint8_t *dot_addr = (uint8_t *)(uintptr_t)((uintptr_t)index * font_area + dot_offset + 4);
                            char_w = (int16_t)(*(dot_addr - 2));
                        }
                    }
                }
                all_char_w += char_w;
            }
            break;
        default:
            break;
        }
    }
    switch (charset)
    {
    case UTF_8_CHARSET:
        gui_free(unicode_buffer);
        break;
    case UTF_16_CHARSET:
        break;
    default:
        break;
    }
    return all_char_w;
}
