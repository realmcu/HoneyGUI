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

typedef enum gui_tab_style
{
    CLASSIC,
    REDUCTION,
    FADE,
    REDUCTION_FADE,
    STACKING,
} SLIDE_STYLE;

/*Data of tab*/
typedef struct
{
    int32_t x;
    int32_t y;
    int32_t z;
} gui_tabview_tab_id_t;


typedef struct gui_tabview
{
    gui_obj_t base;
    uint16_t tab_cnt;
    int8_t tab_cnt_left;
    int8_t tab_cnt_right;
    int8_t tab_cnt_up;
    int8_t tab_cnt_down;
    gui_tabview_tab_id_t cur_id;
    gui_jump_t jump;
    SLIDE_STYLE style;

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

void gui_tabview_jump_tab(gui_tabview_t *parent_tabview, int8_t idx, int8_t idy);

void gui_tabview_set_style(gui_tabview_t *this, SLIDE_STYLE style);

#include "gui_tab.h"


#ifdef __cplusplus
}
#endif

#endif

