/*
 * File      : gui_tab.h
 */
#ifndef __GUI_TAB_H__
#define __GUI_TAB_H__



#ifdef __cplusplus
extern "C" {
#endif
#include <guidef.h>
#include <gui_fb.h>
/**********************
 *      TYPEDEFS
 **********************/
/*Data of tab*/
#include "gui_tabview.h"



typedef struct gui_tab
{
    gui_obj_t base;
    gui_tab_ext_id_t id;
    void (*ctor)(struct gui_tab *this, gui_obj_t *parent, const char *filename, int16_t x, int16_t y,
                 int16_t w, int16_t h, int16_t idx, int16_t idy);
} gui_tab_t;

/**
 * @brief create a tab widget,which should be nested in a tabview.
 * @param parent the father widget it nested in.
 * @param filename this tab widget's name.
 * @param x the X-axis coordinate of the widget.
 * @param x the Y-axis coordinate of the widget.
 * @param w the width of the widget.
 * @param h the hight of the widget.
 * @param idx the X-axis index in a tabview.
 * @param idy the Y-axis index in a tabview.
 * @return return the widget object pointer.
 *
 */
gui_tab_t *gui_tab_create(void *parent, const char *filename, int16_t x, int16_t y,
                          int16_t w, int16_t h, int16_t idx, int16_t idy);


#ifdef __cplusplus
}
#endif

#endif

