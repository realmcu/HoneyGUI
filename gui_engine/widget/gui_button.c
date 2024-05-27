/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file  gui_button.c
  * @brief button widget
  * @details click to trigger
  * @author triton_yu@realsil.com.cn
  * @date 2023/10/17
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "gui_button.h"
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


/** End of WIDGET_Exported_Variables
  * @}
  */

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
/** @defgroup WIDGET_Exported_Functions WIDGET Exported Functions
  * @{
  */
static void gui_button_update_att(gui_obj_t *obj)
{
    gui_button_t *this = (void *)obj;
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

static void gui_button_prepare(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    gui_button_t *this = (void *)obj;

    gui_button_update_att(obj);

    if (this->flag == true)
    {
        gui_obj_event_set(obj, GUI_EVENT_5);
    }

    if (gui_obj_in_rect(obj, 0, 0, gui_get_screen_width(), gui_get_screen_height()) == false)
    {
        return;
    }

    switch (tp->type)
    {
    case TOUCH_SHORT:
        {
            if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true)
            {
                gui_obj_event_set(obj, GUI_EVENT_TOUCH_CLICKED);
            }
        }
        break;

    case TOUCH_LONG:
        {
            if (this->long_flag == false)
            {
                if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true)
                {
                    this->long_flag = true;
                    gui_obj_event_set(obj, GUI_EVENT_TOUCH_LONG);
                }
            }
        }
        break;

    default:
        break;
    }

    if (tp->pressed)
    {
        if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true)
        {
            if (this->on_pic_addr && this->style == 0)
            {
                gui_img_set_attribute(this->img, this->img->base.name, this->on_pic_addr, this->img->base.x,
                                      this->img->base.y);
            }

            gui_obj_event_set(obj, GUI_EVENT_TOUCH_PRESSED);

            this->long_flag = false;
            this->press_flag = true;
        }
    }

    if (this->release_flag)
    {
        this->press_flag = false;
        this->release_flag = false;

        if (this->off_pic_addr && this->style == 0)
        {
            gui_img_set_attribute(this->img, this->img->base.name, this->off_pic_addr, this->img->base.x,
                                  this->img->base.y);
        }

        gui_obj_event_set(obj, GUI_EVENT_TOUCH_RELEASED);

        this->long_flag = false;
    }

    if (tp->released && this->press_flag)
    {
        this->release_flag = true;
    }
}

static void gui_button_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_button_prepare(obj);
            break;

        default:
            break;
        }
    }
}

static void gui_button_ctor(gui_button_t *this,
                            gui_obj_t    *parent,
                            int16_t       x,
                            int16_t       y,
                            int16_t       w,
                            int16_t       h,
                            void         *background_pic,
                            void         *highlight_pic,
                            char         *text)
{
    gui_obj_t *base = (gui_obj_t *)this;
    gui_obj_t *root = (gui_obj_t *)this;

    //for base class
    gui_obj_ctor(base, parent, "DEFAULT", x, y, w, h);

    //for root class
    root->type = BUTTON;
    root->obj_cb = gui_button_cb;
    root->has_prepare_cb = true;

    // for self
    this->off_pic_addr = background_pic;
    this->on_pic_addr = highlight_pic;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void gui_button_text_move(gui_button_t *this, int16_t text_x, int16_t text_y)
{
    if (this->text)
    {
        this->text->base.x = text_x;
        this->text->base.y = text_y;
    }
}

void gui_button_text_color(gui_button_t *this, gui_color_t color)
{
    if (this->text)
    {
        this->text->color = color;
    }
}

void gui_button_img_move(gui_button_t *this, int16_t img_x, int16_t img_y)
{
    if (this->img)
    {
        this->img->base.x = img_x;
        this->img->base.y = img_y;
    }
}

void gui_button_click(gui_button_t *this, gui_event_cb_t event_cb, void *parameter)
{
    gui_obj_add_event_cb(this, event_cb, GUI_EVENT_TOUCH_CLICKED, parameter);
}

void gui_button_press(gui_button_t *this, gui_event_cb_t event_cb, void *parameter)
{
    gui_obj_add_event_cb(this, event_cb, GUI_EVENT_TOUCH_PRESSED, parameter);
}

void gui_button_long(gui_button_t *this, gui_event_cb_t event_cb, void *parameter)
{
    gui_obj_add_event_cb(this, event_cb, GUI_EVENT_TOUCH_LONG, parameter);
}

void gui_button_release(gui_button_t *this, gui_event_cb_t event_cb, void *parameter)
{
    gui_obj_add_event_cb(this, event_cb, GUI_EVENT_TOUCH_RELEASED, parameter);
}

void gui_button_set_animate(gui_button_t *this,
                            uint32_t      dur,
                            int           repeat_count,
                            void         *callback,
                            void         *p)
{
    gui_animate_t *animate = this->animate;

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
    this->animate = animate;
}


static gui_button_t *gui_button_create_core(void                 *parent,
                                            int16_t               x,
                                            int16_t               y,
                                            int16_t               w,
                                            int16_t               h,
                                            void                 *background_pic,
                                            void                 *highlight_pic,
                                            char                 *text,
                                            char                  image_type,
                                            int                   count,
                                            IMG_SOURCE_MODE_TYPE  src_mode)
{
    GUI_ASSERT(parent != NULL);

    gui_button_t *button = gui_malloc(sizeof(gui_button_t));
    GUI_ASSERT(button != NULL);
    memset(button, 0, sizeof(gui_button_t));

    button->src_mode = src_mode;
    gui_button_ctor(button, parent, x, y, w, h, background_pic, highlight_pic, text);

    gui_list_init(&(GET_BASE(button)->child_list));
    if ((GET_BASE(button)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(button)->parent)->child_list),
                               &(GET_BASE(button)->brother_list));
    }

    if (background_pic)
    {
        switch (image_type)
        {
        case BUTTON_BG_ICON:
            {
                if (src_mode == IMG_SRC_FILESYS)
                {
                    button->img = (void *)gui_img_create_from_fs(button, "icon_img", background_pic, 0, 0, 0, 0);
                }
                else if (src_mode == IMG_SRC_MEMADDR)
                {
                    button->img = (void *)gui_img_create_from_mem(button, "icon_img", background_pic, 0, 0, 0, 0);
                }
            }
            break;

        case BUTTON_BG_DYNAMIC:
            {
                if (src_mode == IMG_SRC_FILESYS)
                {
                    button->img = (void *)gui_img_create_from_fs((void *)button, "g", ((void **)background_pic)[0], 0,
                                                                 0, 0, 0);;
                }
                else if (src_mode == IMG_SRC_MEMADDR)
                {
                    button->img = (void *)gui_img_create_from_mem((void *)button, "g", ((void **)background_pic)[0], 0,
                                                                  0, 0, 0);
                }
            }
            break;

        case BUTTON_BG_SVG:
            {
                //button->img = (void *)gui_svg_create_from_mem((void *)button, background_pic, count, 0, 0, w, h);
            }
            break;

        default:
            break;
        }
    }

    if (text)
    {
        gui_img_get_height(button->img);

        button->text = gui_text_create(button, "icon_text", 0, gui_img_get_height(button->img) + 40,
                                       gui_get_screen_width(), h);
        gui_text_set(button->text, text, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                     strlen(text), 32);
        gui_text_mode_set(button->text, LEFT);
    }

    GET_BASE(button)->create_done = true;
    return button;
}

gui_button_t *gui_button_create(void             *parent,
                                int16_t           x,
                                int16_t           y,
                                int16_t           w,
                                int16_t           h,
                                void             *background_pic,
                                void             *highlight_pic,
                                char             *text,
                                T_BUTTON_BG_TYPE  image_type,
                                int               count)
{
    return gui_button_create_core(parent, x, y, w, h, background_pic, highlight_pic, text, image_type,
                                  count, IMG_SRC_MEMADDR);
}

gui_button_t *gui_button_create_from_fs(void             *parent,
                                        int16_t           x,
                                        int16_t           y,
                                        int16_t           w,
                                        int16_t           h,
                                        void             *background_pic,
                                        void             *highlight_pic,
                                        char             *text,
                                        T_BUTTON_BG_TYPE  image_type,
                                        int               count)
{
    return gui_button_create_core(parent, x, y, w, h, background_pic, highlight_pic, text, image_type,
                                  count, IMG_SRC_FILESYS);
}
_GUI_API_ASSIGN(gui_button_t)
.on_click = gui_button_click,
 .on_long_press = gui_button_long,
  .on_press = gui_button_press,
   .on_release = gui_button_release,

};
/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
