/*
 * File      : gui_arc.h
 */
#ifndef __GUI_ARC_H__
#define __GUI_ARC_H__



#ifdef __cplusplus
extern "C" {
#endif

#include <guidef.h>
#include <gui_api.h>
#include "nanovg.h"



typedef struct gui_arc
{
    gui_obj_t base;
    float cx;
    float cy;
    float r;
    float a0;
    float a1;
    int dir;
    NVGcolor color;
    float width;
} gui_arc_t;




gui_arc_t *gui_arc_create(void *parent,  const char *name, int16_t x, int16_t y, int16_t w,
                          int16_t h);

void gui_arc_set(gui_arc_t *this, float cx, float cy, float r, float a0_degree, float a1_degree,
                 int dir);
void gui_arc_set_stroke(gui_arc_t *this, gui_color_t color, float w);


#ifdef __cplusplus
}
#endif

#endif

