/*
 * File      : gui_watch_gradient_spot.h
 */
#ifndef __GUI_WATCH_GRADIENT_SPOT_H__
#define __GUI_WATCH_GRADIENT_SPOT_H__



#ifdef __cplusplus
extern "C" {
#endif

#include <guidef.h>
#include <gui_api.h>
#include <gui_canvas.h>
#include "gui_watch_gradient_spot.h"



typedef struct gui_watch_gradient_spot
{
    gui_obj_t base;
    float c_x;
    float c_y;
} gui_watch_gradient_spot_t;




gui_watch_gradient_spot_t *gui_watch_gradient_spot_create(void *parent,  const char *name,
                                                          int16_t x, int16_t y, int16_t w, int16_t h);

void gui_watch_gradient_spot_set_center(gui_watch_gradient_spot_t *this, float c_x, float c_y);

#ifdef __cplusplus
}
#endif

#endif

