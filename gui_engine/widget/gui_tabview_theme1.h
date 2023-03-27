/*
 * File      : gui_tabview_theme1.h
 */
#ifndef __GUI_TABVIEW_THEME1_H__
#define __GUI_TABVIEW_THEME1_H__



#ifdef __cplusplus
extern "C" {
#endif
#include <guidef.h>
#include <gui_fb.h>
/**********************
 *      TYPEDEFS
 **********************/
/*Data of tab*/





typedef struct gui_tabview_theme1
{
    gui_obj_t base;
    float ccw;
    void (*ctor)(struct gui_tabview_theme1 *this, gui_obj_t *parent, int16_t x,
                 int16_t y,
                 int16_t r);
} gui_tabview_theme1_t;
gui_tabview_theme1_t *gui_tabview_theme1_create(void *parent, int16_t x, int16_t y, int16_t r);

#ifdef __cplusplus
}
#endif

#endif

