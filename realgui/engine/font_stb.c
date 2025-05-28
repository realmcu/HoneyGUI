/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file font_stb.c
  * @brief stb truetype engine
  * @details stb truetype engine
  * @author luke_sun@realsil.com.cn
  * @date 2024/11/05
  * @version v1.1
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "draw_font.h"
#include "font_stb.h"

/*stb truetype function macro*/

/*enable stb truetype*/
#define STB_TRUETYPE_IMPLEMENTATION

/*reduce memory*/
// #define STB_REDUCE_MEMORY
#define STB_REDUCE_MEMORY_FIXD

/*unaligned*/
// #define ALLOW_UNALIGNED_TRUETYPE

/*MVE*/
#if  __ARM_FEATURE_MVE
// #define STB_TRUETYPE_USE_MVE
#define FONT_STB_USE_MVE
#include "arm_mve.h"
#endif

/*stb true type source*/
#include "stb_truetype.h"


/*============================================================================*
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                           Constants
 *============================================================================*/


/*============================================================================*
 *                            Macros
 *============================================================================*/

//Change rawing scheme, default pure stb_truetype
// #define RTK_GUI_FONT_ENABLE_TTF_NANOVG

#ifdef RTK_GUI_FONT_ENABLE_TTF_NANOVG
#include "nanosvg.h"
#include "nanovg.h"
extern void nvgDeleteAGGE(NVGcontext *ctx);
#endif


/*============================================================================*
 *                            Variables
 *============================================================================*/

static stbtt_fontinfo font;


/*============================================================================*
 *                           Private Functions
 *============================================================================*/

#ifndef RTK_GUI_FONT_ENABLE_TTF_NANOVG
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

static bool creat_stb_screen(gui_text_t *text, gui_text_rect_t *rect, FONT_STB_SCREEN *screen)
{
    if (text->mode == LEFT || text->mode == CENTER || text->mode == RIGHT)
    {
        screen->width = _UI_MIN(rect->x2 - rect->x1 + 1, _UI_MAX(text->char_width_sum, 0));
        screen->height = _UI_MIN(text->font_height, rect->y2 - rect->y1 + 1);
    }
    else if (text->mode == MULTI_LEFT || text->mode == MULTI_CENTER || text->mode == MULTI_RIGHT)
    {
        screen->width = (rect->x2 - rect->x1 + 1) * text->base.matrix->m[0][0];
        screen->height = rect->y2 - rect->y1 + 1;
    }
    else if (text->mode == SCROLL_X)
    {
        screen->width =  rect->x2 - rect->x1 + 1;
        screen->height = rect->y2 - rect->y1 + 1;
    }
    else if (text->mode == SCROLL_Y)
    {
        screen->width =  rect->x2 - rect->x1 + 1;
        screen->height = rect->y2 - rect->y1 + 1;
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
        return false;
    }
    memset(buffer, 0, (screen->width) * (screen->height) * sizeof(uint8_t));
    // gui_log("creat_stb_screen width %d , height %d \n", screen->width, screen->height);
    screen->buf = buffer;
    return true;
}
static void font_stb_draw_bitmap(gui_text_t *text, FONT_STB_SCREEN *stb_screen,
                                 gui_text_rect_t *rect)
{
    gui_dispdev_t *dc = gui_get_dc();
    uint8_t *dots = stb_screen->buf;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    int offset = 0;
    switch (text->mode)
    {
    case LEFT:
    case CENTER:
    case RIGHT:
        offset = _UI_MAX((text->base.w - text->char_width_sum) / 2, 0) * text->mode;
        break;
    case MULTI_LEFT:
    case MULTI_CENTER:
    case MULTI_RIGHT:
        break;
    case SCROLL_X:
        break;
    case SCROLL_Y:
    default:
        break;
    }

    int font_x = rect->x1 + offset;
    int font_y = rect->y1;
    int font_w = stb_screen->width;
    int font_h = stb_screen->height;
    int x_start = _UI_MAX(_UI_MAX(font_x, rect->xboundleft), dc->section.x1);
    int x_end;
    if (rect->xboundright != 0)
    {
        x_end = _UI_MIN(_UI_MIN(font_x + font_w, dc->section.x2 + 1), rect->xboundright);
    }
    else
    {
        x_end = _UI_MIN(font_x + font_w, dc->section.x2 + 1);
    }
    int y_start = _UI_MAX(dc->section.y1, _UI_MAX(font_y, rect->yboundtop));
    int y_end;
    if (rect->yboundbottom != 0)
    {
        y_end = _UI_MIN(_UI_MIN(dc->section.y2 + 1, font_y + font_h), rect->yboundbottom);
    }
    else
    {
        y_end = _UI_MIN(dc->section.y2 + 1, font_y + font_h);
    }
    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }

    if (dc_bytes_per_pixel == 4)
    {
        uint32_t *writebuf = (uint32_t *)dc->frame_buf;
        uint32_t color_back;
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) ;
            for (uint32_t j = x_start; j < x_end; j++)
            {
                uint8_t alpha = dots[(i - font_y) * font_w + (j - font_x)];
                if (alpha != 0)
                {
                    color_back = writebuf[write_off + j - dc->section.x1];
                    alpha = text->color.color.rgba.a * alpha / 0xff;
                    writebuf[write_off + j - dc->section.x1] = alphaBlendRGBA(text->color, color_back, alpha);
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
            for (uint32_t j = x_start; j < x_end; j++)
            {
                uint8_t alpha = dots[(i - font_y) * font_w + (j - font_x)];
                if (alpha != 0)
                {
                    alpha = text->color.color.rgba.a * alpha / 0xff;
                    color_back[0] = writebuf[write_off * 3 + j * 3 + 2 - dc->section.x1];
                    color_back[1] = writebuf[write_off * 3 + j * 3 + 1 - dc->section.x1];
                    color_back[2] = writebuf[write_off * 3 + j * 3 + 0 - dc->section.x1];
                    writebuf[write_off * 3 + j * 3 + 0 - dc->section.x1] = (text->color.color.rgba.b * alpha +
                                                                            color_back[2] *
                                                                            (0xff - alpha)) / 0xff;
                    writebuf[write_off * 3 + j * 3 + 1 - dc->section.x1] = (text->color.color.rgba.g * alpha +
                                                                            color_back[1] *
                                                                            (0xff - alpha)) / 0xff;
                    writebuf[write_off * 3 + j * 3 + 2 - dc->section.x1] = (text->color.color.rgba.r * alpha +
                                                                            color_back[0] *
                                                                            (0xff - alpha)) / 0xff;
                }
            }
        }
    }
    else if (dc_bytes_per_pixel == 2)
    {
#ifdef FONT_STB_USE_MVE
        uint16_t *writebuf = (uint16_t *)dc->frame_buf;
        uint16_t color_output = rgba2565(text->color);
        uint16_t color_back;
        uint32_t section_width = dc->section.x2 - dc->section.x1 + 1;
        uint32_t loopCount = (x_end - x_start) / 8;
        uint32_t loopsLeft = (x_end - x_start) % 8;
        bool max_opacity = false;
        if (text->color.color.rgba.a == 0xff)
        {
            max_opacity = true;
        }
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * section_width;
            int dots_off = (i - font_y) * font_w - font_x;

            /*helium code start*/
            for (uint32_t loopc = 0; loopc < loopCount; loopc ++)
            {
                uint16_t js = x_start + 8 * loopc;
                uint16x8_t jv = vidupq_n_u16(js, 1);

                uint16_t alpha[8];
                for (uint32_t i = 0; i < 8; i++)
                {
                    alpha[i] = dots[dots_off + js + i];
                }
                uint16x8_t alphav = vld1q(&alpha[0]);
                if (vaddvq_u16(alphav) == 0)
                {
                    continue;
                }
                if (!max_opacity)
                {
                    alphav = vmulq_n_u16(alphav, (uint16_t)text->color.color.rgba.a);
                    alphav = vrshrq_n_u16(alphav, 8);
                }

                uint16x8_t outrv = vmulq_n_u16(alphav, (uint16_t)text->color.color.rgba.r);
                uint16x8_t outgv = vmulq_n_u16(alphav, (uint16_t)text->color.color.rgba.g);
                uint16x8_t outbv = vmulq_n_u16(alphav, (uint16_t)text->color.color.rgba.b);

                //read back color
                uint16x8_t color_backv = vld1q(&writebuf[write_off + js - dc->section.x1]);

                uint16x8_t color_backr = vbicq_n_u16(color_backv, 0x0700);
                color_backr = vshrq_n_u16(color_backr, 8);
                uint16x8_t color_backg = vbicq_n_u16(color_backv, 0xF800);
                color_backg = vbicq_n_u16(color_backg, 0x001F);
                color_backg = vshrq_n_u16(color_backg, 3);
                uint16x8_t color_backb = vbicq_n_u16(color_backv, 0xFF00);
                color_backb = vbicq_n_u16(color_backb, 0x00E0);
                color_backb = vshlq_n_u16(color_backb, 3);

                uint16x8_t alphabv = vdupq_n_u16(0xff);
                alphabv = vsubq_u16(alphabv, alphav);

                color_backr = vmulq_u16(color_backr, alphabv);
                color_backg = vmulq_u16(color_backg, alphabv);
                color_backb = vmulq_u16(color_backb, alphabv);

                outrv = vaddq_u16(outrv, color_backr);
                outgv = vaddq_u16(outgv, color_backg);
                outbv = vaddq_u16(outbv, color_backb);

                /*vsriq, 6 instructions*/
                // uint16x8_t resultv = vsriq_n_u16(outgv, outbv, 6);
                // resultv = vsriq_n_u16(outrv, resultv, 5);

                /*work around, 10 instructions*/
                uint16x8_t resultv = vshrq_n_u16(outbv, 11);
                outrv = vandq_u16(outrv, vdupq_n_u16(0xF800));
                outgv = vshlq_n_u16(vshrq_n_u16(outgv, 10), 5);
                resultv = vorrq_u16(resultv, outrv);
                resultv = vorrq_u16(resultv, outgv);

                vst1q_u16(&writebuf[write_off + js - dc->section.x1], resultv);
            }
            /*helium code end*/
            for (uint32_t j = x_end - loopsLeft; j < x_end; j++)
            {
                uint8_t alpha = dots[dots_off + j];
                if (alpha != 0)
                {
                    alpha = text->color.color.rgba.a * alpha / 0xff;
                    color_back = writebuf[write_off + j - dc->section.x1];
                    writebuf[write_off + j - dc->section.x1] = alphaBlendRGB565(color_output, color_back, alpha);
                }
            }
        }
#else
        uint16_t *writebuf = (uint16_t *)dc->frame_buf;
        uint16_t color_output = rgba2565(text->color);
        uint16_t color_back;
        uint32_t section_width = dc->section.x2 - dc->section.x1 + 1;
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * section_width;
            int dots_off = (i - font_y) * font_w - font_x;
            for (uint32_t j = x_start; j < x_end; j++)
            {
                uint8_t alpha = dots[dots_off + j];
                if (alpha != 0)
                {
                    alpha = text->color.color.rgba.a * alpha / 0xff;
                    color_back = writebuf[write_off + j - dc->section.x1];
                    writebuf[write_off + j - dc->section.x1] = alphaBlendRGB565(color_output, color_back, alpha);
                }
            }
        }
#endif
    }
}
#endif
#ifdef RTK_GUI_FONT_ENABLE_TTF_NANOVG
static NVGcolor gui_svg_color(unsigned int c, float opacity)
{
    return nvgRGBA(c & 0xFF, (c >> 8) & 0xFF, (c >> 16) & 0xFF, opacity * 255);
}
static bool isHole(stbtt_vertex *vertices, int numVertices)
{
    int sum = 0;
    for (int i = 0; i < numVertices - 1; i++)
    {
        sum += (vertices[i + 1].x - vertices[i].x) * (vertices[i + 1].y + vertices[i].y);
    }
    return sum > 0;
}

static void stb_draw_vg(gui_text_t *text,
                        int x_offset, int y_offset,
                        stbtt_vertex *stbVertex, int verCount,
                        int ascent, float scale)
{
    gui_dispdev_t *dc;
    dc = gui_get_dc();
    GUI_UNUSED(dc);

    NVGcontext *vg;

    float baseline = ascent * scale;
    float alpha = 1.0;
    extern NVGcontext *nvgCreateAGGE(uint32_t w,
                                     uint32_t h,
                                     uint32_t stride,
                                     enum     NVGtexture format,
                                     uint8_t *data);

    vg = nvgCreateAGGE(dc->fb_width, dc->fb_height, dc->fb_width * (dc->bit_depth >> 3),
                       (dc->bit_depth >> 3) == 2 ? NVG_TEXTURE_BGR565 : NVG_TEXTURE_BGRA, dc->frame_buf);
    nvgBeginFrame(vg, dc->fb_width, dc->fb_height, 1);

    nvgScissor(vg, 0, 0, dc->fb_width, dc->fb_height);
    nvgResetTransform(vg);
    nvgTranslate(vg, 0, - (float)dc->fb_height * (float)dc->section_count);
    nvgTransform(vg, text->base.matrix->m[0][0], text->base.matrix->m[1][0], text->base.matrix->m[0][1],
                 text->base.matrix->m[1][1], text->base.matrix->m[0][2], text->base.matrix->m[1][2]);

    nvgTranslate(vg, x_offset, y_offset);

    nvgBeginPath(vg);
    bool pathHole = false;
    bool firstpath = true;

    float start_x, start_y;
    uint32_t last_path_index = 0;
    for (int i = 0; i < verCount; i++)
    {
        switch (stbVertex[i].type)
        {
        case STBTT_vmove:
            if (firstpath)
            {
                firstpath = false;
            }
            else
            {
                if (isHole(&stbVertex[last_path_index], i - last_path_index))
                {
                    nvgPathWinding(vg, NVG_HOLE);
                }
                last_path_index = i;
            }
            nvgMoveTo(vg, stbVertex[i].x * scale, baseline - stbVertex[i].y * scale);
            // gui_log("nvgMoveTo %d %d \n",stbVertex[i].x, stbVertex[i].y);
            break;
        case STBTT_vline:
            nvgLineTo(vg, stbVertex[i].x * scale, baseline - stbVertex[i].y * scale);
            // gui_log("nvgLineTo %d %d \n",stbVertex[i].x, stbVertex[i].y);
            break;
        case STBTT_vcurve:
            nvgBezierTo(vg, stbVertex[i].cx * scale,  baseline - stbVertex[i].cy * scale,
                        stbVertex[i].cx * scale,  baseline - stbVertex[i].cy * scale,
                        stbVertex[i].x * scale,   baseline - stbVertex[i].y * scale);
            // gui_log("nvgBezierTo %d %d \n",stbVertex[i].x, stbVertex[i].y);
            break;
        case STBTT_vcubic:
            break;
        default:
            break;
        }
    }
    if (isHole(&stbVertex[last_path_index], verCount - last_path_index))
    {
        nvgPathWinding(vg, NVG_HOLE);
    }
    nvgFillColor(vg, gui_svg_color(text->color.color.argb_full, alpha));
    nvgFill(vg);

    nvgEndFrame(vg);
    nvgDeleteAGGE(vg);
}
#endif

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void gui_font_stb_load(gui_text_t *text, gui_text_rect_t *rect)
{
    if (text->path)
    {
        gui_font_stb_init(text->path);
    }
    uint32_t *unicode_buf = NULL;
    uint16_t unicode_len = 0;
    unicode_len = process_content_by_charset(text->charset, text->content, text->len, &unicode_buf);

    text->font_len = unicode_len;
    float all_char_w = 0;
    float scale = 0;
    int ch = 0, advance = 0, lsb = 0;
    int line_flag = 0;
    scale = stbtt_ScaleForPixelHeight(&font, text->font_height * text->base.matrix->m[0][0]);
    while (ch < unicode_len)
    {
        stbtt_GetCodepointHMetrics(&font, unicode_buf[ch++], &advance, &lsb);
        all_char_w += advance * scale;
    }
    if (text->char_width_sum == 0)
    {
        switch (text->mode)
        {
        case LEFT:
        case CENTER:
        case RIGHT:
            text->char_width_sum = all_char_w + 1;
            text->char_line_sum = 1;
            break;
        case MULTI_LEFT:
        case MULTI_CENTER:
        case MULTI_RIGHT:
            text->char_width_sum = all_char_w;
            text->char_line_sum = all_char_w / text->base.w + 1 + line_flag;
            break;
        case SCROLL_X:
            text->char_width_sum = (int)(all_char_w + 1);
            text->char_line_sum = 1;
            break;
        case SCROLL_Y:
            text->char_width_sum = all_char_w;
            text->char_line_sum = all_char_w / text->base.w + 1;
        default:
            break;
        }
    }
    gui_free(unicode_buf);
}

void gui_font_stb_unload(gui_text_t *text)
{
    if (text->data)
    {
        gui_free(((FONT_STB_SCREEN *)text->data)->buf);
        gui_free((FONT_STB_SCREEN *)text->data);
        text->data = NULL;
    }
}

void gui_font_stb_draw(gui_text_t *text, gui_text_rect_t *rect)
{
    if (text->font_mode != FONT_SRC_MEMADDR)
    {
        return;
    }
    int ascent = 0, descent = 0, lineGap = 0, ch = 0, line_num = 0;
    float scale = 0, xpos = 0, ypos = 0, baseline = 0;
    scale = stbtt_ScaleForPixelHeight(&font, text->font_height * text->base.matrix->m[0][0]);
    stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);
    baseline = ascent * scale;

    gui_dispdev_t *dc = gui_get_dc();
    uint32_t *unicode_buf = NULL;
    uint16_t unicode_len = 0;
    unicode_len = process_content_by_charset(text->charset, text->content, text->len, &unicode_buf);

#ifdef RTK_GUI_FONT_ENABLE_TTF_NANOVG
    while (ch < unicode_len)
    {
        int glyph_index = stbtt_FindGlyphIndex(&font, unicode_buf[ch]);
        int advance, lsb, x0, y0, x1, y1;
        stbtt_GetGlyphHMetrics(&font, glyph_index, &advance, &lsb);
        // gui_log("unicode : %c\n", unicode_buf[ch]);
        if (text->mode == LEFT || text->mode == CENTER || text->mode == RIGHT)
        {
            if (xpos + advance * scale > text->base.w)
            {
                break;
            }
        }
        else if (text->mode == MULTI_LEFT || text->mode == MULTI_CENTER || text->mode == MULTI_RIGHT)
        {
            if ((xpos + advance * scale > text->base.w) || unicode_buf[ch] == 0x0A)
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
            xpos += (advance * scale);
            ch++;
            continue;
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

        int offset = 0;
        switch (text->mode)
        {
        case LEFT:
        case CENTER:
        case RIGHT:
            offset = _UI_MAX((text->base.w - text->char_width_sum) / 2, 0) * text->mode;
            break;
        case MULTI_LEFT:
        case MULTI_CENTER:
        case MULTI_RIGHT:
            break;
        case SCROLL_X:
            break;
        case SCROLL_Y:
        default:
            break;
        }
        // gui_log("draw %c , ch : %d , xpos : %f , y pos : %f \n",unicode_buf[ch],ch,xpos,ypos);
        stb_draw_vg(text, xpos + offset, ypos, stbVertex, verCount, ascent, scale);
        stbtt_FreeShape(&font, stbVertex);

        xpos += (advance * scale);
        ++ch;
    }
#else
    if (text->data == NULL)
    {
        FONT_STB_SCREEN *stb_screen = gui_malloc(sizeof(FONT_STB_SCREEN));
        bool flag = creat_stb_screen(text, rect, stb_screen);
        text->data = stb_screen;
        if (flag == false)
        {
            return;
        }
        while (ch < unicode_len)
        {
            int glyph_index = stbtt_FindGlyphIndex(&font, unicode_buf[ch]);
            int advance, lsb, x0, y0, x1, y1;
            float x_shift = xpos - (float) floor(xpos);
            stbtt_GetGlyphHMetrics(&font, glyph_index, &advance, &lsb);
            stbtt_GetGlyphBitmapBoxSubpixel(&font, glyph_index, scale, scale, x_shift, 0, &x0, &y0, &x1, &y1);
            if (text->mode == LEFT || text->mode == CENTER || text->mode == RIGHT)
            {
                if (xpos + advance * scale > stb_screen->width)
                {
                    break;
                }
            }
            else if (text->mode == MULTI_LEFT || text->mode == MULTI_CENTER || text->mode == MULTI_RIGHT)
            {
                if ((xpos + advance * scale > stb_screen->width) || unicode_buf[ch] == 0x0A)
                {
                    line_num ++;
                    xpos = 0;
                }
                if ((line_num + 1) *  scale * (ascent - descent + lineGap) > stb_screen->height)
                {
                    break;
                }
            }
            else if (text->mode == SCROLL_X)
            {
                xpos += (advance * scale);
                ch++;
                continue;
            }
            else if (text->mode == SCROLL_Y)
            {
                if (xpos + advance * scale > stb_screen->width)
                {
                    line_num ++;
                    xpos = 0;
                }
            }
            // gui_log("unicode %x \n",unicode_buf[ch]);
            ypos = line_num * scale * (ascent - descent + lineGap);
            /*If a non-standard ttf font is used, the header information of the stb_screen memory may be lost because the drawing is out of bounds, triggering a free problem.*/
            stbtt_MakeCodepointBitmapSubpixel(&font,
                                              &stb_screen->buf[((int)(baseline + 0.99999f) + y0 + (int)ypos)*stb_screen->width + (int) xpos + x0],
                                              x1 - x0, y1 - y0, stb_screen->width, scale, scale, x_shift, 0, unicode_buf[ch]);
            // gui_log("draw %c , ch : %d , xpos : %f , y pos : %f \n",unicode_buf[ch],ch,xpos,ypos);
            ++ch;
            xpos += (advance * scale);
        }
    }
    font_stb_draw_bitmap(text, text->data, rect);
#endif
    gui_free(unicode_buf);
}

void gui_font_stb_init(void *font_ttf_addr)
{
    stbtt_InitFont(&font, font_ttf_addr, 0);
}

