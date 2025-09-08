/*
 * File      : fb_sdl.h
 */
#ifndef __FB_SDL_H__
#define __FB_SDL_H__


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DRV_LCD_WIDTH
#define DRV_LCD_WIDTH   480
#endif
#ifndef DRV_LCD_HIGHT
#define DRV_LCD_HIGHT   480
#endif
#ifndef DRV_PIXEL_BITS
#define DRV_PIXEL_BITS   16
#endif

// #define USE_DC_PFB

void port_direct_draw_bitmap_to_lcd(int16_t x, int16_t y, int16_t width, int16_t height,
                                    const uint8_t *bitmap);



#ifdef __cplusplus
}
#endif

#endif
