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
#include "gui_lvgl.h"
#include <tp_algo.h>



/** @defgroup WIDGET WIDGET
  * @{
  */
/*============================================================================*
 *                           Types
 *============================================================================*/
/** @defgroup WIDGET_Exported_Types WIDGET Exported Types
  * @{
  */


/** End of WIDGET_Exported_Types
  * @}
  */

/*============================================================================*
 *                           Constants
 *============================================================================*/
/** @defgroup WIDGET_Exported_Constants WIDGET Exported Constants
  * @{
  */


/** End of WIDGET_Exported_Constants
  * @}
  */

/*============================================================================*
 *                            Macros
 *============================================================================*/
/** @defgroup WIDGET_Exported_Macros WIDGET Exported Macros
  * @{
  */



/** End of WIDGET_Exported_Macros
  * @}
  */
/*============================================================================*
 *                            Variables
 *============================================================================*/
/** @defgroup WIDGET_Exported_Variables WIDGET Exported Variables
  * @{
  */


/** End of WIDGET_Exported_Variables
  * @}
  */

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
/** @defgroup WIDGET_Exported_Functions WIDGET Exported Functions
  * @{
  */


static void prepare(gui_lvgl_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *root = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(root);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);
}

static void draw(gui_lvgl_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *root = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(root);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);
}
static void end(gui_lvgl_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *root = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(root);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);
}
static void destory(gui_lvgl_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *root = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(root);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);
}



static void LVGL_ctor(gui_lvgl_t *this, gui_obj_t *parent, const char *name,
                      void *data,
                      int16_t x,
                      int16_t y, int16_t w, int16_t h)
{
    //for root class
    gui_obj_t *root = (gui_obj_t *)this;
    gui_obj_ctor(root, parent, name, x, y, w, h);

    root->obj_prepare = (void (*)(struct _gui_obj_t *))prepare;
    root->obj_draw = (void (*)(struct _gui_obj_t *))draw;
    root->obj_end = (void (*)(struct _gui_obj_t *))end;
    root->obj_destory = (void (*)(struct _gui_obj_t *))destory;

    //for self

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
 * @return gui_lvgl_t*
 */
gui_lvgl_t *gui_lvgl_create(void *parent,  const char *name, void *data,
                            int16_t x,
                            int16_t y, int16_t w, int16_t h)
{
    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "LVGL_TEMPLATE";
    }
    gui_lvgl_t *this = gui_malloc(sizeof(gui_lvgl_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_lvgl_t));

    LVGL_ctor(this, (gui_obj_t *)parent, name, data, x, y, w, h);

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }


    GET_BASE(this)->create_done = true;
    return this;
}


/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */






