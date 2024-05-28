/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_switch.c
  * @brief on or off status. Click to change.
  * @details listen to on and off gesture
  * @author triton_yu@realsil.com.cn
  * @date 2023/11/8
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include "gui_switch.h"
#include "gui_server.h"
#include "gui_obj.h"
#include "gui_win.h"
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
void gui_switch_change_state(gui_switch_t *this, bool ifon)
{
    this->ifon = ifon;
    if (this->ifon)
    {
        gui_img_set_attribute(this->switch_picture, this->switch_picture->base.name, this->on_pic_addr, 0,
                              0);
    }
    else
    {
        gui_img_set_attribute(this->switch_picture, this->switch_picture->base.name, this->off_pic_addr, 0,
                              0);
    }
}

void gui_switch_turn_on(gui_switch_t *this)
{
    gui_switch_change_state(this, true);

    if (this->ifon)
    {
        gui_obj_event_set((void *)this, GUI_EVENT_1);
    }
    else if (!this->ifon)
    {
        gui_obj_event_set((void *)this, GUI_EVENT_2);
    }
}

void gui_switch_turn_off(gui_switch_t *this)
{
    gui_switch_change_state(this, false);

    if (this->ifon)
    {
        gui_obj_event_set((void *)this, GUI_EVENT_1);
    }
    else if (!this->ifon)
    {
        gui_obj_event_set((void *)this, GUI_EVENT_2);
    }
}

void gui_switch_is_on(gui_switch_t *this)
{
    gui_switch_change_state(this, true);
}

void gui_switch_is_off(gui_switch_t *this)
{
    gui_switch_change_state(this, false);
}

static void gui_switch_on(gui_switch_t *b, void *callback, void *parameter)
{
    gui_obj_add_event_cb(b, (gui_event_cb_t)callback, GUI_EVENT_1, parameter);
}

static void gui_switch_off(gui_switch_t *b, void *callback, void *parameter)
{
    gui_obj_add_event_cb(b, (gui_event_cb_t)callback, GUI_EVENT_2, parameter);
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
static void gui_switch_hl(gui_switch_t *sw)
{
    if (sw->ifon)
    {
        gui_img_set_attribute(sw->switch_picture, sw->switch_picture->base.name, sw->on_hl_pic_addr, 0, 0);
    }
    else
    {
        gui_img_set_attribute(sw->switch_picture, sw->switch_picture->base.name, sw->off_hl_pic_addr, 0, 0);
    }
}

static void gui_switch_hl_back(gui_switch_t *sw)
{
    if (sw->ifon)
    {
        gui_img_set_attribute(sw->switch_picture, sw->switch_picture->base.name, sw->on_pic_addr, 0, 0);
    }
    else
    {
        gui_img_set_attribute(sw->switch_picture, sw->switch_picture->base.name, sw->off_pic_addr, 0, 0);
    }
}

static void gui_switch_long_touch(gui_switch_t *sw)
{
    gui_img_set_attribute(sw->switch_picture, sw->switch_picture->base.name,
                          sw->long_touch_state_pic_addr, 0, 0);
}

static void gui_switch_long_touch_back(gui_switch_t *sw)
{
    if (sw->ifon)
    {
        gui_img_set_attribute(sw->switch_picture, sw->switch_picture->base.name, sw->on_pic_addr, 0, 0);
    }
    else
    {
        gui_img_set_attribute(sw->switch_picture, sw->switch_picture->base.name, sw->off_pic_addr, 0, 0);
    }
}

static void gui_switch_hl_long_touch(gui_switch_t *sw)
{
    gui_img_set_attribute(sw->switch_picture, sw->switch_picture->base.name,
                          sw->long_touch_state_hl_pic_addr, 0, 0);
}

void gui_tree_disable_widget_gesture_by_type(gui_obj_t *obj, int type)
{
    gui_list_t *node = NULL;
    gui_obj_t *o;

    gui_list_for_each(node, &obj->child_list)
    {
        o = gui_list_entry(node, gui_obj_t, brother_list);
        if (o->type == type)
        {
            gui_obj_event_set(o, (gui_event_t)0);

        }
        gui_tree_disable_widget_gesture_by_type(o, type);
    }
}

static void gui_switch_prepare(gui_obj_t *obj)
{
    gui_switch_t *this = (gui_switch_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    uint8_t last = this->checksum;

    if (obj->not_show)
    {
        last = this->checksum;
        this->checksum = 0;
        this->checksum = gui_obj_checksum(0, (uint8_t *)this, sizeof(gui_img_t));

        if (last != this->checksum)
        {
            gui_fb_change();
        }
        return;
    }

    if (gui_obj_in_rect(obj, 0, 0, gui_get_screen_width(), gui_get_screen_height()) == true)
    {
        if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true)
        {
            switch (tp->type)
            {
            case TOUCH_SHORT:
                {
                }
                break;
            case TOUCH_UP_SLIDE:
                {
                }
                break;
            case TOUCH_DOWN_SLIDE:
                {
                }
                break;
            case TOUCH_LEFT_SLIDE:
                {
                }
                break;
            case TOUCH_RIGHT_SLIDE:
                {
                }
                break;
            case TOUCH_LONG:
                {
                    if (this->long_flag == false)
                    {
                        if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true)
                        {
                            this->long_flag = true;
                            if (this->long_touch_enable)
                            {
                                gui_log("TOUCH_LONG b->long_touch_state = %d\n", this->long_touch_state);

                                this->long_touch_state = !this->long_touch_state;
                                gui_obj_event_set(obj, GUI_EVENT_TOUCH_LONG);
                                gui_switch_hl_back(this);

                                if (this->long_touch_state)
                                {
                                    gui_switch_long_touch(this);
                                }
                                else
                                {
                                    gui_switch_long_touch_back(this);
                                }
                            }
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
                    //gui_log("pressed type 0x%x\n", tp->type);
                    gui_obj_event_set(obj, GUI_EVENT_TOUCH_PRESSED);

                    this->long_flag = false;
                    this->press_flag = true;

                    if (!this->long_touch_state)
                    {
                        gui_switch_hl(this);
                    }
                    else
                    {
                        gui_switch_hl_long_touch(this);
                    }
                }
            }
            //pic can change back if slide happens on pic
            if (tp->pressing)
            {
                if (tp->deltaX != 0 || tp->deltaY != 0 || tp->type == TOUCH_HOLD_X ||
                    tp->type == TOUCH_HOLD_Y)
                {
                    gui_switch_hl_back(this);
                }
            }
            if (this->release_flag)
            {
                if ((tp->type != TOUCH_HOLD_Y) && (tp->type != TOUCH_ORIGIN_FROM_Y))
                {
                    if (this->long_touch_state)
                    {
                        gui_switch_long_touch(this);
                    }
                    else if ((!this->long_touch_state && !this->long_flag) || (!this->long_touch_enable))
                    {
                        if (((tp->deltaX == 0) && (tp->deltaY == 0)) &&
                            (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true))
                        {
                            this->ifon = !(this->ifon);
                            gui_switch_change_state(this, this->ifon);
                            gui_log("sw:0x%x %d\n", this, this->ifon);
                            if (this->ifon)
                            {
                                gui_obj_event_set(obj, GUI_EVENT_1);
                            }
                            else if (!this->ifon)
                            {
                                gui_obj_event_set(obj, GUI_EVENT_2);
                            }
                            gui_tree_disable_widget_gesture_by_type(&(gui_current_app()->screen), WINDOW);
                        }
                    }
                }
                this->press_flag = false;
                this->release_flag = false;
                this->long_flag = false;
            }
            if (tp->released && this->press_flag)
            {
                this->release_flag = true;
            }
            if (this->touch_disable)
            {
                gui_obj_event_set(obj, (gui_event_t)0);
            }
        }
    }
    this->checksum = 0;
    this->checksum = gui_obj_checksum(0, (uint8_t *)this, sizeof(gui_img_t));
    if (last != this->checksum)
    {
        gui_fb_change();
    }
}
static void gui_switch_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_switch_prepare(obj);
            break;

        default:
            break;
        }
    }
}

void gui_switch_ctor(gui_switch_t *this,
                     gui_obj_t    *parent,
                     int16_t       x,
                     int16_t       y,
                     int16_t       w,
                     int16_t       h,
                     void         *off_pic,
                     void         *on_pic)
{
    gui_obj_ctor(&(this->base), parent, "switch", x, y, w, h);

    this->base.type = CLICKSWITCH;
    GET_BASE(this)->obj_cb = gui_switch_cb;
    GET_BASE(this)->has_prepare_cb = true;
    this->off_pic_addr = off_pic;
    this->on_pic_addr = on_pic;
    this->off_hl_pic_addr = this->off_pic_addr;
    this->on_hl_pic_addr = this->on_pic_addr;
    this->long_touch_state_pic_addr = this->off_pic_addr;
    this->long_touch_state_hl_pic_addr = this->off_pic_addr;
}

static gui_switch_t *gui_switch_create_core(void                 *parent,
                                            int16_t               x,
                                            int16_t               y,
                                            int16_t               w,
                                            int16_t               h,
                                            void                 *off_pic,
                                            void                 *on_pic,
                                            IMG_SOURCE_MODE_TYPE  src_mode)
{
    gui_switch_t *this = gui_malloc(sizeof(gui_switch_t));

    memset(this, 0, sizeof(gui_switch_t));
    this->src_mode = src_mode;
    gui_switch_ctor(this, parent, x, y, w, h, off_pic, on_pic);

    gui_list_init(&(((gui_obj_t *)this)->child_list));
    if ((((gui_obj_t *)this)->parent) != ((void *)0))
    {
        gui_list_insert_before(&((((gui_obj_t *)this)->parent)->child_list),
                               &(((gui_obj_t *)this)->brother_list));
    }

    if (src_mode == IMG_SRC_FILESYS)
    {
        this->switch_picture = (void *)gui_img_create_from_fs(this, "switch_picture", off_pic, 0, 0, 0, 0);
    }
    else if (src_mode == IMG_SRC_MEMADDR)
    {
        this->switch_picture = (void *)gui_img_create_from_mem(this, "switch_picture", off_pic, 0, 0, 0, 0);
    }

    ((gui_obj_t *)this)->create_done = 1;

    return this;
}

gui_switch_t *gui_switch_create(void    *parent,
                                int16_t  x,
                                int16_t  y,
                                int16_t  w,
                                int16_t  h,
                                void    *off_pic,
                                void    *on_pic)
{
    return gui_switch_create_core(parent, x, y, w, h, off_pic, on_pic, IMG_SRC_MEMADDR);
}

gui_switch_t *gui_switch_create_from_fs(void    *parent,
                                        int16_t  x,
                                        int16_t  y,
                                        int16_t  w,
                                        int16_t  h,
                                        void    *off_pic,
                                        void    *on_pic)
{
    return gui_switch_create_core(parent, x, y, w, h, off_pic, on_pic, IMG_SRC_FILESYS);
}
_GUI_API_ASSIGN(gui_switch_t)
.ctor = gui_switch_ctor,
 .on_turn_off = gui_switch_off,
  .on_turn_on = gui_switch_on,
   .turn_off = gui_switch_turn_off,
    .turn_on = gui_switch_turn_on,
};
/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
