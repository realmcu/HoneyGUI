/*
 * File      : gui_progressbar.h
 */
#ifndef __GUI_progressBAR_H__
#define __GUI_progressBAR_H__

#include <guidef.h>
#include <gui_fb.h>
#include "gui_img_scope.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "gui_canvas.h"
/**********************
 *      TYPEDEFS
 **********************/
typedef struct gui_progressbar gui_progressbar_t;
struct gui_progressbar
{
    gui_obj_t base;
    gui_canvas_t *c;
    uint32_t color;
    uint32_t color_hl;
    size_t max;
    float per;
    void (*ctor)(gui_progressbar_t *this, gui_obj_t *parent, const char *filename, int16_t x,
                 int16_t y,
                 int16_t w, int16_t h);

} ;



void gui_progressbar_ctor(gui_progressbar_t *this, gui_obj_t *parent, const char *filename,
                          int16_t x,
                          int16_t y, int16_t w, int16_t h);
void gui_progressbar_v_ctor(gui_progressbar_t *this, gui_obj_t *parent, const char *filename,
                            int16_t x,
                            int16_t y, int16_t w, int16_t h);
void gui_progressbar_h_ctor(gui_progressbar_t *this, gui_obj_t *parent, const char *filename,
                            int16_t x,
                            int16_t y, int16_t w, int16_t h);
#if 0
/**
 * @brief create a progressbar widget.
 *
 * @param parent the father widget it nested in.
 * @param filename this progressbar widget's name.
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @param w the width of the widget.
 * @param h the hight of the widget.
 * @return return the widget object pointer.
 *
 */
gui_progressbar_t *gui_progressbar_create(void *parent, const char *filename, int16_t x, int16_t y,
                                          int16_t w, int16_t h);
gui_progressbar_t *gui_progressbar_v_create(void *parent, const char *filename, int16_t x,
                                            int16_t y,
                                            int16_t w, int16_t h);
#endif
gui_progressbar_t *gui_progressbar_picture_create(void *parent, int16_t x, int16_t y,
                                                  int16_t w, int16_t h, void *picture_addr);
/**
 * @brief create a horizontal based on picture progressbar
 *
 * @param parent parent widget
 * @param picture the picture background
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @return gui_progressbar_t*
 */
gui_progressbar_t *gui_progressbar_img_h_create(void *parent, void *picture, int16_t x, int16_t y);

/**
 * @brief create a vertical based on picture progressbar
 *
 * @param parent parent widget
 * @param picture the picture background
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @return gui_progressbar_t*
 */
gui_progressbar_t *gui_progressbar_img_v_create(void *parent, void *picture, int16_t x, int16_t y);
/**
 * @brief create a  based on pictures frame progressbar
 *
 * @param parent parent widget
 * @param picture_array the pictures frame
 * @param array_length picture_array length
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @return gui_progressbar_t*
 */
gui_progressbar_t *gui_progressbar_movie_create(void *parent, void  **picture_array,
                                                uint16_t array_length, int16_t x, int16_t y);
/**
 * @brief set progress
 *
 * @param this
 * @param progress
 */
void gui_progressbar_set_progress(gui_progressbar_t *this, size_t progress);
/**
 * @brief get_progress
 *
 * @param this
 * @return size_t
 */
size_t gui_progressbar_get_progress(gui_progressbar_t *this);
/**
 * @brief set_percentage
 *
 * @param this
 * @param percentage
 */
void gui_progressbar_set_percentage(gui_progressbar_t *this, float percentage);
/**
 * @brief get_percentage
 *
 * @param this
 * @return float
 */
float gui_progressbar_get_percentage(gui_progressbar_t *this);
/**
 * @brief get_max
 *
 * @param this
 * @return size_t
 */
size_t gui_progressbar_get_max(gui_progressbar_t *this);

#ifdef __cplusplus
}
#endif

#endif

