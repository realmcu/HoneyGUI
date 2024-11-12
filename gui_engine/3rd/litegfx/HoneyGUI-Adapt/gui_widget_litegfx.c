/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file
  * @brief
  * @details
  * @author
  * @date
  * @version
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include <string.h>
#include <gui_obj.h>
#include <gui_fb.h>
#include "gui_widget_litegfx.h"
#include <tp_algo.h>
#include <draw_img.h>


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

static void prepare(gui_widget_litegfx_t *this)
{
    if (this->handler == 0)
    {
        gui_dispdev_t *dc = gui_get_dc();

        this->handler = lx_vglite_init(this->instance, NULL, dc->screen_width, dc->screen_height);

        if (this->callback)
        {
            lx_vglite_set_user_callback(this->handler, this->callback, (uintptr_t)this);

            this->callback(LX_CMD_ID_INIT, 0, (uintptr_t)this);
        }

        lx_vglite_setup(this->handler);
    }
    else
    {
        touch_info_t *tp = tp_get_info();
        lx_touch_state_t state = LX_TOUCH_UP;
        bool touched = true;

        if (tp->pressed)
        {
            state = LX_TOUCH_DOWN;
        }
        else if (tp->pressing)
        {
            state = LX_TOUCH_MOVE;
        }
        else if (tp->released)
        {
            state = LX_TOUCH_UP;
        }
        else
        {
            touched = false;
        }

        if (touched)
        {
            lx_vglite_touch(this->handler, state, tp->x + tp->deltaX, tp->y + tp->deltaY);
        }
    }

    lx_vglite_update(this->handler);

    gui_fb_change();
}

static void draw(gui_widget_litegfx_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();

    lx_vglite_render(this->handler, dc->section.x1, dc->section.y1, dc->section.x2, dc->section.y2);
}

static void end(gui_widget_litegfx_t *this)
{
    GUI_UNUSED(this);
}

static void destory(gui_widget_litegfx_t *this)
{
    lx_vglite_teardown(this->handler);
    lx_vglite_deinit(this->handler);
}

static void gui_widget_litegfx_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            prepare((gui_widget_litegfx_t *)obj);
            break;

        case OBJ_DRAW:
            draw((gui_widget_litegfx_t *)obj);
            break;

        case OBJ_END:
            end((gui_widget_litegfx_t *)obj);
            break;

        case OBJ_DESTORY:
            destory((gui_widget_litegfx_t *)obj);
            break;

        default:
            break;
        }
    }
}

static void widget_litegfx_ctor(gui_widget_litegfx_t *this, gui_obj_t *parent, const char *name,
                                uintptr_t instance,
                                lx_vglite_user_cb_t cb,
                                int16_t x,
                                int16_t y, int16_t w, int16_t h)
{
    //for root class
    gui_obj_t *root = (gui_obj_t *)this;
    gui_obj_ctor(root, parent, name, x, y, w, h);

    root->obj_cb = gui_widget_litegfx_cb;
    root->has_prepare_cb = true;
    root->has_draw_cb = true;
    root->has_end_cb = true;
    root->has_destroy_cb = true;

    //for self
    this->handler = 0;
    this->instance = instance;
    this->callback = cb;
}

static uint32_t get_image_size(void *data)
{
    uint32_t size = 0;
    struct gui_rgb_data_head *head_p = (struct gui_rgb_data_head *)data;
    uint8_t source_bytes_per_pixel = 0;

    switch (head_p->type)
    {
    case RGB565:
        source_bytes_per_pixel = 2;
        break;
    case RTKARGB8565:
        source_bytes_per_pixel = 3;
        break;
    case ARGB8565:
        source_bytes_per_pixel = 3;
        break;
    case RGB888:
        source_bytes_per_pixel = 3;
        break;
    case ARGB8888:
        source_bytes_per_pixel = 4;
        break;
    default:
        break;
    }

    size = sizeof(struct gui_rgb_data_head) + head_p->w * head_p->h * source_bytes_per_pixel;
    return size;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

/**
 * @brief
 *
 * @param parent
 * @param name
 * @param data
 * @param x
 * @param y
 * @param w
 * @param h
 * @return gui_widget_litegfx_t*
 */
gui_widget_litegfx_t *gui_widget_litegfx_create(void *parent,  const char *name,
                                                uintptr_t instance,
                                                lx_vglite_user_cb_t cb,
                                                int16_t x,
                                                int16_t y, int16_t w, int16_t h)
{
    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "WIDGET_LITEGFX";
    }
    gui_widget_litegfx_t *this = gui_malloc(sizeof(gui_widget_litegfx_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_widget_litegfx_t));

    widget_litegfx_ctor(this, (gui_obj_t *)parent, name, instance, cb, x, y, w, h);

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }


    GET_BASE(this)->create_done = true;
    return this;
}

void gui_widget_litegfx_set_image(gui_obj_t *obj, uint16_t index, void *data)
{
    gui_widget_litegfx_t *this = (gui_widget_litegfx_t *)obj;
    struct gui_rgb_data_head *head_p = (struct gui_rgb_data_head *)data;

    lx_img_cf_t format = LX_IMG_CF_RGBA5658;

    switch (head_p->type)
    {
    case RGB565:
        format = LX_IMG_CF_RGBA5658;
        break;

    case ARGB8565:
        format = LX_IMG_CF_RGB565;
        break;

    case RGB888:
        format = LX_IMG_CF_RGB888;
        break;

    case ARGB8888:
        format = LX_IMG_CF_RGBA8888;
        break;

    default:
        format = LX_IMG_CF_RGBA5658;
        break;
    }

    lx_vglite_set_image(this->handler, index, data, format, head_p->w, head_p->h, get_image_size(data));
}

void gui_widget_litegfx_set_param(gui_obj_t *obj, uint16_t cmd, uintptr_t param)
{
    gui_widget_litegfx_t *this = (gui_widget_litegfx_t *)obj;

    lx_vglite_set_param(this->handler, cmd, param);
}


