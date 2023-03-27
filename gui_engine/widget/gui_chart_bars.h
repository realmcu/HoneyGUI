/*
 * File      : gui_chart_bars.h
 */
#ifndef __GUI_CHART_BARS_H__
#define __GUI_CHART_BARS_H__




#ifdef __cplusplus
extern "C" {
#endif
#include <guidef.h>
#include "gui_canvas.h"
#include "gui_round_corner_rect.h"
typedef struct gui_chart_bars gui_chart_bars_t;
struct gui_chart_bars
{
    struct gui_canvas base_canvas;
    gui_round_corner_rect_t *back;
    int gap;
    int line_w;
    int bar_count;
    int *data;
    int data_size;
    void (*set_color)(gui_chart_bars_t *this, uint32_t color);
    void (*write)(gui_chart_bars_t *this, int bars_in_row_count, int bars_in_col_count, int data[]);
    void (*set_line_width)(gui_chart_bars_t *this, int width);
    void (*ctor)(gui_chart_bars_t *this,
                 gui_obj_t *parent, int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color);
};
gui_chart_bars_t *gui_chart_bars_create(void *parent, int16_t x, int16_t y, int16_t w, int16_t h,
                                        uint32_t color);

#ifdef __cplusplus
}
#endif

#endif
