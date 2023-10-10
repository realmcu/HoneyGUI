/*
 * File      : gui_tabview.c
 * This file is part of GUI Engine
 */

#include <guidef.h>
#include <string.h>
//#include <math.h>
#include <gui_server.h>
#include <gui_obj.h>
#include <tp_algo.h>
#include <gui_magic_img.h>
#include "gui_card.h"
#include "gui_canvas.h"
//#include "gui_curtain.h"
#include "gui_img.h"
//#include "gui_matrix.h"


// static void gui_card_get_stacking_location(gui_card_stacking_t *result, gui_card_stacking_t *start,
//                                            gui_card_stacking_t *end, float percent)
// {
//     result->scale = start->scale + (end->scale - start->scale) * percent;
//     result->location = start->location + (end->location - start->location) * percent;
//     result->opacity = start->opacity + (end->opacity - start->opacity) * percent;
// }


static void tab_prepare(gui_obj_t *obj)
{
    gui_card_t *this = (gui_card_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    gui_cardview_t *parent = (gui_cardview_t *)(obj->parent);


    float h = gui_get_screen_height();//int32_t i = this->id.z - parent->cur_id.y + 1; //parent->cur_id.y


    //gui_cardview_t *cv = this->base.parent;


    int32_t card_dy = parent->release_y;

    if (parent->mute == true)
    {
        card_dy = 0;
    }



    int32_t location = this->base.h * this->id + card_dy;

    int32_t location_0 = h / 2;
    int32_t location_1 = h / 2 + 20;
    int32_t location_2 = h / 2 + 40;

    float scale_0 = 1.0f;
    float scale_1 = 0.9f;
    //float scale_2 = 0.82f;

    if (location < location_0)
    {
        obj->tx = 0;
        obj->ty = this->base.h * this->id + card_dy;
        obj->sx = scale_0;
        obj->sy = scale_0;
        obj->dx = 0;
        obj->dy = 0;
    }
    else if (location < location_1)
    {
        float scale = (scale_0 - scale_1) * (location - location_0) / (location_1 - location_0);
        obj->tx = 0;
        obj->ty = location;
        obj->sx = 1.0f - scale;
        obj->sy = 1.0f - scale;
        obj->dx = 0;
        obj->dy = 0;
    }
    else
    {
        obj->tx = 0;
        obj->ty = location_1;
        obj->sx = scale_1;
        obj->sy = scale_1;
        obj->dx = 0;
        obj->dy = 0;
    }


    // if(location >= h)
    // {
    //     obj->not_show = true;
    // }



}



void gui_card_ctor(gui_card_t *this, gui_obj_t *parent, const char *filename, int16_t x, int16_t y,
                   int16_t w, int16_t h, int16_t idx, int16_t idy)
{

    gui_obj_ctor(&this->base, parent, filename, x, y, w, h);

    GET_BASE(this)->obj_update_att = NULL;
    GET_BASE(this)->obj_prepare = tab_prepare;
    GET_BASE(this)->type = CARD;

    //gui_cardview_t *parent_ext = (gui_cardview_t *)parent;

    this->id = idy;


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




