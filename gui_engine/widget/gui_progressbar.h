/*
 * File      : gui_progressbar.h
 */
#ifndef __GUI_progressBAR_H__
#define __GUI_progressBAR_H__

#include <guidef.h>
#include <gui_fb.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "gui_graphic.h"
#include "gui_rectangle.h"
/**********************
 *      TYPEDEFS
 **********************/
typedef struct gui_progressbar gui_progressbar_t;
struct gui_progressbar
{
    gui_obj_t base;
    gui_rectangle_t *max_rectangle;
    gui_rectangle_t *progress_rectangle;
    void (*set_progress)(gui_progressbar_t *this, size_t progress);
    size_t (*get_progress)(gui_progressbar_t *this);
    size_t (*get_max)(gui_progressbar_t *this);
    void (*ctor)(gui_progressbar_t *this, gui_obj_t *parent, const char *filename, int16_t x,
                 int16_t y,
                 int16_t w, int16_t h);
} ;

void gui_progressbar_ctor(gui_progressbar_t *this, gui_obj_t *parent, const char *filename,
                          int16_t x,
                          int16_t y, int16_t w, int16_t h);
/**
 * @brief create a progressbar widget,which can nest tabs.
 * @param parent the father widget it nested in.
 * @param filename this progressbar widget's name.
 * @param x the X-axis coordinate of the widget.
 * @param x the Y-axis coordinate of the widget.
 * @param w the width of the widget.
 * @param h the hight of the widget.
 * @return return the widget object pointer.
 *
 */
gui_progressbar_t *gui_progressbar_create(void *parent, const char *filename, int16_t x, int16_t y,
                                          int16_t w, int16_t h);


#ifdef __cplusplus
}
#endif

#endif

