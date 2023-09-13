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
#include "gui_tabview.h"



typedef struct _gui_tab_stacking_t
{
    float scale;
    float location;
    uint8_t opacity;
} gui_tab_stacking_t;




typedef struct gui_tab
{
    gui_obj_t base;
    gui_tabview_tab_id_t id;
    SLIDE_STYLE style;
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

void gui_tab_set_style(gui_tab_t *this, SLIDE_STYLE style);


static void gui_tab_get_stacking_location(gui_tab_stacking_t *result, gui_tab_stacking_t *start,
                                          gui_tab_stacking_t *end, float step);

#ifdef __cplusplus
}
#endif

#endif

