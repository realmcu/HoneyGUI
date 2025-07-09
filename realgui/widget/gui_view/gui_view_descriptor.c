/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_view_descriptor.c
  * @brief view widget
  * @details view widget
  * @author shel_deng@realsil.com.cn
  * @date 2025/2/18
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
#include "gui_server.h"
#include "gui_obj.h"
#include "tp_algo.h"
#include "gui_view.h"
#include "gui_components_init.h"


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
static const gui_view_descriptor_t *descriptor_list[100];
static uint32_t descriptor_count = 0;
/*============================================================================*
 *                           Private Functions
 *============================================================================*/


/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void gui_view_descriptor_register(const gui_view_descriptor_t *descriptor)
{
    descriptor_list[descriptor_count] = descriptor;
    descriptor_count++;
}

const gui_view_descriptor_t *gui_view_descriptor_get(const char *name)
{
    for (int i = 0; i < descriptor_count; i++)
    {
        if (strcmp(descriptor_list[i]->name, name) == 0)
        {
            return descriptor_list[i];
        }
    }
    return NULL;
}

static int gui_view_create_during_init(void)
{
    for (int i = 0; i < descriptor_count; i++)
    {
        if (descriptor_list[i]->keep == 1)
        {
            gui_view_create((void *)gui_obj_get_fake_root(), descriptor_list[i], 0, 0, 0, 0);
            gui_log("Pre-create '%s' view, File: %s, Function: %s\n", descriptor_list[i]->name, __FILE__,
                    __func__);
        }
    }
    return 0;
}
static GUI_INIT_VIEW_CREATE(gui_view_create_during_init);
