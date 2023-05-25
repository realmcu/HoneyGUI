/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <string.h>
#include "trace.h"
#include "gap.h"
#include "gap_br.h"
#include "bt_gap_cb.h"
#include "remote.h"
#include "btm.h"
#include "app_linkback.h"
#include "bt_bond.h"
#include "bt_spp.h"
#include "app_spp.h"
#include "app_sdp.h"
#include "app_bt_policy_int.h"
#include "app_link_util.h"
#include "dp_br_info.h"


static bool linkback_need = false;
static uint8_t client_addr[6];//SPP client bd addr.
static bool is_spp_sdp_ok = false;
static uint8_t remote_server_channel = 0;
static uint8_t local_server_chann = 0;

bool get_linkback_status(void)
{
    return linkback_need;
}

void set_linkback_status(bool flag)
{
    linkback_need = flag;
}

void app_gap_common_callback(uint8_t cb_type, void *p_cb_data)
{
    T_GAP_CB_DATA cb_data;
    memcpy(&cb_data, p_cb_data, sizeof(T_GAP_CB_DATA));
    APP_PRINT_INFO1("app_gap_common_callback: event_type = %d", cb_type);
    switch (cb_type)
    {
    case GAP_MSG_WRITE_AIRPLAN_MODE:
        APP_PRINT_INFO1("app_gap_common_callback: GAP_MSG_WRITE_AIRPLAN_MODE cause 0x%04x",
                        cb_data.p_gap_write_airplan_mode_rsp->cause);
        break;

    case GAP_MSG_READ_AIRPLAN_MODE:
        APP_PRINT_INFO2("app_gap_common_callback: GAP_MSG_READ_AIRPLAN_MODE cause 0x%04x, mode %d",
                        cb_data.p_gap_read_airplan_mode_rsp->cause,
                        cb_data.p_gap_read_airplan_mode_rsp->mode);
        break;

    case GAP_MSG_VENDOR_CMD_CMPL_EVENT:
        break;

    case GAP_MSG_SET_LOCAL_BD_ADDR:
        {
            APP_PRINT_INFO1("app_gap_common_callback: GAP_MSG_SET_LOCAL_BD_ADDR: cause 0x%04x",
                            cb_data.p_gap_set_bd_addr_rsp->cause);
        }
        break;

    default:
        break;
    }
    return;
}
void app_gap_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    T_APP_BR_LINK *p_link;
    APP_PRINT_INFO1("app_gap_bt_cback: event_type:0x%x", event_type);
    switch (event_type)
    {
    case BT_EVENT_READY:
        {
            APP_PRINT_INFO0(" BT_EVENT_READY\n");
            //gap_br_start_inquiry(false, 10);
        }
        break;

    case BT_EVENT_DEVICE_MODE_RSP:
        {
        }
        break;

    case BT_EVENT_INQUIRY_RSP:
        {
            APP_PRINT_INFO1("BT_EVENT_INQUIRY_RSP response cause = 0x%x", param->inquiry_rsp.cause);
        }
        break;

    case BT_EVENT_INQUIRY_RESULT:
        {
            APP_PRINT_INFO1(" BT_EVENT_INQUIRY_RESULT name = %s\n", TRACE_STRING(param->inquiry_result.name));
        }
        break;

    case BT_EVENT_INQUIRY_CMPL:
        {
            APP_PRINT_INFO0("Inquiry complete! \n");
        }
        break;

    case BT_EVENT_INQUIRY_CANCEL_RSP:
        {
            APP_PRINT_INFO0("BT_EVENT_INQUIRY_CANCEL_RSP");
        }
        break;
    case BT_EVENT_ACL_CONN_IND:
        {
            p_link = app_find_br_link(param->acl_conn_ind.bd_addr);
            APP_PRINT_INFO1(" BT_EVENT_ACL_CONN_IND p_link = 0x%x\n", p_link);
            if (p_link != NULL)
            {
                bt_acl_conn_reject(param->acl_conn_ind.bd_addr, BT_ACL_REJECT_UNACCEPTABLE_ADDR);
            }
            else
            {
                bt_acl_conn_accept(param->acl_conn_ind.bd_addr, BT_LINK_ROLE_SLAVE);
            }
        }
        break;
    case BT_EVENT_LINK_KEY_REQ:
        {
            APP_PRINT_INFO0(" BT_EVENT_LINK_KEY_REQ\n");
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
            APP_PRINT_INFO0(" BT_EVENT_LINK_PIN_CODE_REQ\n");
            uint8_t pin_code[4] = {1, 2, 3, 4};
            bt_link_pin_code_cfm(param->link_pin_code_req.bd_addr, pin_code, 4, true);
        }
        break;

    case BT_EVENT_LINK_KEY_INFO:
        APP_PRINT_INFO0(" BT_EVENT_LINK_KEY_INFO\n");
        break;

    case BT_EVENT_LINK_USER_CONFIRMATION_REQ:
        {
            APP_PRINT_INFO0(" BT_EVENT_USER_CONFIRMATION_REQ\n");
            gap_br_user_cfm_req_cfm(param->link_user_confirmation_req.bd_addr, GAP_CFM_CAUSE_ACCEPT);
        }
        break;

    case BT_EVENT_ACL_CONN_SUCCESS:
        {
            APP_PRINT_INFO0(" BT_EVENT_ACL_CONN_SUCCESS\n");//todo will remove for https://jira.realtek.com/browse/BEE3PRORD-469
            app_alloc_br_link(param->acl_conn_success.bd_addr);
            bt_sniff_mode_disable(param->acl_conn_ind.bd_addr);
        }
        break;

    case BT_EVENT_ACL_CONN_DISCONN:
        {
            APP_PRINT_INFO0(" BT_EVENT_ACL_CONN_DISCONN\n");
            p_link = app_find_br_link(param->acl_conn_disconn.bd_addr);
            if (p_link != NULL)
            {
                app_free_br_link(p_link);
            }
        }
        break;

    case BT_EVENT_SDP_ATTR_INFO:
        {
            uint8_t temp_local_server_chann = 0;
            APP_PRINT_INFO0(" BT_EVENT_SDP_ATTR_INFO\n");
            //Remote SPP UUID check and get local SPP server channel.
            if (bt_spp_registered_uuid_check((T_BT_SPP_UUID_TYPE)param->sdp_attr_info.info.srv_class_uuid_type,
                                             (T_BT_SPP_UUID_DATA *)(&param->sdp_attr_info.info.srv_class_uuid_data), &temp_local_server_chann))
            {
                if (param->sdp_attr_info.info.server_channel == RFC_SPP_CHANN_NUM)
                {
                    is_spp_sdp_ok = true;
                    local_server_chann = temp_local_server_chann;
                    remote_server_channel = param->sdp_attr_info.info.server_channel;
                }
            }
        }
        break;

    case BT_EVENT_SDP_DISCOV_CMPL:
        {
            APP_PRINT_INFO0(" BT_EVENT_SDP_DISCOV_CMPL\n");
            if (is_spp_sdp_ok)
            {
                if (param->sdp_discov_cmpl.cause == 0x00)
                {
                    bt_spp_connect_req(client_addr, remote_server_channel, APP_SPP_DEFAULT_MTU_SIZE,
                                       APP_SPP_DEFAULT_CREDITS,
                                       local_server_chann);
                }
            }
        }
        break;
    default:
        break;
    }
}


