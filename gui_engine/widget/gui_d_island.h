/*
 * File      : gui_d_island.h
 */
#ifndef __GUI_D_ISLAND_H__
#define __GUI_D_ISLAND_H__


#ifdef __cplusplus
extern "C" {
#endif
#include <guidef.h>
#include <gui_fb.h>
#include <gui_graphic.h>
#include "gui_round_rect.h"
#include "gui_round_corner_rect.h"
typedef struct gui_d_island gui_d_island_t;
struct gui_d_island
{
    gui_obj_t base;
    int threshold_h;
    int init_w;
    int init_h;
    int max_w;
    int max_h;
    int target_w;
    int target_h;
    int from_w;
    int from_h;
    gui_canvas_t *canvas;
    void (*set_island)(gui_d_island_t *this, int w, int h);
    void (*set_island_color)(gui_d_island_t *this, canvas_fill_t *c);
    void (*ctor)(gui_d_island_t *this, gui_obj_t *parent);
    gui_animate_t *animate;
    void (* set_animate)(gui_d_island_t *d_island, uint32_t dur, int repeatCount, void *callback,
                         void *p);

};
/**
 * @brief create a d_islanddow widget.
 * @param parent the father widget the d_islanddow nested in.
 * @param filename the d_islanddow widget name.
 * @param x the X-axis coordinate.
 * @param x the Y-axis coordinate.
 * @param w the width.
 * @param h the hight.
 * @return return the widget object pointer
 *
 */
gui_d_island_t *gui_d_island_create(void *parent);

#ifdef __cplusplus
}
#endif

#endif

