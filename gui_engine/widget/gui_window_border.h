/*
 * File      : gui_window_border.h
 */
#ifndef __GUI_WINDOW_BORDER_H__
#define __GUI_WINDOW_BORDER_H__



#ifdef __cplusplus
extern "C" {
#endif
#include <guidef.h>
#include <gui_fb.h>

typedef struct gui_window_border
{
    gui_obj_t base;
    void (*ctor)(struct gui_window_border *this, gui_obj_t *parent, const char *filename, int16_t x,
                 int16_t y, int16_t w, int16_t h);
} gui_window_border_t;

gui_window_border_t *gui_window_border_create(void *parent, const char *filename, int16_t x,
                                              int16_t y,
                                              int16_t w, int16_t h);

void window_border_ctor(gui_window_border_t *this, gui_obj_t *parent, const char *filename,
                        int16_t x,
                        int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif

