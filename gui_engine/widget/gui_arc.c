#include "gui_arc.h"
static void arc_set(gui_arc_t *this, uint32_t color, int start_degree, int end_degree, bool round)
{
    this->base_canvas.erase((void *)this);
    this->base_canvas.draw_arc((void *)this, this->radius + this->width, this->radius + this->width,
                               this->radius, this->width, start_degree, end_degree, round);
    this->base_canvas.base.color = color;
}
static void set_animate(gui_arc_t *o, uint32_t dur, int repeatCount, void *callback, void *p)
{
    gui_animate_t *animate = ((gui_arc_t *)o)->animate;
    if (!(animate))
    {
        animate = gui_malloc(sizeof(gui_animate_t));
    }
    memset((animate), 0, sizeof(gui_animate_t));
    animate->animate = true;
    animate->dur = dur;
    animate->callback = (void (*)(void *))callback;
    animate->repeatCount = repeatCount;
    animate->p = p;
    ((gui_arc_t *)o)->animate = animate;
}
static void (arc_ctor)(gui_arc_t *this, gui_obj_t *parent, const char *name, int16_t center_x,
                       int16_t center_y, int16_t radius, uint16_t width, uint32_t color, int start_degree, int end_degree,
                       bool round)
{
    this->base_canvas.ctor = gui_canvas_ctor;
    this->base_canvas.ctor((void *)this, parent, name, center_x - radius - width,
                           center_y - radius - width, (radius + width) * 2, (radius + width) * 2, color);
    this->base_canvas.draw_arc((void *)this, radius + width, radius + width, radius, width,
                               start_degree, end_degree, round);
    this->set = arc_set;
    this->radius = radius;
    this->width = width;
    this->set_animate = set_animate;
}
gui_arc_t *gui_arc_create(
    void *parent,
    const char *name,
    int16_t center_x,
    int16_t center_y,
    int16_t radius,
    uint16_t width,
    uint32_t color,
    int start_degree,
    int end_degree,
    bool round
)
{
#define _gui_arc_create_param this, parent, name, center_x, center_y, radius, width, color, start_degree, end_degree, round
    GUI_NEW(gui_arc_t, arc_ctor, _gui_arc_create_param);
}

