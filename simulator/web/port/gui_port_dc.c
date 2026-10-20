/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * @file gui_port_dc.c
 * @brief Display port for the browser (WebAssembly) simulator
 *
 * The engine renders in horizontal sections (partial frame buffer, same as
 * the desktop simulator). Each section is converted into a full-screen RGBA
 * buffer that JavaScript copies into a <canvas> with putImageData().
 */

#include "guidef.h"
#include "gui_api.h"
#include "gui_port.h"
#include "web_sim.h"
#include <stdlib.h>
#include <string.h>
#include <emscripten.h>

#ifndef DRV_LCD_CORNER_RADIUS
#define DRV_LCD_CORNER_RADIUS 0
#endif

#ifndef DRV_LCD_WIDTH
#define DRV_LCD_WIDTH   480
#endif

#ifndef DRV_LCD_HEIGHT
#define DRV_LCD_HEIGHT  480
#endif

#ifndef DRV_PIXEL_BITS
#define DRV_PIXEL_BITS  16
#endif

#define LCD_SECTION_HEIGHT 20

/* The DRV_* macros above are only the defaults. A prebuilt module is shared
 * by every project, so the page sets the real screen with web_sim_configure()
 * before main() runs (see web_sim.h). */
static int lcd_width = DRV_LCD_WIDTH;
static int lcd_height = DRV_LCD_HEIGHT;
static int lcd_corner_radius = DRV_LCD_CORNER_RADIUS;
static int lcd_pixel_bits = DRV_PIXEL_BITS;

/* Full screen, 4 bytes per pixel in canvas ImageData order (R, G, B, A). */
static uint8_t *screen_rgba;

static inline void put_rgba(uint8_t *dst, uint8_t r, uint8_t g, uint8_t b)
{
    dst[0] = r;
    dst[1] = g;
    dst[2] = b;
    dst[3] = 0xFF;
}

static void convert_row(uint8_t *dst, const uint8_t *src, uint32_t w)
{
    switch (lcd_pixel_bits)
    {
    case 32:
        /* ARGB8888, little endian: B G R A in memory */
        for (uint32_t i = 0; i < w; i++, src += 4, dst += 4)
        {
            put_rgba(dst, src[2], src[1], src[0]);
        }
        break;
    case 24:
        /* RGB888, little endian: B G R in memory */
        for (uint32_t i = 0; i < w; i++, src += 3, dst += 4)
        {
            put_rgba(dst, src[2], src[1], src[0]);
        }
        break;
    case 16:
        {
            const uint16_t *px = (const uint16_t *)src;
            for (uint32_t i = 0; i < w; i++, dst += 4)
            {
                uint16_t c = px[i];
                uint8_t r = (uint8_t)((c >> 11) & 0x1F);
                uint8_t g = (uint8_t)((c >> 5) & 0x3F);
                uint8_t b = (uint8_t)(c & 0x1F);
                put_rgba(dst, (uint8_t)((r << 3) | (r >> 2)), (uint8_t)((g << 2) | (g >> 4)),
                         (uint8_t)((b << 3) | (b >> 2)));
            }
        }
        break;
    default:
        /* 8-bit grayscale */
        for (uint32_t i = 0; i < w; i++, dst += 4)
        {
            put_rgba(dst, src[i], src[i], src[i]);
        }
        break;
    }
}

static void port_gui_lcd_update(struct gui_dispdev *dc)
{
    uint32_t x = dc->section.x1;
    uint32_t y = dc->section.y1;
    uint32_t w = dc->section.x2 - dc->section.x1 + 1;
    uint32_t h = dc->section.y2 - dc->section.y1 + 1;
    const uint8_t *src = dc->frame_buf;

    for (uint32_t row = 0; row < h; row++)
    {
        uint8_t *dst = screen_rgba + ((size_t)(y + row) * lcd_width + x) * 4;
        convert_row(dst, src, w);
        src += (size_t)w * lcd_pixel_bits / 8;
    }
}

static struct gui_dispdev dc =
{
    .fb_height = LCD_SECTION_HEIGHT,
    .type = DC_RAMLESS,
    .section = {0, 0, 0, 0},
    .section_count = 0,
    .lcd_update = port_gui_lcd_update,
};

/* Select the screen. Call before main() (before the engine starts). */
EMSCRIPTEN_KEEPALIVE void web_sim_configure(int32_t width, int32_t height, int32_t pixel_bits,
                                            int32_t corner_radius)
{
    if (screen_rgba != NULL)
    {
        /* The buffers are already sized for the current screen. */
        gui_log("web_sim_configure: ignored, the display is already initialized\n");
        return;
    }
    lcd_width = width;
    lcd_height = height;
    lcd_pixel_bits = pixel_bits;
    lcd_corner_radius = corner_radius;
}

/* Queried by web_sim.js to size and paint the canvas. */
EMSCRIPTEN_KEEPALIVE uint8_t *web_sim_screen_buffer(void)
{
    return screen_rgba;
}

EMSCRIPTEN_KEEPALIVE int32_t web_sim_screen_width(void)
{
    return lcd_width;
}

EMSCRIPTEN_KEEPALIVE int32_t web_sim_screen_height(void)
{
    return lcd_height;
}

EMSCRIPTEN_KEEPALIVE int web_sim_screen_corner_radius(void)
{
    return lcd_corner_radius;
}

void gui_port_dc_init(void)
{
    size_t pixels = (size_t)lcd_width * lcd_height;

    screen_rgba = (uint8_t *)calloc(pixels, 4);
    for (size_t i = 3; i < pixels * 4; i += 4)
    {
        screen_rgba[i] = 0xFF;
    }

    dc.fb_width = lcd_width;
    dc.screen_width = lcd_width;
    dc.screen_height = lcd_height;
    dc.bit_depth = lcd_pixel_bits;

    gui_dc_info_register(&dc);
    dc.disp_buf_1 = malloc((size_t)dc.fb_height * dc.fb_width * lcd_pixel_bits / 8);
    dc.disp_buf_2 = malloc((size_t)dc.fb_height * dc.fb_width * lcd_pixel_bits / 8);
}
