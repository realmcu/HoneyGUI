/*
 * File      : gui_progressbar.h
 */
#ifndef __GUI_switch_H__
#define __GUI_switch_H__

#include <guidef.h>
#include <gui_fb.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "gui_graphic.h"
#include "gui_img.h"
/**********************
 *      TYPEDEFS
 **********************/
typedef struct gui_switch gui_switch_t;
struct gui_switch
{
    gui_obj_t base;
    gui_img_t *switch_picture;
    void *on_pic_addr;
    void *off_pic_addr;
    bool ifon;
    void (*turn_off)(gui_switch_t *sw);
    void (*turn_on)(gui_switch_t *sw);
    void (*onOn)(gui_switch_t *this, void *cb, void *p);
    void (*onOff)(gui_switch_t *this, void *cb, void *p);
    void (*ctor)(gui_switch_t *this, gui_obj_t *parent,
                 int16_t x,
                 int16_t y, int16_t w, int16_t h, void *off_pic, void *on_pic);
} ;



/**
 * @brief create a switch widget.
 * @param parent the father widget it nested in.
 * @param filename this switch widget's name.
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @param w the width of the widget.
 * @param h the hight of the widget.
 * @return return the widget object pointer.
 *
 */
gui_switch_t *gui_switch_create(void *parent, int16_t x, int16_t y,
                                int16_t w, int16_t h, void *off_pic, void *on_pic);


#ifdef __cplusplus
}
#endif

#endif

