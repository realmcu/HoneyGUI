/*
 * File      : gui_canvas.h
 */
#ifndef __GUI_CANVAS_H__
#define __GUI_CANVAS_H__




#ifdef __cplusplus
extern "C" {
#endif
#include <guidef.h>
#include "gui_graphic.h"
#include "gui_bitmap.h"
#include "acc_engine.h"
typedef struct gui_canvas gui_canvas_t;
struct gui_canvas
{
    gui_bitmap_t base;
    void (* set_color)(void *this, uint32_t color);
    uint32_t (* get_color)(gui_canvas_t *this);
    void (*ctor)(gui_canvas_t *this, gui_obj_t *parent, const char *name, int16_t x, int16_t y,
                 int16_t w, int16_t h, uint32_t color);
    gui_animate_t *animate;
    void (*draw)(gui_canvas_t *this);
    float sx;
    float sy;
    float tx;
    float ty; float cx, cy, sw, r, sd, ed, sc; uint32_t s;
    uint8_t opacity_value;

};
typedef struct gui_api_canvas
{
    void (*circle)(gui_canvas_t *this, canvas_circle_t *circle_data);
    void (*rectangle)(gui_canvas_t *this, canvas_rectangle_t *rectangle_data);
    void (*arc)(gui_canvas_t *this, const canvas_arc_t *arc_data);
    void (*line)(gui_canvas_t *this, canvas_line_t *line_data);
    void (*scale)(gui_canvas_t *this, float sx, float sy);
    void (*translate)(gui_canvas_t *this, float tx, float ty);
    void (*svg)(gui_canvas_t *this, void *svg, uint32_t data_length, int x, int y, float scale);
    void (*set_animate)(gui_canvas_t *canvas, uint32_t dur, int repeatCount, void *callback, void *p);
    void (*wave)(gui_canvas_t *this, canvas_wave_t *wave);
    void (*palette_wheel)(gui_canvas_t *this, canvas_palette_wheel_t *pw);
    void (*bezier_curve)(gui_canvas_t *this, const canvas_bezier_curve_t *pw);
} gui_api_canvas_t;

void (gui_canvas_ctor)(gui_canvas_t *this, gui_obj_t *parent, const char *name, int16_t x,
                       int16_t y,
                       int16_t w, int16_t h, uint32_t color);
gui_canvas_t *gui_canvas_create(void *parent, const char *name, int16_t x, int16_t y, int16_t w,
                                int16_t h, uint32_t color);
#define RGBA(R,G,B,A) ((uint32_t)((((uint32_t)R)<<24)+(((uint32_t)G)<<16)+(((uint32_t)B)<<8)+((uint32_t)A)))
typedef struct canvas_color
{
    union
    {
        unsigned long rgba;
        struct
        {
            unsigned char alpha;
            unsigned char blue;
            unsigned char green;
            unsigned char red;
        } channel;
    } color;
} canvas_color_t;
typedef struct canvas_color_msb
{
    union
    {
        uint32_t rgba;
        struct
        {
            unsigned char red; unsigned char green; unsigned char blue; unsigned char alpha;
        } channel;
    } color;
} canvas_color_msb_t;
gui_inline uint16_t rgba2565(uint32_t rgba)
{
    canvas_color_t color_blend = {.color.rgba = rgba};
    uint16_t red = color_blend.color.channel.red * 0x1f / 0xff << 11;
    uint16_t green = color_blend.color.channel.green * 0x3f / 0xff << 5;
    uint16_t blue = color_blend.color.channel.blue * 0x1f / 0xff;
    return red + green + blue;
}
gui_inline uint32_t rgb5652rgba(uint16_t rgb565)
{
    canvas_color_t color_blend ;
    color_blend.color.channel.red = (int)((rgb565 & 0xf800) >> 11) * 0xff / 0x1f;
    color_blend.color.channel.green = (int)((rgb565 & 0x07e0) >> 5) * 0xff / 0x3f;
    color_blend.color.channel.blue = (int)(rgb565 & 0x001F) * 0xff / 0x1f;
    color_blend.color.channel.alpha = 0xff;

    return color_blend.color.rgba;
}
gui_inline uint32_t rgb565_with_alpha2rgba(uint16_t rgb565, uint8_t alpha)
{
    canvas_color_t color_blend ;
    color_blend.color.channel.red = (int)((rgb565 & 0xf800) >> 11) * 0xff / 0x1f;
    color_blend.color.channel.green = (int)((rgb565 & 0x07e0) >> 5) * 0xff / 0x3f;
    color_blend.color.channel.blue = (int)(rgb565 & 0x001F) * 0xff / 0x1f;
    color_blend.color.channel.alpha = alpha;
    return color_blend.color.rgba;
}
gui_inline uint32_t rgba_set_alpha(uint32_t rgba, uint8_t alpha)
{
    canvas_color_t color_blend = {.color.rgba = rgba};
    color_blend.color.channel.alpha = alpha;
    return color_blend.color.rgba;
}
gui_inline uint32_t rgba_mask_alpha(uint32_t rgba, uint8_t alpha)
{
    canvas_color_t color_blend = {.color.rgba = rgba};
    color_blend.color.channel.alpha = color_blend.color.channel.alpha * alpha / UINT8_MAX;
    return color_blend.color.rgba;
}
#define _b_and_f(color_b,color_f,c) (color_b.color.channel.c * color_b.color.channel.alpha + color_f.color.channel.c * color_f.color.channel.alpha) / 0xff;
gui_inline uint32_t blend_b_and_f(uint32_t b, uint32_t f)
{
    canvas_color_t color_b = {.color.rgba = b};
    canvas_color_t color_f = {.color.rgba = f};
    canvas_color_t color_r;
    color_b.color.channel.alpha = 0xff - color_f.color.channel.alpha;
    color_r.color.channel.alpha = color_f.color.channel.alpha;
    color_r.color.channel.blue = _b_and_f(color_b, color_f, blue)
                                 color_r.color.channel.green = _b_and_f(color_b, color_f, green)
                                                               color_r.color.channel.red = _b_and_f(color_b, color_f, red)
                                                                       return color_r.color.rgba;
}
gui_inline void blend_rgba_lsb_to_screen(uint16_t *target_pixel, uint32_t rgba)
{
    uint16_t color_back_565 = *target_pixel;
    *target_pixel = rgba2565(blend_b_and_f(rgb5652rgba(color_back_565), rgba));
}
gui_inline void blend_rgba_to_screen(uint16_t *target_pixel, uint32_t rgba)
{
    uint16_t color_back_565 = *target_pixel;
    //
    canvas_color_t color_blend = {.color.rgba = rgba};
    //canvas_color_msb_t color_blend_msb = {.color.rgba = rgba};
    //canvas_color_t color_blend = {.color.channel.red = color_blend_msb.color.channel.blue,.color.channel.green = color_blend_msb.color.channel.green,.color.channel.blue = color_blend_msb.color.channel.red,.color.channel.alpha = color_blend_msb.color.channel.alpha, };
    if (color_blend.color.rgba)
    {
        *target_pixel = rgba2565(blend_b_and_f(rgb5652rgba(color_back_565), color_blend.color.rgba));
    }


    //printf("%p %p %x,%x,%x,%x ", rgba, color_blend.color.rgba, color_blend.color.channel.red, color_blend.color.channel.green, color_blend.color.channel.blue, color_blend.color.channel.alpha);
}
gui_inline void blend_rgba_to_rgba_screen(uint32_t *target_pixel, uint32_t rgba)
{
    uint32_t color_back_8888 = *target_pixel;
    canvas_color_t color_blend = {.color.rgba = rgba};
    //canvas_color_msb_t color_blend_msb = {.color.rgba = rgba};
    //canvas_color_t color_blend = {.color.channel.red = color_blend_msb.color.channel.blue,.color.channel.green = color_blend_msb.color.channel.green,.color.channel.blue = color_blend_msb.color.channel.red,.color.channel.alpha = color_blend_msb.color.channel.alpha, };
    *target_pixel = (blend_b_and_f((color_back_8888), color_blend.color.rgba));
    gui_log("%x ", *target_pixel);
    //printf("%p %p %x,%x,%x,%x ", rgba, color_blend.color.rgba, color_blend.color.channel.red, color_blend.color.channel.green, color_blend.color.channel.blue, color_blend.color.channel.alpha);
}
gui_inline void blend_rgba_to_rgb_screen(uint8_t *target_pixel, uint32_t rgba)
{
    canvas_color_t color_blend = {.color.rgba = rgba};
    canvas_color_t color_back_8888 = {.color.channel.blue = *(target_pixel),
                                      .color.channel.green = *(target_pixel + 1),
                                      .color.channel.red = *(target_pixel + 2),
                                      .color.channel.alpha = UINT8_MAX
                                     };
    //canvas_color_msb_t color_blend_msb = {.color.rgba = rgba};
    //canvas_color_t color_blend = {.color.channel.red = color_blend_msb.color.channel.blue,.color.channel.green = color_blend_msb.color.channel.green,.color.channel.blue = color_blend_msb.color.channel.red,.color.channel.alpha = color_blend_msb.color.channel.alpha, };
    color_blend.color.rgba = (blend_b_and_f((color_back_8888.color.rgba),
                                            color_blend.color.rgba));//gui_log("%x ",*target_pixel);
    *target_pixel = color_blend.color.channel.blue;
    *(target_pixel + 1) = color_blend.color.channel.green;
    *(target_pixel + 2) = color_blend.color.channel.red;
    //printf("%p %p %x,%x,%x,%x ", rgba, color_blend.color.rgba, color_blend.color.channel.red, color_blend.color.channel.green, color_blend.color.channel.blue, color_blend.color.channel.alpha);
}
gui_inline void blend_rgba_with_alpha_to_screen(uint16_t *target_pixel, uint32_t rgba,
                                                uint8_t alpha)
{
    uint16_t color_back_565 = *target_pixel;

    canvas_color_msb_t color_blend_msb = {.color.rgba = rgba};
    canvas_color_t color_blend = {.color.channel.red = color_blend_msb.color.channel.blue, .color.channel.green = color_blend_msb.color.channel.green, .color.channel.blue = color_blend_msb.color.channel.red, .color.channel.alpha = color_blend_msb.color.channel.alpha * alpha / 255, };
    *target_pixel = rgba2565(blend_b_and_f(rgb5652rgba(color_back_565), color_blend.color.rgba));
    //printf("%p %p %x,%x,%x,%x ", rgba, color_blend.color.rgba, color_blend.color.channel.red, color_blend.color.channel.green, color_blend.color.channel.blue, color_blend.color.channel.alpha);
}
gui_inline void blend_565withalpha_to_screen(uint16_t *target_pixel, uint16_t rgb565, uint8_t alpha)
{
    blend_rgba_lsb_to_screen(target_pixel, rgb565_with_alpha2rgba(rgb565, alpha));
}
gui_inline unsigned char get_alpha_from_rgba(uint32_t rgba)
{
    canvas_color_t c = {.color.rgba = rgba};
    return c.color.channel.alpha;
}
gui_inline void blend_565withalpha_to_rgb_screen(uint8_t *target_pixel, uint16_t rgb565,
                                                 uint8_t alpha)
{
    blend_rgba_to_rgb_screen(target_pixel, rgb565_with_alpha2rgba(rgb565, alpha));
}
bool judge_point_in_circle(gui_circleshape_t *circle, gui_point_t *point);
void get_line_from_point_and_degree(gui_point_t *point1, float degree, gui_line_t *line);
void get_piont_line_cross_circle(int cx, int cy, int r, gui_line_t *l, gui_point_t *result,
                                 bool if_left);
extern gui_api_canvas_t gui_canvas_api;
#ifdef __cplusplus
}
#endif

#endif
