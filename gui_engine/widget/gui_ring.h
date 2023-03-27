/*
 * File      : gui_ring.h
 */
#ifndef __GUI_RING_H__
#define __GUI_RING_H__




#ifdef __cplusplus
extern "C" {
#endif
#include <guidef.h>
#include "gui_graphic.h"
#include "gui_canvas.h"
typedef struct gui_ring gui_ring_t;
struct gui_ring
{
    gui_canvas_t base_canvas;
    void (*set)(gui_ring_t *this, int16_t center_x, int16_t center_y, int16_t radius, uint16_t width,
                uint32_t color);
    void (*ctor)(gui_ring_t *this, gui_obj_t *parent, const char *name, int16_t center_x,
                 int16_t center_y, int16_t radius, uint16_t width, uint32_t color);
};
gui_ring_t *gui_ring_create(void *parent, const char *name, int16_t center_x, int16_t center_y,
                            int16_t radius, uint16_t width, uint32_t color);
#ifdef __cplusplus
}
#endif

#endif
