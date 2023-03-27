#include "gui_bitmap.h"
void gui_bitmap_ctor(gui_bitmap_t *this, gui_obj_t *parent, const char *name, uint8_t *bitmap,
                     int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color)
{
    gui_img_from_mem_ctor(&(this->base), parent, name, bitmap, x, y, w, h);
    this->color = color;
}
gui_bitmap_t *gui_bitmap_create(void *parent, const char *name, uint8_t *bitmap, int16_t x,
                                int16_t y,
                                int16_t w, int16_t h, uint32_t color)
{
#define _GUI_NEW_param_gui_bitmap_create_ this, parent, name, bitmap, x, y, w, h, color
    GUI_NEW(gui_bitmap_t, gui_bitmap_ctor, _GUI_NEW_param_gui_bitmap_create_)
}
