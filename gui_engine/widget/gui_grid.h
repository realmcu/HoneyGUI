/*
 * File      : gui_grid.h
 */
#ifndef __GUI_grid_H__
#define __GUI_grid_H__



#ifdef __cplusplus
extern "C" {
#endif
#include <guidef.h>
#include <gui_fb.h>
#include "gui_obj.h"
enum gui_grid_style
{
    GRID_CLASSIC,
    GRID_SCALE,
};
typedef struct gui_grid
{
    gui_obj_t base;
    int16_t row; int16_t col; uint16_t gap_col; uint16_t gap_row; uint16_t gap_col_scale;
    uint16_t gap_row_scale;
    float scale;
    enum gui_grid_style style;
    void (*ctor)(struct gui_grid *this, gui_obj_t *parent, int16_t x,
                 int16_t y, int16_t row, int16_t col, uint16_t gap_col, uint16_t gap_row);
} gui_grid_t;

/**
 * @brief create a grid widget.
 * @param parent the father widget the grid nested in.
 * @param x the X-axis coordinate.
 * @param x the Y-axis coordinate.
 * @param row Rows number.
 * @param col Columns number.
 * @param gap_col gap between two Columns.
 * @param gap_row gap between two Rows.
 * @return return the widget object pointer
 *
 */
gui_grid_t *gui_grid_create(void *parent,
                            int16_t x,
                            int16_t y,
                            int16_t row,
                            int16_t col,
                            uint16_t gap_col,
                            uint16_t gap_row);

#ifdef __cplusplus
}
#endif

#endif

