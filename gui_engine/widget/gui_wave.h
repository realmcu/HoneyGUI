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
    void *user_data;
} gui_wave_t;




gui_wave_t *gui_wave_create(void *parent,  const char *name, int16_t x, int16_t y, int16_t w,
                            int16_t h);


#ifdef __cplusplus
}
#endif

#endif

