
#include <font.h>
#include <dfs_posix.h>
#include "GT5SLCW5SL96.h"
#include "font_genitop_algo.h"
typedef struct
{
    uint32_t unicode;
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
    uint32_t dot_off;
} genitop_char_t;
int genitop_fd = 0;
static bool font_genitop_init_flag = false;
void rtgui_font_genitop_load(struct rtgui_font *font)
{
    font->fd_table = open("/font/GT5SLCW5SL96.bin", O_RDONLY | O_BINARY, 0);
    if (font->fd_table < 0)
    {
        GUI_PRINTF("create filerw failed! line = %d \n", __LINE__);
        return;
    }
    genitop_fd = font->fd_table;
    if (font_genitop_init_flag == false)
    {
        if (font_genitop_algo_init())
        {
            font_genitop_init_flag = true;
        }
        //return;
    }

    genitop_char_t *chr = malloc(sizeof(genitop_char_t) * font->len);
    font->data = chr;

    uint16_t *p_buf = NULL;
    p_buf = malloc(128 * sizeof(uint16_t));
    uint16_t unicode_len = utf8_to_unicode(font->utf8, font->len, p_buf, 128);

    for (uint32_t i = 0; i < unicode_len; i++)
    {
        chr[i].unicode = U2G(p_buf[i]);
        chr[i].w = 32;
        chr[i].h = 32;
    }
    font->len = unicode_len;
    free(p_buf);

}

void rtgui_font_genitop_unload(struct rtgui_font *font)
{
    free(font->data);
    close(font->fd_table);
    font->data = RT_NULL;
    free(font);
}

static void genitop_draw_unicode(genitop_char_t *chr, struct rtgui_dc *dc, uint16_t color)
{
    uint16_t *writebuf = (uint16_t *)dc->frame_buf;

    uint8_t dots[128];

    int font_x = chr->x;
    int font_y = chr->y;
    int font_w = chr->w;
    int font_h = chr->h;

    int x_start = _UI_MAX(font_x, 0);
    int x_end = _UI_MIN(font_x + font_w, dc->width);
    int y_start = _UI_MAX(dc->section.y1, font_y);
    int y_end = _UI_MIN(dc->section.y2, font_y + font_h);


    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }

    get_font(dots, VEC_BLACK_STY, chr->unicode, 32, 32, 64);
    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->width ;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            if ((dots[(i - font_y) * (font_w / 8) + (j - font_x) / 8] >> (7 - (j - font_x) % 8)) & 0x01)
            {
                writebuf[write_off + j] = color;
            }
        }
    }
}

void rtgui_font_genitop_draw(struct rtgui_font *font, struct rtgui_dc *dc, struct rtgui_rect *rect)
{
    genitop_char_t *chr = font->data;


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
                chr[i].x = chr[i - 1].x + chr[i - 1].w;
            }
            if ((chr[i].x + chr[i].w) > rect->x2)
            {
                line++;
                chr[i].x = rect->x1;
            }
            chr[i].y = rect->y1 + line * chr[i].h;
        }
    }

    for (uint16_t i = 0; i < font->len; i++)
    {
        genitop_draw_unicode(chr + i, dc, font->color);
    }

}


struct rtgui_font_engine rtgui_font_genitop_engine =
{
    "genitop",
    { RT_NULL },
    rtgui_font_genitop_load,
    rtgui_font_genitop_unload,
    rtgui_font_genitop_draw,
};

void rtgui_font_genitop_init(void)
{
    rtgui_font_register_engine(&rtgui_font_genitop_engine);
}

