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
