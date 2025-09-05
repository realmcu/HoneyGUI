/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_gray.c
  * @brief Gray scale widget
  * @details Widget that converts content in its area to grayscale
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/25
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
#include "gui_fb.h"
#include "gui_gray.h"
#include "tp_algo.h"


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

static void gui_gray_prepare(gui_gray_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);

    if (gui_obj_out_screen(obj))
    {
        return;
    }
    this->gray = gui_malloc(sizeof(engine_gray_t));
    memset(this->gray, 0x00, sizeof(engine_gray_t));
    this->gray->x = obj->x;
    this->gray->y = obj->y;
    this->gray->w = obj->w;
    this->gray->h = obj->h;
    this->gray->data = this->data;

    memcpy(&this->gray->inverse, obj->matrix, sizeof(struct gui_matrix));
    matrix_inverse(&this->gray->inverse);

    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED);

    uint8_t last = this->checksum;
    this->checksum = 0;
    this->checksum = gui_obj_checksum(0, (uint8_t *)this, sizeof(gui_gray_t));

    if (last != this->checksum)
    {
        gui_fb_change();
    }


}

static void gui_gray_draw(gui_gray_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);

    switch (dc->bit_depth)
    {
    case 4:
        engine_gray16_blit_to_dc(this->gray, dc, NULL);
        break;
    case 8:
        engine_gray256_blit_to_dc(this->gray, dc, NULL);
        break;
    default:
        break;
    }
}

static void gui_gray_end(gui_gray_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);
}

static void gui_gray_destroy(gui_gray_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);
}

static void gui_gray_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_gray_prepare((gui_gray_t *)obj);
            break;

        case OBJ_DRAW:
            gui_gray_draw((gui_gray_t *)obj);
            break;

        case OBJ_END:
            gui_gray_end((gui_gray_t *)obj);
            break;

        case OBJ_DESTROY:
            gui_gray_destroy((gui_gray_t *)obj);
            break;

        default:
            break;
        }
    }
}

static void gui_gray_ctor(gui_gray_t *this,
                          gui_obj_t  *parent,
                          const char *name,
                          void       *data,
                          int16_t     x,
                          int16_t     y,
                          int16_t     w,
                          int16_t     h)
{
    //for obj class
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_obj_ctor(obj, parent, name, x, y, w, h);

    obj->obj_cb = gui_gray_cb;
    obj->has_prepare_cb = true;
    obj->has_draw_cb = true;
    obj->has_end_cb = true;
    obj->has_destroy_cb = true;

    /* Initialize gray level to middle value (128) */
    this->gray_level = 128;
    this->data = data;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

/**
 * @brief Create a gray scale widget
 *
 * @param parent Parent object pointer
 * @param name Widget name
 * @param data User data pointer
 * @param x X coordinate
 * @param y Y coordinate
 * @param w Width
 * @param h Height
 * @return gui_gray_t* Pointer to the created gray scale widget
 */
gui_gray_t *gui_gray_create(void       *parent,
                            const char *name,
                            void       *data,
                            int16_t     x,
                            int16_t     y,
                            int16_t     w,
                            int16_t     h)
{
    GUI_ASSERT(parent != NULL);

    if (name == NULL)
    {
        name = "GRAY";
    }

    gui_gray_t *this = gui_malloc(sizeof(gui_gray_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_gray_t));

    gui_gray_ctor(this, (gui_obj_t *)parent, name, data, x, y, w, h);

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }

    GET_BASE(this)->create_done = true;
    return this;
}

void gui_gray_set_attribute(gui_gray_t  *this,
                            const char *name,
                            void       *data,
                            int16_t     x,
                            int16_t     y)
{
    GUI_ASSERT(this != NULL);

    if ((!name) && (!data))
    {
        return;
    }

    this->base.x = x;
    this->base.y = y;

    if (name != NULL)
    {
        this->base.name = name;
    }
    else
    {
        this->base.name = "gui_gray_set_attribute";
    }

    this->data = data;
}

/**
 * @brief Set the gray level of the widget
 *
 * @param gray Gray scale widget pointer
 * @param level Gray level (0-255), 0 for black, 255 for white
 */
void gui_gray_set_level(gui_gray_t *gray, uint8_t level)
{
    GUI_ASSERT(gray != NULL);
    gray->gray_level = level;
}
