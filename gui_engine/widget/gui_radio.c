/*
 * File      : gui_tabview.c
 * This file is part of GUI Engine
 */

#include <guidef.h>
#include <gui_radio.h>
#include <string.h>
#include <gui_server.h>
#include <gui_obj.h>
#include <gui_win.h>
#include "gui_text.h"

static uint32_t (get_checked)(gui_radio_t *this)
{
    if (this)
    {
        return this->checked_index;
    }
    else
    {
        return 0;
    }
}
static gui_radio_switch_t *get_sw_by_index(gui_radio_t *this, uint32_t index)
{
    int count = 0;
    gui_list_t *node = NULL;
    gui_list_for_each(node, &this->base.child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);

        if (obj->type == RADIOSWITCH)
        {
            if (count++ == index)
            {
                return (void *)obj;
            }
        }
    }
    return NULL;
}
static uint32_t get_index_by_sw(gui_radio_t *this, gui_radio_switch_t *sw)
{
    int count = 0;
    gui_list_t *node = NULL;
    gui_list_for_each(node, &this->base.child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->type == RADIOSWITCH)
        {
            if ((void *)obj == (void *)sw)
            {
                return count;
            }
            count++;
        }
    }
    return 0;
}
static void (check)(gui_radio_t *this, uint32_t index)
{
    if (index <= this->length - 1)
    {
        gui_radio_switch_t *sw = get_sw_by_index(this, this->checked_index);
        if (sw)
        {
            sw->turn_off(sw);
        }
        sw = get_sw_by_index(this, index);
        if (sw)
        {
            sw->turn_on(sw);
            this->checked_index = index;
        }

    }
}


static void on(gui_radio_switch_t *this)
{
    if (get_index_by_sw((void *)(this->base.parent),
                        this) != ((gui_radio_t *)(this->base.parent))->checked_index)
    {
        gui_radio_switch_t *sw = get_sw_by_index(((gui_radio_t *)(this->base.parent)),
                                                 ((gui_radio_t *)(this->base.parent))->checked_index);
        if (sw)
        {
            sw->turn_off(sw);
        }
        ((gui_radio_t *)(this->base.parent))->checked_index =  get_index_by_sw((void *)(this->base.parent),
                                                                               this);
    }
    /*if (this->base.parent->callback.link_cb)
    {
        gui_server_cb_p cb = (gui_server_cb_p)(this->base.parent->callback.link_cb);
        cb(this->base.parent->callback.param);
    }*/


}
static void (append)(gui_radio_t *this, char *text)
{
    gui_radio_switch_t *sw = gui_radio_switch_create(this, this->gap * this->length, 0,
                                                     GET_BASE(this)->w, GET_BASE(this)->h, this->off_pic_addr, this->on_pic_addr);
    //gui_get_render_api_table()[GET_BASE(this)->type].obj_prepare = radio_switch_prepare;
    sw->onOn(sw, on, sw);
    gui_text_t *t = gui_text_create(sw, "radio_text", this->text_x, this->text_y,
                                    gui_get_screen_width(), gui_get_screen_height());
    gui_text_set(t, text, "rtk_font_stb", this->text_color, strlen(text), 30);
    this->length++;
}
static void (append_vertical)(gui_radio_t *this, char *text)
{
    gui_radio_switch_t *sw = gui_radio_switch_create(this, 0, this->gap * this->length,
                                                     GET_BASE(this)->w, GET_BASE(this)->h, this->off_pic_addr, this->on_pic_addr);
    //gui_get_render_api_table()[GET_BASE(this)->type].obj_prepare = radio_switch_prepare;
    sw->onOn(sw, on, sw);
    gui_text_t *t = gui_text_create(sw, "radio_text", this->text_x, this->text_y,
                                    gui_get_screen_width(), gui_get_screen_height());
    gui_text_set(t, text, "rtk_font_stb", this->text_color, strlen(text), 30);
    this->length++;
}
static void onCheck(gui_radio_t *this, void *cb, void *p)
{
    //gui_cb_set_with_param(this, cb, SHORT_TOUCH_CB, p);
}
static void (set_text_xy)(gui_radio_t *this, int text_x, int text_y)
{
    this->text_x = text_x;
    this->text_y = text_y;
}
static void (set_gap)(gui_radio_t *this, int gap)
{
    this->gap = gap;
}
void gui_radio_ctor(gui_radio_t *this, gui_obj_t *parent,
                    int16_t x,
                    int16_t y, int16_t w, int16_t h, void *off_pic, void *on_pic)
{
    gui_obj_ctor(&(this->base), parent, "radio", x, y, w, h);
    this->on_pic_addr = on_pic;
    this->off_pic_addr = off_pic;
    this->gap = 100;
    this->base.type = RADIO;
    //this->append(this, "a");
    this->checked_index = 0xffffffff;
    this->text_x = 0;
    this->text_y = 100;
    this->text_color = 0x8f71;


}

gui_radio_t *gui_radio_create(void *parent, int16_t x, int16_t y,
                              int16_t w, int16_t h, void *off_pic, void *on_pic)
{
#define _radio_p this, parent, x, y, w, h, off_pic, on_pic
    GUI_NEW(gui_radio_t, gui_radio_ctor, _radio_p)
}




void gui_radio_switch_change_radio_switch(gui_radio_switch_t *sw)
{

    //sw->off->base.not_show = (sw->ifon);
    //sw->on->base.not_show = !(sw->ifon);
    if (sw->ifon)
    {
        gui_log("on\n");
        gui_img_set_attribute(sw->radio_switch_picture, 0, sw->on_pic_addr, 0, 0);
    }
    else
    {
        gui_log("off\n");
        gui_img_set_attribute(sw->radio_switch_picture, 0, sw->off_pic_addr, 0, 0);
    }
    //gui_app_exec(gui_current_app());


}
static void radio_switch_prepare(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    //if (!(((gui_radio_switch_t *)obj)->ifon))
    if ((obj->dx < (int)gui_get_screen_width()) && ((obj->dx + obj->w) >= 0) && \
        (obj->dy < (int)gui_get_screen_height()) && ((obj->dy + obj->h) >= 0))
    {
        /*if (obj->callback.link_cb != NULL)
        {
            if (((tp->type == TOUCH_SHORT && obj->callback.cb_type == SHORT_TOUCH_CB) &&
                 ((tp->x >= obj->dx && tp->x <= (obj->dx + obj->w)) && (tp->y >= obj->dy &&
                                                                        tp->y <= (obj->dy + obj->h)))))
            {
                rtgui_msg_t msg;
                msg.type = GUI_SRV_CB;
                msg.cb = obj->callback.link_cb;
                msg.u.payload = obj->callback.param;
                send_msg_to_gui_server(&msg);
            }
        }*/




        if (((tp->type == TOUCH_SHORT && /*obj->callback.cb_type == SHORT_TOUCH_CB) &&*/
              ((tp->x >= obj->dx && tp->x <= (obj->dx + obj->w)) && (tp->y >= obj->dy &&
                                                                     tp->y <= (obj->dy + obj->h))))))
        {
            gui_log("tp->type:%d,%d,%d\n", tp->type, obj->dx, obj->w, obj->h);
            gui_radio_switch_t *sw = (gui_radio_switch_t *)obj;
            sw->ifon = !(sw->ifon);
            //send_msg_to_gui_server(&msg);
            gui_radio_switch_change_radio_switch(sw);
            /*if (sw->cb_on.link_cb && sw->ifon)
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
            }*/

        }
    }
}
static void sw_turn_on(gui_radio_switch_t *this)
{
    this->ifon = true;
    {
        rtgui_msg_t msg;
        msg.type = GUI_SRV_CB;
        msg.cb = gui_radio_switch_change_radio_switch;
        msg.u.payload = this;
        gui_radio_switch_t *sw = (gui_radio_switch_t *)this;
        /*  send_msg_to_gui_server(&msg);
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
        */
    }
}

static void sw_turn_off(gui_radio_switch_t *this)
{
    this->ifon = false;
    {
        gui_radio_switch_t *sw = (gui_radio_switch_t *)this;
        gui_radio_switch_change_radio_switch(this);
        /*if (sw->cb_on.link_cb && sw->ifon)
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
        }*/

    }
}
static void onOn(gui_radio_switch_t *this, void *cb, void *p)
{
    //this->cb_on.link_cb = cb;
    //this->cb_on.param = p;
}
static void onOff(gui_radio_switch_t *this, void *cb, void *p)
{
    //this->cb_off.link_cb = cb;
    //this->cb_off.param = p;
}
void gui_radio_switch_ctor(gui_radio_switch_t *this, gui_obj_t *parent,
                           int16_t x,
                           int16_t y, int16_t w, int16_t h, void *off_pic, void *on_pic)
{
    gui_obj_ctor(&(this->base), parent, "radio_switch", x, y, w, h);
    //this->on = gui_img_create_from_fs(this, "_on", "app/system/resource/radio_switchOn.bin", 0,0,0,0);

    this->base.type = RADIOSWITCH;
    GET_BASE(this)->obj_prepare = radio_switch_prepare;
    this->turn_off = sw_turn_off;
    this->turn_on = sw_turn_on;
    this->onOn = onOn;
    this->onOff = onOff;
    this->off_pic_addr = off_pic;
    this->on_pic_addr = on_pic;
}
#include "gui_magic_img.h"
gui_radio_switch_t *gui_radio_switch_create(void *parent, int16_t x, int16_t y,
                                            int16_t w, int16_t h, void *off_pic, void *on_pic)
{
    gui_radio_switch_t *this = gui_malloc(sizeof(gui_radio_switch_t));
    memset(this, 0, sizeof(gui_radio_switch_t));
    gui_radio_switch_ctor(this, parent, x, y, w, h, off_pic, on_pic);
    gui_list_init(&(((gui_obj_t *)this)->child_list));
    if ((((gui_obj_t *)this)->parent) != ((void *)0))
    { gui_list_insert_before(&((((gui_obj_t *)this)->parent)->child_list), &(((gui_obj_t *)this)->brother_list)); }
    this->radio_switch_picture = (void *)gui_magic_img_create_from_mem(this, "radio_switch_picture",
                                                                       off_pic, 0, 0, 0, 0);;
    ((gui_obj_t *)this)->create_done = 1;
    return this;
}

gui_api_radio_t gui_radio_api =
{
    .append = append,
    .check = check,
    .ctor = gui_radio_ctor,
    .get_checked = get_checked,
    .onCheck = onCheck,
    .set_gap = set_gap,
    .set_text_xy = set_text_xy,
    .append_vertical = append_vertical,
};
