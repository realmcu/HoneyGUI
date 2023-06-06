/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include <string.h>
#include "trace.h"
#include "btm.h"
#include "app_br_link_util.h"
#include "bt_pbap.h"
#include "app_sdp.h"



static void app_pbap_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    bool handle = true;

    switch (event_type)
    {
    case BT_EVENT_PBAP_CONN_CMPL:
        {
            T_APP_BR_LINK *p_link;

            p_link = app_find_br_link(param->pbap_conn_cmpl.bd_addr);
            if (p_link != NULL)
            {
                bt_pbap_phone_book_set(p_link->bd_addr, BT_PBAP_REPOSITORY_LOCAL, BT_PBAP_PATH_PB);
            }
        }
        break;

    case BT_EVENT_PBAP_CALLER_ID_NAME:
        {

        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_pbap_bt_cback: event_type 0x%04x", event_type);
    }
}

void app_pbap_init(void)
{
    bt_pbap_init(MAX_BR_LINK_NUM);
    bt_mgr_cback_register(app_pbap_bt_cback);
}
