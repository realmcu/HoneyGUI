/*
 * File      : gui_tabview.c
 * This file is part of GUI Engine
 */

#include <guidef.h>
#include <gui_switch.h>
#include <string.h>
#include <gui_server.h>
#include <gui_obj.h>
#include <gui_win.h>
#include "tp_algo.h"
void gui_switch_change_switch(gui_switch_t *sw)
{

    //sw->off->base.not_show = (sw->ifon);
    //sw->on->base.not_show = !(sw->ifon);
    if (sw->ifon)
    {
        gui_img_set_attribute(sw->switch_picture, 0, sw->on_pic_addr, 0, 0);
    }
    else
    {
        gui_img_set_attribute(sw->switch_picture, 0, sw->off_pic_addr, 0, 0);
    }
    //gui_app_exec(gui_current_app());


}
static void gui_switch_hl(gui_switch_t *sw)
{

    //sw->off->base.not_show = (sw->ifon);
    //sw->on->base.not_show = !(sw->ifon);
    if (sw->ifon)
    {
        gui_img_set_attribute(sw->switch_picture, 0, sw->on_hl_pic_addr, 0, 0);
    }
    else
    {
        gui_img_set_attribute(sw->switch_picture, 0, sw->off_hl_pic_addr, 0, 0);
    }
    //gui_app_exec(gui_current_app());


}
void gui_tree_disable_widget_gesture_by_type(gui_obj_t *obj, int type)
{
    gui_list_t *node = NULL;
    gui_list_for_each(node, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        //gui_log("obj name = %s, name =%s\n", obj->name, name);
        if (obj->type == type)
        {
            gui_obj_event_set(obj, (gui_event_t)0);

        }
        gui_tree_disable_widget_gesture_by_type(obj, type);
    }
}
static void switch_prepare(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    // gui_log("switch_prepare\n");
    if (((obj->ax + obj->tx) < (int)gui_get_screen_width()) && (((obj->ax + obj->tx) + obj->w) >= 0) &&
        \
        ((obj->ay + obj->ty) < (int)gui_get_screen_height()) && (((obj->ay + obj->ty) + obj->h) >= 0))
    {

        /*if (((tp->type == TOUCH_SHORT) &&
             ((tp->x >= obj->ax && tp->x <= (obj->ax + obj->w)) && (tp->y >= obj->ay &&
                                                                    tp->y <= (obj->ay + obj->h)))))
        {
            //gui_log("switch_prepare2\n");
            gui_switch_t *sw = (gui_switch_t *)obj;
            sw->ifon = !(sw->ifon);
            gui_switch_change_switch(sw);//gui_log("switch_prepare3\n");
            if (sw->ifon)
            {
                //gui_log("switch_prepare4\n");
                gui_obj_event_set(obj, GUI_EVENT_1);
            }
            else if (!sw->ifon)
            {
                //gui_log("switch_prepare5\n");
                gui_obj_event_set(obj, GUI_EVENT_2);
            }

        }*/
        if (
            ((tp->x >= (obj->ax + obj->tx) && tp->x <= ((obj->ax + obj->tx) + obj->w)) &&
             (tp->y >= (obj->ay + obj->ty) &&
              tp->y <= ((obj->ay + obj->ty) + obj->h))))
        {
            gui_switch_t *b = (void *)obj;
            switch (tp->type)
            {
            case TOUCH_SHORT:
                {
                    gui_log("Touch_short,%d,%d,%d,%f,%d\n", tp->x, (obj->ax + obj->tx), obj->dx, obj->sx, obj->tx);
                    if ((tp->x >= (obj->ax + obj->tx) && tp->x <= ((obj->ax + obj->tx) + obj->w)) &&
                        (tp->y >= (obj->ay + obj->ty) && tp->y <= ((obj->ay + obj->ty) + obj->h)))
                    {
                        gui_log("%d\n", __LINE__);
                        gui_switch_t *sw = (gui_switch_t *)obj;
                        sw->ifon = !(sw->ifon);
                        gui_switch_change_switch(sw);//gui_log("switch_prepare3\n");
                        if (sw->ifon)
                        {
                            //gui_log("switch_prepare4\n");
                            gui_obj_event_set(obj, GUI_EVENT_1);
                        }
                        else if (!sw->ifon)
                        {
                            //gui_log("switch_prepare5\n");
                            gui_obj_event_set(obj, GUI_EVENT_2);
                        }
                        gui_tree_disable_widget_gesture_by_type(&(gui_current_app()->screen), WINDOW);
                    }

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
                    if (b->long_flag == false)
                    {

                        //if (b->long_click_cb)
                        {
                            if ((tp->x >= (obj->ax + obj->tx) && tp->x <= ((obj->ax + obj->tx) + obj->w)) &&
                                (tp->y >= (obj->ay + obj->ty) && tp->y <= ((obj->ay + obj->ty) + obj->h)))
                            {
                                b->long_flag = true;
                                //gui_send_callback_p_to_server(b->long_click_cb, b->long_click_cb_p);
                                // gui_obj_event_set(obj, GUI_EVENT_TOUCH_LONG);
                            }

                        }
                    }
                }
                break;

            default:
                break;
            }


            {
                if (tp->pressed)
                {

                    if ((tp->x >= (obj->ax + obj->tx) && tp->x <= ((obj->ax + obj->tx) + obj->w)) &&
                        (tp->y >= (obj->ay + obj->ty) && tp->y <= ((obj->ay + obj->ty) + obj->h)))
                    {

                        //gui_send_callback_p_to_server(b->press_cb, b->press_cb_p);

                        //gui_log("%d\n", __LINE__);
                        //gui_obj_event_set(obj, GUI_EVENT_TOUCH_PRESSED);  //gui_log("%d\n", __LINE__);
                        b->long_flag = false;
                        b->press_flag = true;
                        gui_switch_hl(b);
                    }
                }

                if (b->release_flag)
                {

                    {
                        b->press_flag = false;
                        b->release_flag = false;
                        //gui_send_callback_p_to_server(b->press_cb, b->press_cb_p);
                        //gui_log("%d\n", __LINE__);

                        //if (callback)
                        {
                            //gui_obj_event_set(obj, GUI_EVENT_TOUCH_RELEASED);
                        }  //gui_log("%d\n", __LINE__);

                        b->long_flag = false;
////gui_log("%s\n", "TOUCH_SHORT");
                        //

                        //if (callback)
                        {
                            //gui_log("%d\n", __LINE__);

                        }
                    }
                }
                if (tp->released && b->press_flag)
                {
                    b->release_flag = true;
                }
            }

        }
    }
}
/*static void switch_prepare2(gui_obj_t *obj, struct gui_dispdev *dc, struct rtgui_rect *rect,
                           touch_info_t *tp)
{   if (!(((gui_switch_t *)obj)->ifon))
    if ((obj->ax < (int)gui_get_screen_width()) && ((obj->ax + obj->w) >= 0) && \
        (obj->ay < (int)gui_get_screen_height()) && ((obj->ay + obj->h) >= 0))
    {gui_log("1");
        if (obj->callback.link_cb != NULL)
        {
            if (((tp->type == TOUCH_SHORT && obj->callback.cb_type == SHORT_TOUCH_CB) &&
                 ((tp->x >= obj->ax && tp->x <= (obj->ax + obj->w)) && (tp->y >= obj->ay &&
                                                                        tp->y <= (obj->ay + obj->h)))))
            {
                rtgui_msg_t msg;
                msg.type = GUI_SRV_CB;
                msg.cb = obj->callback.link_cb;
                msg.u.payload = obj->callback.param;
                send_msg_to_gui_server(&msg);
            }
        }gui_log("2");
        if (((tp->type == TOUCH_SHORT && obj->callback.cb_type == SHORT_TOUCH_CB) &&
             ((tp->x >= obj->ax && tp->x <= (obj->ax + obj->w)) && (tp->y >= obj->ay &&
                                                                    tp->y <= (obj->ay + obj->h)))))
        {
            gui_switch_t *sw = (gui_switch_t *)obj;
            sw->ifon = !(sw->ifon);
            //send_msg_to_gui_server(&msg);
            gui_switch_change_switch(sw);gui_log("3");
            if (sw->cb_on.link_cb && sw->ifon)
            {
                rtgui_msg_t msg;
                msg.type = GUI_SRV_CB;
                msg.cb = sw->cb_on.link_cb;
                msg.u.payload = sw->cb_on.param;gui_log("4");
                send_msg_to_gui_server(&msg);gui_log("5");
            }
            else if(sw->cb_off.link_cb && !sw->ifon)
            {
                rtgui_msg_t msg;
                msg.type = GUI_SRV_CB;
                msg.cb = sw->cb_off.link_cb;
                msg.u.payload = sw->cb_off.param;gui_log("6");
                send_msg_to_gui_server(&msg);gui_log("7");
            }

        }
    }
}*/
static void sw_turn_on(gui_switch_t *this)
{
    this->ifon = true;
    {

        gui_switch_change_switch(this);

        if (this->ifon)
        {
            gui_obj_event_set((void *)this, GUI_EVENT_1);
        }
        else if (!this->ifon)
        {
            gui_obj_event_set((void *)this, GUI_EVENT_2);
        }

    }
}
static void sw_turn_off(gui_switch_t *this)
{
    this->ifon = false;
    {

        gui_switch_change_switch(this);

        if (this->ifon)
        {
            gui_obj_event_set((void *)this, GUI_EVENT_1);
        }
        else if (!this->ifon)
        {
            gui_obj_event_set((void *)this, GUI_EVENT_2);
        }

    }
}
/*static void sw_turn_off2(gui_switch_t *this)
{
    this->ifon = false;
    {
        gui_switch_t *sw = (gui_switch_t *)this;
        gui_switch_change_switch(this);
        if (sw->cb_on.link_cb && sw->ifon)
        {
            rtgui_msg_t msg;
            msg.type = GUI_SRV_CB;
            msg.cb = sw->cb_on.link_cb;
            msg.u.payload = sw->cb_on.param;
            send_msg_to_gui_server(&msg);
        }
        else if(sw->cb_off.link_cb && !sw->ifon)
        {
            rtgui_msg_t msg;
            msg.type = GUI_SRV_CB;
            msg.cb = sw->cb_off.link_cb;
            msg.u.payload = sw->cb_off.param;
            send_msg_to_gui_server(&msg);
        }

    }
}*/

static void (onOn)(gui_switch_t *b, void *callback, void *parameter)
{
    gui_obj_add_event_cb(b, (gui_event_cb_t)callback, GUI_EVENT_1, parameter);
}
static void (onOff)(gui_switch_t *b, void *callback, void *parameter)
{
    gui_obj_add_event_cb(b, (gui_event_cb_t)callback, GUI_EVENT_2, parameter);
}
void gui_switch_ctor(gui_switch_t *this, gui_obj_t *parent,
                     int16_t x,
                     int16_t y, int16_t w, int16_t h, void *off_pic, void *on_pic)
{
    gui_obj_ctor(&(this->base), parent, "switch", x, y, w, h);
    //this->on = gui_img_create_from_fs(this, "_on", "app/system/resource/switchOn.bin", 0,0,0,0);

    this->base.type = CLICKSWITCH;
    GET_BASE(this)->obj_prepare = switch_prepare;
    this->turn_off = sw_turn_off;
    this->turn_on = sw_turn_on;
    this->onOn = onOn;
    this->onOff = onOff;
    this->off_pic_addr = off_pic;
    this->on_pic_addr = on_pic;
    this->off_hl_pic_addr = this->off_pic_addr;
    this->on_hl_pic_addr = this->on_pic_addr;
}
#include "gui_magic_img.h"
gui_switch_t *gui_switch_create(void *parent, int16_t x, int16_t y,
                                int16_t w, int16_t h, void *off_pic, void *on_pic)
{
    //#define sw_p this, parent, x, y, w, h, off_pic, on_pic
    //GUI_NEW(gui_switch_t, gui_switch_ctor, sw_p)
    gui_log("gui_switch_create:%p,%d,%d,%d,%d,%p,%p\n", parent, x, y, w, h, off_pic, on_pic);
    gui_switch_t *this = gui_malloc(sizeof(gui_switch_t));
    memset(this, 0, sizeof(gui_switch_t));
    gui_switch_ctor(this, parent, x, y, w, h, off_pic, on_pic);
    gui_list_init(&(((gui_obj_t *)this)->child_list));
    if ((((gui_obj_t *)this)->parent) != ((void *)0))
    { gui_list_insert_before(&((((gui_obj_t *)this)->parent)->child_list), &(((gui_obj_t *)this)->brother_list)); }
    this->switch_picture = (void *)gui_magic_img_create_from_mem(this, "switch_picture", off_pic, 0, 0,
                                                                 0, 0);
    ((gui_obj_t *)this)->create_done = 1;
    return this;
}

