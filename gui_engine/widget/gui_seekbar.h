/*
 * File      : gui_seekbar.h
 */
#ifndef __GUI_SEEKBAR_H__
#define __GUI_SEEKBAR_H__

#include <guidef.h>
#include <gui_fb.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "gui_progressbar.h"
/**********************
 *      TYPEDEFS
 **********************/
typedef struct gui_seekbar gui_seekbar_t;
struct gui_seekbar
{
    gui_progressbar_t base;
    union
    {
        gui_img_t *slider_img;
    } slider;
    bool hit_slider;
    bool press_flag;
    void *press_cb;
    void *release_cb;
    void *release_cb_p;
    void *press_cb_p;
    int deltaX_old;
    uint16_t arcx;
    uint16_t arcy;
    uint16_t arc_r;
    uint16_t arc_w;
    float arc_start;
    float arc_end;
    void (*ctor)(gui_seekbar_t *this, gui_obj_t *parent, const char *filename, int16_t x,
                 int16_t y,
                 int16_t w, int16_t h);
} ;

typedef struct gui_api_seekbar
{
    void (*onPress)(gui_seekbar_t *this, void *cb, void *p);
    void (*onRelease)(gui_seekbar_t *this, void *cb, void *p);
    void (*onPressing)(gui_seekbar_t *this, void *cb, void *p);
} gui_api_seekbar_t;
extern gui_api_seekbar_t gui_seekbar_api;
/**
 * @brief create a seekbar widget.
 *
 * @param parent the father widget it nested in.
 * @param filename this seekbar widget's name.
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @param w the width of the widget.
 * @param h the hight of the widget.
 * @return return the widget object pointer.
 *
 */
gui_seekbar_t *gui_seekbar_create(void *parent, const char *filename, int16_t x, int16_t y,
                                  int16_t w, int16_t h);

gui_seekbar_t *gui_seekbar_h_create(void *parent, const char *filename, int16_t x, int16_t y,
                                    int16_t w, int16_t h);
gui_seekbar_t *gui_seekbar_create_img_v(void *parent, const char *filename, int16_t x, int16_t y
                                       );
gui_seekbar_t *gui_seekbar_create_img_h(void *parent, const char *filename, int16_t x, int16_t y
                                       );
gui_seekbar_t *gui_seekbar_create_movie_h(void *parent, void  **picture_array,
                                          uint16_t array_length, int16_t x, int16_t y);
gui_seekbar_t *gui_seekbar_create_movie_v(void *parent, void  **picture_array,
                                          uint16_t array_length, int16_t x, int16_t y);
gui_seekbar_t *gui_seekbar_create_movie_arc(void *parent, void  **picture_array,
                                            uint16_t array_length, int16_t x, int16_t y, uint16_t arc_x, uint16_t arc_y, uint16_t arc_r,
                                            uint16_t arc_w, float arc_start, float arc_end);
#ifdef __cplusplus
}
#endif

#endif

