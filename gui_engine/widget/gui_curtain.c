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
#include <gui_magic_img.h>


static void curtain_prepare(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    gui_curtain_t *ext = (gui_curtain_t *)obj;
    gui_obj_t *parent = obj->parent;
    gui_curtainview_t *parent_ext = (gui_curtainview_t *)parent;
    int8_t cur_idy = 0;
    int8_t cur_idx = 0;
    int8_t idy = 0;
    int8_t idx = 0;
    switch (parent_ext->cur_curtain)
    {
    case CURTAIN_UP:
        cur_idy = -1;
        cur_idx = 0;
        break;
    case CURTAIN_DOWN:
        cur_idy = 1;
        cur_idx = 0;
        break;
    case CURTAIN_LEFT:
        cur_idy = 0;
        cur_idx = -1;
        break;
    case CURTAIN_RIGHT:
        cur_idy = 0;
        cur_idx = 1;
        break;
    case CURTAIN_MIDDLE:
        cur_idy = 0;
        cur_idx = 0;
        break;
    default:
        break;
    }
    switch (ext->orientation)
    {
    case CURTAIN_UP:
        idy = -1;
        idx = 0;
        break;
    case CURTAIN_DOWN:
        idy = 1;
        idx = 0;
        break;
    case CURTAIN_LEFT:
        idy = 0;
        idx = -1;
        break;
    case CURTAIN_RIGHT:
        idy = 0;
        idx = 1;
        break;
    case CURTAIN_MIDDLE:
        idy = 0;
        idx = 0;
        break;
    default:
        break;
    }
    {
        obj->y = (idy - cur_idy) * (int)gui_get_screen_height() ;
    }
    if ((0 == idy))
    {
        obj->y = 0 - parent->y;
    }
    else
    {
        if (cur_idy == idy)
        {
            obj->y = (idy - cur_idy) * (int)gui_get_screen_height() + (int)gui_get_screen_height() * idy *
                     (1 - ext->scope);
        }
    }

    {
        obj->x = (idx - cur_idx) * (int)gui_get_screen_width();
    }
    if ((0 == idx))
    {
        obj->x = 0 - parent->x;
    }
    else
    {
        if (cur_idx == idx)
        {
            obj->x = (idx - cur_idx) * (int)gui_get_screen_width() + (int)gui_get_screen_width() * idx *
                     (1 - ext->scope);
        }
    }
    if (parent_ext->cur_curtain != CURTAIN_MIDDLE)
    {
        extern void gui_tree_disable_widget_gesture_by_type(gui_obj_t *obj, int type);
        gui_tree_disable_widget_gesture_by_type(&(gui_current_app()->screen), WINDOW);
    }

}


void gui_curtain_ctor(gui_curtain_t *this, gui_obj_t *parent, const char *filename, int16_t x,
                      int16_t y,
                      int16_t w, int16_t h, gui_curtain_enum_t orientation, float scope)
{
    gui_obj_ctor(&this->base, parent, filename, x, y, w, h);
    ((gui_obj_t *)this)->obj_prepare = curtain_prepare;
    ((gui_obj_t *)this)->type = CURTAIN;
    if (scope == 0)
    {
        scope = 1.0;
    }
    this->scope = scope;
    if (parent != NULL)
    {
        gui_curtainview_t *parent_ext = (gui_curtainview_t *)parent;
        this->orientation = orientation;

        parent_ext->curtain_cnt++;
        switch (orientation)
        {
        case CURTAIN_UP:
            parent_ext->orientations.up = true;
            parent_ext->scopeup = this->scope;
            break;
        case CURTAIN_DOWN:
            parent_ext->orientations.down = true;
            parent_ext->scopedown = this->scope;
            break;
        case CURTAIN_LEFT:
            parent_ext->orientations.left = true;
            parent_ext->scopeleft = this->scope;
            break;
        case CURTAIN_RIGHT:
            parent_ext->orientations.right = true;
            parent_ext->scoperight = this->scope;
            break;
        default:
            break;
        }

    }
}
gui_curtain_t *gui_curtain_create(void *parent, const char *filename, int16_t x, int16_t y,
                                  int16_t w, int16_t h, gui_curtain_enum_t orientation, float scope)
{
#define _paramgui_curtain_create_ this, parent, filename, x, y, w, h, orientation, scope
    GUI_NEW(gui_curtain_t, gui_curtain_ctor, _paramgui_curtain_create_)
}

