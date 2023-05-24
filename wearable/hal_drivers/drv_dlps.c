/*
 * Copyright (c) 2022
 *
 *
 *
 *
 *
 *
 */

#include "drv_dlps.h"
#include "trace.h"
#include "app_section.h"


static dlps_slist_t drv_dlps_exit_slist =
{
    .next = NULL,
};
static dlps_slist_t drv_dlps_enter_slist =
{
    .next = NULL,
};
static dlps_slist_t drv_dlps_cleck_slist =
{
    .next = NULL,
};
static dlps_slist_t drv_dlps_wakeup_slist =
{
    .next = NULL,
};


/**
 * @brief    System_Handler
 * @note     system handle to judge which pin is wake source
 * @return   void
 */
RAM_FUNCTION
void System_Handler(void)
{
    DBG_DIRECT("SYSTEM_HANDLER 0x%x", get_aon_wakeup_int());

    dlps_slist_t *node;
    for (node = dlps_slist_first(&(drv_dlps_wakeup_slist)); node; node = slist_next(node))
    {
        drv_dlps_cb_item_t *p_item = dlps_container_of(node, drv_dlps_cb_item_t, slist);
        p_item->dlps_cb();
    }

    clear_aon_wakeup_int();
}

void drv_dlps_exit_cbacks_register(void *cbacks)
{
    drv_dlps_cb_item_t *p_item = malloc(sizeof(drv_dlps_cb_item_t));
    if (p_item == NULL)
    {
        return;
    }

    p_item->dlps_cb = cbacks;


    dlps_slist_append(&drv_dlps_exit_slist, &(p_item->slist));

}




/************** end of file ********************/
