/*
 * File      : gui_tabview.c
 * This file is part of GUI Engine
 */

#include <guidef.h>
#include <string.h>
#include <math.h>
#include <gui_server.h>
#include <gui_obj.h>
#include <tp_algo.h>
#include <gui_tabview.h>
#include <gui_magic_img.h>
#include "gui_tab.h"
#include "gui_canvas.h"
#include "gui_curtain.h"
#include "gui_img.h"

void tabview_prepare(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    gui_tabview_t *tabview = (gui_tabview_t *)obj;

    if (tabview->jump.jump_flag)
    {
        tabview->cur_id.y = tabview->jump.jump_id.y;
        tabview->cur_id.x = tabview->jump.jump_id.x;
        tabview->jump.jump_flag = false;
        return;
    }
    bool cover;
    cover = false;
    bool cover1 = false;
    gui_tree_get_cover(obj, CURTAINVIEW, &cover1);

    bool cover2 = false;
    gui_tree_get_cover(obj, ICONLIST, &cover2);

    cover = cover1 || cover2;
    gui_tree_get_cover(obj, SEEKBAR, &cover1);
    cover = cover1 || cover;
    //gui_log("cover = %d\n", cover);
    //gui_log("outer cover=%d,cur_id.x=%d,cur_id.y=%d\n", cover, ext->cur_id.x, ext->cur_id.y);
    if (cover && tabview->cur_id.x == 0)
    {
        return;
    }
    if (tabview->cur_id.x != 0)
    {
        if (tp->type == TOUCH_HOLD_Y || tp->type == TOUCH_DOWN_SLIDE || tp->type == TOUCH_UP_SLIDE)
        {
            return;
        }
    }
    if (tabview->cur_id.y != 0)
    {
        if (tp->type == TOUCH_HOLD_X || tp->type == TOUCH_LEFT_SLIDE || tp->type == TOUCH_RIGHT_SLIDE)
        {
            return;
        }
    }
    //gui_log("tp->type = %d\n", tp->type);
    switch (tp->type)
    {
    case TOUCH_HOLD_X:
        if (tabview->tab_cnt_left == 0 && tabview->tab_cnt_right == 0)
        {
            break;
        }
        obj->dx += tp->deltaX;
        if (tabview->cur_id.x == 0 && tabview->tab_cnt_right == 0)
        {
            if (obj->dx < 0)
            {
                obj->dx = 0;
                break;
            }
        }
        if (tabview->cur_id.x == 0 && tabview->tab_cnt_left == 0)
        {
            if (obj->dx > 0)
            {
                obj->dx = 0;
                break;
            }
        }
        break;
    case TOUCH_HOLD_Y:
        if (tabview->tab_cnt_down == 0 && tabview->tab_cnt_up == 0)
        {
            break;
        }
        obj->dy += tp->deltaY;
        if (tabview->cur_id.y == 0 && tabview->tab_cnt_down == 0)
        {
            if (obj->dy < 0)
            {
                obj->dy = 0;
                break;
            }
        }
        if (tabview->cur_id.y == 0 && tabview->tab_cnt_up == 0)
        {
            if (obj->dy > 0)
            {
                obj->dy = 0;
                break;
            }
        }
        break;
    case TOUCH_LEFT_SLIDE:
        gui_log("TOUCH_LEFT_SLIDE\n");
        if (tabview->tab_cnt_right == 0 && tabview->cur_id.x == 0)
        {
            break;
        }
        //when current tab is the end,come back to current tab if sliding.
        if (tabview->cur_id.x == tabview->tab_cnt_right)
        {
            return;
        }
        tabview->cur_id.x = tabview->cur_id.x + 1;
        break;
    case TOUCH_RIGHT_SLIDE:
        gui_log("TOUCH_RIGHT_SLIDE0\n");
        if (tabview->tab_cnt_left == 0 && tabview->cur_id.x == 0)
        {
            break;
        }
        //when current tab is the end,come back to current tab if sliding.
        if (tabview->cur_id.x == tabview->tab_cnt_left)
        {
            return;
        }
        tabview->cur_id.x = tabview->cur_id.x - 1;
        break;
    case TOUCH_DOWN_SLIDE:
        gui_log("TOUCH_DOWN_SLIDE\n");
        if (tabview->tab_cnt_down == 0 && tabview->cur_id.y == 0)
        {
            break;
        }
        //when current tab is the end,come back to current tab if sliding.
        if (tabview->cur_id.y == tabview->tab_cnt_down)
        {
            return;
        }
        tabview->cur_id.y = tabview->cur_id.y + 1;
        break;
    case TOUCH_UP_SLIDE:
        gui_log("TOUCH_UP_SLIDE\n");
        if (tabview->tab_cnt_up == 0 && tabview->cur_id.y == 0)
        {
            break;
        }
        //when current tab is the end,come back to current tab if sliding.
        if (tabview->cur_id.y == tabview->tab_cnt_up)
        {
            return;
        }
        tabview->cur_id.y = tabview->cur_id.y - 1;
        break;
    default:
        break;
    }
}


void gui_tabview_ctor(gui_tabview_t *this, gui_obj_t *parent, const char *filename, int16_t x,
                      int16_t y,
                      int16_t w, int16_t h)
{
    gui_obj_ctor(&this->base, parent, filename, x, y, w, h);
    GET_BASE(this)->obj_prepare = tabview_prepare;
    GET_BASE(this)->type = TABVIEW;
}

gui_tabview_t *gui_tabview_create(void *parent, const char *filename, int16_t x, int16_t y,
                                  int16_t w, int16_t h)
{
    gui_tabview_t *this = gui_malloc(sizeof(gui_tabview_t));
    memset(this, 0, sizeof(gui_tabview_t));
    gui_tabview_ctor(this, parent, filename, x, y, w, h);
    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list), &(GET_BASE(this)->brother_list));
    }
    GET_BASE(this)->create_done = true;
    return this;
}

void gui_tabview_set_style(gui_tabview_t *this, enum gui_tabview_style style)
{
    this->style = style;
}

void gui_tabview_jump_tab(gui_tabview_t *parent_tabview, int8_t idx, int8_t idy)
{
    parent_tabview->jump.jump_flag = true;
    parent_tabview->jump.jump_id.x = idx;
    parent_tabview->jump.jump_id.y = idy;
}






