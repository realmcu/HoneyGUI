/*
 * File      : gui_round_rect.h
 */
#ifndef __GUI_ROUND_RECT_H__
#define __GUI_ROUND_RECT_H__




#ifdef __cplusplus
extern "C" {
#endif
#include <guidef.h>
#include "gui_graphic.h"
#include "gui_canvas.h"
typedef struct gui_round_rect gui_round_rect_t;
struct gui_round_rect
{
    gui_canvas_t base_canvas;
    void (*set)(gui_round_rect_t *this, int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color);
    void (*ctor)(gui_round_rect_t *this, gui_obj_t *parent, const char *name, int16_t x, int16_t y,
                 int16_t w, int16_t h, uint32_t color);
};
gui_round_rect_t *gui_rount_rect_create(void *parent, const char *name, int16_t x, int16_t y,
                                        int16_t w, int16_t h, uint32_t color);
#ifdef __cplusplus
}
#endif

#endif
