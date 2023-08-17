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
    GRID_3D
};
typedef struct gui_grid
{
    gui_obj_t base;
    int16_t row; int16_t col; uint16_t gap_col; uint16_t gap_row; uint16_t gap_col_scale;
    uint16_t gap_row_scale;
    float scale;
    int yold;
    int start_x;
    int start_y;
    int id_offset;
    int row_count;
    bool row_count_flag;
    int col_count;
    bool col_count_flag;
    int change_threshold;
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
/**
 * @brief config grid style
 *
 * @param grid grid wodget pointer
 * @param style GRID_CLASSIC, GRID_SCALE, GRID_3D
 */
void gui_grid_style(gui_grid_t *grid, enum gui_grid_style style);
#ifdef __cplusplus
}
#endif

#endif

