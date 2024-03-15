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
static void gui_button_update_att(gui_obj_t *this)
{
    T_GUI_BUTTON *obj = (void *)this;
    size_t frame_count;

    if (obj->animate && obj->animate->animate)
    {
        frame_count = obj->animate->dur * (1000 / 15) / (1000);
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

static void gui_button_prepare(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    T_GUI_BUTTON *this = (void *)obj;

    gui_button_update_att(obj);

    if (GUI_TYPE(T_GUI_BUTTON, obj)->flag == true)
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
            if (gui_point_in_obj_rect(obj, tp->x, tp->y) == true)
            {
                gui_obj_event_set(obj, GUI_EVENT_TOUCH_CLICKED);
            }
        }
        break;

    case TOUCH_LONG:
        {
            if (this->long_flag == false)
            {
                if (gui_point_in_obj_rect(obj, tp->x, tp->y) == true)
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
        if (gui_point_in_obj_rect(obj, tp->x, tp->y) == true)
        {
            if (this->on_pic_addr && this->style == 0)
            {
                gui_imgconfig_t config =
                {
                    .src_mode = this->src_mode,
                    .addr = this->on_pic_addr,
                    .x = this->img->base.x,
                    .y = this->img->base.y,
                };
                gui_img_set_config(this->img, &config);
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
            gui_imgconfig_t config =
            {
                .src_mode = this->src_mode,
                .addr = this->off_pic_addr,
                .x = this->img->base.x,
                .y = this->img->base.y,
            };
            gui_img_set_config(this->img, &config);
        }

        gui_obj_event_set(obj, GUI_EVENT_TOUCH_RELEASED);
        this->long_flag = false;
    }

    if (tp->released && this->press_flag)
    {
        this->release_flag = true;
    }
}

static void gui_button_ctor(T_GUI_BUTTON *this,
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
    root->obj_prepare = gui_button_prepare;

    // for self
    this->off_pic_addr = background_pic;
    this->on_pic_addr = highlight_pic;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void gui_button_text_move(T_GUI_BUTTON *this, int16_t text_x, int16_t text_y)
{
    if (this->text)
    {
        this->text->base.x = text_x;
        this->text->base.y = text_y;
    }
}

void gui_button_text_color(T_GUI_BUTTON *this, gui_color_t color)
{
    if (this->text)
    {
        this->text->color = color;
    }
}

void gui_button_img_move(T_GUI_BUTTON *this, int16_t img_x, int16_t img_y)
{
    if (this->img)
    {
        this->img->base.x = img_x;
        this->img->base.y = img_y;
    }
}

void gui_button_click(T_GUI_BUTTON *this, gui_event_cb_t event_cb, void *parameter)
{
    gui_obj_add_event_cb(this, event_cb, GUI_EVENT_TOUCH_CLICKED, parameter);
}

void gui_button_press(T_GUI_BUTTON *this, gui_event_cb_t event_cb, void *parameter)
{
    gui_obj_add_event_cb(this, event_cb, GUI_EVENT_TOUCH_PRESSED, parameter);
}

void gui_button_long(T_GUI_BUTTON *this, gui_event_cb_t event_cb, void *parameter)
{
    gui_obj_add_event_cb(this, event_cb, GUI_EVENT_TOUCH_LONG, parameter);
}

void gui_button_release(T_GUI_BUTTON *this, gui_event_cb_t event_cb, void *parameter)
{
    gui_obj_add_event_cb(this, event_cb, GUI_EVENT_TOUCH_RELEASED, parameter);
}

void gui_button_set_animate(T_GUI_BUTTON *this,
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
    animate->callback = (void (*)(void *))callback;
    animate->repeatCount = repeat_count;
    animate->p = p;
    this->animate = animate;
}

T_GUI_BUTTON *gui_button_create(void                 *parent,
                                int16_t               x,
                                int16_t               y,
                                int16_t               w,
                                int16_t               h,
                                void                 *background_pic,
                                void                 *highlight_pic,
                                char                 *text,
                                T_IMG_SOURCE_IMG_TYPE image_type,
                                int                   count,
                                IMG_SOURCE_MODE_TYPE  src_mode)
{
    GUI_ASSERT(parent != NULL);

    T_GUI_BUTTON *button = gui_malloc(sizeof(T_GUI_BUTTON));
    GUI_ASSERT(button != NULL);
    memset(button, 0, sizeof(T_GUI_BUTTON));

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
        case IMG_SRC_ICON:
            {
                if (src_mode == IMG_SRC_FILESYS)
                {
                    button->img = (void *)gui_img_create_from_fs(button,
                                                                 background_pic,
                                                                 0,
                                                                 0);
                }
                else if (src_mode == IMG_SRC_MEMADDR)
                {
                    button->img = (void *)gui_img_create_from_mem(button,
                                                                  "icon_img",
                                                                  background_pic,
                                                                  0,
                                                                  0,
                                                                  0,
                                                                  0);
                }
            }
            break;

        case IMG_SRC_LVG:
            {
                if (src_mode == IMG_SRC_FILESYS)
                {
                    button->img = (void *)gui_img_create_from_fs((void *)button,
                                                                 ((void **)background_pic)[0],
                                                                 0,
                                                                 0);
                }
                else if (src_mode == IMG_SRC_MEMADDR)
                {
                    button->img = (void *)gui_img_create_from_mem((void *)button,
                                                                  "g",
                                                                  ((void **)background_pic)[0],
                                                                  0,
                                                                  0,
                                                                  0,
                                                                  0);
                }
            }
            break;

        case IMG_SRC_CANVAS:
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

        button->text = gui_text_create(button, "icon_text", 0, button->img->draw_img.img_h + 40,
                                       gui_get_screen_width(), h);
        gui_text_set(button->text, text, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                     strlen(text), 32);
        gui_text_mode_set(button->text, LEFT);
    }

    GET_BASE(button)->create_done = true;
    return button;
}

/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
