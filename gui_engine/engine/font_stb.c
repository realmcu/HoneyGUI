#include "gui_config.h"
#include <draw_font.h>
#define STB_TRUETYPE_IMPLEMENTATION
// #define ALLOW_UNALIGNED_TRUETYPE
#include "stb_truetype.h"

static stbtt_fontinfo font;

static void rtgui_font_stb_load(gui_text_t *text)
{

}

static void rtgui_font_stb_unload(gui_text_t *text)
{

}

#define _b_and_f(color_b,color_f,c) (color_b.channel.c * color_b.channel.alpha + color_f.channel.c * color_f.channel.alpha) / 0xff;
gui_inline uint32_t blend_b_and_f(uint32_t b, uint32_t f, uint8_t a)
{
    gui_color_t color_b = {.rgba = b};
    gui_color_t color_f = {.rgba = f};
    color_f.channel.alpha = a;
    gui_color_t color_r;
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

static void rtgui_font_stb_draw(gui_text_t *text, struct rtgui_rect *rect)
{
    int width = rect->x2 - rect->x1;
    int height = rect->y2 - rect->y1;
    uint8_t *screen = gui_malloc((width) * (height) * sizeof(uint8_t));
    if (screen == NULL)
    {
        GUI_ASSERT(NULL != NULL);
    }
    memset(screen, 0, (width) * (height)*sizeof(uint8_t));
    int ascent = 0, baseline = 0, ch = 0;
    float scale = 0, xpos = 0;
    scale = stbtt_ScaleForPixelHeight(&font, text->font_height);
    stbtt_GetFontVMetrics(&font, &ascent, 0, 0);
    baseline = (int)(ascent * scale);

    uint16_t *p_buf = gui_malloc(text->len * sizeof(uint16_t));
    if (p_buf == NULL)
    {
        GUI_ASSERT(NULL != NULL);
    }
    uint16_t unicode_len = utf8_to_unicode(text->utf_8, text->len, p_buf, text->len);
    // p_buf[unicode_len]=0;
    while (ch < unicode_len)
    {
        int advance, lsb, x0, y0, x1, y1;
        float x_shift = xpos - (float) floor(xpos);
        stbtt_GetCodepointHMetrics(&font, p_buf[ch], &advance, &lsb);
        stbtt_GetCodepointBitmapBoxSubpixel(&font, p_buf[ch], scale, scale, x_shift, 0, &x0, &y0, &x1, &y1);
        // if ((baseline + y0) >= height || ((int) xpos + x0) >= width)
        // {
        //     printf("baseline + y0 %d,  ((int) xpos + x0) %d\n",baseline + y0, ((int) xpos + x0));
        // }
        stbtt_MakeCodepointBitmapSubpixel(&font, &screen[(baseline + y0)*width + (int) xpos + x0],
                                          x1 - x0, y1 - y0, width, scale, scale, x_shift, 0, p_buf[ch]);
        xpos += (advance * scale);
        if (ch < unicode_len - 1)
        {
            xpos += scale * stbtt_GetCodepointKernAdvance(&font, p_buf[ch], p_buf[ch + 1]);
        }
        ++ch;
    }
    gui_free(p_buf);
    gui_dispdev_t *dc = gui_get_dc();
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;

    if (dc_bytes_per_pixel == 4)
    {
        uint32_t *writebuf = (uint32_t *)dc->frame_buf + (rect->y1) * dc->fb_width + rect->x1;
        uint32_t color_back;
        for (uint32_t i = 0; i < height && (i + rect->y1 <= dc->fb_height); i++)
        {
            int write_off = (i + dc->section.y1) * dc->fb_width;
            for (uint32_t j = 0; j < width && (j + rect->x1 <= dc->fb_width); j++)
            {
                if (screen[i * width + j] != 0)
                {
                    color_back = writebuf[write_off + j];
                    writebuf[write_off + j] = blend_b_and_f(color_back, text->color, screen[i * width + j]);
                }
            }
        }
    }
    else if (dc_bytes_per_pixel == 2)
    {
        uint16_t *writebuf = (uint16_t *)dc->frame_buf + (rect->y1) * dc->fb_width + rect->x1;
        uint16_t color_back;
        for (uint32_t i = 0; i < height && (i + rect->y1 <= dc->fb_height); i++)
        {
            int write_off = (i + dc->section.y1) * dc->fb_width;
            for (uint32_t j = 0; j < width && (j + rect->x1 <= dc->fb_width); j++)
            {
                if (screen[i * width + j] != 0)
                {
                    color_back = writebuf[write_off + j];
                    // writebuf[write_off + j] = rgba2565(blend_b_and_f(rgb5652rgba(color_back_565), rgb5652rgba(text->color),screen[i][j]));
                    writebuf[write_off + j] = rgba2565(blend_b_and_f(rgb5652rgba(color_back), text->color,
                                                                     screen[i * width + j]));
                }
            }
        }
    }
    gui_free(screen);
}

struct rtgui_font_engine rtgui_font_stb_engine =
{
    "rtk_font_stb",
    { NULL },
    rtgui_font_stb_load,
    rtgui_font_stb_unload,
    rtgui_font_stb_draw,
};

void rtgui_font_stb_init(void)
{
    rtgui_font_register_engine(&rtgui_font_stb_engine);
}

void gui_font_stb_init(void *font_ttf_addr)
{
    stbtt_InitFont(&font, font_ttf_addr, 0);
}