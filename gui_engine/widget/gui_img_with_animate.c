#include "gui_img_with_animate.h"
void gui_img_with_animate_set_animate(gui_img_with_animate_t *o, uint32_t dur, int repeatCount,
                                      void *callback,
                                      void *p)
{
    gui_img_with_animate_t *a = (o);
    GUI_UNUSED(a);

    gui_animate_t *animate = ((gui_img_with_animate_t *)o)->animate;
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
    ((gui_img_with_animate_t *)o)->animate = animate;
}
static void (obj_update_att)(struct _gui_obj_t *o)
{
    gui_img_with_animate_t *obj = (void *)o;
    if (obj->animate && obj->animate->animate)
    {
        size_t frame_count = obj->animate->dur * 30 / (1000);
        obj->animate->callback(obj->animate->p);
        obj->animate->current_frame++;

        if (obj->animate->current_frame > frame_count)
        {
            if (obj->animate->repeatCount == 0)
            {
                obj->animate->animate = false;
            }
            else if (obj->animate->repeatCount < 0)
            {
                obj->animate->current_frame = 0;
            }
            else if (obj->animate->repeatCount > 0)
            {
                obj->animate->current_repeat_count++;
                if (obj->animate->current_repeat_count >= obj->animate->repeatCount)
                {
                    obj->animate->animate = false;
                }
                else
                {
                    obj->animate->current_frame = 0;
                }
            }
        }
        obj->animate->progress_percent = ((float)(obj->animate->current_frame)) / ((float)(
                                                                                       frame_count));

    }
}

static void img_with_animate_ctor(gui_img_with_animate_t *this, void *parent,
                                  void *resourse_address, int16_t x, int16_t y)
{
    gui_img_from_mem_ctor((void *)this, parent, "_default_img_with_animate", resourse_address, x, y,
                          0, 0);
    GET_BASE(this)->obj_update_att = obj_update_att;
    this->base.draw_img.blend_mode = IMG_BYPASS_MODE;
}
gui_img_with_animate_t *gui_img_with_animate_create(void *parent, void *resourse_address, int16_t x,
                                                    int16_t y)
{
#define _IMG_WITH_ANIMATE_CTOR_PARAM_ this, parent, resourse_address, x, y
    GUI_NEW(gui_img_with_animate_t, img_with_animate_ctor, _IMG_WITH_ANIMATE_CTOR_PARAM_)
}
