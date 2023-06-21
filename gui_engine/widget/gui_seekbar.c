/*
 * File      : gui_tabview.c
 * This file is part of GUI Engine
 */

#include <guidef.h>
#include <gui_seekbar.h>
#include <string.h>
#include <gui_server.h>
#include <gui_obj.h>
#include <tp_algo.h>
static void seekbar_preapre(gui_obj_t *obj)
{
    GUI_RENDER_DATA
    gui_seekbar_t *circle = (gui_seekbar_t *)obj;
    //gui_seekbar_t *b =  circle;
    //circle->slider.slider_circle->set((gui_circle_t *)circle, circle->base.get_progress(&(circle->base))+obj->x, (int16_t)(circle->slider.slider_circle->circle.center.y));
    if ((obj->dx < (int)gui_get_screen_width()) && ((obj->dx + obj->w) >= 0) && \
        (obj->dy < (int)gui_get_screen_height()) && ((obj->dy + obj->h) >= 0))
    {

        if (tp->type == TOUCH_HOLD_X || tp->type == TOUCH_HOLD_Y || tp->pressed)
        {
            if ((tp->x >= obj->dx && tp->x <= (obj->dx + obj->w)) && (tp->y >= obj->dy &&
                                                                      tp->y <= (obj->dy + obj->h)))
            {
                int pro = tp->y + tp->deltaY - obj->dy;
                if (pro <= 0) { pro = 1; }
                if (pro >= obj->h) { pro = obj->h; }
                gui_progressbar_api.set_progress((void *)circle, obj->h - pro);
            }
        }


    }
    if ((obj->dx < (int)gui_get_screen_width()) && ((obj->dx + obj->w) >= 0) && \
        (obj->dy < (int)gui_get_screen_height()) && ((obj->dy + obj->h) >= 0))
    {
        if (tp->type != 271)
        {
            ////gui_log("type2:%d,%d\n", tp->type, tp->released);
        }
        gui_seekbar_t *b = (void *)obj;
        switch (tp->type)
        {
        case TOUCH_SHORT:
            {
                //////gui_log("%s\n", "TOUCH_SHORT");
                //
                bool callback = false;
                if (callback) { callback = false; }
                for (uint32_t i = 0; i < obj->event_dsc_cnt; i++)
                {
                    gui_event_dsc_t *event_dsc = obj->event_dsc + i;
                    if (event_dsc->filter == GUI_EVENT_TOUCH_CLICKED)
                    {
                        callback = true;
                    }
                } ////gui_log("%d\n", __LINE__);
                //if (callback)
                {
                    ////gui_log("%d\n", __LINE__);
                    if ((tp->x >= obj->dx && tp->x <= (obj->dx + obj->w)) &&
                        (tp->y >= obj->dy && tp->y <= (obj->dy + obj->h)))
                    {
                        ////gui_log("%d\n", __LINE__);
                        gui_obj_event_set(obj, GUI_EVENT_TOUCH_CLICKED);
                    }
                }
            }
            break;
        case TOUCH_LONG:
            {

            }
            break;

        default:
            break;
        }


        {
            if (tp->pressed)
            {


                if ((tp->x >= obj->dx && tp->x <= (obj->dx + obj->w)) &&
                    (tp->y >= obj->dy && tp->y <= (obj->dy + obj->h)))
                {
                    b->press_flag = true;
                    //gui_send_callback_p_to_server(b->press_cb, b->press_cb_p);

                    ////gui_log("%d\n", __LINE__);
                    gui_obj_event_set(obj, GUI_EVENT_1);  ////gui_log("%d\n", __LINE__);

                }
            }

            if (tp->released)
            {

                {


                    {
                        b->press_flag = false;
                        gui_obj_event_set(obj, GUI_EVENT_2);
                    }  ////gui_log("%d\n", __LINE__);

                }
            }
            if (b->press_flag)
            {
                gui_obj_event_set(obj, GUI_EVENT_3);
            }


        }
    }
    obj->cover = ((gui_seekbar_t *)obj)->hit_slider;

}

static void seekbar_h_preapre(gui_obj_t *obj)
{
    GUI_RENDER_DATA
    gui_seekbar_t *circle = (gui_seekbar_t *)obj;
    //gui_seekbar_t *b =  circle;
    //circle->slider.slider_circle->set((gui_circle_t *)circle, circle->base.get_progress(&(circle->base))+obj->x, (int16_t)(circle->slider.slider_circle->circle.center.y));
    if ((obj->dx < (int)gui_get_screen_width()) && ((obj->dx + obj->w) >= 0) && \
        (obj->dy < (int)gui_get_screen_height()) && ((obj->dy + obj->h) >= 0))
    {

        if (tp->type == TOUCH_HOLD_X || tp->type == TOUCH_HOLD_Y || tp->pressed)
        {
            if ((tp->x >= obj->dx && tp->x <= (obj->dx + obj->w)) && (tp->y >= obj->dy &&
                                                                      tp->y <= (obj->dy + obj->h)))
            {
                int pro = tp->x + tp->deltaX - obj->dx;
                if (pro <= 0) { pro = 1; }
                if (pro >= obj->w) { pro = obj->w; }
                gui_progressbar_api.set_progress((void *)circle, pro);
            }
        }
    }
    if ((obj->dx < (int)gui_get_screen_width()) && ((obj->dx + obj->w) >= 0) && \
        (obj->dy < (int)gui_get_screen_height()) && ((obj->dy + obj->h) >= 0))
    {
        if (tp->type != 271)
        {
            ////gui_log("type2:%d,%d\n", tp->type, tp->released);
        }
        gui_seekbar_t *b = (void *)obj;
        switch (tp->type)
        {
        case TOUCH_SHORT:
            {
                //////gui_log("%s\n", "TOUCH_SHORT");
                //
                bool callback = false;
                if (callback) { callback = false; }
                for (uint32_t i = 0; i < obj->event_dsc_cnt; i++)
                {
                    gui_event_dsc_t *event_dsc = obj->event_dsc + i;
                    if (event_dsc->filter == GUI_EVENT_TOUCH_CLICKED)
                    {
                        callback = true;
                    }
                } ////gui_log("%d\n", __LINE__);
                //if (callback)
                {
                    ////gui_log("%d\n", __LINE__);
                    if ((tp->x >= obj->dx && tp->x <= (obj->dx + obj->w)) &&
                        (tp->y >= obj->dy && tp->y <= (obj->dy + obj->h)))
                    {
                        ////gui_log("%d\n", __LINE__);
                        gui_obj_event_set(obj, GUI_EVENT_TOUCH_CLICKED);
                    }
                }
            }
            break;
        case TOUCH_LONG:
            {

            }
            break;

        default:
            break;
        }


        {
            if (tp->pressed)
            {


                if ((tp->x >= obj->dx && tp->x <= (obj->dx + obj->w)) &&
                    (tp->y >= obj->dy && tp->y <= (obj->dy + obj->h)))
                {
                    b->press_flag = true;
                    //gui_send_callback_p_to_server(b->press_cb, b->press_cb_p);

                    ////gui_log("%d\n", __LINE__);
                    gui_obj_event_set(obj, GUI_EVENT_1);  ////gui_log("%d\n", __LINE__);

                }
            }

            if (tp->released)
            {

                {


                    {
                        b->press_flag = false;
                        gui_obj_event_set(obj, GUI_EVENT_2);
                    }  ////gui_log("%d\n", __LINE__);

                }
            }
            if (b->press_flag)
            {
                gui_obj_event_set(obj, GUI_EVENT_3);
            }


        }
    }

    obj->cover = ((gui_seekbar_t *)obj)->hit_slider;

}
static void (onPress)(gui_seekbar_t *b, void *callback, void *parameter)
{
    gui_obj_add_event_cb(b, (gui_event_cb_t)callback, GUI_EVENT_1, parameter);
}
static void (onRelease)(gui_seekbar_t *b, void *callback, void *parameter)
{
    gui_obj_add_event_cb(b, (gui_event_cb_t)callback, GUI_EVENT_2, parameter);
}
static void (onPressing)(gui_seekbar_t *b, void *callback, void *parameter)
{
    gui_obj_add_event_cb(b, (gui_event_cb_t)callback, GUI_EVENT_3, parameter);
}
void gui_seekbar_ctor(gui_seekbar_t *this, gui_obj_t *parent, const char *filename, int16_t x,
                      int16_t y, int16_t w, int16_t h)
{
    this->base.ctor = gui_progressbar_v_ctor;
    this->base.ctor(&(this->base), parent, filename, x, y, w, h);
    //this->slider.slider_circle = gui_circle_create(this, "slider", this->base.get_progress(&(this->base)),
    //this->base.base.y+this->base.max_rectangle->base.base.base.h/2, this->base.max_rectangle->base.base.base.h/2, 0x0f0f);
    this->base.base.type = SEEKBAR;
    GET_BASE(this)->obj_prepare = seekbar_preapre;
    //gui_get_render_api_table()[GET_BASE(this)->type].obj_update_att = seekbar_update_att;
}
void gui_seekbar_h_ctor(gui_seekbar_t *this, gui_obj_t *parent, const char *filename, int16_t x,
                        int16_t y, int16_t w, int16_t h)
{
    this->base.ctor = gui_progressbar_ctor;
    this->base.ctor(&(this->base), parent, filename, x, y, w, h);
    //this->slider.slider_circle = gui_circle_create(this, "slider", this->base.get_progress(&(this->base)),
    //this->base.base.y+this->base.max_rectangle->base.base.base.h/2, this->base.max_rectangle->base.base.base.h/2, 0x0f0f);
    this->base.base.type = SEEKBAR;
    GET_BASE(this)->obj_prepare = seekbar_h_preapre;
    //gui_get_render_api_table()[GET_BASE(this)->type].obj_update_att = seekbar_update_att;
}
gui_api_seekbar_t gui_seekbar_api =
{
    .onPress = onPress,
    .onRelease = onRelease,
    .onPressing = onPressing,
};
gui_seekbar_t *gui_seekbar_create(void *parent, const char *filename, int16_t x, int16_t y,
                                  int16_t w, int16_t h)
{
//#define _param_gui_seekbar_t this, parent, filename, x,y,w,h
//GUI_NEW(gui_seekbar_t, gui_seekbar_ctor, _param_gui_seekbar_t)

    gui_seekbar_t *this = gui_malloc(sizeof(gui_seekbar_t));
    memset(this, 0, sizeof(gui_seekbar_t));
    gui_seekbar_ctor(this, parent, filename, x, y, w, h);
//gui_list_init(&(((gui_obj_t *)this)->child_list));
//if ((((gui_obj_t *)this)->parent) != ((void *)0))
//{ gui_list_insert_before(&((((gui_obj_t *)this)->parent)->child_list), &(((gui_obj_t *)this)->brother_list)); }
    ((gui_obj_t *)this)->create_done = 1;
    return this;
}
gui_seekbar_t *gui_seekbar_h_create(void *parent, const char *filename, int16_t x, int16_t y,
                                    int16_t w, int16_t h)
{
///#define _param_gui_seekbar_t this, parent, filename, x,y,w,h
//GUI_NEW(gui_seekbar_t, gui_seekbar_h_ctor, _param_gui_seekbar_t)
    gui_seekbar_t *this = gui_malloc(sizeof(gui_seekbar_t));
    memset(this, 0, sizeof(gui_seekbar_t));
    gui_seekbar_h_ctor(this, parent, filename, x, y, w, h);
//gui_list_init(&(((gui_obj_t *)this)->child_list));
//if ((((gui_obj_t *)this)->parent) != ((void *)0))
//{ gui_list_insert_before(&((((gui_obj_t *)this)->parent)->child_list), &(((gui_obj_t *)this)->brother_list)); }
    ((gui_obj_t *)this)->create_done = 1;
    return this;
}
