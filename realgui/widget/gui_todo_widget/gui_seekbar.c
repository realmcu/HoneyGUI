/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_seekbar.c
  * @brief seekbar widget
  * @details seekbar widget
  * @author luke_sun@realsil.com.cn
  * @date 2023/11/06
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <math.h>
#include <string.h>
#include "gui_seekbar.h"
#include "gui_obj.h"
#include "tp_algo.h"


/*============================================================================*
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                           Constants
 *============================================================================*/


/*============================================================================*
 *                            Macros
 *============================================================================*/


/*============================================================================*
 *                            Variables
 *============================================================================*/

extern void gui_progressbar_ctor(gui_progressbar_t *this, gui_obj_t *parent, const char *name,
                                 int16_t x, int16_t y, int16_t w, int16_t h);
extern void gui_progressbar_v_ctor(gui_progressbar_t *this, gui_obj_t *parent, const char *name,
                                   int16_t x, int16_t y, int16_t w, int16_t h);
extern void gui_progressbar_v_img_ctor(gui_progressbar_t *this, gui_obj_t *parent, void *picture,
                                       int16_t x, int16_t y);
extern void gui_progressbar_h_img_ctor(gui_progressbar_t *this, gui_obj_t *parent, void *picture,
                                       int16_t x, int16_t y);
extern void gui_progressbar_movie_ctor(gui_progressbar_t *this, gui_obj_t *parent,
                                       void **picture_array, uint16_t array_number, int16_t x, int16_t y);
extern void gui_progressbar_h_thumb_ctor(gui_progressbar_t *this,
                                         gui_obj_t         *parent,
                                         void              *picture,
                                         int16_t            x,
                                         int16_t            y,
                                         uint16_t bar_width);

/*============================================================================*
 *                           Private Functions
 *============================================================================*/

static void gui_seekbar_update_att(gui_obj_t *obj)
{
    gui_seekbar_t *this = (void *)obj;
    animate_frame_update(this->animate, obj);
}

static void gui_seekbar_prepare(gui_obj_t *obj)
{
    touch_info_t *tp;
    gui_seekbar_t *circle;

    gui_seekbar_update_att(obj);
    tp = tp_get_info();
    circle = (gui_seekbar_t *)obj;

    if (gui_obj_in_rect(obj, 0, 0, gui_get_screen_width(), gui_get_screen_height()) == true)
    {
        if (tp->type == TOUCH_HOLD_Y)
        {
            if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true)
            {
                int pro = tp->y + tp->deltaY;//todo

                if (pro <= 0)
                {
                    pro = 1;
                }

                if (pro >= obj->h)
                {
                    pro = obj->h;
                }

                if (GET_BASE(circle->base.c)->type == IMAGE_FROM_MEM)
                {
                    pro = pro * (circle->base.max - 2) / obj->h;
                }

                gui_progressbar_set_progress((void *)circle, (circle->base.max - 2) - pro);
            }
        }
    }

    if (gui_obj_in_rect(obj, 0, 0, gui_get_screen_width(), gui_get_screen_height()) == true)
    {
        gui_seekbar_t *b = (void *)obj;

        switch (tp->type)
        {
        case TOUCH_SHORT:
            {
                bool callback = false;

                if (!callback)
                {
                    callback = false;
                }

                for (uint8_t i = 0; i < obj->event_dsc_cnt; i++)
                {
                    gui_event_dsc_t *event_dsc = obj->event_dsc + i;
                    if (event_dsc->filter == GUI_EVENT_TOUCH_CLICKED)
                    {
                        callback = true;
                    }
                }

                if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true)
                {
                    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED);
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

        if (tp->pressed)
        {
            if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true)
            {
                b->press_flag = true;
                //gui_obj_enable_event(obj, GUI_EVENT_1);
            }
        }

        if ((tp->released) && (b->press_flag))
        {
            b->press_flag = false;
            //gui_obj_enable_event(obj, GUI_EVENT_2);
        }

        if (b->press_flag)
        {
            //gui_obj_enable_event(obj, GUI_EVENT_3);
        }
    }
}

static float gui_seekbar_get_gegree_to_center(uint16_t cx,
                                              uint16_t cy,
                                              uint16_t point_x,
                                              uint16_t point_y)
{
    float x, y;

    x = point_x - cx;
    y = point_y - cy;

    if ((x >= 0) && (y < 0))
    {
        return atan2f(x, -y);
    }
    else if ((x > 0) && (y >= 0))
    {
        return M_PI - atan2f(x, y);
    }
    else if ((x <= 0) && (y > 0))
    {
        return M_PI + atan2f(-x, y);
    }
    else if ((x < 0) && (y <= 0))
    {
        return 1.5f * M_PI + atan2f(-y, -x);
    }

    return 0.0f;
}

static void gui_seekbar_prepare_arc(gui_obj_t *obj)
{
    touch_info_t *tp;
    gui_seekbar_t *circle;

    gui_seekbar_update_att(obj);
    tp = tp_get_info();
    circle = (gui_seekbar_t *)obj;

    if (gui_obj_in_rect(obj, 0, 0, gui_get_screen_width(), gui_get_screen_height()) == true)
    {
        if ((tp->type == TOUCH_HOLD_X)
            || (tp->type == TOUCH_HOLD_Y)
            || (tp->pressed))
        {
            if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true)
            {
                //         gui_obj_t *o = obj;
                //         o = obj;
                //         int ax = obj->x, ay = obj->y;
                // while (o->parent != NULL)
                // {
                //     o = o->parent;
                //     ax += o->x;
                //     ay += o->y;
                // }
                float pro = gui_seekbar_get_gegree_to_center(circle->arcx + obj->matrix->m[0][2],
                                                             circle->arcy + obj->matrix->m[1][2], tp->x + tp->deltaX,
                                                             tp->y + tp->deltaY);
                float progress = 0;
                GUI_UNUSED(progress);

                float start = circle->arc_start * M_PI / 180.f;
                float end = circle->arc_end * M_PI / 180.f;

                if (start > end)
                {
                    start = start - 2 * M_PI;
                    if (pro > end)
                    {
                        pro = pro - 2 * M_PI;
                    }

                    if ((pro > end) || (pro < start))
                    {
                        return;
                    }
                    else
                    {
                        progress = (pro - start) / (end - start);
                    }
                }
                else
                {
                    if ((pro > end) || (pro < start))
                    {
                        return;
                    }
                    else
                    {
                        progress = (pro - start) / (end - start);
                    }
                }

                gui_progressbar_set_percentage((void *)circle, progress);
            }
        }
    }

    if (gui_obj_in_rect(obj, 0, 0, gui_get_screen_width(), gui_get_screen_height()) == true)
    {
        gui_seekbar_t *b = (void *)obj;
        switch (tp->type)
        {
        case TOUCH_SHORT:
            {
                bool callback = false;
                if (!callback)
                {
                    callback = false;
                }

                for (uint8_t i = 0; i < obj->event_dsc_cnt; i++)
                {
                    gui_event_dsc_t *event_dsc = obj->event_dsc + i;
                    if (event_dsc->filter == GUI_EVENT_TOUCH_CLICKED)
                    {
                        callback = true;
                    }
                }

                if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true)
                {
                    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED);
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

        if (tp->pressed)
        {
            if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true)
            {
                b->press_flag = true;

                //gui_obj_enable_event(obj, GUI_EVENT_1);
                GUI_ASSERT(0);
            }
        }

        if ((tp->released) && (b->press_flag))
        {
            b->press_flag = false;
            //gui_obj_enable_event(obj, GUI_EVENT_2);
            GUI_ASSERT(0);
        }

        if (b->press_flag)
        {
            //gui_obj_enable_event(obj, GUI_EVENT_3);
        }
    }
    for (uint8_t i = 0; i < obj->event_dsc_cnt; i++)
    {
        gui_event_dsc_t *event_dsc = obj->event_dsc + i;
        GUI_UNUSED(event_dsc);
    }
}

static void gui_seekbar_h_prepare(gui_obj_t *obj)
{
    touch_info_t *tp;
    gui_seekbar_t *circle;

    gui_seekbar_update_att(obj);
    tp = tp_get_info();
    circle = (gui_seekbar_t *)obj;
    static bool event_4;
    if (gui_obj_in_rect(obj, 0, 0, gui_get_screen_width(), gui_get_screen_height()) == true)
    {
        if ((tp->type == TOUCH_HOLD_X)
            || (tp->type == TOUCH_HOLD_Y)
            || (tp->pressed))
        {
            if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true)
            {
                int pro = circle->last_pro + tp->deltaX;
                if (pro <= 0) { pro = 0; }
                if (pro >= (obj->w - circle->base.thumb_width)) { pro = (obj->w - circle->base.thumb_width); }
                if (GET_BASE(circle->base.c)->type == IMAGE_FROM_MEM)
                {
                    pro = pro * circle->base.max / (obj->w - circle->base.thumb_width);
                    gui_log("pro:%d\n", pro);
                }
                gui_progressbar_set_progress((void *)circle, pro);//gui_log("per:%f\n", circle->base.per);
                if (circle->base.per == 1.0f)
                {
                    if (circle->thumb_picture_highlight)
                    {
                        GUI_TYPE(gui_img_t, circle->base.c)->data = (void *)circle->thumb_picture_highlight;
                    }


                    if (!event_4)
                    {
                        //gui_obj_enable_event(obj, GUI_EVENT_4);
                        GUI_ASSERT(0);
                        gui_log("GUI_EVENT_4:\n");
                        event_4 = 1;
                    }

                }
                else
                {
                    if (circle->thumb_picture)
                    {
                        GUI_TYPE(gui_img_t, circle->base.c)->data = (void *)circle->thumb_picture;
                    }

                }


            }
        }
    }

    if (gui_obj_in_rect(obj, 0, 0, gui_get_screen_width(), gui_get_screen_height()) == true)
    {
        gui_seekbar_t *b = (void *)obj;
        switch (tp->type)
        {
        case TOUCH_SHORT:
            {
                bool callback = false;
                if (!callback)
                {
                    callback = false;
                }

                for (uint8_t i = 0; i < obj->event_dsc_cnt; i++)
                {
                    gui_event_dsc_t *event_dsc = obj->event_dsc + i;
                    if (event_dsc->filter == GUI_EVENT_TOUCH_CLICKED)
                    {
                        callback = true;
                    }
                }

                if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true)
                {
                    //gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED);
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

        if (tp->pressed)
        {
            if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true)
            {
                b->press_flag = true;
                //gui_obj_enable_event(obj, GUI_EVENT_1);  ////gui_log("%d\n", __LINE__);
                event_4 = 0;
            }
        }

        if ((tp->released) && (b->press_flag))
        {
            b->press_flag = false;
            //gui_obj_enable_event(obj, GUI_EVENT_2);
            b->last_pro = gui_progressbar_get_progress((void *)b);
        }
        if (b->press_flag)
        {
            //gui_obj_enable_event(obj, GUI_EVENT_3);
        }
    }


}
static void gui_seekbar_h_prepare_double(gui_obj_t *obj)
{
    touch_info_t *tp;
    gui_seekbar_t *circle;

    gui_seekbar_update_att(obj);
    tp = tp_get_info();
    circle = (gui_seekbar_t *)obj;
    static float pro_last;
    if (tp->released)
    {
        pro_last = 0;
    }
    if (gui_obj_in_rect(obj, 0, 0, gui_get_screen_width(), gui_get_screen_height()) == true)
    {

        if (tp->pressed)
        {
            pro_last = circle->base.per;
        }
        if ((tp->type == TOUCH_HOLD_X)
            || (tp->type == TOUCH_HOLD_Y)
            || (tp->pressed))
        {
            if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true)
            {
                float pro;
                int middle = obj->x + obj->w / 2;
                if (tp->x < middle)
                {
                    pro = pro_last + ((float)(tp->deltaX)) / ((float)obj->w * 0.5f);
                }
                else
                {
                    pro = pro_last - ((float)(tp->deltaX)) / ((float)obj->w * 0.5f);
                }


                if (pro <= 0) { pro = 0; }
                if (pro >= 1) { pro = 1; }
                gui_progressbar_set_percentage((void *)circle, pro);
            }
        }
    }

    if (gui_obj_in_rect(obj, 0, 0, gui_get_screen_width(), gui_get_screen_height()) == true)
    {
        gui_seekbar_t *b = (void *)obj;
        switch (tp->type)
        {
        case TOUCH_SHORT:
            {
                bool callback = false;
                if (!callback)
                {
                    callback = false;
                }

                for (uint8_t i = 0; i < obj->event_dsc_cnt; i++)
                {
                    gui_event_dsc_t *event_dsc = obj->event_dsc + i;
                    if (event_dsc->filter == GUI_EVENT_TOUCH_CLICKED)
                    {
                        callback = true;
                    }
                }

                if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true)
                {
                    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED);
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

        if (tp->pressed)
        {
            if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true)
            {
                b->press_flag = true;
                //gui_obj_enable_event(obj, GUI_EVENT_1);  ////gui_log("%d\n", __LINE__);
                GUI_ASSERT(0);
            }
        }

        if ((tp->released) && (b->press_flag))
        {
            b->press_flag = false;
            //gui_obj_enable_event(obj, GUI_EVENT_2);
            GUI_ASSERT(0);
        }
        if (b->press_flag)
        {
            //gui_obj_enable_event(obj, GUI_EVENT_3);
        }
    }
}
static void gui_seekbar_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_seekbar_prepare(obj);
            break;

        default:
            break;
        }
    }
}

static void gui_seekbar_h_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_seekbar_h_prepare(obj);
            break;

        default:
            break;
        }
    }
}
static void gui_seekbar_h_cb_double(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_seekbar_h_prepare_double(obj);
            break;

        default:
            break;
        }
    }
}
static void gui_seekbar_arc_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_seekbar_prepare_arc(obj);
            break;

        default:
            break;
        }
    }
}


void gui_seekbar_ctor_img_v(gui_seekbar_t *this,
                            gui_obj_t     *parent,
                            void          *picture,
                            int16_t        x,
                            int16_t        y)
{
    gui_progressbar_v_img_ctor(&(this->base), parent, picture, x, y);

    this->base.base.type = SEEKBAR;
    GET_BASE(this)->obj_cb = gui_seekbar_cb;
    GET_BASE(this)->has_prepare_cb = true;
}

void gui_seekbar_ctor_img_h(gui_seekbar_t *this,
                            gui_obj_t     *parent,
                            void          *picture,
                            int16_t        x,
                            int16_t        y)
{
    gui_progressbar_h_img_ctor(&(this->base), parent, picture, x, y);

    this->base.base.type = SEEKBAR;
    GET_BASE(this)->obj_cb = gui_seekbar_h_cb;
    GET_BASE(this)->has_prepare_cb = true;
    this->last_pro = this->base.per * this->base.max;
}
void gui_seekbar_ctor_thumb_h(gui_seekbar_t *this,
                              gui_obj_t     *parent,
                              void          *picture,
                              void          *picture_hl,
                              int16_t        x,
                              int16_t        y,
                              uint16_t bar_width)
{
    gui_progressbar_h_thumb_ctor(&(this->base), parent, picture, x, y, bar_width);

    this->base.base.type = SEEKBAR;
    GET_BASE(this)->obj_cb = gui_seekbar_h_cb;
    GET_BASE(this)->has_prepare_cb = true;
    this->thumb_picture = picture;
    this->thumb_picture_highlight = picture_hl;
    //this->last_pro = this->base.per*this->base.max;
}
void gui_seekbar_ctor_movie_h(gui_seekbar_t  *this,
                              gui_obj_t      *parent,
                              void          **picture_array,
                              uint16_t        array_length,
                              int16_t         x,
                              int16_t         y)
{
    gui_progressbar_movie_ctor(&(this->base), parent, picture_array, array_length, x, y);

    this->base.base.type = SEEKBAR;
    GET_BASE(this)->obj_cb = gui_seekbar_h_cb;
    GET_BASE(this)->has_prepare_cb = true;
    uint16_t w = gui_img_get_width((void *)this->base.c);
    uint16_t h = gui_img_get_height((void *)this->base.c);
    GET_BASE(this)->w = w;
    GET_BASE(this)->h = h;
}
static void gui_seekbar_ctor_movie_h_double(gui_seekbar_t  *this,
                                            gui_obj_t      *parent,
                                            void          **picture_array,
                                            uint16_t        array_length,
                                            int16_t         x,
                                            int16_t         y)
{
    gui_progressbar_movie_ctor(&(this->base), parent, picture_array, array_length, x, y);

    this->base.base.type = SEEKBAR;
    GET_BASE(this)->obj_cb = gui_seekbar_h_cb_double;
    GET_BASE(this)->has_prepare_cb = true;
    uint16_t w = gui_img_get_width((void *)this->base.c);
    uint16_t h = gui_img_get_height((void *)this->base.c);
    GET_BASE(this)->w = w;
    GET_BASE(this)->h = h;
}
void gui_seekbar_ctor_movie_v(gui_seekbar_t  *this,
                              gui_obj_t      *parent,
                              void          **picture_array,
                              uint16_t        array_length,
                              int16_t         x,
                              int16_t         y)
{
    gui_progressbar_movie_ctor(&(this->base), parent, picture_array, array_length, x, y);

    this->base.base.type = SEEKBAR;
    GET_BASE(this)->obj_cb = gui_seekbar_cb;
    GET_BASE(this)->has_prepare_cb = true;
    uint16_t w = gui_img_get_width((void *)this->base.c);
    uint16_t h = gui_img_get_height((void *)this->base.c);
    GET_BASE(this)->w = w;
    GET_BASE(this)->h = h;
}

void gui_seekbar_ctor_movie_arc(gui_seekbar_t  *this,
                                gui_obj_t      *parent,
                                void          **picture_array,
                                uint16_t        array_length,
                                int16_t         x,
                                int16_t         y,
                                uint16_t        arc_x,
                                uint16_t        arc_y,
                                uint16_t        arc_r,
                                uint16_t        arc_w,
                                float           arc_start,
                                float           arc_end)
{
    uint16_t w, h;

    gui_progressbar_movie_ctor(&(this->base), parent, picture_array, array_length, x, y);

    this->arc_end = arc_end;
    this->arc_r = arc_r;
    this->arc_start = arc_start;
    this->arc_w = arc_w;
    this->arcx = arc_x;
    this->arcy = arc_y;
    this->base.base.type = SEEKBAR;

    GET_BASE(this)->obj_cb = gui_seekbar_arc_cb;
    GET_BASE(this)->has_prepare_cb = true;
    w = gui_img_get_width((void *)this->base.c);
    h = gui_img_get_height((void *)this->base.c);
    GET_BASE(this)->w = w;
    GET_BASE(this)->h = h;
}

void gui_seekbar_ctor(gui_seekbar_t *this,
                      gui_obj_t     *parent,
                      const char    *filename,
                      int16_t        x,
                      int16_t        y,
                      int16_t        w,
                      int16_t        h)
{
    this->base.ctor = gui_progressbar_v_ctor;
    this->base.ctor(&(this->base), parent, filename, x, y, w, h);
    this->base.base.type = SEEKBAR;
    GET_BASE(this)->obj_cb = gui_seekbar_cb;
    GET_BASE(this)->has_prepare_cb = true;
}

void gui_seekbar_h_ctor(gui_seekbar_t *this,
                        gui_obj_t     *parent,
                        const char    *filename,
                        int16_t        x,
                        int16_t        y,
                        int16_t        w,
                        int16_t        h)
{
    this->base.ctor = gui_progressbar_ctor;
    this->base.ctor(&(this->base), parent, filename, x, y, w, h);
    this->base.base.type = SEEKBAR;
    GET_BASE(this)->obj_cb = gui_seekbar_h_cb;
    GET_BASE(this)->has_prepare_cb = true;
}
static void gui_seekbar_set_animate(gui_seekbar_t *this, uint32_t dur, int repeat_count,
                                    void *callback,
                                    void *p)
{
    GUI_SET_ANIMATE_HELPER
}
static void on_change(gui_seekbar_t *this, gui_event_cb_t function, void *param)
{
    //gui_obj_add_event_cb(this, function, GUI_EVENT_5, param);
    GUI_ASSERT(0);// send message
}
//_GUI_API_ASSIGN(gui_seekbar_t)
_gui_api_gui_seekbar_t _gui_api_for_gui_seekbar_t =
{
    .animate = gui_seekbar_set_animate,
    .get_progress = (float(*)(gui_seekbar_t *))gui_progressbar_get_percentage,
    .set_progress = (void (*)(gui_seekbar_t *, float))gui_progressbar_set_percentage,
    .on_change = on_change,
};
/*============================================================================*
 *                           Public Functions
 *============================================================================*/
gui_seekbar_t *gui_seekbar_create_img_v(void       *parent,
                                        const char *name,
                                        int16_t     x,
                                        int16_t     y)
{
    gui_seekbar_t *this = gui_malloc(sizeof(gui_seekbar_t));
    memset(this, 0, sizeof(gui_seekbar_t));
    gui_seekbar_ctor_img_v(this, parent, (void *)name, x, y);
    ((gui_obj_t *)this)->create_done = 1;

    return this;
}

gui_seekbar_t *gui_seekbar_create_img_h(void       *parent,
                                        const char *name,
                                        int16_t     x,
                                        int16_t     y)
{
    gui_seekbar_t *this = gui_malloc(sizeof(gui_seekbar_t));
    memset(this, 0, sizeof(gui_seekbar_t));
    gui_seekbar_ctor_img_h(this, parent, (void *)name, x, y);
    ((gui_obj_t *)this)->create_done = 1;

    return this;
}

gui_seekbar_t *gui_seekbar_create_movie_h(void      *parent,
                                          void     **picture_array,
                                          uint16_t   array_length,
                                          int16_t    x,
                                          int16_t    y)
{
    gui_seekbar_t *this = gui_malloc(sizeof(gui_seekbar_t));
    memset(this, 0, sizeof(gui_seekbar_t));
    gui_seekbar_ctor_movie_h(this, parent, picture_array, array_length, x, y);
    ((gui_obj_t *)this)->create_done = 1;

    return this;
}
gui_seekbar_t *gui_seekbar_create_movie_h_double(void      *parent,
                                                 void     **picture_array,
                                                 uint16_t   array_length,
                                                 int16_t    x,
                                                 int16_t    y)
{
    gui_seekbar_t *this = gui_malloc(sizeof(gui_seekbar_t));
    memset(this, 0, sizeof(gui_seekbar_t));
    gui_seekbar_ctor_movie_h_double(this, parent, picture_array, array_length, x, y);
    ((gui_obj_t *)this)->create_done = 1;

    return this;
}
gui_seekbar_t *gui_seekbar_create_movie_v(void      *parent,
                                          void     **picture_array,
                                          uint16_t   array_length,
                                          int16_t    x,
                                          int16_t    y)
{
    gui_seekbar_t *this = gui_malloc(sizeof(gui_seekbar_t));
    memset(this, 0, sizeof(gui_seekbar_t));
    gui_seekbar_ctor_movie_v(this, parent, picture_array, array_length, x, y);
    ((gui_obj_t *)this)->create_done = 1;

    return this;
}

gui_seekbar_t *gui_seekbar_create_movie_arc(void      *parent,
                                            void     **picture_array,
                                            uint16_t   array_length,
                                            int16_t    x,
                                            int16_t    y,
                                            uint16_t   arc_x,
                                            uint16_t   arc_y,
                                            uint16_t   arc_r,
                                            uint16_t   arc_w,
                                            float      arc_start,
                                            float      arc_end)
{
    gui_seekbar_t *this = gui_malloc(sizeof(gui_seekbar_t));
    memset(this, 0, sizeof(gui_seekbar_t));
    gui_seekbar_ctor_movie_arc(this, parent, picture_array, array_length, x, y,
                               arc_x, arc_y,  arc_r,  arc_w, arc_start, arc_end);
    ((gui_obj_t *)this)->create_done = 1;

    return this;
}
gui_seekbar_t *gui_seekbar_create_thumb_h(void    *parent,
                                          void    *picture_pointer,
                                          void    *picture_highlight_pointer,
                                          int16_t  x,
                                          int16_t  y,
                                          uint16_t bar_width)
{
    gui_seekbar_t *this = gui_malloc(sizeof(gui_seekbar_t));
    memset(this, 0, sizeof(gui_seekbar_t));
    gui_seekbar_ctor_thumb_h(this, parent, picture_pointer, picture_highlight_pointer, x, y, bar_width);
    ((gui_obj_t *)this)->create_done = 1;

    return this;
}

















