#include "gui_arc_hl.h"
static void set_animate(gui_arc_hl_t *o, uint32_t dur, int repeatCount, void *callback, void *p)
{
    gui_animate_t *animate = ((gui_arc_hl_t *)o)->animate;
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
    ((gui_arc_hl_t *)o)->animate = animate;
}
static void (arc_hl_ctor)(gui_arc_hl_t *this,
                          gui_obj_t *parent,
                          const char *name,
                          int16_t center_x,
                          int16_t center_y,
                          int16_t radius,
                          uint16_t width,
                          uint32_t color,
                          uint32_t background_color,
                          int start_degree,
                          int end_degree,
                          bool round)
{
    gui_obj_ctor((void *)this, parent, name, center_x - radius - width, center_y - radius - width,
                 radius * 2,
                 radius * 2);
    this->set_animate = set_animate;
}
gui_arc_hl_t *gui_arc_highlight_create(
    void *parent,
    const char *name,
    int16_t center_x,
    int16_t center_y,
    int16_t radius,
    uint16_t width,
    uint32_t color,
    uint32_t background_color,
    int start_degree,
    int end_degree,
    bool round
)
{
    gui_arc_hl_t *this = gui_malloc(sizeof(gui_arc_hl_t));
    memset(this, 0, sizeof(gui_arc_hl_t));
    this->ctor = arc_hl_ctor;
    this->ctor(this, parent, name, center_x, center_y, radius, width, color, background_color,
               start_degree, end_degree, round);
    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list), &(GET_BASE(this)->brother_list));
    }
    this->background = gui_ring_create(this, "_backarc", radius + width, radius + width, radius, width,
                                       background_color);
    this->highlight = gui_arc_create(this, "_hl_arc", radius + width, radius + width, radius, width,
                                     color, start_degree, end_degree, round);
    GET_BASE(this)->create_done = true;
    return this;
}
