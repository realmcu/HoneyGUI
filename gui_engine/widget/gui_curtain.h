/*
 * File      : gui_curtain.h
 */
#ifndef __GUI_CURTAIN_H__
#define __GUI_CURTAIN_H__



#ifdef __cplusplus
extern "C" {
#endif
#include <guidef.h>
#include <gui_fb.h>
#include "gui_curtainview.h"
/**********************
 *      TYPEDEFS
 **********************/
/*Data of tab*/



typedef struct gui_curtain
{
    gui_obj_t base;
    gui_curtain_enum_t orientation;
    float scope;
    void (*ctor)(struct gui_curtain *this, gui_obj_t *parent, const char *filename, int16_t x,
                 int16_t y,
                 int16_t w, int16_t h, gui_curtain_enum_t orientation, float scope);
} gui_curtain_t;


/**
 * @brief create a curtain widget,which should be nested in a curtainview.
 * @param parent the father widget nested in.
 * @param filename this curtain widget's name.
 * @param x the X-axis coordinate of the widget.
 * @param x the Y-axis coordinate of the widget.
 * @param w the width of the widget.
 * @param h the hight of the widget.
 * @param orientation the orientation of the curtain,using gui_curtain_enum_t range.
 * @return return the widget object pointer.
 *
 */
gui_curtain_t *gui_curtain_create(void *parent, const char *filename, int16_t x, int16_t y,
                                  int16_t w, int16_t h, gui_curtain_enum_t orientation, float scope);

#ifdef __cplusplus
}
#endif

#endif



