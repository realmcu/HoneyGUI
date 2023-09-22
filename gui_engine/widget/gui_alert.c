/*
 * File      : gui_img.c
 * This file is part of GUI Engine
 */

#include <guidef.h>
#include <gui_alert.h>
//#include <string.h>
#include <gui_server.h>
//#include "gui_curtain.h"
#include "gui_obj.h"

void gui_alert_ctor(struct gui_alert *this, gui_obj_t *parent, const char *widgetname, int16_t x,
                    int16_t y, int16_t w, int16_t h, uint32_t timeout)
{
    gui_obj_ctor(&this->base, parent, widgetname, x, y, w, h);
    uint32_t *p = gui_malloc(sizeof(int) * 2);
}
gui_alert_t *gui_alert_create(gui_obj_t *screen, const char *widget_name, int16_t x, int16_t y,
                              int16_t w, int16_t h, uint32_t timeout)
{
#define _gui_alert_param this, screen,widget_name, x, y, w, h, timeout
    GUI_NEW(gui_alert_t, gui_alert_ctor, _gui_alert_param)
}









