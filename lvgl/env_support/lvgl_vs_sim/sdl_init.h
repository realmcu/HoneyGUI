/*
 * File      : sdl_init.h
 */
#ifndef __SDL_INIT_H__
#define __SDL_INIT_H__



#ifdef __cplusplus
extern "C" {
#endif

void sdl_dc_update(void);
void sdl_dc_init(void);
void *sdl_getframebuffer(void);

#ifdef __cplusplus
}
#endif

#endif
