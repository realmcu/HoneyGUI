/*
 * File      : gui_progressbar.h
 */
#ifndef __GUI_radio_H__
#define __GUI_radio_H__

#include <guidef.h>
#include <gui_fb.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "gui_img.h"
/**********************
 *      TYPEDEFS
 **********************/
typedef struct gui_radio gui_radio_t;
struct gui_radio
{
    gui_obj_t base;
    uint32_t length;
    uint32_t checked_index;
    void *on_pic_addr;
    void *off_pic_addr;
    uint32_t gap;
    int text_x;
    int text_y;
    uint16_t text_color;
    void (*ctor)(gui_radio_t *this, gui_obj_t *parent,
                 int16_t x,
                 int16_t y, int16_t w, int16_t h, void *off_pic, void *on_pic);
} ;

typedef struct gui_api_radio
{
    uint32_t (*get_checked)(gui_radio_t *this);
    void (*check)(gui_radio_t *this, uint32_t index);
    void (*set_text_xy)(gui_radio_t *this, int text_x, int text_y);
    void (*set_gap)(gui_radio_t *this, int gap);
    void (*append)(gui_radio_t *this, char *text);
    void (*append_vertical)(gui_radio_t *this, char *text);
    void (*onCheck)(gui_radio_t *this, void *cb, void *p);
    void (*ctor)(gui_radio_t *this, gui_obj_t *parent,
                 int16_t x,
                 int16_t y, int16_t w, int16_t h, void *off_pic, void *on_pic);
} gui_api_radio_t;


/**
 * @brief create a radio widget.
 * @param parent the father widget it nested in.
 * @param filename this radio widget's name.
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @param w the width of the widget.
 * @param h the hight of the widget.
 * @return return the widget object pointer.
 *
 */
gui_radio_t *gui_radio_create(void *parent, int16_t x, int16_t y,
                              int16_t w, int16_t h, void *off_pic, void *on_pic);


typedef struct gui_radio_switch gui_radio_switch_t;
struct gui_radio_switch
{
    gui_obj_t base;
    gui_img_t *radio_switch_picture;
    void *on_pic_addr;
    void *off_pic_addr;
    bool ifon;
    //gui_obj_cb_t cb_on;
    //gui_obj_cb_t cb_off;
    void (*turn_off)(gui_radio_switch_t *sw);
    void (*turn_on)(gui_radio_switch_t *sw);
    void (*onOn)(gui_radio_switch_t *this, void *cb, void *p);
    void (*onOff)(gui_radio_switch_t *this, void *cb, void *p);
    void (*ctor)(gui_radio_switch_t *this, gui_obj_t *parent,
                 int16_t x,
                 int16_t y, int16_t w, int16_t h, void *off_pic, void *on_pic);
} ;



/**
 * @brief create a radio_switch widget.
 * @param parent the father widget it nested in.
 * @param filename this radio_switch widget's name.
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @param w the width of the widget.
 * @param h the hight of the widget.
 * @return return the widget object pointer.
 *
 */
gui_radio_switch_t *gui_radio_switch_create(void *parent, int16_t x, int16_t y,
                                            int16_t w, int16_t h, void *off_pic, void *on_pic);


extern gui_api_radio_t gui_radio_api;





#ifdef __cplusplus
}
#endif

#endif

