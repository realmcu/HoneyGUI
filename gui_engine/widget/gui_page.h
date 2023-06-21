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
    int get_yend;
    void (*ctor)(struct gui_page *this, gui_obj_t *parent, const char *filename, int16_t x,
                 int16_t y, int16_t w, int16_t h);
} gui_page_t;
typedef struct _gui_api_page
{
    void (*gui_page_add_scroll_bar)(gui_page_t *this, void *bar_pic);
    void (*set_offset)(gui_page_t *this, int offset);
    int (*get_offset)(gui_page_t *this);
} _gui_api_page_t;

void page_update(gui_obj_t *obj);
void gui_page_ctor(gui_page_t *this, gui_obj_t *parent, const char *filename, int16_t x,
                   int16_t y, int16_t w, int16_t h);
//gui_grid
/**
 * @brief create a page widget.
 *
 * @param parent the father widget the page nested in.
 * @param filename the page widget name.
 * @param x the X-axis coordinate.
 * @param x the Y-axis coordinate.
 * @param w the width.
 * @param h the hight.
 * @return return the widget object pointer
 *
 */
gui_page_t *gui_page_create(void *parent, const char *filename, int16_t x, int16_t y,
                            int16_t w, int16_t h);

extern _gui_api_page_t gui_page_api;

#ifdef __cplusplus
}
#endif

#endif

