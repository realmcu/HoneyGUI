#include "gui_chart_bars.h"
static void (chart_bars_write)(gui_chart_bars_t *this, int bars_in_row_count, int bars_in_col_count,
                               int data[])
{
    this->base_canvas.erase((void *)this);
    for (size_t i = 0; i < bars_in_col_count; i++)
    {
        for (size_t j = 0; j < bars_in_row_count; j++)
        {
            if (data[(bars_in_row_count * i + j) * 2 + 1] > 0)
            {
                int x, y, h;
                x = this->gap + i * (this->gap + this->line_w);
                y = this->gap + data[(bars_in_row_count * i + j) * 2];
                //w = this->line_w;
                h = data[(bars_in_row_count * i + j) * 2 + 1] - data[(bars_in_row_count * i + j) * 2];
                this->base_canvas.draw_bar_v((void *)this, x, y, this->line_w, h);
            }
        }
    }
}
static void (set_line_width)(gui_chart_bars_t *this, int width)
{
    this->line_w = width;
}
static void (chart_bars_ctor)(gui_chart_bars_t *this,
                              gui_obj_t *parent, int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color)
{
    this->base_canvas.ctor = gui_canvas_ctor;
    this->base_canvas.ctor((void *)this, parent, "_chart_bars", x, y, w, h, color);
    this->gap = 5;
    this->line_w = 10;
    this->bar_count = 24;

    this->write = chart_bars_write;
    this->set_line_width = set_line_width;
    for (size_t i = 0; i < this->bar_count; i++)
    {
        int x, y, hh;
        x = this->gap + i * (this->gap + this->line_w);
        y = this->gap;
        hh = h - this->gap * 2;
        this->base_canvas.draw_bar_v((void *)this, x, y, this->line_w, hh);
    }

}
gui_chart_bars_t *gui_chart_bars_create(void *parent, int16_t x, int16_t y, int16_t w, int16_t h,
                                        uint32_t color)
{
    gui_chart_bars_t *this = gui_malloc(sizeof(gui_chart_bars_t));
    memset(this, 0, sizeof(gui_chart_bars_t));
    this->ctor = chart_bars_ctor;
    this->ctor(this, parent, x, y, w, h, color);
    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list), &(GET_BASE(this)->brother_list));
    }
    this->back = gui_round_corner_rect_create(this, "_chart_bars_back", 0, 0, w, h, this->line_w,
                                              color);
    GET_BASE(this)->create_done = true;
    return this;
}

