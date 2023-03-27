#include "gui_ring.h"
static void (ring_ctor)(gui_ring_t *this, gui_obj_t *parent, const char *name, int16_t center_x,
                        int16_t center_y, int16_t radius, uint16_t width, uint32_t color)
{
    this->base_canvas.ctor = gui_canvas_ctor;
    this->base_canvas.ctor((void *)this, parent, name, center_x - radius - width,
                           center_y - radius - width, (radius + width) * 2, (radius + width) * 2, color);
    this->base_canvas.draw_ring((void *)this, radius + width, radius + width, radius, width);
}
gui_ring_t *gui_ring_create(void *parent, const char *name, int16_t center_x, int16_t center_y,
                            int16_t radius, uint16_t width, uint32_t color)
{
#define _gui_ring_create_param this, parent, name, center_x, center_y, radius, width, color
    GUI_NEW(gui_ring_t, ring_ctor, _gui_ring_create_param);
}
