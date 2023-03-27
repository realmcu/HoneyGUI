/*
 * File      : gui_scroll_wheel.h
 */
#ifndef __GUI_SCROLL_WHEEL_H__
#define __GUI_SCROLL_WHEEL_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <guidef.h>
#include <gui_fb.h>
#include "gui_page.h"


#define SCROLL_WHEEL_ROW 20
typedef struct scroll_wheel_picture
{
    void *pic_addr;
    void *pic_hl_addr;
    gui_img_t *pic;
} scroll_wheel_picture_t;
typedef struct gui_scroll_wheel
{
    gui_page_t base;
    int child_gap;
    int highlight_start;
    int highlight_end;
    void (*ctor)(struct gui_scroll_wheel *this, gui_obj_t *parent, int16_t x, int16_t y, int16_t w,
                 int16_t h);
    uint32_t index;
    uint32_t (*get_index)(struct gui_scroll_wheel *this);
    struct scroll_wheel_picture picture[SCROLL_WHEEL_ROW];
    bool press_flag;
    char row_count;
} gui_scroll_wheel_t;

/**
 * @brief Create a scroll_whell.https://vendorjira.realtek.com/secure/attachment/204169/scrollwheel.mp4
 *
 * @param parent the father widget it nested in.
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @param w the width of the widget.
 * @param h the hight of the widget.
 * @return return the widget object pointer.
 */
gui_scroll_wheel_t *gui_scroll_wheel_create(void *parent,  void *addr, int16_t x, int16_t y,
                                            int16_t w, int16_t h);
/**
 * @brief Append a picture in the wheel.
 *
 * @param this  The pointer of scroll_wheel widget.
 * @param num_pic The picture array address.
 * @param num_pic_hl The picture(highlight) array address.
 */
void gui_scrollwheel_append(gui_scroll_wheel_t *this, void *num_pic, void *num_pic_hl);
void scroll_wheel_update_att(gui_obj_t *obj);

#ifdef __cplusplus
}
#endif

#endif
