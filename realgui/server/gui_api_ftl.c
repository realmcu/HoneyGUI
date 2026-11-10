/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
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
