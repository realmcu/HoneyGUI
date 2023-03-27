/*
 * File      : gui_arc.h
 */
#ifndef __GUI_ARC_H__
#define __GUI_ARC_H__




#ifdef __cplusplus
extern "C" {
#endif
#include <guidef.h>
#include "gui_graphic.h"
#include "gui_canvas.h"
typedef struct gui_arc gui_arc_t;
struct gui_arc
{
    struct gui_canvas base_canvas;
    int16_t radius;
    uint16_t width;
    void (*set)(gui_arc_t *this, uint32_t color, int start_degree, int end_degree, bool round);
    void (*ctor)(gui_arc_t *this, gui_obj_t *parent, const char *name, int16_t center_x,
                 int16_t center_y, int16_t radius, uint16_t width, uint32_t color, int start_degree, int end_degree,
                 bool round);
    gui_animate_t *animate;
    void (* set_animate)(gui_arc_t *arc, uint32_t dur, int repeatCount, void *callback, void *p);
};
gui_arc_t *gui_arc_create(
    void *parent,
    const char *name,
    int16_t center_x,
    int16_t center_y,
    int16_t radius,
    uint16_t width,
    uint32_t color,
    int start_degree,
    int end_degree,
    bool round
);
#ifdef __cplusplus
}
#endif

#endif
