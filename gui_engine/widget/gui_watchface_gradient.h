/*
 * File      : gui_watchface_gradient.h
 */
#ifndef __GUI_WATCHFACE_GRADIENT_H__
#define __GUI_WATCHFACE_GRADIENT_H__



#ifdef __cplusplus
extern "C" {
#endif

#include <guidef.h>
#include <gui_api.h>
#include <gui_canvas.h>
#include "gui_watchface_gradient.h"



typedef struct gui_watchface_gradient
{
    gui_canvas_t base;
} gui_watchface_gradient_t;




gui_watchface_gradient_t *gui_watchface_gradient_create(void *parent,  const char *name,
                                                        int16_t x, int16_t y, int16_t w, int16_t h);


#ifdef __cplusplus
}
#endif

#endif

