/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_card.c
  * @brief 3d card container
  * @details slide to switch card
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/24
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
#include "guidef.h"
#include "gui_obj.h"
#include "gui_card.h"
#include "gui_matrix.h"


/*============================================================================*
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                           Constants
 *============================================================================*/


/*============================================================================*
 *                            Macros
 *============================================================================*/


/*============================================================================*
 *                            Variables
 *============================================================================*/


/*============================================================================*
 *                           Private Functions
 *============================================================================*/

static void gui_card_input_prepare(gui_obj_t *obj)
{
    gui_card_t *this = (gui_card_t *)obj;
    matrix_translate(0, this->id * obj->h, obj->matrix);
}


static void gui_card_prepare_reduction(gui_obj_t *obj)
{
    gui_card_t *this = (gui_card_t *)obj;
    gui_cardview_t *parent = (gui_cardview_t *)(obj->parent);
    int32_t location = this->ay + parent->hold_y + parent->offset_y;
    float h = gui_get_screen_height();


    int32_t location_0 = h / 2;
    int32_t location_1 = h / 2 + 40;

    float scale_0 = 1.0f;
    float scale_1 = 0.9f;

    if (location < location_0)
    {
        matrix_translate(0, location, obj->matrix);
        matrix_translate(obj->w / 2, obj->h / 2, obj->matrix);
        matrix_scale(scale_0, scale_0, obj->matrix);
        matrix_translate(-obj->w / 2, -obj->h / 2, obj->matrix);
    }
    else if (location < location_1)
    {
        float scale = (scale_0 - scale_1) * (location - location_0) / (location_1 - location_0);

        matrix_translate(0, location, obj->matrix);
        matrix_translate(obj->w / 2, obj->h / 2, obj->matrix);
        matrix_scale(1.0f - scale, 1.0f - scale, obj->matrix);
        matrix_translate(-obj->w / 2, -obj->h / 2, obj->matrix);
    }
    else if (location < h)
    {
        matrix_translate(0, location_1, obj->matrix);
        matrix_translate(obj->w / 2, obj->h / 2, obj->matrix);
        matrix_scale(scale_1, scale_1, obj->matrix);
        matrix_translate(-obj->w / 2, -obj->h / 2, obj->matrix);
    }
    else
    {
        //out of rang
        matrix_translate(1000, 1000, obj->matrix);
    }

}


static void gui_card_prepare_classic(gui_obj_t *obj)
{
    gui_card_t *this = (gui_card_t *)obj;
    gui_cardview_t *parent = (gui_cardview_t *)(obj->parent);

    int32_t location = this->ay + parent->hold_y + parent->offset_y;
    matrix_translate(0, location, obj->matrix);
}

static void gui_card_prepare(gui_obj_t *obj)
{
    gui_cardview_t *parent = (gui_cardview_t *)(obj->parent);

    switch (parent->style)
    {
    case CLASSIC:
        {
            gui_card_prepare_classic(obj);
        }
        break;

    case REDUCTION:
        {
            gui_card_prepare_reduction(obj);
        }
        break;

    default:
        break;
    }
}

static void gui_card_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            gui_card_input_prepare(obj);
            break;

        case OBJ_PREPARE:
            gui_card_prepare(obj);
            break;

        default:
            break;
        }
    }
}

static void gui_card_ctor(gui_card_t *this,
                          gui_obj_t  *parent,
                          const char *name,
                          int16_t    x,
                          int16_t    y,
                          int16_t    w,
                          int16_t    h)
{
    gui_obj_ctor(&this->base, parent, name, x, y, w, h);
    gui_cardview_t *cardview = (gui_cardview_t *)parent;
    gui_obj_t *obj = GET_BASE(this);

    GET_BASE(this)->obj_cb = gui_card_cb;
    GET_BASE(this)->has_input_prepare_cb = true;
    GET_BASE(this)->has_prepare_cb = true;
    GET_BASE(this)->type = CARD;
    if (parent->type != CARDVIEW)
    {
        GUI_ASSERT(NULL != NULL);
    }
    cardview->total_cnt++;
    cardview->card_height = obj->h;

    this->id = cardview->total_cnt - 1;
    this->ay = cardview->height;
    cardview->height += obj->h;

    // gui_log("card[%s] id is = %d, ay = %d, cardview->height = %d \n", obj->name, this->id, this->ay,
    //         cardview->height);
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
gui_card_t *gui_card_create(void       *parent,
                            const char *name,
                            int16_t     x,
                            int16_t     y,
                            int16_t     w,
                            int16_t     h)
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

    gui_card_ctor(this, (gui_obj_t *)parent, name, x, y, w, h);

    gui_list_init(&(GET_BASE(this)->child_list));// be carefull here, not same as others
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert(&((GET_BASE(this)->parent)->child_list),
                        &(GET_BASE(this)->brother_list));
    }

    GET_BASE(this)->create_done = true;
    return this;
}

