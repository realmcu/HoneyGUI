/*
 * File      : gui_canvas.h
 */
#ifndef __GUI_CANVAS_H__
#define __GUI_CANVAS_H__



#ifdef __cplusplus
extern "C" {
#endif

#include <guidef.h>
#include <gui_api.h>
#include <nanovg.h>



typedef struct _gui_canvas
{
    gui_obj_t base;
    NVGcontext *vg;
    void (*nanovg_canvas_cb)(struct _gui_canvas *this);
} gui_canvas_t;




gui_canvas_t *gui_canvas_create(void *parent,  const char *name, void *addr,
                                int16_t x, int16_t y, int16_t w, int16_t h);

void gui_canvas_set_canvas_cb(gui_canvas_t *this,
                              void (*cb)(gui_canvas_t *this));

#ifdef __cplusplus
}
#endif

#endif

