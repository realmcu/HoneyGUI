#ifndef SDL_DRIVER_H
#define SDL_DRIVER_H

#include <stdint.h>
#include <stdint.h>
#include <string.h>
#include <SDL.h>

#define SDL_USEREVENT_LCD_UPDATE  (SDL_USEREVENT + 1)


#define BLIT_RECT_EX(T, DST, SRC, WIDTH, X1, X2, Y1, Y2)         \
    do {                                                             \
        T * __src = (T *)(SRC);                                      \
        T * __dst = (T *)(DST);                                      \
        const uint32_t __w = (uint32_t)((X2) - (X1) + 1);            \
        for (uint32_t __i = (Y1); __i <= (Y2); ++__i) {              \
            T * __dst_row = __dst + __i * (WIDTH) + (X1);            \
            memcpy(__dst_row, __src, __w * sizeof(T));               \
            __src += __w;                                            \
        }                                                            \
    } while (0)



void sdl_driver_init(uint32_t width, uint32_t height, uint32_t bpp);
void sdl_driver_update_window(uint8_t *input, uint16_t xStart, uint16_t yStart, uint16_t w,
                              uint16_t h);
void sdl_driver_set_canvas(int canvas_w, int canvas_h, int offset_x, int offset_y,
                           int corner_radius, const char *frame_path);

#endif /* SDL_DRIVER_H */
