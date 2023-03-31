/*
 * File      : gui_button.h
 */
#ifndef __GUI_BUTTON_H__
#define __GUI_BUTTON_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <guidef.h>
#include "gui_text.h"
#include "gui_img.h"
#include "gui_win.h"
#define BUTTON_COLOR 0xffffffff
#define BUTTON_HLCOLOR 0x0bffff
typedef struct gui_button gui_button_t;
struct gui_button
{
    gui_obj_t base;
    gui_img_t *img;
    gui_text_t *text;
    bool press_flag;
    bool long_flag;
    void *on_pic_addr;
    void *off_pic_addr;
    gui_animate_t *animate;
    void (*ctor)(
        gui_button_t *this,
        gui_obj_t *parent,
        int16_t x,
        int16_t y,
        int16_t w,
        int16_t h,
        void *background_pic,
        void *highlight_pic,
        char *text
    );
};
typedef struct gui_api_button
{
    void (*set_animate)(gui_button_t *b, uint32_t dur, int repeatCount, void *callback, void *p);
    void (*onPress)(gui_button_t *b, void *callback, void *parameter);
    void (*onRelease)(gui_button_t *b, void *callback, void *parameter);
    void (*onLong)(gui_button_t *b, void *callback, void *parameter);
    void (*onClick)(gui_button_t *b, void *callback, void *parameter);
} gui_api_button_t;

extern gui_api_button_t gui_button_api;
/**
 * @brief Creat a button widget.
 *
 * @param parent The father widget which the button nested in.
 * @param x The X-axis coordinate of the button.
 * @param y The Y-axis coordinate of the button.
 * @param w The width of the button.
 * @param h The hight of the button.
 * @param background_pic The image shown when button was not pressed.
 * @param highlight_pic The image shown when button was pressed.
 * @param text The text of the button which always shown.
 * @return Return the widget object pointer
 */
gui_button_t *gui_button_create(
    void *parent,
    int16_t x,
    int16_t y,
    int16_t w,
    int16_t h,
    void *background_pic,
    void *highlight_pic,
    char *text,
    char image_type,
    int count
);
/**
 * @brief Set click cb of button.
 *
 * @param this The pointer of this button widget.
 * @param event_cb Callback event.
 */
void gui_button_click(gui_button_t *this, gui_event_cb_t event_cb);

void gui_button_press(gui_button_t *this, gui_event_cb_t event_cb, void *parameter);
void gui_button_long(gui_button_t *this, gui_event_cb_t event_cb, void *parameter);
void gui_button_release(gui_button_t *this, gui_event_cb_t event_cb, void *parameter);
/**
 * @brief Set the color of text which belongs to a button.
 *
 * @param this The pointer of this button widget.
 * @param color The color of text.
 */
void gui_button_text_color(gui_button_t *this, uint32_t color);
/**
 * @brief Move the img of this button.
 *
 * @param this The pointer of this button widget.
 * @param img_x The X value of img.
 * @param img_y The Y value of img.
 */
void gui_button_img_move(gui_button_t *this, int16_t img_x, int16_t img_y);
void gui_button_text_move(gui_button_t *this, int16_t text_x, int16_t text_y);


#ifdef __cplusplus
}
#endif

#endif
