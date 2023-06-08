/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <string.h>
#include "trace.h"
#include "gap.h"
#include "gap_br.h"
#include "br_gap_cb.h"
#include "remote.h"
#include "btm.h"
#include "app_linkback.h"
#include "bt_bond.h"
#include "bt_spp.h"
#include "app_spp.h"
#include "app_sdp.h"
#include "app_bt_policy_int.h"
#include "dp_br_info.h"
#include "app_cfg.h"
#include "app_br_link_util.h"
#include "dp_app.h"


static bool linkback_need = false;
static uint8_t client_addr[6];//SPP client bd addr.
static bool is_spp_sdp_ok = false;
static uint8_t remote_server_channel = 0;
static uint8_t local_server_chann = 0;
const uint8_t null_addr[6] = {0};

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
            memcpy(app_db.factory_addr, param->ready.bd_addr, 6);
            APP_PRINT_INFO1("app_gap_bt_cback: bt_ready, bd_addr %b",
                            TRACE_BDADDR(param->ready.bd_addr));
            if (!memcmp(app_cfg_nv.bud_local_addr, null_addr, 6))
            {
                memcpy(app_cfg_nv.bud_local_addr, app_db.factory_addr, 6);
                remote_local_addr_set(app_cfg_nv.bud_local_addr);
            }

            gap_set_bd_addr(app_cfg_nv.bud_local_addr);
            set_linkback_status(true);
            bt_event_handle(EVENT_STARTUP, NULL);
        }
        break;

    case BT_EVENT_DEVICE_MODE_RSP:
        {
        }
        break;

    /*20230607    case BT_EVENT_USER_CONFIRMATION_REQ:
            {
                gap_br_user_cfm_req_cfm(param->user_confirmation_req.bd_addr, GAP_CFM_CAUSE_ACCEPT);
            }
            break;*/

    case BT_EVENT_INQUIRY_RSP:
        {
            APP_PRINT_INFO1("BT_EVENT_INQUIRY_RSP response cause = 0x%x", param->inquiry_rsp.cause);
        }
        break;

    case BT_EVENT_INQUIRY_RESULT:
        {
            APP_PRINT_INFO0("BT_EVENT_INQUIRY_RESULT");
            APP_PRINT_INFO6("<-- inquiry device info: addr =[%02x:%02x:%02x:%02x:%02x:%02x]\r\n",
                            param->inquiry_result.bd_addr[5], param->inquiry_result.bd_addr[4],
                            param->inquiry_result.bd_addr[3], param->inquiry_result.bd_addr[2],
                            param->inquiry_result.bd_addr[1], param->inquiry_result.bd_addr[0]);
            APP_PRINT_INFO1("<-- inquiry device info: name = %s", TRACE_STRING(param->inquiry_result.name));
            APP_PRINT_INFO1("<-- inquiry device info: name len = %d",
                            strlen((const char *)param->inquiry_result.name));
            APP_PRINT_INFO1("<-- inquiry device info: rssi = %d", param->inquiry_result.rssi);
            APP_PRINT_INFO1("<-- inquiry device info: class of device = 0x%x", param->inquiry_result.cod);


            /*20230607  if ((param->inquiry_result.rssi >= -60) && \
                            (((param->acl_conn_ind.cod & 0x1F00) >> 8) == 0x04))
                        {
                            uint8_t *device_name = (uint8_t *)param->inquiry_result.name;
                            uint8_t length = strlen((const char *)param->inquiry_result.name);
                            if (length > 50)
                            {
                                length = 50;
                            }
                            memcpy(search_result_temp.bd_addr, param->inquiry_result.bd_addr, 6);
                            search_result_temp.nam_len = watch_utf8_to_unicode(device_name, length,
                                                                               search_result_temp.device_name, 20);
                            search_result_temp.cod = param->acl_conn_ind.cod;
                            search_update.type = SEARCH_RESULT_UPDATE;
                            search_update.u.buf = &search_result_temp;
                            gui_update_by_event((UI_MenuTypeDef *)&Search_buds, &search_update, false);
                        }*/
        }
        break;

    case BT_EVENT_INQUIRY_CMPL:
        {
            APP_PRINT_INFO0("BT_EVENT_INQUIRY_CMPL");
            /*20230607  search_update.type = SEARCH_STATUS_UPDATE;
                        search_update.u.param = SEARCH_FINISH;
                        gui_update_by_event((UI_MenuTypeDef *)&Search_buds, &search_update, false);*/
        }
        break;

    case BT_EVENT_INQUIRY_CANCEL_RSP:
        {
            APP_PRINT_INFO0("BT_EVENT_INQUIRY_CANCEL_RSP");
        }
        break;

    case BT_EVENT_LINK_USER_CONFIRMATION_REQ:
        {
            APP_PRINT_INFO0(" BT_EVENT_USER_CONFIRMATION_REQ\n");
            gap_br_user_cfm_req_cfm(param->link_user_confirmation_req.bd_addr, GAP_CFM_CAUSE_ACCEPT);
        }
        break;

    default:
        break;
    }
}


