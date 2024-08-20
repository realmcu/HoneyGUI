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
#include "gui_server.h"
#include "gui_obj.h"
#include "tp_algo.h"


/** @defgroup WIDGET WIDGET
  * @{
  */
/*============================================================================*
 *                           Types
 *============================================================================*/
/** @defgroup WIDGET_Exported_Types WIDGET Exported Types
  * @{
  */

/** @brief  ... */

/** End of WIDGET_Exported_Types
  * @}
  */

/*============================================================================*
 *                           Constants
 *============================================================================*/
/** @defgroup WIDGET_Exported_Constants WIDGET Exported Constants
  * @{
  */

/** End of WIDGET_Exported_Constants
  * @}
  */

/*============================================================================*
 *                            Macros
 *============================================================================*/
/** @defgroup WIDGET_Exported_Macros WIDGET Exported Macros
  * @{
  */


/** End of WIDGET_Exported_Macros
  * @}
  */
/*============================================================================*
 *                            Variables
 *============================================================================*/
/** @defgroup WIDGET_Exported_Variables WIDGET Exported Variables
  * @{
  */
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

/** End of WIDGET_Exported_Variables
  * @}
  */

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
/** @defgroup WIDGET_Exported_Functions WIDGET Exported Functions
  * @{
  */
static void gui_seekbar_update_att(gui_obj_t *obj)
{
    gui_seekbar_t *this = (void *)obj;
    uint32_t cur_time_gap;
    if (this->animate && this->animate->animate)
    {
        this->animate->Beginning_frame = 0;
        this->animate->end_frame = 0;
        if (this->animate->progress_percent == 0 && !this->animate->init)
        {
            this->animate->init = 1;
            this->animate->init_time_ms = gui_ms_get();
            this->animate->Beginning_frame = 1;
        }

        this->animate->cur_time_ms = gui_ms_get();
        cur_time_gap = this->animate->cur_time_ms - this->animate->init_time_ms;

        if (this->animate->repeat_count == 0)
        {
            this->animate->progress_percent = (float)(cur_time_gap % this->animate->dur) /
                                              (float)this->animate->dur;
            if (cur_time_gap / this->animate->dur >= 1)
            {
                this->animate->end_frame = 1;
                this->animate->progress_percent = 1;
                this->animate->animate = 0;
            }
            this->animate->callback(this->animate->p, this);

        }
        else if (this->animate->repeat_count == -1)
        {
            uint32_t  round_count = cur_time_gap / this->animate->dur;
            if (round_count > this->animate->last_round)
            {
                this->animate->Beginning_frame = 1;
            }
            this->animate->last_round = round_count;
            this->animate->progress_percent = (float)(cur_time_gap % this->animate->dur) /
                                              (float)this->animate->dur;
            this->animate->callback(this->animate->p, this);
        }
        else
        {
            uint32_t  round_count = cur_time_gap / this->animate->dur;
            if (round_count > this->animate->repeat_count)
            {
                this->animate->animate = 0;
                return;
            }

            if (round_count > this->animate->last_round)
            {
                this->animate->Beginning_frame = 1;
            }
            this->animate->last_round = round_count;
            this->animate->progress_percent = (float)(cur_time_gap % this->animate->dur) /
                                              (float)this->animate->dur;
            this->animate->callback(this->animate->p, this);
        }
    }
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
                    gui_obj_event_set(obj, GUI_EVENT_TOUCH_CLICKED);
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
                gui_obj_event_set(obj, GUI_EVENT_1);
            }
        }

        if ((tp->released) && (b->press_flag))
        {
            b->press_flag = false;
            gui_obj_event_set(obj, GUI_EVENT_2);
        }

        if (b->press_flag)
        {
            gui_obj_event_set(obj, GUI_EVENT_3);
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
                float pro = gui_seekbar_get_gegree_to_center(circle->arcx + 0, circle->arcy + 0, tp->x + tp->deltaX,
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
                    gui_obj_event_set(obj, GUI_EVENT_TOUCH_CLICKED);
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

                gui_obj_event_set(obj, GUI_EVENT_1);
            }
        }

        if ((tp->released) && (b->press_flag))
        {
            b->press_flag = false;
            gui_obj_event_set(obj, GUI_EVENT_2);
        }

        if (b->press_flag)
        {
            gui_obj_event_set(obj, GUI_EVENT_3);
        }
    }
}

static void gui_seekbar_h_prepare(gui_obj_t *obj)
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
                int pro = tp->x + tp->deltaX - obj->x;
                if (pro <= 0) { pro = 1; }
                if (pro >= obj->w) { pro = obj->w; }
                if (GET_BASE(circle->base.c)->type == IMAGE_FROM_MEM)
                {
                    pro = pro * circle->base.max / obj->w;
                    gui_log("pro:%d\n", pro);
                }
                gui_progressbar_set_progress((void *)circle, pro);
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
                    gui_obj_event_set(obj, GUI_EVENT_TOUCH_CLICKED);
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
                gui_obj_event_set(obj, GUI_EVENT_1);  ////gui_log("%d\n", __LINE__);
            }
        }

        if ((tp->released) && (b->press_flag))
        {
            b->press_flag = false;
            gui_obj_event_set(obj, GUI_EVENT_2);
        }
        if (b->press_flag)
        {
            gui_obj_event_set(obj, GUI_EVENT_3);
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
                    gui_obj_event_set(obj, GUI_EVENT_TOUCH_CLICKED);
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
                gui_obj_event_set(obj, GUI_EVENT_1);  ////gui_log("%d\n", __LINE__);
            }
        }

        if ((tp->released) && (b->press_flag))
        {
            b->press_flag = false;
            gui_obj_event_set(obj, GUI_EVENT_2);
        }
        if (b->press_flag)
        {
            gui_obj_event_set(obj, GUI_EVENT_3);
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
static void gui_seekbar_set_animate(gui_seekbar_t *o, uint32_t dur, int repeat_count,
                                    void *callback,
                                    void *p)
{
    gui_animate_t *animate = ((gui_seekbar_t *)o)->animate;
    if (!(animate))
    {
        animate = gui_malloc(sizeof(gui_animate_t));
    }
    memset((animate), 0, sizeof(gui_animate_t));
    animate->animate = true;
    animate->dur = dur;
    animate->callback = (void (*)(void *, void *))callback;
    animate->repeat_count = repeat_count;
    animate->p = p;
    ((gui_seekbar_t *)o)->animate = animate;
}
_GUI_API_ASSIGN(gui_seekbar_t)
.animate = gui_seekbar_set_animate,
 .get_progress = (float(*)(gui_seekbar_t *))gui_progressbar_get_percentage,
  .set_progress = (void (*)(gui_seekbar_t *, float))gui_progressbar_set_percentage,
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

/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
