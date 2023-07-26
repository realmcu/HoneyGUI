/*
 * File      : gui_colorwheel.h
 */
#ifndef __GUI_COLORWHEEL_H__
#define __GUI_COLORWHEEL_H__



#ifdef __cplusplus
extern "C" {
#endif

#include <guidef.h>
#include <gui_api.h>
#include <gui_canvas.h>
#include "gui_colorwheel.h"



typedef struct gui_colorwheel
{
    gui_obj_t base;
    float x;
    float y;
    float w;
    float h;
    float t;
} gui_colorwheel_t;




gui_colorwheel_t *gui_colorwheel_create(void *parent,  const char *name, void *addr,
                                        int16_t x, int16_t y, int16_t w, int16_t h);

void gui_colorwheel_set(gui_colorwheel_t *this, float x, float y, float w, float h, float t);

#ifdef __cplusplus
}
#endif

#endif

