/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include "os_mem.h"
#include "trace.h"
#include "bt_bond.h"
#include "btm.h"
#include "remote.h"
#include "app_bond.h"
#include "ftl.h"
#include "gap_br.h"
#include "app_linkback.h"
#include <stdlib.h>
#include <section.h>
#include "os_sched.h"
#include "os_task.h"
#if (RTK_BR_TASK == 1)
#include "dp_br_info.h"
#include "app_cfg.h"
#endif

uint8_t default_addr[6] = {0};
static T_APP_BOND_DEVICE temp_search_device[TEMP_MAX_SEARCH] = {0};
static T_APP_BOND_DEVICE temp_cache_device[TEMP_MAX_SEARCH] = {0};

static uint16_t temp_priority = 0;
static uint8_t temp_flag = 1;
static bool connect_status_flag = true;
static uint8_t connect_status_flag_addr[6] = {0};

void app_bond_set_priority(uint8_t *bd_addr)
{
    uint8_t temp_addr[6];

    if (bt_bond_addr_get(1, temp_addr) == true)
    {
        if (memcmp(bd_addr, temp_addr, 6))
        {
            bt_bond_priority_set(bd_addr);
        }
    }
}

bool get_connect_status_flag(void)
{
    return connect_status_flag;
}

void set_connect_status_flag(bool flag)
{
    connect_status_flag = flag;
}

uint8_t *get_connect_status_flag_addr(void)
{
    return connect_status_flag_addr;
}

void set_connect_status_flag_addr(uint8_t *addr)
{
    memcpy(connect_status_flag_addr, addr, 6);
}

uint32_t app_bt_bond_load_device_info_from_ftl(T_APP_BOND_DEVICE *bond_info)
{
    T_APP_BOND_DEVICE *temp = NULL;
    uint32_t ret = 1;

    ret = ftl_load_from_storage(bond_info, APP_BOND_DEVICE_INFO_ADDR, APP_BOND_DEVICE_INFO_SIZE);
    temp = bond_info;
    for (uint8_t i = 0; i < MAX_BOND_INFO_NUM; i++)
    {
        APP_PRINT_INFO5("app_bt_bond_load_device_info_from_ftl: bd_addr %s, device type %d ,used %d ,priority %d, flag %x",
                        TRACE_BDADDR(temp[i].bd_addr), temp[i].device_type, temp[i].used, temp[i].priority, temp[i].flag);
    }
    return ret;
}

uint32_t app_bt_bond_save_device_info_to_ftl(void *bond_info)
{
    T_APP_BOND_DEVICE *temp = (T_APP_BOND_DEVICE *)bond_info;
    uint32_t ret = 1;

    for (uint8_t i = 0; i < MAX_BOND_INFO_NUM; i++)
    {
        APP_PRINT_INFO5("app_bt_bond_save_device_info_to_ftl: bd_addr %s, device type %d ,used %d ,priority %d, flag %x",
                        TRACE_BDADDR(temp[i].bd_addr), temp[i].device_type, temp[i].used, temp[i].priority, temp[i].flag);
    }
    ret = ftl_save_to_storage(bond_info, APP_BOND_DEVICE_INFO_ADDR, APP_BOND_DEVICE_INFO_SIZE);
    return ret;
}

//return the num has been used(0-8)
uint16_t app_bt_bond_get_num(void)
{
    uint16_t device_num = 0;

    for (uint8_t i = 0; i < MAX_BOND_INFO_NUM; i++)
    {
        if (br_db.bond_device[i].flag >> 8)
        {
            device_num++;
        }
    }
    APP_PRINT_INFO1("app_bt_bond_get_num num %d ", device_num);
    return device_num;
}

uint8_t app_bt_bond_get_num_ear(void)
{
    uint8_t device_num = 0;

    for (uint8_t i = 1; i < MAX_BOND_INFO_NUM; i++)
    {
        if (br_db.bond_device[i].flag >> 8)
        {
            device_num++;
        }
    }
    APP_PRINT_INFO1("app_bt_bond_get_num_ear num %d ", device_num);
    return device_num;
}

//free all bond device
void app_bt_bond_free_all_device(void)
{
    APP_PRINT_TRACE0("app_bt_bond_free_temp_search_device start");
    for (int i = 0; i < MAX_BOND_INFO_NUM; i++)
    {
        br_db.bond_device[i].used = false;
    }
}

T_APP_BOND_DEVICE *app_bt_bond_get_device_by_addr(uint8_t *bd_addr)
{
    for (uint8_t i = 0; i < MAX_BOND_INFO_NUM; i++)
    {
        if (!memcmp(bd_addr, br_db.bond_device[i].bd_addr, 6))
        {
            return &br_db.bond_device[i];
        }
    }
    return NULL;
}

uint8_t app_bt_bond_get_index_by_addr(uint8_t *bd_addr)
{
    for (uint8_t i = 0; i < MAX_BOND_INFO_NUM; i++)
    {
        APP_PRINT_INFO2("app_bt_bond_get_index_by_addr: bd_addr %s, bd_addr1 %s",
                        TRACE_BDADDR(bd_addr), TRACE_BDADDR(br_db.bond_device[i].bd_addr));
        if (!memcmp(bd_addr, br_db.bond_device[i].bd_addr, 6))
        {
            return i;
        }
    }
    return MAX_BOND_INFO_NUM;
}

bool app_bt_bond_check_exist_device_info(void)
{
    bool flag = 0;
    for (uint8_t i = 0; i < MAX_BOND_INFO_NUM; i++)
    {
        if (br_db.bond_device[i].flag >> 8)
        {
            flag = true;
        }
    }
    return flag;
}

void app_bt_bond_sort_priority(T_APP_BOND_DEVICE *temp, int len)
{
    T_APP_BOND_DEVICE temp_elem;
    for (int i = 1; i < len - 1; i ++)
    {
        for (int j = i + 1; j < len; j ++)
        {
            if (temp[i].priority < temp[j].priority)
            {
                temp_elem = temp[i];
                temp[i] = temp[j];
                temp[j] = temp_elem;
            }
        }
    }
    for (int i = 0; i < len; i ++)
    {
        APP_PRINT_INFO5("app_bt_bond_sort_priority: bd_addr %s, device type %d ,used %d ,priority %d, flag %x",
                        TRACE_BDADDR(br_db.bond_device[i].bd_addr), br_db.bond_device[i].device_type,
                        br_db.bond_device[i].used, br_db.bond_device[i].priority, br_db.bond_device[i].flag);

    }
}

//updata priority(from 0-8) if delete node,then update. if active node,then update
void app_bt_bond_update_device_priority(uint8_t *bd_addr, bool delete_device)
{
    APP_PRINT_INFO2("app_bt_bond_update_device_priority: bd_addr %s , delete_device =%d",
                    TRACE_BDADDR(bd_addr), delete_device);
    if (delete_device)
    {
        for (uint8_t i = 0; i < MAX_BOND_INFO_NUM; i++)
        {
            if (temp_priority < br_db.bond_device[i].priority)
            {
                br_db.bond_device[i].priority = br_db.bond_device[i].priority - 1;
            }

        }
    }
    else
    {
        uint16_t priority = app_bt_bond_get_num();
        for (uint8_t i = 0; i < MAX_BOND_INFO_NUM; i++)
        {
            if (!memcmp(bd_addr, br_db.bond_device[i].bd_addr, 6))
            {
                br_db.bond_device[i].priority = priority;
            }
            else
            {
                if (temp_priority < br_db.bond_device[i].priority && br_db.bond_device[i].priority != 0)
                {
                    br_db.bond_device[i].priority = br_db.bond_device[i].priority - 1;
                }
            }
        }
    }
//20230607  gui_update_by_event((UI_MenuTypeDef *)&Bondlist, NULL, false);
}

//disconnect and inactive
bool app_bt_bond_inactive_device(uint8_t *bd_addr)
{
    APP_PRINT_INFO1("app_bt_bond_inactive_device: bd_addr %s",
                    TRACE_BDADDR(bd_addr));
    for (uint8_t i = 0; i < MAX_BOND_INFO_NUM; i++)
    {
        if (!memcmp(bd_addr, br_db.bond_device[i].bd_addr, 6))
        {
            br_db.bond_device[i].used = false;
//20230607  gui_update_by_event((UI_MenuTypeDef *)&Bondlist, NULL, false);
            return true;
        }
    }
    return false;
}

//disconnect and inactive by index
bool app_bt_bond_inactive_device_by_index(uint8_t index)
{
    APP_PRINT_INFO1("app_bt_bond_inactive_device_by_index: index %s",
                    index);
    br_db.bond_device[index].used = false;
//20230607  gui_update_by_event((UI_MenuTypeDef *)&Bondlist, NULL, false);
    return true;
}

//del node,need use bt_bond_delete after this func
bool app_bt_bond_del_bond_device(uint8_t *bd_addr)
{
    for (uint8_t i = 0; i < MAX_BOND_INFO_NUM; i++)
    {
        if (!memcmp(bd_addr, br_db.bond_device[i].bd_addr, 6))
        {
            br_db.bond_device[i].used = false;
            memcpy(br_db.bond_device[i].bd_addr, default_addr, 6);
            temp_priority = br_db.bond_device[i].priority;
            br_db.bond_device[i].priority = 0;
            br_db.bond_device[i].flag = br_db.bond_device[i].flag & 0;
            app_bt_bond_update_device_priority(bd_addr, true);
            br_db.bond_device[i].device_type = T_DEVICE_TYPE_PHONE;
            APP_PRINT_INFO1("app_bt_bond_del_bond_device: bd_addr %s",
                            TRACE_BDADDR(br_db.bond_device[i].bd_addr));
            if (i != 0)
            {
                app_bt_bond_sort_priority(br_db.bond_device, MAX_BOND_INFO_NUM);
            }
            return true;
        }
    }
    return false;
}

//1-8
uint8_t app_bt_bond_get_active_num_by_type(T_DEVICE_TYPE device_type)
{
    uint8_t device_num = 0;

    for (uint8_t i = 0; i < MAX_BOND_INFO_NUM; i++)
    {
        if (br_db.bond_device[i].used && (br_db.bond_device[i].device_type == device_type))
        {
            device_num++;
        }
    }
    return device_num;
}

T_APP_BOND_DEVICE *app_bt_bond_get_active_device_by_type(T_DEVICE_TYPE device_type)
{
    for (uint8_t i = 0; i < MAX_BOND_INFO_NUM; i++)
    {
        if (br_db.bond_device[i].used && (br_db.bond_device[i].device_type == device_type))
        {
            return &br_db.bond_device[i];
        }
    }
    return NULL;
}

uint8_t app_bt_bond_get_num_by_type(T_DEVICE_TYPE device_type)
{
    uint8_t device_num = 0;

    for (uint8_t i = 0; i < MAX_BOND_INFO_NUM; i++)
    {
        if ((br_db.bond_device[i].flag >> 8) && (br_db.bond_device[i].device_type == device_type))
        {
            device_num++;
        }
    }
    return device_num;
}

//the first position is for phone
uint8_t *app_bt_bond_find_least_priority_device_new(void)
{
    uint8_t flag_count = 1;
    for (uint8_t i = 2; i < MAX_BOND_INFO_NUM; i++)
    {
        if (br_db.bond_device[i].priority < br_db.bond_device[flag_count].priority)
        {
            flag_count = i;
        }
    }
    return br_db.bond_device[flag_count].bd_addr;
}



uint8_t app_bt_bond_check_bond_device_full(void)
{
    APP_PRINT_TRACE0("app_bt_bond_check_bond_device_full start");
    uint8_t ret = 0;
    uint8_t *temp_addr = NULL;
    uint16_t temp_bond_num = app_bt_bond_get_num();
    if (temp_bond_num < MAX_BOND_INFO_NUM)
    {
        return ret;
    }
    else
    {
        temp_addr = app_bt_bond_find_least_priority_device_new();
        bt_bond_delete(temp_addr);
        app_bt_bond_del_bond_device(temp_addr);
        APP_PRINT_TRACE1("app_bt_bond_check_bond_device_full bond device reach max, delete device(%s)",
                         TRACE_BDADDR(temp_addr));
        return ret;
    }
}

bool app_bt_bond_add_device(uint8_t *bd_addr, T_DEVICE_TYPE device_type)
{
    uint8_t bond_temp = app_bt_bond_get_index_by_addr(bd_addr);
    if (bond_temp < MAX_BOND_INFO_NUM)//active old device
    {
        if (br_db.bond_device[bond_temp].used)
        {
            return true;
        }
        br_db.bond_device[bond_temp].used = true;
        br_db.bond_device[bond_temp].flag = ((uint16_t)temp_flag << 8) |
                                            (br_db.bond_device[bond_temp].flag & 0x00FF);
        temp_priority = br_db.bond_device[bond_temp].priority;
        if (br_db.bond_device[bond_temp].device_type != T_DEVICE_TYPE_EAR)
        {
            br_db.bond_device[bond_temp].device_type = T_DEVICE_TYPE_PHONE;
        }
        if ((br_db.bond_device[bond_temp].flag & 0x00FF) == 0)
        {
            gap_br_get_remote_name(bd_addr);
            APP_PRINT_INFO0("app_bt_bond_add_device req active name");
        }
        app_bt_bond_update_device_priority(bd_addr, false);
        app_bt_bond_sort_priority(br_db.bond_device, MAX_BOND_INFO_NUM);
        APP_PRINT_INFO2("app_bt_bond_add_device1: bd_addr %s, device_type  %x",
                        TRACE_BDADDR(bd_addr), br_db.bond_device[bond_temp].device_type);
        return true;
    }
    else
    {
        if ((br_db.bond_device[0].flag >> 8))
        {
            app_bt_bond_del_bond_device(br_db.bond_device[0].bd_addr);
        }
        br_db.bond_device[0].used = true;
        memcpy(br_db.bond_device[0].bd_addr, bd_addr, 6);
        br_db.bond_device[0].device_type = T_DEVICE_TYPE_PHONE;
        br_db.bond_device[0].flag = (uint16_t)temp_flag << 8;
        br_db.bond_device[0].priority = app_bt_bond_get_num();
        if ((br_db.bond_device[bond_temp].flag & 0x00FF) == 0)
        {
            gap_br_get_remote_name(bd_addr);
            APP_PRINT_INFO0("app_bt_bond_add_device req new name");
        }
        app_bt_bond_sort_priority(br_db.bond_device, MAX_BOND_INFO_NUM);
        return true;
    }
}

uint8_t *app_bt_bond_check_exit_other_active_device(uint8_t *addr, T_DEVICE_TYPE device_type)
{
    if (app_bt_bond_get_active_num_by_type(device_type) == 0)
    {
        return NULL;
    }
    for (uint8_t i = 0; i < MAX_BOND_INFO_NUM; i++)
    {
        if (br_db.bond_device[i].used && memcmp(br_db.bond_device[i].bd_addr, addr, 6) != 0 &&
            br_db.bond_device[i].device_type == device_type)
        {
            return br_db.bond_device[i].bd_addr;
        }
    }
    return NULL;
}

bool app_bt_bond_add_search_device(uint8_t *bd_addr)
{
    APP_PRINT_INFO1("app_bt_bond_add_search_device: bd_addr %s",
                    TRACE_BDADDR(bd_addr));
    T_APP_BOND_DEVICE *temp_bond_device = NULL;

    uint8_t bond_temp = app_bt_bond_get_index_by_addr(bd_addr);
    temp_bond_device = app_bt_bond_get_temp_search_device(bd_addr);
    if (temp_bond_device != NULL)
    {
        if (bond_temp < MAX_BOND_INFO_NUM)//active old device
        {
            br_db.bond_device[bond_temp].used = true;
            br_db.bond_device[bond_temp].device_type = T_DEVICE_TYPE_EAR;

            if ((uint8_t)temp_bond_device->flag != 0)
            {
                memcpy((uint8_t *)br_db.bond_device[bond_temp].device_name,
                       (uint8_t *)temp_bond_device->device_name,
                       sizeof(br_db.bond_device[bond_temp].device_name));
                br_db.bond_device[bond_temp].flag = ((uint16_t)temp_flag << 8) | (temp_bond_device->flag & 0x00FF);
            }
            temp_priority = br_db.bond_device[bond_temp].priority;
            app_bt_bond_update_device_priority(bd_addr, false);
            app_bt_bond_sort_priority(br_db.bond_device, MAX_BOND_INFO_NUM);
            return true;
        }
        else
        {
            if ((temp_bond_device->flag & 0x00FF) == 0)
            {
                gap_br_get_remote_name(bd_addr);
                APP_PRINT_INFO0("app_bt_bond_add_search_device error: no remote name info");
                return false;
            }

            app_bt_bond_check_bond_device_full();
            for (uint8_t i = 1; i < MAX_BOND_INFO_NUM; i++)
            {
                if (!(br_db.bond_device[i].flag >> 8))
                {
                    br_db.bond_device[i].used = true;
                    memcpy(br_db.bond_device[i].bd_addr, bd_addr, 6);
                    br_db.bond_device[i].device_type = T_DEVICE_TYPE_EAR;

                    memcpy((uint8_t *)br_db.bond_device[i].device_name, (uint8_t *)temp_bond_device->device_name,
                           sizeof(br_db.bond_device[i].device_name));

                    br_db.bond_device[i].flag = ((uint16_t)temp_flag << 8) | (temp_bond_device->flag & 0x00FF);
                    APP_PRINT_INFO1("app_bt_bond_add_search_device flag %x", br_db.bond_device[i].flag);
                    br_db.bond_device[i].priority = app_bt_bond_get_num();
                    app_bt_bond_sort_priority(br_db.bond_device, MAX_BOND_INFO_NUM);
                    return true;
                }
            }
        }
    }
    else
    {
        APP_PRINT_INFO1("app_bt_bond_add_search_device: bd_addr %s ,error!!",
                        TRACE_BDADDR(bd_addr));
    }
    return false;
}

bool app_bt_bond_check_active_device_info_by_addr(uint8_t *bd_addr)
{
    for (uint8_t i = 0; i < MAX_BOND_INFO_NUM; i++)
    {
        if (br_db.bond_device[i].used && (!memcmp(bd_addr, br_db.bond_device[i].bd_addr, 6)))
        {
            return true;
        }
    }
    return false;
}

T_DEVICE_TYPE app_bt_bond_get_cod_type(uint8_t *bd_addr)
{
    T_APP_BOND_DEVICE *cod_info = NULL;

    cod_info = app_bt_bond_get_device_by_addr(bd_addr);
    if (NULL != cod_info)
    {
        return cod_info->device_type;
    }
    return T_DEVICE_TYPE_DEFAULT;
}

//cod change format
T_DEVICE_TYPE app_bt_bond_get_device_type(uint32_t cod)
{
    if ((cod & 0x1F00) >> 8 == 0x02)
    {
        return T_DEVICE_TYPE_PHONE;
    }
    else if ((cod & 0x1F00) >> 8 == 0x04)
    {
        return T_DEVICE_TYPE_EAR;
    }
    return T_DEVICE_TYPE_DEFAULT;
}

//cod change format
uint32_t app_bt_bond_get_device_cod(T_DEVICE_TYPE type)
{
    uint32_t cod = 0x0000;

    if (T_DEVICE_TYPE_PHONE == type)
    {
        cod = 0x0200;
    }
    if (T_DEVICE_TYPE_EAR == type)
    {
        cod = 0x0400;
    }
    if (T_DEVICE_TYPE_PHONE == type)
    {
        cod = 0xff00;
    }
    return cod;
}

//get access addr
static T_APP_BOND_DEVICE *app_bt_bond_get_free_temp_search_device_info(void)
{
    for (uint8_t i = 0; i < TEMP_MAX_SEARCH; i++)
    {
        if (temp_search_device[i].used == false)
        {
            return &temp_search_device[i];
        }
    }
    return NULL;
}

//add search device
uint8_t app_bt_bond_add_temp_search_device(uint8_t *bd_addr, uint32_t cod, uint16_t *name,
                                           uint8_t len)
{
    APP_PRINT_TRACE0("app_bt_bond_add_temp_search_device start");
    uint8_t ret = 1;
    uint8_t flag_temp = 0;
    T_APP_BOND_DEVICE *temp_search_info = NULL;

    temp_search_info = app_bt_bond_get_temp_search_device(bd_addr);

    if (temp_search_info == NULL)
    {
        temp_search_info = app_bt_bond_get_free_temp_search_device_info();
    }

    if (temp_search_info)
    {
        temp_search_info->used = true;
        memcpy(temp_search_info->bd_addr, bd_addr, 6);
        temp_search_info->device_type = app_bt_bond_get_device_type(cod);
        memcpy((uint8_t *)temp_search_info->device_name, (uint8_t *)name,
               sizeof(temp_search_info->device_name));
        temp_search_info->flag = ((uint16_t)flag_temp << 8) | len;
        ret = 0;
    }
//20230607  gui_update_by_event((UI_MenuTypeDef *)&Bondlist, NULL, false);

    return ret;
}

bool app_bt_bond_add_temp_search_device_name_info(uint8_t *bd_addr, uint16_t *name, uint8_t len)
{
    APP_PRINT_TRACE0("app_bt_bond_add_temp_search_device_name_info");

    T_APP_BOND_DEVICE *temp_bond_device = NULL;
    temp_bond_device = app_bt_bond_get_temp_search_device(bd_addr);
    if (temp_bond_device != NULL)
    {
        if (len == 0)
        {
            return false;
        }
        memcpy((uint8_t *)temp_bond_device->device_name, (uint8_t *)name,
               sizeof(temp_bond_device->device_name));
        temp_bond_device->flag = len;

        return true;
    }
    else
    {
        APP_PRINT_INFO0("app_bt_bond_atsd_name_info error: no temp device searched");
        return false;
    }
}

//get search device info
T_APP_BOND_DEVICE *app_bt_bond_get_temp_search_device(uint8_t *bd_addr)
{
    for (uint8_t i = 0; i < TEMP_MAX_SEARCH; i++)
    {
        if (temp_search_device[i].used && !memcmp(temp_search_device[i].bd_addr, bd_addr, 6))
        {
            return &temp_search_device[i];
        }
    }
    return NULL;
}

//get search device type
T_DEVICE_TYPE app_bt_bond_get_search_device_cod_type(uint8_t *bd_addr)
{
    APP_PRINT_TRACE0("app_bt_bond_get_search_device_cod_type start");
    for (int i = 0; i < TEMP_MAX_SEARCH; i++)
    {
        if ((temp_search_device[i].used == true) &&
            (!memcmp(temp_search_device[i].bd_addr, bd_addr, 6)))
        {
            return temp_search_device[i].device_type;
        }
    }
    return T_DEVICE_TYPE_DEFAULT;
}

uint32_t app_bt_bond_get_search_device_cod(uint8_t *bd_addr)
{
    APP_PRINT_TRACE0("app_bt_bond_get_search_device_cod start");
    for (int i = 0; i < TEMP_MAX_SEARCH; i++)
    {
        if ((temp_search_device[i].used == true) && (br_db.bond_device[i].flag >> 8))
        {
            return app_bt_bond_get_device_cod(temp_search_device[i].device_type);
        }
    }
    return 0;
}

//get cache device info
T_APP_BOND_DEVICE *app_bt_bond_get_temp_cache_device(uint8_t *bd_addr)
{
    for (uint8_t i = 0; i < TEMP_MAX_SEARCH; i++)
    {
        if (temp_cache_device[i].used && !memcmp(temp_cache_device[i].bd_addr, bd_addr, 6))
        {
            return &temp_cache_device[i];
        }
    }
    return NULL;
}

//get cache addr
static T_APP_BOND_DEVICE *app_bt_bond_get_free_temp_cache_device_info(void)
{
    for (uint8_t i = 0; i < TEMP_MAX_SEARCH; i++)
    {
        if (temp_cache_device[i].used == false)
        {
            return &temp_cache_device[i];
        }
    }
    return NULL;
}

//add cache device
uint8_t app_bt_bond_add_temp_cache_device(uint8_t *bd_addr, uint32_t cod, uint16_t *name,
                                          uint8_t len)
{
    APP_PRINT_TRACE0("app_bt_bond_add_temp_cache_device start");
    uint8_t ret = 1;
    uint8_t flag_temp = 0;
    T_APP_BOND_DEVICE *temp_cache_info = NULL;

    temp_cache_info = app_bt_bond_get_temp_cache_device(bd_addr);

    if (temp_cache_info == NULL)
    {
        temp_cache_info = app_bt_bond_get_free_temp_cache_device_info();
    }

    if (temp_cache_info)
    {
        temp_cache_info->used = true;
        memcpy(temp_cache_info->bd_addr, bd_addr, 6);
        temp_cache_info->device_type = app_bt_bond_get_device_type(cod);
        memcpy((uint8_t *)temp_cache_info->device_name, (uint8_t *)name,
               sizeof(temp_cache_info->device_name));
        temp_cache_info->flag = ((uint16_t)flag_temp << 8) | len;
        ret = 0;
    }
//20230607  gui_update_by_event((UI_MenuTypeDef *)&Bondlist, NULL, false);

    return ret;
}

//get cache device info
T_APP_BOND_DEVICE *app_bt_bond_get_temp_cache_device_by_index(uint8_t index)
{
    if (temp_cache_device[index].used)
    {
        return &temp_cache_device[index];
    }
    else
    {
        return NULL;
    }
}

//free all cache device
void app_bt_bond_free_temp_cache_device(void)
{
    APP_PRINT_TRACE0("app_bt_bond_free_temp_cache_device start");
    for (int i = 0; i < TEMP_MAX_SEARCH; i++)
    {
        temp_cache_device[i].used = false;
    }
}

bool app_bt_bond_temp_cache_exsit_device(void)
{
    for (uint8_t i = 0; i < TEMP_MAX_SEARCH; i++)
    {
        if (temp_cache_device[i].used == true &&
            (memcmp(temp_cache_device[i].bd_addr, default_addr, 6) != 0))
        {
            return true;
        }
    }
    return false;

}

void app_bt_bond_temp_cache_save_to_search(void)
{
    memcpy(temp_search_device, temp_cache_device, sizeof(temp_cache_device));
}

int app_bt_bond_get_hightest_device(T_DEVICE_TYPE type)
{
    int temp_priority = -1;
    int temp_index = -1;

    for (int i = 0; i < MAX_BOND_INFO_NUM; i++)
    {
        if (br_db.bond_device[i].device_type == type && br_db.bond_device[i].priority > temp_priority &&
            (br_db.bond_device[i].flag >> 8))
        {
            temp_index = i;
            temp_priority = br_db.bond_device[i].priority;
        }
    }
    return temp_index;
}

bool app_bt_bond_linkback(uint16_t retry_timeout)
{
    bool ret = false;
    int bond_num;
    uint32_t bond_flag;
    uint32_t plan_profs;

    linkback_todo_queue_delete_all_node();

    bond_num = app_bt_bond_get_hightest_device(T_DEVICE_TYPE_PHONE);
    APP_PRINT_INFO1("linkback_load_bond_list phone bond_num = %d", bond_num);
    if (bond_num >= 0)
    {
        bond_flag = 0;
        bt_bond_flag_get(br_db.bond_device[bond_num].bd_addr, &bond_flag);
        if (bond_flag & (BOND_FLAG_HFP | BOND_FLAG_HSP))
        {
            plan_profs = (HFP_PROFILE_MASK);
            linkback_todo_queue_insert_normal_node(br_db.bond_device[bond_num].bd_addr, plan_profs,
                                                   retry_timeout);
            ret = true;
        }
    }

    bond_num = app_bt_bond_get_hightest_device(T_DEVICE_TYPE_EAR);
    APP_PRINT_INFO1("linkback_load_bond_list ear bond_num = %d", bond_num);
    if (bond_num >= 0)
    {
        bond_flag = 0;
        bt_bond_flag_get(br_db.bond_device[bond_num].bd_addr, &bond_flag);
        if (bond_flag & BOND_FLAG_A2DP)
        {
            plan_profs = (A2DP_PROFILE_MASK);
            linkback_todo_queue_insert_normal_node(br_db.bond_device[bond_num].bd_addr, plan_profs,
                                                   retry_timeout);
            ret = true;
        }
    }

    linkback_todo_queue_all_node();
    return ret;
}
