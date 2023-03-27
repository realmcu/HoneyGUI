/*
 * File      : gui_page.h
 */
#ifndef __GUI_page_H__
#define __GUI_page_H__

#include <guidef.h>
#include <gui_fb.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <gui_curtain.h>
#include <gui_iconlist.h>
typedef struct gui_page
{
    gui_obj_t base;
    uint32_t current_id;
    uint32_t widget_count;
    uint32_t width;
    int yold;
    int start_x;
    int start_y;
    gui_img_t *scroll_bar;
    void (*ctor)(struct gui_page *this, gui_obj_t *parent, const char *filename, int16_t x,
                 int16_t y, int16_t w, int16_t h);
} gui_page_t;
void page_update(gui_obj_t *obj);
void gui_page_ctor(gui_page_t *this, gui_obj_t *parent, const char *filename, int16_t x,
                   int16_t y, int16_t w, int16_t h);
//gui_grid
/**
 * @brief create a page widget.
 * @param parent the father widget the pagedow nested in.
 * @param filename the pagedow widget name.
 * @param x the X-axis coordinate.
 * @param y the Y-axis coordinate.
 * @param w the width.
 * @param h the hight.
 * @return return the widget object pointer
 *
 */
gui_page_t *gui_page_create(void *parent, const char *filename, int16_t x, int16_t y,
                            int16_t w, int16_t h);
typedef gui_curtain_enum_t BAR_LAYOUT_T;
typedef struct gui_pagebar
{
    gui_obj_t base;
    BAR_LAYOUT_T bar_layout;
    uint32_t page_count;
    uint32_t current_page;
    gui_iconlist_t *ic;
    uint32_t icon_w;
    uint32_t icon_h;
    int page_x;
    int page_y;
    uint32_t page_w;
    uint32_t page_h;
    void (*ctor)(struct gui_page *this, gui_obj_t *parent, const char *filename, int16_t x,
                 int16_t y, int16_t w, int16_t h);
} gui_pagebar_t;

gui_pagebar_t *gui_pagebar_create(void *parent, const char *filename, int16_t x, int16_t y,
                                  int16_t w, int16_t h, BAR_LAYOUT_T bar_layout);

/**
 * @brief add scroll bar in page
 *
 * @param this the page widget object pointer
 * @param bar_pic the bar picture address
 */
void gui_page_add_scroll_bar(gui_page_t *this, void *bar_pic);

#ifdef __cplusplus
}
#endif

#endif

