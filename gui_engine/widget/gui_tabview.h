/*
 * File      : gui_tabview.h
 */
#ifndef __GUI_TABVIEW_H__
#define __GUI_TABVIEW_H__



#ifdef __cplusplus
extern "C" {
#endif
#include <guidef.h>
#include <gui_fb.h>
/**********************
 *      TYPEDEFS
 **********************/
/*Data of tab*/
typedef struct
{
    int32_t x;
    int32_t y;
} gui_tab_ext_id_t;


enum gui_tabview_style
{
    SLIDE_CLASSIC,
    SLIDE_FADE,
    SLIDE_WHEEL,
    SLIDE_SCALE,
    SLIDE_SCALE_FADE,
};


typedef struct gui_tabview
{
    gui_obj_t base;
    uint16_t tab_cnt;
    int8_t tab_cnt_left;
    int8_t tab_cnt_right;
    int8_t tab_cnt_up;
    int8_t tab_cnt_down;
    gui_tab_ext_id_t cur_id;
    gui_jump_t jump;
    enum gui_tabview_style style;

} gui_tabview_t;

/**
 * @brief create a tabview widget,which can nest tabs.
 * @param parent the father widget it nested in.
 * @param filename this tabview widget's name.
 * @param x the X-axis coordinate of the widget.
 * @param x the Y-axis coordinate of the widget.
 * @param w the width of the widget.
 * @param h the hight of the widget.
 * @return return the widget object pointer.
 *
 */
gui_tabview_t *gui_tabview_create(void *parent, const char *filename, int16_t x, int16_t y,
                                  int16_t w, int16_t h);
void gui_tabview_set_style(gui_tabview_t *this, enum gui_tabview_style style);
void gui_tabview_jump_tab(gui_tabview_t *parent_tabview, int8_t idx, int8_t idy);

#include "gui_tab.h"


#ifdef __cplusplus
}
#endif

#endif

