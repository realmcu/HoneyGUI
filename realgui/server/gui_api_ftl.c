/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_api_ftl.c
  * @brief Application Programming Interface for UI
  * @details Flash Translation Layer
  * @author sienna_shen@realsil.com.cn
  * @date 2025/3/5
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */


/*============================================================================*
 *                        Header Files
 *============================================================================*/

#include "guidef.h"
#include "gui_api_ftl.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/



/*============================================================================*
 *                            Variables
 *============================================================================*/

static struct gui_ftl *ftl = NULL;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/


/*============================================================================*
 *                           Public Functions
 *============================================================================*/


void gui_ftl_info_register(struct gui_ftl *info)
{
    ftl = info;
}

int gui_ftl_read(uintptr_t addr, uint8_t *buf, size_t len)
{
    if (ftl->read)
    {
        ftl->read(addr, buf, len);
        return len;
    }
    else
    {
        return 0;
    }
}
int gui_ftl_write(uintptr_t addr, const uint8_t *buf, size_t len)
{
    if (ftl->write)
    {
        ftl->write(addr, buf, len);
        return len;
    }
    else
    {
        return 0;
    }
}
int gui_ftl_erase(uintptr_t addr, size_t len)
{
    if (ftl->erase)
    {
        ftl->erase(addr, len);
    }
    return 0;
}
