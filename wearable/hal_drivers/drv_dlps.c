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
#include "dlps.h"
#include "trace.h"
#include "app_section.h"
#include "stdlib.h"




bool dlps_flag = false;

static dlps_slist_t drv_dlps_exit_slist =
{
    .next = NULL,
};
static dlps_slist_t drv_dlps_enter_slist =
{
    .next = NULL,
};
static dlps_slist_t drv_dlps_check_slist =
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
RTL_HAL_RAM_CODE
void System_Handler(void)
{
#ifdef RTL8772F
    DBG_DIRECT("SYSTEM_HANDLER 0x%x", get_aon_wakeup_int());
#endif

    dlps_slist_t *node;
    for (node = dlps_slist_first(&(drv_dlps_wakeup_slist)); node; node = dlps_slist_next(node))
    {
        drv_dlps_cb_item_t *p_item = dlps_container_of(node, drv_dlps_cb_item_t, slist);
        p_item->dlps_cb();
    }

#ifdef RTL8772F
    clear_aon_wakeup_int();
#endif
}

/**
 * @brief this function will be called before enter DLPS
 *
 *  set PAD and wakeup pin config for enterring DLPS
 *
 * @param none
 * @return none
 * @retval void
*/
RTL_HAL_RAM_CODE
static void app_enter_dlps_config(void)
{
    DBG_DIRECT("DLPS ENTER");
    extern void Pad_ClearAllWakeupINT(void);
    Pad_ClearAllWakeupINT();
    dlps_slist_t *node;
    for (node = dlps_slist_first(&(drv_dlps_enter_slist)); node; node = dlps_slist_next(node))
    {
        drv_dlps_cb_item_t *p_item = dlps_container_of(node, drv_dlps_cb_item_t, slist);
        p_item->dlps_cb();
    }

    dlps_flag = true;
}

/**
 * @brief this function will be called after exit DLPS
 *
 *  set PAD and wakeup pin config for enterring DLPS
 *
 * @param none
 * @return none
 * @retval void
*/
RTL_HAL_RAM_CODE
static void app_exit_dlps_config(void)
{
    DBG_DIRECT("DLPS EXIT");
    dlps_slist_t *node;
    for (node = dlps_slist_first(&(drv_dlps_exit_slist)); node; node = dlps_slist_next(node))
    {
        drv_dlps_cb_item_t *p_item = dlps_container_of(node, drv_dlps_cb_item_t, slist);
        p_item->dlps_cb();
    }

    dlps_flag = false;
    DBG_DIRECT("DLPS EXIT END");
}

/**
 * @brief DLPS CallBack function
 * @param none
* @return true : allow enter dlps
 * @retval void
*/
#if defined RTL8772F || defined RTL8762G
RTL_HAL_RAM_CODE
static PMCheckResult app_dlps_check_cb(void)
{
    dlps_slist_t *node;
    for (node = dlps_slist_first(&(drv_dlps_check_slist)); node; node = dlps_slist_next(node))
    {
        drv_dlps_cb_item_t *p_item = dlps_container_of(node, drv_dlps_cb_item_t, slist);
        //DBG_DIRECT("%s check fail! Module[%s]", __func__, p_item->name);
        if (p_item->dlps_cb() == false)
        {
            return PM_CHECK_FAIL;
        }
    }

    return PM_CHECK_PASS;
}
#elif defined RTL8762D
RTL_HAL_RAM_CODE
static bool app_dlps_check_cb(void)
{
    dlps_slist_t *node;
    for (node = dlps_slist_first(&(drv_dlps_check_slist)); node; node = dlps_slist_next(node))
    {
        drv_dlps_cb_item_t *p_item = dlps_container_of(node, drv_dlps_cb_item_t, slist);
        //DBG_DIRECT("%s check fail! Module[%s]", __func__, p_item->name);
        if (p_item->dlps_cb() == false)
        {
            return false;
        }
    }

    return true;
}
#endif


/******************************************************************
 * @brief  pwr_mgr_init() contains the setting about power mode.
 * @param  none
 * @return none
 * @retval void
 */
void pwr_mgr_init(void)
{
    if (false == dlps_check_cb_reg(app_dlps_check_cb))
    {
        APP_PRINT_ERROR0("Error: dlps_check_cb_reg(app_dlps_check_cb) failed!");
    }
    DLPS_IORegUserDlpsEnterCb(app_enter_dlps_config);
    DLPS_IORegUserDlpsExitCb(app_exit_dlps_config);
    DLPS_IORegister();
#ifdef RTL8762G
    lps_mode_set(PLATFORM_DLPS_PFM);
#endif
#ifdef RTL8772F
    lps_mode_set(PLATFORM_DLPS);
#endif
}

void drv_dlps_exit_cbacks_register(const char *name, bool (*cbacks)(void))
{
    drv_dlps_cb_item_t *p_item = malloc(sizeof(drv_dlps_cb_item_t));
    if (p_item == NULL)
    {
        return;
    }
    p_item->name = name;
    p_item->dlps_cb = cbacks;


    dlps_slist_append(&drv_dlps_exit_slist, &(p_item->slist));
}

void drv_dlps_enter_cbacks_register(const char *name, bool (*cbacks)(void))
{
    drv_dlps_cb_item_t *p_item = malloc(sizeof(drv_dlps_cb_item_t));
    if (p_item == NULL)
    {
        return;
    }
    p_item->name = name;
    p_item->dlps_cb = cbacks;

    dlps_slist_append(&drv_dlps_enter_slist, &(p_item->slist));
}

void drv_dlps_check_cbacks_register(const char *name, bool (*cbacks)(void))
{
    drv_dlps_cb_item_t *p_item = malloc(sizeof(drv_dlps_cb_item_t));
    if (p_item == NULL)
    {
        return;
    }
    p_item->name = name;
    p_item->dlps_cb = cbacks;

    dlps_slist_append(&drv_dlps_check_slist, &(p_item->slist));
}

void drv_dlps_wakeup_cbacks_register(const char *name, bool (*cbacks)(void))
{
    drv_dlps_cb_item_t *p_item = malloc(sizeof(drv_dlps_cb_item_t));
    if (p_item == NULL)
    {
        return;
    }
    p_item->name = name;
    p_item->dlps_cb = cbacks;

    dlps_slist_append(&drv_dlps_wakeup_slist, &(p_item->slist));
}




/************** end of file ********************/
