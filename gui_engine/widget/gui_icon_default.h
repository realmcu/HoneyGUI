/*
 * File      : gui_icon_default.h
 */
#ifndef __GUI_ICON_DEFAULT_H__
#define __GUI_ICON_DEFAULT_H__




#ifdef __cplusplus
extern "C" {
#endif
#include <guidef.h>
#include "gui_icon.h"
typedef struct gui_icon_default
{
    gui_icon_t base;
    gui_img_t *back_img;
    void(*ctor)(struct gui_icon_default *this, gui_obj_t *parent, const char *filename, int16_t x,
                int16_t y, int16_t w, int16_t h, int16_t id);
} gui_icon_default_t;
#ifdef __cplusplus
}
#endif

#endif
