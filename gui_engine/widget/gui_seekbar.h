/*
 * File      : gui_seekbar.h
 */
#ifndef __GUI_SEEKBAR_H__
#define __GUI_SEEKBAR_H__

#include <guidef.h>
#include <gui_fb.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "gui_progressbar.h"
#include "gui_circle.h"
/**********************
 *      TYPEDEFS
 **********************/
typedef struct gui_seekbar gui_seekbar_t;
struct gui_seekbar
{
    gui_progressbar_t base;
    union
    {
        gui_img_t *slider_img;
        gui_circle_t *slider_circle;
    } slider;
    bool hit_slider;
    int deltaX_old;
    void (*hit_cb)(void *obj, gui_event_t e);
    void (*free_cb)(void *obj, gui_event_t e);
    void (*slider_cb)(void *obj, gui_event_t e);
    void (*ctor)(gui_seekbar_t *this, gui_obj_t *parent, const char *filename, int16_t x,
                 int16_t y,
                 int16_t w, int16_t h);
} ;


/**
 * @brief create a seekbar widget,which can nest tabs.
 * @param parent the father widget it nested in.
 * @param filename this seekbar widget's name.
 * @param x the X-axis coordinate of the widget.
 * @param x the Y-axis coordinate of the widget.
 * @param w the width of the widget.
 * @param h the hight of the widget.
 * @return return the widget object pointer.
 *
 */
gui_seekbar_t *gui_seekbar_create(void *parent, const char *filename, int16_t x, int16_t y,
                                  int16_t w, int16_t h);


#ifdef __cplusplus
}
#endif

#endif

