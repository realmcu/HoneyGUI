/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include <string.h>
#include "trace.h"
#include "btm.h"
#include "app_br_link_util.h"
#include "app_avrcp.h"


bool first_con_volume = true;

static void app_avrcp_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    T_APP_BR_LINK *p_link;
    bool handle = true;

    switch (event_type)
    {
    case BT_EVENT_AVRCP_CONN_IND:
        {
            p_link = app_find_br_link(param->avrcp_conn_ind.bd_addr);
            if (p_link != NULL)
            {
                bt_avrcp_connect_cfm(p_link->bd_addr, true);
            }
        }
        break;

    case BT_EVENT_AVRCP_ABSOLUTE_VOLUME_SET:
        {

        }
        break;

    case BT_EVENT_AVRCP_PLAY_STATUS_CHANGED:
        {

        }
        break;

    case BT_EVENT_AVRCP_CONN_CMPL:
        {
            first_con_volume = true;
        }
        break;
    case BT_EVENT_AVRCP_VOLUME_CHANGED:
        {

        }
        break;

    case BT_EVENT_AVRCP_KEY_VOLUME_UP:
        {

        }
        break;

    case BT_EVENT_AVRCP_KEY_VOLUME_DOWN:
        {

        }
        break;

    case BT_EVENT_AVRCP_REG_VOLUME_CHANGED:
        {

        }
        break;

    case BT_EVENT_AVRCP_PLAY:
        {

        }
        break;

    case BT_EVENT_AVRCP_PAUSE:
        {

        }
        break;

    case BT_EVENT_AVRCP_STOP:
        {
        }
        break;

    case BT_EVENT_AVRCP_FORWARD:
        {
        }
        break;

    case BT_EVENT_AVRCP_BACKWARD:
        {
        }
        break;

    default:
        {
            handle = false;
        }
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_avrcp_bt_cback: event_type 0x%04x", event_type);
    }
}

void app_avrcp_init(void)
{
    bt_avrcp_init(MAX_BR_LINK_NUM);
    bt_mgr_cback_register(app_avrcp_bt_cback);
}

