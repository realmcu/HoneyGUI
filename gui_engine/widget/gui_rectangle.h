/*
 * File      : gui_rectangle.h
 */
#ifndef __GUI_RECTANGLE_H__
#define __GUI_RECTANGLE_H__




#ifdef __cplusplus
extern "C" {
#endif
#include <guidef.h>
#include "gui_graphic.h"
#include "gui_bitmap.h"
typedef struct gui_rectangle gui_rectangle_t;
struct gui_rectangle
{
    gui_bitmap_t base;
    void (*set)(gui_rectangle_t *this, int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color);
    void (*ctor)(gui_rectangle_t *this, gui_obj_t *parent, const char *name, int16_t x, int16_t y,
                 int16_t w, int16_t h, uint32_t color);
};
gui_rectangle_t *gui_rectangle_create(void *parent, const char *name, int16_t x, int16_t y,
                                      int16_t w, int16_t h, uint32_t color);
#ifdef __cplusplus
}
#endif

#endif
