/*
 * File      : sdl_init.h
 */
#ifndef __SDL_INIT_H__
#define __SDL_INIT_H__



#ifdef __cplusplus
extern "C" {
#endif

void sdl_dc_update(uint8_t *framebuffer, uint16_t xStart, uint16_t yStart, uint16_t w,
                   uint16_t h);
void sdl_dc_init(void);

#ifdef __cplusplus
}
#endif

#endif
