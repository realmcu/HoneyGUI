#include "gui_config.h"
#include <draw_font.h>
#define STB_TRUETYPE_IMPLEMENTATION
// #define ALLOW_UNALIGNED_TRUETYPE
#include "stb_truetype.h"
#include "acc_engine.h"

static stbtt_fontinfo font;

static void rtgui_font_stb_load(gui_text_t *text)
{
    if (text->path)
    {
        gui_font_stb_init(text->path);
    }
    uint16_t *p_buf = gui_malloc(text->len * sizeof(uint16_t));
    if (p_buf == NULL)
    {
        GUI_ASSERT(NULL != NULL);
    }
    uint16_t unicode_len = utf8_to_unicode(text->utf_8, text->len, p_buf, text->len);
    text->font_len = unicode_len;
    float all_char_w = 0;
    float scale = 0;
    int ch = 0, advance = 0, lsb = 0;
    int line_flag;
    scale = stbtt_ScaleForPixelHeight(&font, text->font_height);
    while (ch < unicode_len)
    {
        stbtt_GetCodepointHMetrics(&font, p_buf[ch++], &advance, &lsb);
        all_char_w += advance * scale;
    }
    if (text->text_offset == 0)
    {
        switch (text->mode)
        {
        case LEFT:
        case CENTER:
        case RIGHT:
            text->text_offset = _UI_MAX((text->base.w - all_char_w) / 2, 0);
            break;
        case MUTI_LEFT:
        case MUTI_CENTER:
        case MUTI_RIGHT:
            text->text_offset = all_char_w / text->base.w + 1 + line_flag;
            break;
        case SCROLL_X:
            text->text_offset = (int)(all_char_w + 1);
            break;
        case SCROLL_Y:
            text->text_offset = (all_char_w / text->base.w + 1) * text->font_height;
        default:
            break;
        }
    }
    gui_free(p_buf);
}

static void rtgui_font_stb_unload(gui_text_t *text)
{

}
#ifndef RTK_GUI_TTF_SVG
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
static bool creat_stb_screen(gui_text_t *text, struct rtgui_rect *rect, FONT_STB_SCREEN *screen)
{
    if (text->mode == LEFT || text->mode == CENTER || text->mode == RIGHT)
    {
        screen->width = _UI_MIN(rect->x2 - rect->x1, _UI_MAX(text->base.w - text->text_offset * 2, 0));
        screen->height = rect->y2 - rect->y1;
    }
    else if (text->mode == MUTI_LEFT || text->mode == MUTI_CENTER || text->mode == MUTI_RIGHT)
    {
        screen->width =  rect->x2 - rect->x1;
        screen->height = rect->y2 - rect->y1;
    }
    else if (text->mode == SCROLL_X)
    {
        screen->width =  rect->x2 - rect->x1;
        screen->height = rect->y2 - rect->y1;
    }
    else if (text->mode == SCROLL_Y)
    {
        screen->width =  rect->x2 - rect->x1;
        screen->height = rect->y2 - rect->y1;
    }
    if (screen->width == 0 || screen->height == 0)
    {
        return false;
    }
    GUI_ASSERT(screen->width > 0);
    GUI_ASSERT(screen->height > 0);
    uint8_t *buffer = gui_malloc((screen->width) * (screen->height) * sizeof(uint8_t));
    if (buffer == NULL)
    {
        GUI_ASSERT(NULL != NULL);
    }
    memset(buffer, 0, (screen->width) * (screen->height)*sizeof(uint8_t));
    screen->buf = buffer;
    return true;
}
static void font_stb_draw_bitmap(gui_text_t *text, FONT_STB_SCREEN *stb_screen,
                                 struct rtgui_rect *rect)
{
    gui_dispdev_t *dc = gui_get_dc();
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    int offset;
    switch (text->mode)
    {
    case LEFT:
    case CENTER:
    case RIGHT:
        offset = text->text_offset * text->mode;
        break;
    case MUTI_LEFT:
    case MUTI_CENTER:
    case MUTI_RIGHT:
        break;
    case SCROLL_X:
        break;
    case SCROLL_Y:
    default:
        break;
    }
    if (dc_bytes_per_pixel == 4)
    {
        uint32_t *writebuf = (uint32_t *)dc->frame_buf + (rect->y1) * dc->fb_width + rect->x1 + offset;
        uint32_t color_back;
        for (int32_t i  = _UI_MAX(text->base.dy - rect->y1, 0); i < stb_screen->height &&
             (i + rect->y1 <= text->base.dy + text->base.h); i++)
        {
            int write_off = (i + dc->section.y1) * dc->fb_width;
            for (int32_t j = _UI_MAX(text->base.dx - rect->x1, 0); j < stb_screen->width &&
                 (j + rect->x1 <= text->base.dx + text->base.w); j++)
            {
                if (stb_screen->buf[i * stb_screen->width + j] != 0)
                {
                    color_back = writebuf[write_off + j];
                    writebuf[write_off + j] = blend_b_and_f_with_a(color_back, text->color,
                                                                   stb_screen->buf[i * stb_screen->width + j]);
                }
            }
        }
    }
    else if (dc_bytes_per_pixel == 2)
    {
        uint16_t *writebuf = (uint16_t *)dc->frame_buf + (rect->y1) * dc->fb_width + rect->x1 + offset;
        uint16_t color_back;
        for (int32_t i  = _UI_MAX(text->base.dy - rect->y1, 0); i < stb_screen->height &&
             (i + rect->y1 <= text->base.dy + text->base.h); i++)
        {
            int write_off = (i + dc->section.y1) * dc->fb_width;
            for (int32_t j = _UI_MAX(text->base.dx - rect->x1, 0); j < stb_screen->width &&
                 (j + rect->x1 <= text->base.dx + text->base.w); j++)
            {
                if (stb_screen->buf[i * stb_screen->width + j] != 0)
                {
                    color_back = writebuf[write_off + j];
                    // writebuf[write_off + j] = rgba2565(blend_b_and_f(rgb5652rgba(color_back_565), rgb5652rgba(text->color),screen[i][j]));
                    writebuf[write_off + j] = rgba2565(blend_b_and_f_with_a(rgb5652rgba(color_back), text->color,
                                                                            stb_screen->buf[i * stb_screen->width + j]));
                }
            }
        }
    }
}
#endif // !RTK_GUI_TTF_SVG
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
    memset(path->pts, 0, path->npts * 2 * sizeof(float));
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
    int ascent = 0, descent = 0, lineGap = 0, ch = 0, line_num = 0;
    float scale = 0, xpos = 0, ypos = 0, baseline = 0;
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
        // gui_log("unicode : %c\n", p_buf[ch]);
        if (text->mode == LEFT || text->mode == CENTER || text->mode == RIGHT)
        {
            if (xpos + advance * scale > text->base.w)
            {
                break;
            }
        }
        else if (text->mode == MUTI_LEFT || text->mode == MUTI_CENTER || text->mode == MUTI_RIGHT)
        {
            if (xpos + advance * scale > text->base.w)
            {
                line_num ++;
                xpos = 0;
            }
            if ((line_num + 1) *  scale * (ascent - descent + lineGap) > text->base.h)
            {
                break;
            }
        }
        else if (text->mode == SCROLL_X)
        {
            if (rect->x1 + xpos + advance * scale >= text->base.dx + text->base.w)
            {
                break;
            }
            if (rect->x1 + xpos < text->base.dx)
            {
                xpos += (advance * scale);
                ch++;
                continue;
            }
        }
        else if (text->mode == SCROLL_Y)
        {
            if (xpos + advance * scale > text->base.w)
            {
                line_num ++;
                xpos = 0;
            }
        }
        ypos = line_num * scale * (ascent - descent + lineGap);
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
        //font_shape->fill.d.color = text->color;
        gui_color_t color_r = {0};
        color_r.rgba = text->color;
        gui_color_msb_t color_f = {0};
        color_f.channel.alpha = color_r.channel.alpha;
        color_f.channel.red = color_r.channel.blue;
        color_f.channel.blue = color_r.channel.red;
        color_f.channel.green = color_r.channel.green;

        font_shape->fill.d.color = color_f.rgba;
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
        float px, py, cx, cy;
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
        // gui_log("fill ok stb \n");
        int offset = 0;
        switch (text->mode)
        {
        case LEFT:
        case CENTER:
        case RIGHT:
            offset = text->text_offset * text->mode;
            break;
        case MUTI_LEFT:
        case MUTI_CENTER:
        case MUTI_RIGHT:
            break;
        case SCROLL_X:
            break;
        case SCROLL_Y:
        default:
            break;
        }
        // gui_log("draw %c , ch : %d , xpos : %f , y pos : %f \n",p_buf[ch],ch,xpos,ypos);
        gui_get_acc()->draw_svg(font_svg, 1, dc, rect->x1 + xpos + offset, rect->y1 + ypos, 1, 0, 0, 0);
        extern void nsvgDelete(NSVGimage * image);
        nsvgDelete(font_svg);
        stbtt_FreeShape(&font, stbVertex);

        xpos += (advance * scale);
        ++ch;
    }
#else
    FONT_STB_SCREEN stb_screen;
    bool flag = creat_stb_screen(text, rect, &stb_screen);
    if (flag == false)
    {
        return;
    }
    while (ch < unicode_len)
    {
        int glyph_index = stbtt_FindGlyphIndex(&font, p_buf[ch]);
        int advance, lsb, x0, y0, x1, y1;
        float x_shift = xpos - (float) floor(xpos);
        stbtt_GetGlyphHMetrics(&font, glyph_index, &advance, &lsb);
        stbtt_GetGlyphBitmapBoxSubpixel(&font, glyph_index, scale, scale, x_shift, 0, &x0, &y0, &x1, &y1);
        if (text->mode == LEFT || text->mode == CENTER || text->mode == RIGHT)
        {
            if (xpos + advance * scale > stb_screen.width)
            {
                break;
            }
        }
        else if (text->mode == MUTI_LEFT || text->mode == MUTI_CENTER || text->mode == MUTI_RIGHT)
        {
            if (xpos + advance * scale > stb_screen.width)
            {
                line_num ++;
                xpos = 0;
            }
            if ((line_num + 1) *  scale * (ascent - descent + lineGap) > stb_screen.height)
            {
                break;
            }
        }
        else if (text->mode == SCROLL_X)
        {
            if (rect->x1 + xpos > text->base.dx + text->base.w)
            {
                break;
            }
            if (rect->x1 + xpos + advance * scale < text->base.dx)
            {
                xpos += (advance * scale);
                ch++;
                continue;
            }
        }
        else if (text->mode == SCROLL_Y)
        {
            if (xpos + advance * scale > stb_screen.width)
            {
                line_num ++;
                xpos = 0;
            }
        }
        // gui_log("unicode %x \n",p_buf[ch]);
        ypos = line_num * scale * (ascent - descent + lineGap);
        stbtt_MakeCodepointBitmapSubpixel(&font,
                                          &stb_screen.buf[((int)baseline + y0 + (int)ypos)*stb_screen.width + (int) xpos + x0],
                                          x1 - x0, y1 - y0, stb_screen.width, scale, scale, x_shift, 0, p_buf[ch]);
        // gui_log("draw %c , ch : %d , xpos : %f , y pos : %f \n",p_buf[ch],ch,xpos,ypos);
        ++ch;
        xpos += (advance * scale);
    }
    font_stb_draw_bitmap(text, &stb_screen, rect);
    gui_free(stb_screen.buf);
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
