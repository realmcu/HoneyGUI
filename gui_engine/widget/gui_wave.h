/*
 * File      : gui_wave.h
 */
#ifndef __GUI_WAVE_H__
#define __GUI_WAVE_H__



#ifdef __cplusplus
extern "C" {
#endif

#include <guidef.h>
#include <gui_api.h>



typedef struct gui_wave
{
    gui_obj_t base;
    float x;
    float y;
    float w;
    float h;
    float t;
} gui_wave_t;




gui_wave_t *gui_wave_create(void *parent,  const char *name, int16_t x, int16_t y, int16_t w,
                            int16_t h);

void gui_wave_set(gui_wave_t *this, float x, float y, float w, float h, float t);

#ifdef __cplusplus
}
#endif

#endif

