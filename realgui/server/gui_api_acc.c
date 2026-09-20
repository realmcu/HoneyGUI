/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/

#include "guidef.h"
#include "gui_api_acc.h"


/*============================================================================*
 *                            Macros
 *============================================================================*/



/*============================================================================*
 *                            Variables
 *============================================================================*/

static struct acc_engine *acc = NULL;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/


/*============================================================================*
 *                           Public Functions
 *============================================================================*/

static void gui_acc_ops_init(struct acc_engine *info)
{
    if ((info->blit != NULL) && (info->blit->init != NULL))
    {
        info->blit->init();
    }
    if ((info->blur != NULL) && (info->blur->init != NULL))
    {
        info->blur->init();
    }
    if ((info->idu != NULL) && (info->idu->init != NULL))
    {
        info->idu->init();
    }
    if ((info->jpeg != NULL) && (info->jpeg->init != NULL))
    {
        info->jpeg->init();
    }
}

static void gui_acc_ops_deinit(struct acc_engine *info)
{
    if ((info->jpeg != NULL) && (info->jpeg->deinit != NULL))
    {
        info->jpeg->deinit();
    }
    if ((info->idu != NULL) && (info->idu->deinit != NULL))
    {
        info->idu->deinit();
    }
    if ((info->blur != NULL) && (info->blur->deinit != NULL))
    {
        info->blur->deinit();
    }
    if ((info->blit != NULL) && (info->blit->deinit != NULL))
    {
        info->blit->deinit();
    }
}

void gui_acc_info_register(struct acc_engine *info)
{
    if (acc == info)
    {
        return;
    }
    if (acc != NULL)
    {
        gui_acc_ops_deinit(acc);
    }
    acc = info;
    if (acc != NULL)
    {
        gui_acc_ops_init(acc);
    }
}

struct acc_engine *gui_get_acc(void)
{
    return acc;
}

