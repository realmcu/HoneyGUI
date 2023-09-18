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
#include "gui_card.h"
#include "gui_canvas.h"
#include "gui_curtain.h"
#include "gui_img.h"
#include "gui_matrix.h"


static void gui_card_get_stacking_location(gui_card_stacking_t *result, gui_card_stacking_t *start,
                                           gui_card_stacking_t *end, float percent)
{
    result->scale = start->scale + (end->scale - start->scale) * percent;
    result->location = start->location + (end->location - start->location) * percent;
    result->opacity = start->opacity + (end->opacity - start->opacity) * percent;
}


static void tab_prepare(gui_obj_t *obj)
{
    gui_card_t *this = (gui_card_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    gui_cardview_t *parent = (gui_cardview_t *)(obj->parent);
    if (this->style == STACKING)
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

        percent += (float)obj->dy / gui_get_screen_width();

        if (percent > 1.0f)
        {
            percent = 1.0f;
        }
        if (percent < 0.0f)
        {
            percent = 0.0f;
        }


        int32_t i = this->id.z - parent->cur_id.y + 1; //parent->cur_id.y
        if ((i >= 0) && (i < 3))
        {
            gui_card_stacking_t result;
            gui_card_stacking_t start = {.location = location[i],          .scale = scale[i],      .opacity = opacity[i]};
            gui_card_stacking_t end =   {.location = location[i + 1],      .scale = scale[i + 1],  .opacity = opacity[i + 1]};
            gui_card_get_stacking_location(&result, &start, &end, percent);
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



void gui_card_ctor(gui_card_t *this, gui_obj_t *parent, const char *filename, int16_t x, int16_t y,
                   int16_t w, int16_t h, int16_t idx, int16_t idy)
{

    gui_obj_ctor(&this->base, parent, filename, x, y, w, h);

    GET_BASE(this)->obj_update_att = NULL;
    GET_BASE(this)->obj_prepare = tab_prepare;
    GET_BASE(this)->type = CARD;

    gui_cardview_t *parent_ext = (gui_cardview_t *)parent;
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

gui_card_t *gui_card_create(void *parent, const char *name, int16_t x, int16_t y,
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

    gui_card_t *this = gui_malloc(sizeof(gui_card_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_card_t));

    gui_card_ctor(this, (gui_obj_t *)parent, name, x, y, w, h, idx, idy);

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }
    GET_BASE(this)->create_done = true;
    return this;
}

void gui_card_set_style(gui_card_t *this, SLIDE_STYLE style)
{
    this->style = style;
}



