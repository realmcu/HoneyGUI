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
#include <gui_magic_img.h>
#include "gui_tab.h"
#include "gui_canvas.h"
#include "gui_curtain.h"
#include "gui_img.h"
#include "gui_matrix.h"


static void gui_tab_get_stacking_location(gui_tab_stacking_t *result, gui_tab_stacking_t *start,
                                          gui_tab_stacking_t *end, float percent)
{
    result->scale = start->scale + (end->scale - start->scale) * percent;
    result->location = start->location + (end->location - start->location) * percent;
    result->opacity = start->opacity + (end->opacity - start->opacity) * percent;
}


static void tab_prepare(gui_obj_t *obj)
{
    gui_tab_t *this = (gui_tab_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    gui_tabview_t *parent = (gui_tabview_t *)(obj->parent);
    if (this->style == CLASSIC)
    {
        obj->tx = (this->id.x - parent->cur_id.x) * (int)gui_get_screen_width();
        obj->ty = (this->id.y - parent->cur_id.y) * (int)gui_get_screen_height();
        return;
    }
    else if (this->style == FADE)
    {
        float x = 1.0f - (float)abs(obj->dx) / gui_get_screen_width();
        if (x < 0.2f)
        {
            x = 0.2f;
        }
        if (x >= 1.0f)
        {
            x = 1.0f;
        }
        obj->sx = x;
        obj->sy = x;
    }
    else if (this->style == REDUCTION_FADE)
    {
        float x = 1.0f - (float)abs(obj->dx) / gui_get_screen_width();
        if (x < 0.2f)
        {
            x = 0.2f;
        }
        if (x >= 1.0f)
        {
            x = 1.0f;
        }
        obj->sx = x;
        obj->sy = x;
    }
    else if (this->style == REDUCTION)
    {
        float s;
        obj->tx = (this->id.x - parent->cur_id.x) * (int)gui_get_screen_width();
        obj->ty = (this->id.y - parent->cur_id.y) * (int)gui_get_screen_height();
        int sx = abs(obj->dx + obj->ax + obj->tx);
        sx = sx % gui_get_screen_width();
        s = 1.0f - (float)sx / gui_get_screen_width();

        if (s < 0.2f)
        {
            s = 0.2f;
        }
        if (s >= 1.0f)
        {
            s = 1.0f;
        }
        obj->sx = s;
        obj->sy = s;

    }
    else if (this->style == STACKING)
    {
        // scale 0.5f ~ 0.5f, 10 ~ 10, opa, 0~255
        // scale 0.5f ~ 0.8f, 10 ~ 1/4h, opa, 255
        // scale 0.8f ~ 0.9f, 1/4h ~ 3/4h, opa, 255
        // scale 0.9f ~ 0.9f, 3/4h ~ h, opa, 255

        float h = gui_get_screen_height();

        float scale[5] = {0.5f, 0.5f, 0.8f, 0.9f, 0.9f};
        uint8_t opacity[5] = {0, 255, 255, 255, 255};
        float location[5] = {5, 5, h / 4, h * 3 / 4, h};

        float percent = 0.5f;
        if (tp->type == TOUCH_HOLD_Y)
        {
            percent += (float)tp->deltaY / gui_get_screen_width();
        }


        int32_t i = this->id.z - parent->cur_id.y + 1; //parent->cur_id.y
        if ((i >= 0) && (i < 3))
        {
            gui_tab_stacking_t result;
            gui_tab_stacking_t start = {.location = location[i],          .scale = scale[i],      .opacity = opacity[i]};
            gui_tab_stacking_t end =   {.location = location[i + 1],      .scale = scale[i + 1],  .opacity = opacity[i + 1]};
            gui_tab_get_stacking_location(&result, &start, &end, percent);
            obj->tx = 0;
            obj->ty = (int16_t)(h * (result.scale - 1.0f) / 2 + result.location);
            obj->sx = result.scale;
            obj->sy = result.scale;
            obj->dy = obj->dy - parent->base.dy;
        }
        else
        {
            obj->tx = 100 * gui_get_screen_width(); //for out of range
        }
    }
}



void gui_tab_ctor(gui_tab_t *this, gui_obj_t *parent, const char *filename, int16_t x, int16_t y,
                  int16_t w, int16_t h, int16_t idx, int16_t idy)
{

    gui_obj_ctor(&this->base, parent, filename, x, y, w, h);

    GET_BASE(this)->obj_update_att = NULL;
    GET_BASE(this)->obj_prepare = tab_prepare;
    GET_BASE(this)->type = TAB;

    gui_tabview_t *parent_ext = (gui_tabview_t *)parent;
    this->style = parent_ext->style;
    this->id.x = idx;
    this->id.y = idy;
    this->id.z = idy;
    parent_ext->tab_cnt++;
    if (idx > 0)
    {
        parent_ext->tab_cnt_right++;
    }
    else if (idx < 0)
    {
        parent_ext->tab_cnt_left--;
    }
    if (idy > 0)
    {
        parent_ext->tab_cnt_down++;
    }
    else if (idy < 0)
    {
        parent_ext->tab_cnt_up--;
    }

}

gui_tab_t *gui_tab_create(void *parent, const char *name, int16_t x, int16_t y,
                          int16_t w, int16_t h, int16_t idx, int16_t idy)
{
    if (w == 0)
    {
        w = (int)gui_get_screen_width();
    }
    if (h == 0)
    {
        h = (int)gui_get_screen_height();
    }

    gui_tab_t *this = gui_malloc(sizeof(gui_tab_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_tab_t));

    gui_tab_ctor(this, (gui_obj_t *)parent, name, x, y, w, h, idx, idy);

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }
    GET_BASE(this)->create_done = true;
    return this;
}

void gui_tab_set_style(gui_tab_t *this, SLIDE_STYLE style)
{
    this->style = style;
}



