/*
 * File      : gui_round_corner_rect.h
 */
#ifndef __GUI_ROUND_CORNER_RECT_H__
#define __GUI_ROUND_CORNER_RECT_H__




#ifdef __cplusplus
extern "C" {
#endif
#include <guidef.h>
#include "gui_graphic.h"
#include "gui_canvas.h"
typedef struct gui_round_corner_rect gui_round_corner_rect_t;
struct gui_round_corner_rect
{
    gui_canvas_t base_canvas;
    uint16_t corner_radius;
    void (*set_color)(gui_round_corner_rect_t *this, uint32_t color);
    void (*set)(gui_round_corner_rect_t *this,
                int16_t x,
                int16_t y,
                int16_t w,
                int16_t h,
                uint16_t corner_radius,
                uint32_t color);
    void (*ctor)(gui_round_corner_rect_t *this,
                 gui_obj_t *parent,
                 const char *name,
                 int16_t x,
                 int16_t y,
                 int16_t w,
                 int16_t h,
                 uint16_t corner_radius,
                 uint32_t color);
    gui_animate_t *animate;
    void (* set_animate)(gui_round_corner_rect_t *round_corner_rect, uint32_t dur, int repeatCount,
                         void *callback, void *p);

};
gui_round_corner_rect_t *gui_round_corner_rect_create(
    void *parent,
    const char *name,
    int16_t x,
    int16_t y,
    int16_t w,
    int16_t h,
    uint16_t corner_radius,
    uint32_t color
);
#ifdef __cplusplus
}
#endif

#endif
