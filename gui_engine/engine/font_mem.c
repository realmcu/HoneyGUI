
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
    char *font_lib_name;
    char *font_lib_tab_name;
} fontlib_name_t;
static fontlib_name_t fontlib_name;
static fontlib_name_t *get_fontlib_name(uint8_t font_size)
{
    fontlib_name_t *font_name = &fontlib_name;
    uint8_t tab_size = sizeof(font_lib_tab) / sizeof(struct font_lib);
    for (size_t i = 0; i < tab_size; i++)
    {
        if (font_lib_tab[i].font_size == font_size)
        {
            font_name->font_lib_name = font_lib_tab[i].dot_name;
            font_name->font_lib_tab_name = font_lib_tab[i].table_name;
            return font_name;
        }
    }
    return NULL;
}

void rtgui_font_mem_load(gui_text_t *text)
{
    fontlib_name_t *font_name;
    font_name = get_fontlib_name(text->font_height);

    uint32_t dot_offset;
    uint32_t table_offset;
    if (text->font_height == 0)
    {
        text->font_height = RTK_GUI_DEFAULT_FONT_SIZE;
    }

    dot_offset = (uint32_t)font_name->font_lib_name;
    table_offset = (uint32_t)font_name->font_lib_tab_name;
    // gui_log("dot_offset is  %p , table_offset is %p \n", dot_offset, table_offset);

    mem_char_t *chr = gui_malloc(sizeof(mem_char_t) * text->len);
    if (chr == NULL)
    {
        GUI_ASSERT(NULL != NULL);
    }
    text->data = chr;
    uint8_t aliened_font_size;
    aliened_font_size = text->font_height;
    if (text->font_height % 8 != 0)
    {
        aliened_font_size = 8 - text->font_height % 8 + text->font_height;
    }

    uint32_t font_area = aliened_font_size * text->font_height / 8 + 4;

    uint16_t *p_buf = gui_malloc(text->len * sizeof(uint16_t));
    if (p_buf == NULL)
    {
        GUI_ASSERT(NULL != NULL);
    }

    uint16_t unicode_len = utf8_to_unicode(text->utf_8, text->len, p_buf, text->len);
    uint32_t all_char_w = 0;
    uint32_t line_flag = 0;
    for (uint32_t i = 0; i < unicode_len; i++)
    {
        chr[i].unicode = p_buf[i];
        chr[i].w = aliened_font_size;
        chr[i].h = text->font_height;
        uint16_t offset = 0;
        if (chr[i].unicode == 0x20 || chr[i].unicode == 0x0D || offset == 0xFFFF)
        {
            offset = *(uint16_t *)(0x20 * 2 + table_offset);
            chr[i].dot_addr = (uint8_t *)(offset * font_area + dot_offset + 4);
            chr[i].char_w = text->font_height / 2;
        }
        else if (chr[i].unicode == 0x0A)
        {
            line_flag ++;
            offset = *(uint16_t *)(0x20 * 2 + table_offset);
            chr[i].dot_addr = (uint8_t *)(offset * font_area + dot_offset + 4);
            chr[i].char_w = 0;
        }
        else
        {
            offset = *(uint16_t *)(chr[i].unicode * 2 + table_offset);
            chr[i].dot_addr = (uint8_t *)(offset * font_area + dot_offset + 4);
            chr[i].char_w = (int16_t)(*(chr[i].dot_addr - 2));
        }
        all_char_w += chr[i].char_w;
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
//gui_inline rtgui_color_t rtgui_color_from_565(uint16_t pixel)
//{
//    int16_t r, g, b;
//    rtgui_color_t color;
//#ifdef RTGUI_rgb_CHANGE_ENDIAN
//    pixel = (((pixel & 0x00ff) << 8) + ((pixel & 0xff00) >> 8));
//#endif
//    r = (pixel >> 11) & 0x1f;
//    g = (pixel >> 5)  & 0x3f;
//    b = pixel & 0x1f;

//    color = b * 255 / 31 + ((g * 255 / 63) << 8) + ((r * 255 / 31) << 16);

//    return color;
//}

//gui_inline rtgui_color_t rtgui_color_from_565p(uint16_t pixel)
//{
//    uint8_t r, g, b;
//    rtgui_color_t color;

//    r = pixel & 0x1f;
//    g = (pixel >> 5) & 0x3f;
//    b = (pixel >> 11) & 0x1f;

//    color = b * 255 / 31 + ((g * 255 / 63) << 8) + ((r * 255 / 31) << 16);

//    return color;
//}
static void rtk_draw_unicode(int dx, mem_char_t *chr, uint32_t color, struct rtgui_rect *rect)
{
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
                    writebuf[write_off + j] = color;
                }
            }
        }
    }
    else if (dc_bytes_per_pixel == 3)
    {

    }
    else if (dc_bytes_per_pixel == 4)
    {
        uint32_t *writebuf = (uint32_t *)dc->frame_buf;
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
}

void rtgui_font_mem_draw(gui_text_t *text, struct rtgui_rect *rect)
{
    mem_char_t *chr = text->data;
    gui_dispdev_t *dc = gui_get_dc();
    gui_text_line_t *line_buf;
    uint16_t dx = 0;
    uint32_t line = 0;
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
            rtk_draw_unicode(dx, chr + i, text->color, rect);
        }
        break;
    case MUTI_LEFT:
    case MUTI_CENTER:
    case MUTI_RIGHT:
        line_buf = gui_malloc(text->text_offset * sizeof(gui_text_line_t));
        memset(line_buf, 0, text->text_offset * sizeof(gui_text_line_t));
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
            rtk_draw_unicode(line_buf[line].line_dx, chr + i, text->color, rect);
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
            rtk_draw_unicode(0, chr + i, text->color, rect);
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
            rtk_draw_unicode(0, chr + i, text->color, rect);
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
    size_t i = 0;
    for (; i < sizeof(font_lib_tab) / sizeof(struct font_lib); i++)
    {
        if (font_lib_tab[i].font_size == font_size)
        {
            break;
        }
        if (font_lib_tab[i].dot_name == 0 && font_lib_tab[i].font_size == 0 &&
            font_lib_tab[i].table_name == 0)
        {
            break;
        }
    }
    font_lib_tab[i].dot_name = font_bitmap_addr;
    font_lib_tab[i].font_size = font_size;
    font_lib_tab[i].table_name = font_table_addr;
}
