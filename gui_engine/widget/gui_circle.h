/*
 * File      : gui_circle.h
 */
#ifndef __GUI_CIRCLE_H__
#define __GUI_CIRCLE_H__




#ifdef __cplusplus
extern "C" {
#endif
#include <guidef.h>
#include "gui_graphic.h"
#include "gui_bitmap.h"
typedef struct gui_circle gui_circle_t;
struct gui_circle
{
    gui_bitmap_t base;
    gui_circleshape_t circle;
    void(*set)(gui_circle_t *this, int16_t center_x, int16_t center_y, int16_t radius, uint32_t color);
    void(*ctor)(gui_circle_t *this, gui_obj_t *parent, const char *name, int16_t center_x,
                int16_t center_y, int16_t radius, uint32_t color);
};
gui_circle_t *gui_circle_create(void *parent, const char *name, int16_t center_x, int16_t center_y,
                                uint16_t radius, uint32_t color);
#ifdef __cplusplus
}
#endif

#endif
