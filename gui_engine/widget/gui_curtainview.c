/*
 * File      : gui_tabview.c
 * This file is part of GUI Engine
 */

#include <guidef.h>
#include <gui_curtain.h>
#include <string.h>
#include <gui_server.h>
#include "gui_obj.h"
#include <tp_algo.h>

void curtainvie_update_att(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    gui_curtainview_t *ext = (gui_curtainview_t *)obj;
    int frame_step = GUI_FRAME_STEP;
    if (frame_step / 10 >= 1)
    {
        frame_step = frame_step / 10;
    }

    if (!((obj->parent->dx == 0) && (obj->parent->dy == 0)))
    {
        ext->cur_curtain = CURTAIN_MIDDLE;
        obj->cover = false;
        return;
    }
    gui_curtain_t *c_middle = NULL;
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    gui_list_for_each_safe(node, tmp, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->type == CURTAIN)
        {
            gui_curtain_t *c = (void *)obj;
            if (c->orientation == CURTAIN_MIDDLE)
            {
                c_middle = c;
                break;
            }
        }
    }
    GUI_UNUSED(c_middle);
    switch (ext->cur_curtain)
    {
    case CURTAIN_MIDDLE:
        obj->cover = false;
        if ((tp->type == TOUCH_HOLD_Y))
        {
            obj->y = tp->deltaY;
            if (obj->y > 0 && ext->orientations.up == false)
            {
                obj->y = 0;
            }
            if (obj->y < 0 && ext->orientations.down == false)
            {
                obj->y = 0;
            }
            if (obj->y < -(int)gui_get_screen_height() * ext->scopedown * 0.5f)
            {
                ext->cur_curtain = CURTAIN_DOWN;
                obj->y = 0;
                ext->down_flag = true;
            }
            else if (obj->y > (int)gui_get_screen_height() * ext->scopeup * 0.5f)
            {
                ext->cur_curtain = CURTAIN_UP;
                obj->y = 0;
                ext->down_flag = true;
            }
        }
        else if ((tp->type == TOUCH_HOLD_X))
        {
            obj->x = tp->deltaX;
            if (obj->x < 0 && ext->orientations.right == false)
            {
                obj->x = 0;
            }
            if (obj->x > 0 && ext->orientations.left == false)
            {
                obj->x = 0;
            }
            if (obj->x < -(int)gui_get_screen_width() * ext->scoperight * 0.6f)
            {
                ext->cur_curtain = CURTAIN_RIGHT;
                obj->x = 0;
            }
            else if (obj->x > (int)gui_get_screen_width() * ext->scopeleft * 0.6f)
            {
                ext->cur_curtain = CURTAIN_LEFT;
                obj->x = 0;
            }
        }
        else
        {
            obj->x = 0;
            obj->y = 0;
        }
        break;
    case CURTAIN_UP:
        obj->cover = true;
        if (ext->down_flag)
        {
            //if (c_middle)
            //{
            //    GET_BASE(c_middle)->not_show = true;
            //}

            if (obj->y > 100)
            {
                ext->spring_flag = true;
            }
            if (ext->spring_flag)
            {
                ext->spring_value -= GUI_FRAME_STEP;
                obj->y += ext->spring_value;
                if (obj->y < 0)
                {
                    ext->down_flag = false;
                    ext->spring_flag = false;
                    ext->spring_value = 0;
                    //if (c_middle)
                    //{
                    //    GET_BASE(c_middle)->not_show = false;
                    //}

                }

            }
            else
            {
                ext->spring_value += GUI_FRAME_STEP;
                obj->y += ext->spring_value;
            }


            break;
        }
        if ((tp->type == TOUCH_HOLD_Y))
        {
            if (tp->deltaY < 0)
            {
                obj->y = tp->deltaY;
            }
            //if (obj->y < -(int)gui_get_screen_height() * ext->scopeup * 0.5f)
            //{
            //    ext->cur_curtain = CURTAIN_MIDDLE;gui_log("CURTAIN_MIDDLE:%d\n",obj->y );
            //    obj->y = 0;
            //}
        }
        else if (tp->type == TOUCH_DOWN_SLIDE)
        {
            ext->cur_curtain = CURTAIN_MIDDLE;
            obj->y = 0;
        }
        else
        {
            obj->y = 0;
        }

        break;
    case CURTAIN_DOWN:
        obj->cover = true;

        if (ext->down_flag)
        {
            //if (c_middle)
            //{
            //    GET_BASE(c_middle)->not_show = true;
            //}
//
            if (obj->y < -100)
            {
                ext->spring_flag = true;
            }
            if (ext->spring_flag)
            {
                ext->spring_value += GUI_FRAME_STEP;
                obj->y += ext->spring_value;
                if (obj->y > 0)
                {
                    ext->down_flag = false;
                    ext->spring_flag = false;
                    ext->spring_value = 0;
                    //if (c_middle)
                    //{
                    //    GET_BASE(c_middle)->not_show = false;
                    //}
//
                }

            }
            else
            {
                ext->spring_value -= GUI_FRAME_STEP;
                obj->y += ext->spring_value;
            }


            break;
        }

        if ((tp->type == TOUCH_HOLD_Y))
        {
            if (tp->deltaY > 0)
            {
                obj->y = tp->deltaY;
                gui_log("obj->y = %d\n", obj->y);
            }
        }
        else if (tp->type == TOUCH_UP_SLIDE)
        {
            ext->cur_curtain = CURTAIN_MIDDLE;
            obj->y = 0;
        }
        else
        {
            obj->y = 0;
        }

        break;
    case CURTAIN_LEFT:
        obj->cover = true;
        if ((tp->type == TOUCH_HOLD_X))
        {
            if (tp->deltaX < 0)
            {
                obj->x = tp->deltaX;
            }
        }
        else if (tp->type == TOUCH_LEFT_SLIDE)
        {
            ext->cur_curtain = CURTAIN_MIDDLE;
            obj->x = 0;
        }
        else
        {
            obj->x = 0;
        }
        break;
    case CURTAIN_RIGHT:
        obj->cover = true;
        if ((tp->type == TOUCH_HOLD_X))
        {
            if (tp->deltaX > 0)
            {
                obj->x = tp->deltaX;
            }
        }
        else if (tp->type == TOUCH_RIGHT_SLIDE)
        {
            ext->cur_curtain = CURTAIN_MIDDLE;
            obj->x = 0;
        }
        else
        {
            obj->x = 0;
        }
        break;

    default:
        break;
    }
    //gui_log("ext->cur_curtain=%d\n", ext->cur_curtain);
    if (ext->cur_curtain == CURTAIN_MIDDLE)
    {
        obj->cover = false;
    }
    else
    {
        obj->cover = true;
    }


}
void gui_curtainview_ctor(gui_curtainview_t *this, gui_obj_t *parent, const char *filename,
                          int16_t x,
                          int16_t y, int16_t w, int16_t h)
{
    gui_obj_ctor(&this->base, parent, filename, x, y, w, h);
    ((gui_obj_t *)this)->obj_update_att = curtainvie_update_att;
    ((gui_obj_t *)this)->type = CURTAINVIEW;
    this->cur_curtain = CURTAIN_MIDDLE;
}
gui_curtainview_t *gui_curtainview_create(void *parent, const char *filename, int16_t x,
                                          int16_t y,
                                          int16_t w, int16_t h)
{
#define _paramgui_curtainview_create_ this, parent, filename, x, y, w, h
    GUI_NEW(gui_curtainview_t, gui_curtainview_ctor, _paramgui_curtainview_create_)
}
