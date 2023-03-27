#include <draw_font.h>


typedef struct
{
    uint16_t unicode;
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
    int16_t char_w;
    uint32_t dot_off;
    uint32_t font_area;
} rtk_char_t;


void rtgui_font_rtk_load(struct rtgui_font *font)
{
    font->font_size = 32;
    font->fd_table = gui_fs_open(FS_FONT_TABLE_PATH, 0);
    if (font->fd_table < 0)
    {
        gui_log("create font lib table filerw failed! line = %d \n", __LINE__);
        return;
    }
    font->fd_dot = gui_fs_open(FS_FONT_DOT_PATH, 0);
    if (font->fd_dot < 0)
    {
        gui_log("create font lib filerw failed! line = %d \n", __LINE__);
        return;
    }
    rtk_char_t *chr = gui_malloc(sizeof(rtk_char_t) * font->len);
    font->data = chr;
    uint8_t aliened_font_size;
    aliened_font_size = font->font_size;
    if (font->font_size % 8 != 0)
    {
        aliened_font_size = 8 - font->font_size % 8 + font->font_size;
    }

    uint16_t *p_buf = NULL;
    uint32_t font_area;
    font_area = aliened_font_size * font->font_size / 8;
    p_buf = gui_malloc(font_area * sizeof(uint16_t));
    uint16_t unicode_len = utf8_to_unicode(font->utf8, font->len, p_buf, font_area);
    for (uint32_t i = 0; i < unicode_len; i++)
    {
        chr[i].unicode = p_buf[i];

        int offset = gui_fs_lseek(font->fd_table, chr[i].unicode * 2, SEEK_SET);
        uint8_t dot_addr[2];
        if (gui_fs_read(font->fd_table, dot_addr, sizeof(dot_addr)) != sizeof(dot_addr))
        {
            gui_log("read dot addr Fail! \n");
        }
        chr[i].dot_off = font_area * (dot_addr[0] + (dot_addr[1] << 8));
        chr[i].w = aliened_font_size;
        chr[i].h = font->font_size;
        chr[i].font_area = font_area;
    }
    for (size_t i = 0; i < unicode_len; i++)
    {
        //gui_log("%c \n", string[i]);
        if (p_buf[i] >= 0 && p_buf[i] <= 127)
        {
            //gui_log("%c = %d\n",string[i],string[i]);
            chr[i].char_w = font->font_size / 2;
        }
        else
        {
            //gui_log("chinese \n");
            chr[i].char_w = font->font_size;
        }
    }
    font->len = unicode_len;
    gui_free(p_buf);

}

void rtgui_font_rtk_unload(struct rtgui_font *font)
{
    gui_free(font->data);
    gui_fs_close(font->fd_table);
    gui_fs_close(font->fd_dot);
    font->data = NULL;
    gui_free(font);
}
gui_inline rtgui_color_t rtgui_color_from_565(uint16_t pixel)
{
    int16_t r, g, b;
    rtgui_color_t color;
#ifdef RTGUI_rgb_CHANGE_ENDIAN
    pixel = (((pixel & 0x00ff) << 8) + ((pixel & 0xff00) >> 8));
#endif
    r = (pixel >> 11) & 0x1f;
    g = (pixel >> 5)  & 0x3f;
    b = pixel & 0x1f;

    color = b * 255 / 31 + ((g * 255 / 63) << 8) + ((r * 255 / 31) << 16);

    return color;
}

gui_inline rtgui_color_t rtgui_color_from_565p(uint16_t pixel)
{
    uint8_t r, g, b;
    rtgui_color_t color;

    r = pixel & 0x1f;
    g = (pixel >> 5) & 0x3f;
    b = (pixel >> 11) & 0x1f;

    color = b * 255 / 31 + ((g * 255 / 63) << 8) + ((r * 255 / 31) << 16);

    return color;
}
static void rtk_draw_unicode(int fd, rtk_char_t *chr, struct gui_dispdev *dc, uint16_t color)
{
    uint16_t *writebuf = (uint16_t *)dc->frame_buf;

    uint8_t dots[chr->font_area];

    int font_x = chr->x;
    int font_y = chr->y;
    int font_w = chr->w;
    int font_h = chr->h;

    int x_start = _UI_MAX(font_x, 0);
    int x_end = _UI_MIN(font_x + chr->char_w, dc->fb_width);
    int y_start = _UI_MAX(dc->section.y1, font_y);
    int y_end = _UI_MIN(dc->section.y2, font_y + font_h);


    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;//

    gui_fs_lseek(fd, chr->dot_off, SEEK_SET);
    gui_fs_read(fd, dots, chr->font_area);

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
        GUI_ASSERT(NULL != NULL);
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
                    writebuf[write_off + j] = rtgui_color_from_565(color);
                    writebuf[write_off + j] = rtgui_color_from_565p(color);
                }
            }
        }

    }

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

void rtgui_font_rtk_draw(struct rtgui_font *font, struct gui_dispdev *dc, struct rtgui_rect *rect)
{
    rtk_char_t *chr = font->data;


    if (dc->section_count == 0)
    {
        uint32_t line = 0;
        for (uint16_t i = 0; i < font->len; i++)
        {
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
                line++;
                chr[i].x = rect->x1;
            }
            chr[i].y = rect->y1 + line * chr[i].h;
            if (chr[i].y >= rect->y2)
            {
                font->len = i;
                break;
            }
        }
    }

    for (uint16_t i = 0; i < font->len; i++)
    {
        rtk_draw_unicode(font->fd_dot, chr + i, dc, font->color);
    }

}


struct rtgui_font_engine rtgui_font_rtk_engine =
{
    "rtk_font_fs32",
    { NULL },
    rtgui_font_rtk_load,
    rtgui_font_rtk_unload,
    rtgui_font_rtk_draw,
};

void rtgui_font_rtk_init(void)
{
    rtgui_font_register_engine(&rtgui_font_rtk_engine);
}

