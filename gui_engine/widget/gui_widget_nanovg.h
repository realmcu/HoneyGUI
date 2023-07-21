/*
 * File      : gui_widget_nanovg.h
 */
#ifndef __GUI_WIDGET_NANOVG_H__
#define __GUI_WIDGET_NANOVG_H__



#ifdef __cplusplus
extern "C" {
#endif

#include <guidef.h>
#include <gui_api.h>
#include <nanovg.h>



typedef struct _gui_widget_nanovg
{
    gui_obj_t base;
    NVGcontext *vg;
    void (*nanovg_canvas_cb)(struct _gui_widget_nanovg *this);
} gui_widget_nanovg_t;




gui_widget_nanovg_t *gui_widget_nanovg_create(void *parent,  const char *name, void *addr,
                                              int16_t x, int16_t y, int16_t w, int16_t h);

void gui_widget_nanovg_set_canvas_cb(gui_widget_nanovg_t *this,
                                     void (*cb)(gui_widget_nanovg_t *this));

#ifdef __cplusplus
}
#endif

#endif

