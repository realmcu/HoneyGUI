/*
 * File      : gui_win.h
 */
#ifndef __GUI_WIN_H__
#define __GUI_WIN_H__

#include <guidef.h>
#include <gui_fb.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gui_win
{
    gui_obj_t base;
    void (*ctor)(struct gui_win *this, gui_obj_t *parent, const char *filename, int16_t x,
                 int16_t y, int16_t w, int16_t h);
    gui_animate_t *animate;
    void (* set_animate)(struct gui_win *win, uint32_t dur, int repeatCount, void *callback, void *p);

} gui_win_t;

/**
 * @brief create a window widget.
 * @param parent the father widget the window nested in.
 * @param filename the window widget name.
 * @param x the X-axis coordinate.
 * @param x the Y-axis coordinate.
 * @param w the width.
 * @param h the hight.
 * @return return the widget object pointer
 *
 */
gui_win_t *gui_win_create(void *parent, const char *filename, int16_t x, int16_t y,
                          int16_t w, int16_t h);


void gui_win_ctor(gui_win_t *this, gui_obj_t *parent, const char *filename, int16_t x,
                  int16_t y, int16_t w, int16_t h);


#ifdef __cplusplus
}
#endif

#endif

