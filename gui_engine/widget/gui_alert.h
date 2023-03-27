/*
 * File      : gui_win.h
 */
#ifndef __GUI_ALERT_H__
#define __GUI_ALERT_H__

#include <guidef.h>
#include <gui_fb.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gui_alert
{
    gui_obj_t base;
    void (*ctor)(struct gui_alert *this, gui_obj_t *parent, const char *widgetname, int16_t x,
                 int16_t y, int16_t w, int16_t h, uint32_t timeout);
} gui_alert_t;

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
gui_alert_t *gui_alert_create(gui_obj_t *screen, const char *widget_name, int16_t x, int16_t y,
                              int16_t w, int16_t h, uint32_t timeout);



#ifdef __cplusplus
}
#endif

#endif

