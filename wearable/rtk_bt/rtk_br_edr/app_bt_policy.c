/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include <string.h>
#include "trace.h"
#include "os_sched.h"

#include "gap_br.h"
#include "gap_le.h"
#include "btm.h"
#include "bt_avrcp.h"
#include "bt_spp.h"
#include "bt_bond.h"
#include "bt_rdtp.h"
#include "bt_hfp.h"





static void app_bt_policy_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    APP_PRINT_INFO1("app_bt_policy_cback  event_type = 0x%x", event_type);

    switch (event_type)
    {
    case BT_EVENT_ACL_CONN_SUCCESS:
        {
        }
        break;
    case BT_EVENT_ACL_CONN_FAIL:
        {
            APP_PRINT_INFO1("BT_EVENT_ACL_CONN_FAIL acl_conn_fail.cause =%x ",
                            param->acl_conn_fail.cause);
        }
        break;
    case BT_EVENT_ACL_AUTHEN_SUCCESS:
        {
        }
        break;
    case BT_EVENT_ACL_AUTHEN_FAIL:
        {
            APP_PRINT_INFO1("BT_EVENT_ACL_AUTHEN_FAIL acl_authen_fail.cause =%x ",
                            param->acl_authen_fail.cause);
        }
        break;
    case BT_EVENT_ACL_CONN_DISCONN:
        {
            APP_PRINT_INFO1("BT_EVENT_ACL_CONN_DISCONN reason = %d ", param->acl_conn_disconn.cause);
        }
        break;
    case BT_EVENT_ACL_CONN_READY:
        {
            /* TODO not set pkt 2m in DUT Test Mode */
            bt_acl_pkt_type_set(param->acl_conn_ready.bd_addr, BT_ACL_PKT_TYPE_2M);
        }
        break;


    case BT_EVENT_ACL_CONN_IND:
        {
        }
        break;

    case BT_EVENT_LINK_KEY_INFO:
        {
            bt_bond_key_set(param->link_key_info.bd_addr, param->link_key_info.link_key,
                            param->link_key_info.key_type);
        }
        break;

    case BT_EVENT_LINK_KEY_REQ:
        {
            uint8_t link_key[16];
            T_BT_LINK_KEY_TYPE type;

            if (bt_bond_key_get(param->link_key_req.bd_addr, link_key, (uint8_t *)&type))
            {
                bt_link_key_cfm(param->link_key_req.bd_addr, true, type, link_key);
            }
            else
            {
                bt_link_key_cfm(param->link_key_req.bd_addr, false, type, link_key);
            }
        }
        break;

    case BT_EVENT_LINK_PIN_CODE_REQ:
        {
        }
        break;

    case BT_EVENT_SCO_CONN_CMPL:
        {
        }
        break;

    case BT_EVENT_SCO_DISCONNECTED:
        {
        }
        break;

    case BT_EVENT_SDP_ATTR_INFO:
        {
        }
        break;

    case BT_EVENT_SDP_DISCOV_CMPL:
        {
        }
        break;

    case  BT_EVENT_DID_ATTR_INFO:
        {
        }
        break;

    case BT_EVENT_SDP_DISCOV_STOP:
        {

        }
        break;

    default:
        break;
    }
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void app_bt_policy_init(void)
{
    APP_PRINT_INFO0("app_bt_policy_init");
    bt_mgr_cback_register(app_bt_policy_cback);
}



