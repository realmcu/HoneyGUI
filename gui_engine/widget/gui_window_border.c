/*
 * File      : gui_img.c
 * This file is part of GUI Engine
 */

#include <guidef.h>
#include <gui_win.h>
#include <string.h>
#include <gui_server.h>

#include "gui_obj.h"

#include "gui_window_border.h"
void window_border_ctor(gui_window_border_t *this, gui_obj_t *parent, const char *filename,
                        int16_t x,
                        int16_t y, int16_t w, int16_t h)
{
    gui_obj_ctor(&this->base, parent, filename, x, y, w, h);
    this->base.type = WINDOWWITHBORDER;
}
gui_window_border_t *gui_window_border_create(void *parent, const char *filename, int16_t x,
                                              int16_t y,
                                              int16_t w, int16_t h)
{
#define _param_window_border_ctor this, parent, filename, x, y, w, h
    GUI_NEW(gui_window_border_t, window_border_ctor, _param_window_border_ctor)
}









