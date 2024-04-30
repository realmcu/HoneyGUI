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
#include"guidef.h"
#include "gui_obj.h"
#include "gui_widget_template.h"
#include "tp_algo.h"



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


static void gui_widget_template_prepare(gui_widget_template_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);
}

static void gui_widget_template_draw(gui_widget_template_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);
}

static void gui_widget_template_end(gui_widget_template_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);
}

static void gui_widget_template_destory(gui_widget_template_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);
}

static void gui_widget_template_ctor(gui_widget_template_t *this,
                                     gui_obj_t              *parent,
                                     const char             *name,
                                     void                   *data,
                                     int16_t                x,
                                     int16_t                y,
                                     int16_t                w,
                                     int16_t                h)
{
    //for obj class
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_obj_ctor(obj, parent, name, x, y, w, h);

    obj->obj_prepare = (void (*)(struct _gui_obj_t *))gui_widget_template_prepare;
    obj->obj_draw = (void (*)(struct _gui_obj_t *))gui_widget_template_draw;
    obj->obj_end = (void (*)(struct _gui_obj_t *))gui_widget_template_end;
    obj->obj_destory = (void (*)(struct _gui_obj_t *))gui_widget_template_destory;
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
 * @return gui_widget_template_t*
 */
gui_widget_template_t *gui_widget_template_create(void       *parent,
                                                  const char *name,
                                                  void       *data,
                                                  int16_t    x,
                                                  int16_t    y,
                                                  int16_t    w,
                                                  int16_t    h)
{
    GUI_ASSERT(parent != NULL);

    if (name == NULL)
    {
        name = "WIDGET_TEMPLATE";
    }

    gui_widget_template_t *this = gui_malloc(sizeof(gui_widget_template_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_widget_template_t));

    gui_widget_template_ctor(this, (gui_obj_t *)parent, name, data, x, y, w, h);

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
