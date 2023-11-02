
#include <string.h>
#include <draw_font.h>
static struct font_lib font_lib_tab[RTK_GUI_FONT_MEM];
typedef struct
{
    uint16_t unicode;
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
    int16_t char_w;
    uint8_t *dot_addr;
} mem_char_t;

typedef struct
{
    uint8_t *font_lib_name;
    uint8_t *font_lib_tab_name;
    uint8_t rendor_mode : 4;
    uint8_t index_method : 1;
} fontlib_name_t;
static fontlib_name_t fontlib_name;
static fontlib_name_t *get_fontlib_name(uint8_t *dot_addr, uint8_t font_size)
{
    fontlib_name_t *font_name = &fontlib_name;
    uint8_t tab_size = sizeof(font_lib_tab) / sizeof(struct font_lib);
    if (dot_addr)
    {
        for (size_t i = 0; i < tab_size; i++)
        {
            if (font_lib_tab[i].dot_name == dot_addr || font_lib_tab[i].font_file == dot_addr)
            {
                font_name->font_lib_name = font_lib_tab[i].dot_name;
                font_name->font_lib_tab_name = font_lib_tab[i].table_name;
                font_name->rendor_mode = font_lib_tab[i].rendor_mode;
                font_name->index_method = font_lib_tab[i].font_mode_detail.detail.index_method;
                return font_name;
            }
        }
    }
    else if (font_size)
    {
        for (size_t i = 0; i < tab_size; i++)
        {
            if (font_lib_tab[i].font_size == font_size)
            {
                font_name->font_lib_name = font_lib_tab[i].dot_name;
                font_name->font_lib_tab_name = font_lib_tab[i].table_name;
                font_name->rendor_mode = font_lib_tab[i].rendor_mode;
                font_name->index_method = font_lib_tab[i].font_mode_detail.detail.index_method;
                return font_name;
            }
        }
    }
    return NULL;
}

void rtgui_font_mem_load(gui_text_t *text)
{
    fontlib_name_t *font_name;
    font_name = get_fontlib_name(text->path, text->font_height);

    uint32_t dot_offset;
    uint32_t table_offset;
    uint8_t rendor_mode;
    if (text->font_height == 0)
    {
        text->font_height = RTK_GUI_DEFAULT_FONT_SIZE;
    }

    dot_offset = (uint32_t)font_name->font_lib_name;
    table_offset = (uint32_t)font_name->font_lib_tab_name;
    rendor_mode = font_name->rendor_mode;
    // gui_log("dot_offset is  %p , table_offset is %p \n", dot_offset, table_offset);

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
    uint8_t aliened_font_size;
    aliened_font_size = text->font_height;
    if (text->font_height % 8 != 0)
    {
        aliened_font_size = 8 - text->font_height % 8 + text->font_height;
    }

    uint32_t font_area = aliened_font_size * text->font_height / 8 * rendor_mode + 4;

    uint16_t *p_buf = gui_malloc(text->len * sizeof(uint16_t));
    if (p_buf == NULL)
    {
        GUI_ASSERT(NULL != NULL);
        return;
    }
    uint16_t unicode_len = 0;
    if (p_buf)
    {
        unicode_len = utf8_to_unicode(text->utf_8, text->len, p_buf, text->len);
    }

    uint32_t all_char_w = 0;
    uint32_t line_flag = 0;
    switch (font_name->index_method)
    {
    case 0: //address
        for (uint32_t i = 0; i < unicode_len; i++)
        {
            chr[i].unicode = p_buf[i];
            chr[i].w = aliened_font_size;
            chr[i].h = text->font_height;
            uint16_t offset = 0;
            if (chr[i].unicode == 0x20 || chr[i].unicode == 0x0D || offset == 0xFFFF)
            {
                offset = *(uint16_t *)(0x20 * 2 + table_offset);
                if (offset == 0xFFFF) { continue; }
                chr[i].dot_addr = (uint8_t *)(offset * font_area + dot_offset + 4);
                chr[i].char_w = text->font_height / 2;
            }
            else if (chr[i].unicode == 0x0A)
            {
                line_flag ++;
                offset = *(uint16_t *)(0x20 * 2 + table_offset);
                if (offset == 0xFFFF) { continue; }
                chr[i].dot_addr = (uint8_t *)(offset * font_area + dot_offset + 4);
                chr[i].char_w = 0;
            }
            else
            {
                offset = *(uint16_t *)(chr[i].unicode * 2 + table_offset);
                if (offset == 0xFFFF) { continue; }
                chr[i].dot_addr = (uint8_t *)(offset * font_area + dot_offset + 4);
                chr[i].char_w = (int16_t)(*(chr[i].dot_addr - 2));
            }
            all_char_w += chr[i].char_w;
        }
        break;
    case 1: //offset
        {
            void *index_area_size_addr = ((uint8_t *)text->path + 9);
            uint32_t index_area_size = *(uint32_t *)index_area_size_addr;
            for (uint32_t i = 0; i < unicode_len; i++)
            {
                chr[i].unicode = p_buf[i];
                chr[i].w = aliened_font_size;
                chr[i].h = text->font_height;
                chr[i].char_w = text->font_height / 2;
                uint32_t index = 0;
                if (chr[i].unicode == 0x0A)
                {
                    line_flag ++;
                    chr[i].char_w = 0;
                }
                else
                {
                    for (; index < index_area_size / 2; index ++)
                    {
                        if (chr[i].unicode == *(uint16_t *)(table_offset + index * 2))
                        {
                            chr[i].dot_addr = (uint8_t *)(index * font_area + dot_offset + 4);
                            chr[i].char_w = (int16_t)(*(chr[i].dot_addr - 2));
                            break;
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
    if (text->text_offset == 0)
    {
        switch (text->mode)
        {
        case LEFT:
        case CENTER:
        case RIGHT:
            text->text_offset = (text->base.w - all_char_w) / 2;
            break;
        case MUTI_LEFT:
        case MUTI_CENTER:
        case MUTI_RIGHT:
            text->text_offset = all_char_w / text->base.w + 1 + line_flag;
            break;
        case SCROLL_X:
            text->text_offset = all_char_w;
            break;
        case SCROLL_Y:
        // text->text_offset = (all_char_w / text->base.w + 1 + line_flag)*text->font_height;
        default:
            break;
        }
    }
    text->font_len = unicode_len;
    gui_free(p_buf);
}

void rtgui_font_mem_unload(gui_text_t *text)
{
    gui_free(text->data);
    return;
}

#define _b_and_f(color_b,color_f,c) (color_b.channel.c * color_b.channel.alpha + color_f.channel.c * color_f.channel.alpha) / 0xff;
gui_inline uint32_t blend_b_and_f_with_a(uint32_t b, uint32_t f, uint8_t a)
{
    gui_color_t color_b = {.rgba = b};
    gui_color_t color_f = {.rgba = f};
    color_f.channel.alpha = a;
    gui_color_msb_t color_r;
    color_b.channel.alpha = 0xff - color_f.channel.alpha;
    color_r.channel.blue = _b_and_f(color_b, color_f, blue)
                           color_r.channel.green = _b_and_f(color_b, color_f, green)
                                                   color_r.channel.red = _b_and_f(color_b, color_f, red)
                                                                         return color_r.rgba;
}
gui_inline uint16_t rgba2565(uint32_t rgba)
{
    gui_color_t color_blend = {.rgba = rgba};
    uint16_t red = color_blend.channel.red * 0x1f / 0xff << 11;
    uint16_t green = color_blend.channel.green * 0x3f / 0xff << 5;
    uint16_t blue = color_blend.channel.blue * 0x1f / 0xff;
    return red + green + blue;
}
gui_inline uint32_t rgb5652rgba(uint16_t rgb565)
{
    gui_color_t color_blend ;
    color_blend.channel.red = (int)((rgb565 & 0xf800) >> 11) * 0xff / 0x1f;
    color_blend.channel.green = (int)((rgb565 & 0x07e0) >> 5) * 0xff / 0x3f;
    color_blend.channel.blue = (int)(rgb565 & 0x001F) * 0xff / 0x1f;
    color_blend.channel.alpha = 0xff;

    return color_blend.rgba;
}

static void rtk_draw_unicode(int dx, mem_char_t *chr, uint32_t color, uint8_t rendor_mode,
                             struct rtgui_rect *rect)
{
    if (chr->dot_addr == NULL)
    {
        return;
    }
    uint8_t *dots = chr->dot_addr;
    gui_dispdev_t *dc = gui_get_dc();
    int font_x = chr->x + dx;
    int font_y = chr->y;
    int font_w = chr->w;
    int font_h = chr->h;
    // rt_kprintf("chr->x %d, chr->y %d, chr->w %d, chr->h %d\n",chr->x,chr->y,chr->w,chr->h);
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
    // gui_log("chr->h_bound is %d\n");
    if (rect->yboundbottom != 0)
    {
        y_end = _UI_MIN(_UI_MIN(dc->section.y2, font_y + font_h), rect->yboundbottom);
    }
    else
    {
        y_end = _UI_MIN(dc->section.y2, font_y + font_h);
    }
    // rt_kprintf("x_start is  %d, x_end is %d\n", x_start, x_end);
    // rt_kprintf("y_start is  %d, y_end is %d\n", y_start, y_end);
    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }
    // rt_kprintf("dc->section.y1 %d, font_y %d, chr->y_bound %d, chr->h_bound %d\n",dc->section.y1,font_y,chr->y_bound,chr->h_bound);
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;//

    switch (rendor_mode)
    {
    case 4:
        if (dc_bytes_per_pixel == 2)
        {
            uint16_t *writebuf = (uint16_t *)dc->frame_buf;
            uint16_t color_back;
            for (uint32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * dc->fb_width ;

                for (uint32_t j = x_start; j < x_end; j++)
                {
                    uint8_t alpha = dots[(i - font_y) * (font_w / 2) + (j - font_x) / 2] >> (4 - (j - font_x) % 2 * 4);
                    if (alpha != 0)
                    {
                        alpha = alpha & 0x0f;
                        alpha *= 16;
                        color_back = writebuf[write_off + j];
                        writebuf[write_off + j] = rgba2565(blend_b_and_f_with_a(rgb5652rgba(color_back), color, alpha));
                    }
                }
            }
        }
        else if (dc_bytes_per_pixel == 3)
        {
            uint8_t *writebuf = (uint8_t *)dc->frame_buf;
            uint8_t color_back[3];
            uint8_t *pixel = (uint8_t *)&color;
            for (uint32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * dc->fb_width ;
                for (uint32_t j = x_start; j < x_end; j++)
                {
                    uint8_t alpha = (dots[(i - font_y) * (font_w / 2) + (j - font_x) / 2] >> (4 -
                                                                                              (j - font_x) % 2 * 4));
                    if (alpha != 0)
                    {
                        alpha = alpha & 0x0f;
                        alpha *= 16;
                        color_back[0] = writebuf[write_off * 3 + j * 3 + 2];
                        color_back[1] = writebuf[write_off * 3 + j * 3 + 1];
                        color_back[2] = writebuf[write_off * 3 + j * 3 + 0];
                        writebuf[write_off * 3 + j * 3 + 0] = (pixel[2] * alpha + color_back[2] * (0xff - alpha) / 0xff);
                        writebuf[write_off * 3 + j * 3 + 1] = (pixel[1] * alpha + color_back[1] * (0xff - alpha) / 0xff);
                        writebuf[write_off * 3 + j * 3 + 2] = (pixel[0] * alpha + color_back[0] * (0xff - alpha) / 0xff);
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
                int write_off = (i - dc->section.y1) * dc->fb_width ;

                for (uint32_t j = x_start; j < x_end; j++)
                {
                    uint8_t alpha = (dots[(i - font_y) * (font_w / 2) + (j - font_x) / 2] >> (4 -
                                                                                              (j - font_x) % 2 * 4));
                    if (alpha != 0)
                    {
                        alpha = alpha & 0x0f;
                        alpha *= 16;
                        color_back = writebuf[write_off + j];
                        writebuf[write_off + j] = blend_b_and_f_with_a(color_back, color, alpha);
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
            for (uint32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * dc->fb_width ;

                for (uint32_t j = x_start; j < x_end; j++)
                {
                    uint8_t alpha = dots[(i - font_y) * font_w + (j - font_x)];
                    if (alpha != 0)
                    {
                        color_back = writebuf[write_off + j];
                        writebuf[write_off + j] = rgba2565(blend_b_and_f_with_a(rgb5652rgba(color_back), color, alpha));
                    }
                }
            }
        }
        else if (dc_bytes_per_pixel == 3)
        {
            uint8_t *writebuf = (uint8_t *)dc->frame_buf;
            uint8_t color_back[3];
            uint8_t *pixel = (uint8_t *)&color;
            for (uint32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * dc->fb_width ;
                for (uint32_t j = x_start; j < x_end; j++)
                {
                    uint8_t alpha = dots[(i - font_y) * font_w + (j - font_x)];
                    if (alpha != 0)
                    {
                        color_back[0] = writebuf[write_off * 3 + j * 3 + 2];
                        color_back[1] = writebuf[write_off * 3 + j * 3 + 1];
                        color_back[2] = writebuf[write_off * 3 + j * 3 + 0];
                        writebuf[write_off * 3 + j * 3 + 0] = (pixel[2] * alpha + color_back[2] * (0xff - alpha) / 0xff);
                        writebuf[write_off * 3 + j * 3 + 1] = (pixel[1] * alpha + color_back[1] * (0xff - alpha) / 0xff);
                        writebuf[write_off * 3 + j * 3 + 2] = (pixel[0] * alpha + color_back[0] * (0xff - alpha) / 0xff);
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
                int write_off = (i - dc->section.y1) * dc->fb_width ;

                for (uint32_t j = x_start; j < x_end; j++)
                {
                    uint8_t alpha = dots[(i - font_y) * font_w + (j - font_x)];
                    if (alpha != 0)
                    {
                        color_back = writebuf[write_off + j];
                        writebuf[write_off + j] = blend_b_and_f_with_a(color_back, color, alpha);
                    }
                }
            }
        }
        break;
    case 1:
        if (dc_bytes_per_pixel == 2)
        {
            uint16_t *writebuf = (uint16_t *)dc->frame_buf;
            for (uint32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * dc->fb_width ;

                for (uint32_t j = x_start; j < x_end; j++)
                {
                    if ((dots[(i - font_y) * (font_w / 8) + (j - font_x) / 8] >> (7 - (j - font_x) % 8)) & 0x01)
                    {
                        writebuf[write_off + j] = rgba2565(color);
                    }
                }
            }
        }
        else if (dc_bytes_per_pixel == 3)
        {
            uint8_t *writebuf = (uint8_t *)dc->frame_buf;
            uint8_t *pixel = (uint8_t *)&color;
            for (uint32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * dc->fb_width ;

                for (uint32_t j = x_start; j < x_end; j++)
                {
                    if ((dots[(i - font_y) * (font_w / 8) + (j - font_x) / 8] >> (7 - (j - font_x) % 8)) & 0x01)
                    {
                        writebuf[write_off * 3 + j * 3] = pixel[2];
                        writebuf[write_off * 3 + j * 3 + 1] = pixel[1];
                        writebuf[write_off * 3 + j * 3 + 2] = pixel[0];
                    }
                }
            }
        }
        else if (dc_bytes_per_pixel == 4)
        {
            uint32_t *writebuf = (uint32_t *)dc->frame_buf;

#if defined(_WIN32)
            uint32_t alpha = color & 0xff;
            color = (color >> 8) + (alpha << 24);
#endif

            for (uint32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * dc->fb_width ;

                for (uint32_t j = x_start; j < x_end; j++)
                {
                    if ((dots[(i - font_y) * (font_w / 8) + (j - font_x) / 8] >> (7 - (j - font_x) % 8)) & 0x01)
                    {
                        writebuf[write_off + j] = color;
                    }
                }
            }
        }
        break;
    default:
        break;
    }
}


void rtgui_font_mem_draw(gui_text_t *text, struct rtgui_rect *rect)
{
    mem_char_t *chr = text->data;
    gui_dispdev_t *dc = gui_get_dc();
    gui_text_line_t *line_buf;
    uint16_t dx = 0;
    uint32_t line = 0;
    fontlib_name_t *font_name = get_fontlib_name(text->path, text->font_height);
    uint8_t rendor_mode = font_name->rendor_mode;
    switch (text->mode)
    {
    case LEFT:
    case CENTER:
    case RIGHT:
        dx = text->text_offset * text->mode;
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
            if ((chr[i].x + chr[i].char_w) > rect->x2)
            {
                text->font_len = i;
                break;
            }
            rtk_draw_unicode(dx, chr + i, text->color, rendor_mode, rect);
        }
        break;
    case MUTI_LEFT:
    case MUTI_CENTER:
    case MUTI_RIGHT:
        line_buf = gui_malloc((text->text_offset + 1) * sizeof(gui_text_line_t));
        memset(line_buf, 0, (text->text_offset + 1) * sizeof(gui_text_line_t));
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
                line_buf[line].line_dx = (text->base.w - chr[i].x + rect->x1) / 2 * (text->mode - 3);
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
        line_buf[line].line_dx = (text->base.w - chr[text->font_len - 1].x + rect->x1 - chr[text->font_len -
                                  1].char_w) / 2 * (text->mode - 3);
        line = 0;
        for (uint16_t i = 0; i < text->font_len; i++)
        {
            if (i > line_buf[line].line_char)
            {
                line++;
            }
            rtk_draw_unicode(line_buf[line].line_dx, chr + i, text->color, rendor_mode, rect);
        }
        gui_free(line_buf);
        break;
    case SCROLL_X:
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
            rtk_draw_unicode(0, chr + i, text->color, rendor_mode, rect);
        }
        break;
    case SCROLL_Y:
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
            if (text->text_offset != 0)
            {
                if (chr[i].y >= rect->yboundbottom)
                {
                    text->font_len = i;
                    break;
                }
            }
            rtk_draw_unicode(0, chr + i, text->color, rendor_mode, rect);
        }
        if (text->text_offset == 0)
        {
            text->text_offset = line * text->font_height;
        }
        break;
    default:
        break;
    }
}


struct rtgui_font_engine rtgui_font_mem_engine =
{
    "rtk_font_mem",
    { NULL },
    rtgui_font_mem_load,
    rtgui_font_mem_unload,
    rtgui_font_mem_draw,
};

void rtgui_font_mem_init(void)
{
    rtgui_font_register_engine(&rtgui_font_mem_engine);
}

void gui_set_font_mem_resourse(unsigned char font_size, void *font_bitmap_addr,
                               void *font_table_addr)
{
    int i = 0;
    for (; i < sizeof(font_lib_tab) / sizeof(struct font_lib); i++)
    {
        if (font_lib_tab[i].dot_name == 0 && font_lib_tab[i].font_size == 0 &&
            font_lib_tab[i].table_name == 0)
        {
            break;
        }
        if (font_lib_tab[i].dot_name == font_bitmap_addr)
        {
            break;
        }
    }
    if (i >= sizeof(font_lib_tab) / sizeof(struct font_lib))
    {
        return;
    }
    font_lib_tab[i].font_file = font_bitmap_addr;
    font_lib_tab[i].dot_name = font_bitmap_addr;
    font_lib_tab[i].font_size = font_size;
    font_lib_tab[i].table_name = font_table_addr;
    font_lib_tab[i].rendor_mode = 1;
    font_lib_tab[i].font_mode_detail.value = 0;
}

void gui_font_mem_init(uint8_t *font_bin_addr)
{
    int i = 0;
    if (!font_bin_addr)
    {
        return;
    }
    uint8_t font_file_flag = * (uint8_t *)(font_bin_addr + 1);
    if (font_file_flag == 0)
    {
        return;
    }
    for (; i < sizeof(font_lib_tab) / sizeof(struct font_lib); i++)
    {
        if (font_lib_tab[i].dot_name == 0)
        {
            break;
        }
        if (font_lib_tab[i].font_file == font_bin_addr)
        {
            break;
        }
    }
    if (i >= sizeof(font_lib_tab) / sizeof(struct font_lib))
    {
        return;
    }
    font_lib_tab[i].font_file = font_bin_addr;
    font_lib_tab[i].font_size = * (uint8_t *)(font_bin_addr + 6);
    font_lib_tab[i].rendor_mode = * (uint8_t *)(font_bin_addr + 7);
    uint8_t head_length = * (uint8_t *)(font_bin_addr);
    font_lib_tab[i].table_name = (uint8_t *)(font_bin_addr + head_length);
    void *index_area_size_addr = (uint8_t *)(font_bin_addr + 9);
    uint32_t index_area_size = *(uint32_t *)index_area_size_addr;
    font_lib_tab[i].dot_name = (uint8_t *)(font_bin_addr + index_area_size + head_length);
    font_lib_tab[i].font_mode_detail.value =  * (uint8_t *)(font_bin_addr + 8);
}
