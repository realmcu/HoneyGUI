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
#include <string.h>
#include "guidef.h"
#include "gui_obj.h"
#include "gui_fb.h"
#include "gui_arm2d.h"
#include "tp_algo.h"
#include "gui_arm2d.h"
#include "arm_2d.h"
#include "arm_2d_disp_adapters.h"
#include "arm_2d_helper.h"


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

static void gui_arm2d_prepare(gui_arm2d_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);
    gui_fb_change();
}

static void gui_arm2d_draw(gui_arm2d_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    while (arm_fsm_rt_cpl != disp_adapter0_task());

    GUI_UNUSED(this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);
}

static void gui_arm2d_end(gui_arm2d_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);
}

static void gui_arm2d_destroy(gui_arm2d_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);
}

static void gui_arm2d_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_arm2d_prepare((gui_arm2d_t *)obj);
            break;

        case OBJ_DRAW:
            gui_arm2d_draw((gui_arm2d_t *)obj);
            break;

        case OBJ_END:
            gui_arm2d_end((gui_arm2d_t *)obj);
            break;

        case OBJ_DESTROY:
            gui_arm2d_destroy((gui_arm2d_t *)obj);
            break;

        default:
            break;
        }
    }
}

static void gui_arm2d_ctor(gui_arm2d_t *this,
                           gui_obj_t              *parent,
                           const char             *name,
                           int16_t                 x,
                           int16_t                 y,
                           int16_t                 w,
                           int16_t                 h)
{
    //for obj class
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_obj_ctor(obj, parent, name, x, y, w, h);

    obj->obj_cb = gui_arm2d_cb;
    obj->has_prepare_cb = true;
    obj->has_draw_cb = true;
    obj->has_end_cb = true;
    obj->has_destroy_cb = true;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

/**
 * @brief
 *
 * @param parent
 * @param name
 * @param cb
 * @param x
 * @param y
 * @param w
 * @param h
 * @return gui_arm2d_t*
 */
gui_arm2d_t *gui_arm2d_create(void       *parent,
                              const char *name,
                              void (*cb)(gui_arm2d_t *this),
                              int16_t     x,
                              int16_t     y,
                              int16_t     w,
                              int16_t     h)
{
    GUI_ASSERT(parent != NULL);

    if (name == NULL)
    {
        name = "WIDGET_ARM2D";
    }

    gui_arm2d_t *this = gui_malloc(sizeof(gui_arm2d_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_arm2d_t));

    gui_arm2d_ctor(this, (gui_obj_t *)parent, name, x, y, w, h);

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }

    GET_BASE(this)->create_done = true;

    arm_irq_safe
    {
        arm_2d_init();
    }

    disp_adapter0_init();
    cb(this);

    return this;
}

