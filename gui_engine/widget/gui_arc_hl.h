/*
 * File      : gui_arc_hl.h
 */
#ifndef __GUI_ARC_HL_H__
#define __GUI_ARC_HL_H__




#ifdef __cplusplus
extern "C" {
#endif
#include <guidef.h>
#include "gui_ring.h"
#include "gui_arc.h"
typedef struct gui_arc_hl gui_arc_hl_t;
struct gui_arc_hl
{
    gui_obj_t base;
    gui_ring_t *background;
    struct gui_arc *highlight;
    gui_animate_t *animate;
    void (* set_animate)(gui_arc_hl_t *arc_hl, uint32_t dur, int repeatCount, void *callback, void *p);
    void (*ctor)(gui_arc_hl_t *this, gui_obj_t *parent, const char *name, int16_t center_x,
                 int16_t center_y, int16_t radius, uint16_t width, uint32_t color, uint32_t background_color,
                 int start_degree, int end_degree, bool round);
};
gui_arc_hl_t *gui_arc_highlight_create(
    void *parent,
    const char *name,
    int16_t center_x,
    int16_t center_y,
    int16_t radius,
    uint16_t width,
    uint32_t color,
    uint32_t background_color,
    int start_degree,
    int end_degree,
    bool round
);
#ifdef __cplusplus
}
#endif

#endif
