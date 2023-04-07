#include "gui_config.h"
#include <draw_font.h>
#define STB_TRUETYPE_IMPLEMENTATION
// #define ALLOW_UNALIGNED_TRUETYPE
#include "stb_truetype.h"
#include "acc_engine.h"
// #define RTK_GUI_TTF_SVG

static stbtt_fontinfo font;

static void rtgui_font_stb_load(gui_text_t *text)
{

}

static void rtgui_font_stb_unload(gui_text_t *text)
{

}

#define _b_and_f(color_b,color_f,c) (color_b.channel.c * color_b.channel.alpha + color_f.channel.c * color_f.channel.alpha) / 0xff;
gui_inline uint32_t blend_b_and_f_with_a(uint32_t b, uint32_t f, uint8_t a)
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
#include "nanosvg.h"
#include "nanosvgrast.h"
#ifdef RTK_GUI_TTF_SVG
static void stb_add_path(NSVGshape *shape, stbtt_vertex *stbVertex, int line_count)
{
    NSVGpath *path = NULL;
    path = gui_malloc(sizeof(NSVGpath));
    if (path == NULL)
    {
        GUI_ASSERT(NULL != NULL);
    }
    memset(path, 0, sizeof(NSVGpath));

    path->closed = 1;
    path->npts = (line_count) * 3 + 1;

    path->pts = (float *)gui_malloc(path->npts * 2 * sizeof(float));
    if (path->pts == NULL)
    {
        GUI_ASSERT(NULL != NULL);
    }

    if (shape->paths)
    {
        path->next = shape->paths;
    }
    shape->paths = path;
}
#endif
static void rtgui_font_stb_draw(gui_text_t *text, struct rtgui_rect *rect)
{
    int width = rect->x2 - rect->x1;
    int height = rect->y2 - rect->y1;
    int ascent = 0, descent = 0, lineGap = 0, ch = 0;
    float scale = 0, xpos = 0, baseline = 0;
    scale = stbtt_ScaleForPixelHeight(&font, text->font_height);
    stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);
    baseline = ascent * scale;
    // baseline = 900 * scale;
    gui_dispdev_t *dc = gui_get_dc();
    uint16_t *p_buf = gui_malloc(text->len * sizeof(uint16_t));
    if (p_buf == NULL)
    {
        GUI_ASSERT(NULL != NULL);
    }
    uint16_t unicode_len = utf8_to_unicode(text->utf_8, text->len, p_buf, text->len);

#ifdef RTK_GUI_TTF_SVG
    while (ch < unicode_len)
    {
        int glyph_index = stbtt_FindGlyphIndex(&font, p_buf[ch]);
        int advance, lsb, x0, y0, x1, y1;
        stbtt_GetGlyphHMetrics(&font, glyph_index, &advance, &lsb);
        if (xpos + advance * scale >= text->base.w)
        {
            break;
        }
        stbtt_GetGlyphBox(&font, glyph_index, &x0, &y0, &x1, &y1);
        // stbtt_GetGlyphBitmapBoxSubpixel(&font, glyph_index, scale, scale, 0, 0, &x0, &y0, &x1, &y1);
        stbtt_GetGlyphBitmapBox(&font, glyph_index, scale, scale, &x0, &y0, &x1, &y1);
        stbtt_vertex *stbVertex = NULL;

        int verCount = 0;
        verCount = stbtt_GetGlyphShape(&font, glyph_index, &stbVertex);

//start fill svg
        NSVGimage *font_svg = gui_malloc(sizeof(NSVGimage));
        if (font_svg == NULL)
        {
            GUI_ASSERT(NULL != NULL);
        }
        memset(font_svg, 0, sizeof(NSVGimage));
        font_svg->height = baseline - y0;
        font_svg->width = x1;

        NSVGshape *font_shape = gui_malloc(sizeof(NSVGshape));
        if (font_shape == NULL)
        {
            GUI_ASSERT(NULL != NULL);
        }
        memset(font_shape, 0, sizeof(NSVGshape));
        font_shape->opacity = 1;
        font_shape->fill.type = 1;
        font_shape->fill.d.color = text->color;
        font_shape->flags = 1;
        // font_shape->bounds[0] = 0;
        // font_shape->bounds[1] = 0;
        // font_shape->bounds[2] = 20;
        // font_shape->bounds[3] = 20;
        font_svg->shapes = font_shape;

        int path_count = 1, path_npts = 1;
        for (int i = 0; i < verCount; i++)
        {
            switch (stbVertex[i].type)
            {
            case 1:
                path_count++;
                break;
            case 2:
            case 3:
                path_npts += 3;
                break;
            default:
                break;
            }
        }

        int *path_num = gui_malloc(path_count * sizeof(int));
        int cur_path = 0;
        for (int i = 0; i < verCount; i++)
        {
            if (stbVertex[i].type == 1)
            {
                path_num[cur_path++] = i;
            }
        }
        path_num[cur_path] = verCount - 1;

        int  pts_count = 0;
        float curt_x, curt_y, px, py, cx, cy;
        for (int i = 0; i < cur_path; i++)
        {
            for (int j = path_num[i]; j <= path_num[i + 1]; j++)
            {
                px = (float)stbVertex[j].x * scale;
                py = baseline - ((float)stbVertex[j].y * scale);
                switch (stbVertex[j].type)
                {
                case 1:
                    stb_add_path(font_shape, &stbVertex[path_num[i]], path_num[i + 1] - path_num[i]);
                    pts_count = 0;
                    font_shape->paths->pts[pts_count++] = px;
                    font_shape->paths->pts[pts_count++] = py;
                    break;
                case 2:
                    font_shape->paths->pts[pts_count++] = px;
                    font_shape->paths->pts[pts_count++] = py;
                    font_shape->paths->pts[pts_count++] = px;
                    font_shape->paths->pts[pts_count++] = py;
                    font_shape->paths->pts[pts_count++] = px;
                    font_shape->paths->pts[pts_count++] = py;
                    break;
                case 3:
                    cx = (float)stbVertex[j].cx * scale;
                    cy = baseline - ((float)stbVertex[j].cy * scale);
                    font_shape->paths->pts[pts_count++] = cx;
                    font_shape->paths->pts[pts_count++] = cy;
                    font_shape->paths->pts[pts_count++] = cx;
                    font_shape->paths->pts[pts_count++] = cy;
                    font_shape->paths->pts[pts_count++] = px;
                    font_shape->paths->pts[pts_count++] = py;
                    break;
                default:
                    break;
                }
            }
        }
        gui_free(path_num);

//svg fill ok
        gui_get_acc()->draw_svg(font_svg, 1, dc, text->base.x + xpos, text->base.y, 1, 0, 0, 0);
        extern void nsvgDelete(NSVGimage * image);
        nsvgDelete(font_svg);
        stbtt_FreeShape(&font, stbVertex);

        xpos += (advance * scale);
        ++ch;
    }
#else
    uint8_t *screen = gui_malloc((width) * (height) * sizeof(uint8_t));
    if (screen == NULL)
    {
        GUI_ASSERT(NULL != NULL);
    }
    memset(screen, 0, (width) * (height)*sizeof(uint8_t));
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
        stbtt_MakeCodepointBitmapSubpixel(&font, &screen[((int)baseline + y0)*width + (int) xpos + x0],
                                          x1 - x0, y1 - y0, width, scale, scale, x_shift, 0, p_buf[ch]);
        xpos += (advance * scale);
        if (ch < unicode_len - 1)
        {
            xpos += scale * stbtt_GetCodepointKernAdvance(&font, p_buf[ch], p_buf[ch + 1]);
        }
        ++ch;
    }
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
                    writebuf[write_off + j] = blend_b_and_f_with_a(color_back, text->color, screen[i * width + j]);
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
                    writebuf[write_off + j] = rgba2565(blend_b_and_f_with_a(rgb5652rgba(color_back), text->color,
                                                                            screen[i * width + j]));
                }
            }
        }
    }
    gui_free(screen);
#endif
    gui_free(p_buf);
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
