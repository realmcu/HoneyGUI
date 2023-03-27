/*
 * File      : gui_iconlist_default.h
 */
#ifndef __GUI_ICONLIST_DEFAULT_H__
#define __GUI_ICONLIST_DEFAULT_H__

#include <guidef.h>
#include <gui_fb.h>

#ifdef __cplusplus
extern "C" {
#endif


#include <gui_graphic.h>
#include "gui_iconlist.h"
/**********************
 *      TYPEDEFS
 **********************/
/*Data of icon*/

typedef struct gui_iconlist_default
{
    gui_iconlist_t base;
    uint32_t icon_gap;
    void *back_img_src;
    void *back_img_highlight_src;
    void(*ctor)(gui_obj_t *parent, const char *filename, int16_t x,
                int16_t y, int16_t w, int16_t h, struct gui_iconlist_default *this);
} gui_iconlist_default_t;




#ifdef __cplusplus
}
#endif

#endif


