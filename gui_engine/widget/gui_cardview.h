/*
 * File      : gui_cardview.h
 */
#ifndef __GUI_CARDVIEW_H__
#define __GUI_CARDVIEW_H__



#ifdef __cplusplus
extern "C" {
#endif

#include <guidef.h>
#include <gui_api.h>


typedef struct
{
    int32_t x;
    int32_t y;
    int32_t z;
} gui_card_id_t;
typedef struct gui_cardview
{
    gui_obj_t base;
    gui_list_t  tab_list;
    gui_card_id_t id;
    uint16_t tab_cnt;
    int8_t tab_cnt_left;
    int8_t tab_cnt_right;
    int8_t tab_cnt_up;
    int8_t tab_cnt_down;
    gui_card_id_t cur_id;
    SLIDE_STYLE style;
    int16_t release_y;
} gui_cardview_t;




gui_cardview_t *gui_cardview_create(void *parent,  const char *name,
                                    int16_t x, int16_t y, int16_t w, int16_t h);

void gui_cardview_add_tab(void *addr);

void gui_cardview_set_style(gui_cardview_t *this, SLIDE_STYLE style);

void gui_cardview_default(void);


#ifdef __cplusplus
}
#endif

#endif

