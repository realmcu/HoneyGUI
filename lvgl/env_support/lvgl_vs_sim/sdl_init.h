/*
 * File      : sdl_init.h
 */
#ifndef __SDL_INIT_H__
#define __SDL_INIT_H__

#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif

void sdl_dc_update(uint8_t *framebuffer, uint16_t xStart, uint16_t yStart, uint16_t w,
                   uint16_t h);
void sdl_dc_init(void);

bool sdl_get_touch_status(void);

uint32_t sdl_get_x(void);

uint32_t sdl_get_y(void);

#ifdef __cplusplus
}
#endif

#endif
