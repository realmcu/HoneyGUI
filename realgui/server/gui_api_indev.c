/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_api_indev.c
  * @brief Application Programming Interface for UI
  * @details Input Device
  * @author sienna_shen@realsil.com.cn
  * @date 2025/3/5
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */


/*============================================================================*
 *                        Header Files
 *============================================================================*/

#include "guidef.h"
#include "gui_api_indev.h"
#include "gui_api_os.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/



/*============================================================================*
 *                            Variables
 *============================================================================*/

static struct gui_indev *indev = NULL;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/


/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void gui_indev_info_register(struct gui_indev *info)
{
    indev = info;
}

struct gui_indev *gui_get_indev(void)
{
    return indev;
}

void ext_button_set_indicate(void (*callback)(void))
{
    if (indev->ext_button_indicate)
    {
        indev->ext_button_indicate(callback);
    }
}

gui_touch_port_data_t *touchpad_get_data(void)
{
    if (indev->tp_get_data)
    {
        return indev->tp_get_data();
    }
    GUI_ASSERT(NULL != NULL);
    return NULL;
}

gui_kb_port_data_t *kb_get_data(void)
{
    if (indev->kb_get_port_data)
    {
        return indev->kb_get_port_data();
    }
    GUI_ASSERT(NULL != NULL);
    return NULL;
}

gui_wheel_port_data_t *wheel_get_data(void)
{
    if (indev->wheel_get_port_data)
    {
        return indev->wheel_get_port_data();
    }
    GUI_ASSERT(NULL != NULL);
    return NULL;
}
