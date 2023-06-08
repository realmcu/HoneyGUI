/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include <string.h>
#include "trace.h"
#include "os_sched.h"
#include "app_timer.h"
#include "gap_br.h"
#include "gap_le.h"
#include "btm.h"
#include "bt_avrcp.h"
#include "bt_spp.h"
#include "bt_bond.h"
#include "bt_rdtp.h"
#include "bt_hfp.h"
#include "app_cfg.h"
#include "dp_br_info.h"
#include "app_hfp.h"
#include "app_linkback.h"
#include "app_mmi.h"
#include "app_bond.h"
#include "app_report.h"
#include "audio_type.h"
#include "app_sdp.h"
#include "app_hfp.h"
#include "app_bt_policy_int.h"
#include "bt_task.h"
#include "app_linkback.h"
#include "os_timer.h"
#include "watch_msg.h"


extern T_LINKBACK_ACTIVE_NODE linkback_active_node;
T_STATE cur_state = STATE_INIT;
T_BT_DEVICE_MODE radio_mode = BT_DEVICE_MODE_CONNECTABLE;
void *acl_reconnect_timer = NULL;
void *reset_hfp_reconnect = NULL;
extern uint8_t default_addr[6];
uint8_t reconn_cause_timeout = 0;
uint8_t reconn_cause_timeout_addr[6];

static void app_acl_reconnect_callback(void *pxTimer)
{
    APP_PRINT_INFO0("BT_EVENT_ACL_CONN_DISCONN app_create_connection");
    app_create_connection(br_db.acl_info.acl_reconnect_addr, A2DP_PROFILE_MASK);
    memcpy(br_db.acl_info.acl_reconnect_addr, default_addr, 6);
}

uint32_t get_profs_by_bond_flag(uint32_t bond_flag)
{
    uint32_t profs = 0;

    if ((T_LINKBACK_SCENARIO)app_cfg_const.link_scenario == LINKBACK_SCENARIO_HFP_BASE)
    {
        if (bond_flag & BOND_FLAG_HFP)
        {
            profs |= HFP_PROFILE_MASK;

            if (app_cfg_const.supported_profile_mask & PBAP_PROFILE_MASK)
            {
                profs |= PBAP_PROFILE_MASK;
            }
        }
        else if (bond_flag & BOND_FLAG_HSP)
        {
            profs |= HSP_PROFILE_MASK;

            if (app_cfg_const.supported_profile_mask & PBAP_PROFILE_MASK)
            {
                profs |= PBAP_PROFILE_MASK;
            }
        }

        if (bond_flag & BOND_FLAG_SPP)
        {
            if (app_cfg_const.supported_profile_mask & SPP_PROFILE_MASK)
            {
                profs |= SPP_PROFILE_MASK;
            }
        }
        else if (bond_flag & BOND_FLAG_IAP)
        {
            if (app_cfg_const.supported_profile_mask & IAP_PROFILE_MASK)
            {
                profs |= IAP_PROFILE_MASK;
            }
        }
    }
    else if ((T_LINKBACK_SCENARIO)app_cfg_const.link_scenario == LINKBACK_SCENARIO_A2DP_BASE)
    {
        if (bond_flag & (BOND_FLAG_A2DP))
        {
            profs |= A2DP_PROFILE_MASK;
            profs |= AVRCP_PROFILE_MASK;
        }

        if (bond_flag & (BOND_FLAG_HID))
        {
            if (app_cfg_const.supported_profile_mask & HID_PROFILE_MASK)
            {
                profs |= HID_PROFILE_MASK;
            }
        }
    }
    else if ((T_LINKBACK_SCENARIO)app_cfg_const.link_scenario == LINKBACK_SCENARIO_HF_A2DP_LAST_DEVICE)
    {
        if (bond_flag & BOND_FLAG_HFP)
        {
            profs |= HFP_PROFILE_MASK;

            if (app_cfg_const.supported_profile_mask & PBAP_PROFILE_MASK)
            {
                profs |= PBAP_PROFILE_MASK;
            }
        }
        else if (bond_flag & BOND_FLAG_HSP)
        {
            profs |= HSP_PROFILE_MASK;

            if (app_cfg_const.supported_profile_mask & PBAP_PROFILE_MASK)
            {
                profs |= PBAP_PROFILE_MASK;
            }
        }

        if (bond_flag & (BOND_FLAG_A2DP))
        {
            profs |= A2DP_PROFILE_MASK;
            profs |= AVRCP_PROFILE_MASK;
        }

        if (bond_flag & (BOND_FLAG_HID))
        {
            if (app_cfg_const.supported_profile_mask & HID_PROFILE_MASK)
            {
                profs |= HID_PROFILE_MASK;
            }
        }

        if (bond_flag & BOND_FLAG_SPP)
        {
            if (app_cfg_const.supported_profile_mask & SPP_PROFILE_MASK)
            {
                profs |= SPP_PROFILE_MASK;
            }
        }
        else if (bond_flag & BOND_FLAG_IAP)
        {
            if (app_cfg_const.supported_profile_mask & IAP_PROFILE_MASK)
            {
                profs |= IAP_PROFILE_MASK;
            }
        }
    }
    else if ((T_LINKBACK_SCENARIO)app_cfg_const.link_scenario == LINKBACK_SCENARIO_SPP_BASE)
    {
        if (bond_flag & BOND_FLAG_SPP)
        {
            if (app_cfg_const.supported_profile_mask & SPP_PROFILE_MASK)
            {
                profs |= SPP_PROFILE_MASK;
            }
        }
        else if (bond_flag & BOND_FLAG_IAP)
        {
            if (app_cfg_const.supported_profile_mask & IAP_PROFILE_MASK)
            {
                profs |= IAP_PROFILE_MASK;
            }
        }
    }
    else if ((T_LINKBACK_SCENARIO)app_cfg_const.link_scenario == LINKBACK_SCENARIO_HID_BASE)
    {
        if (bond_flag & (BOND_FLAG_HID))
        {
            if (app_cfg_const.supported_profile_mask & HID_PROFILE_MASK)
            {
                profs |= HID_PROFILE_MASK;
            }
        }
    }

    return profs;
}


static void b2s_connected_add_prof(uint8_t *bd_addr, uint32_t prof)
{
    T_APP_BR_LINK *p_link;

    p_link = app_find_br_link(bd_addr);
    if (p_link != NULL)
    {
        p_link->connected_profile |= prof;

        switch (prof)
        {
        case A2DP_PROFILE_MASK:
            bt_bond_flag_add(bd_addr, BOND_FLAG_A2DP);
            break;

        case HFP_PROFILE_MASK:
            {
                bt_bond_flag_remove(bd_addr, BOND_FLAG_HSP);
                bt_bond_flag_add(bd_addr, BOND_FLAG_HFP);
                if ((p_link->connected_profile & PBAP_PROFILE_MASK) == 0)
                {
//                    app_create_connection(bd_addr, PBAP_PROFILE_MASK);
                    linkback_active_node_remain_profs_add(PBAP_PROFILE_MASK, false);
                }

            }
            break;

        case HSP_PROFILE_MASK:
            {
                bt_bond_flag_remove(bd_addr, BOND_FLAG_HFP);
                bt_bond_flag_add(bd_addr, BOND_FLAG_HSP);
            }
            break;

        case SPP_PROFILE_MASK:
            bt_bond_flag_add(bd_addr, BOND_FLAG_SPP);
            break;

        case PBAP_PROFILE_MASK:
            bt_bond_flag_add(bd_addr, BOND_FLAG_PBAP);
            break;

        case HID_PROFILE_MASK:
            bt_bond_flag_add(bd_addr, BOND_FLAG_HID);
            break;

        case IAP_PROFILE_MASK:
            bt_bond_flag_add(bd_addr, BOND_FLAG_IAP);
            break;

        default:
            break;
        }
    }
}

static void b2s_connected_del_prof(uint8_t *bd_addr, uint32_t prof)
{
    T_APP_BR_LINK *p_link;

    p_link = app_find_br_link(bd_addr);
    if (p_link != NULL)
    {
        p_link->connected_profile &= ~prof;
    }
}

static bool bt_connected_del_node(uint8_t *bd_addr)
{
    T_APP_BR_LINK *p_link;

    p_link = app_find_br_link(bd_addr);
    if (p_link != NULL)
    {
//        gap_stop_timer(&p_link->timer_handle_role_switch);
//        gap_stop_timer(&p_link->timer_handle_later_avrcp);
//        gap_stop_timer(&p_link->timer_handle_check_role_switch);
        app_free_br_link(p_link);
        return true;
    }
    else
    {
        return false;
    }
}

//static bool b2s_connected_find_node(uint8_t *bd_addr, uint32_t *profs)
//{
//    T_APP_BR_LINK *p_link;

//    p_link = app_find_br_link(bd_addr);
//    if (p_link != NULL)
//    {
//        if (profs != NULL)
//        {
//            *profs = p_link->connected_profile;
//        }
//        return true;
//    }
//    else
//    {
//        return false;
//    }
//}

static bool b2s_connected_no_profs(uint8_t *bd_addr)
{
    T_APP_BR_LINK *p_link;
    bool ret = true;

    p_link = app_find_br_link(bd_addr);
    if (p_link != NULL)
    {
        if (p_link->connected_profile != 0)
        {
            ret = false;
        }
    }

    return ret;
}


static void connected_node_auth_suc(uint8_t *bd_addr)
{
    T_APP_BR_LINK *p_link;

    p_link = app_find_br_link(bd_addr);
    if (p_link != NULL)
    {
        p_link->auth_flag = true;
    }
}

//static bool connected_node_is_authed(uint8_t *bd_addr)
//{
//    T_APP_BR_LINK *p_link;
//    bool ret = false;

//    p_link = app_find_br_link(bd_addr);
//    if (p_link != NULL)
//    {
//        ret = p_link->auth_flag;
//    }

//    return ret;
//}


//static void conn_sniff_event_handle(uint8_t *bd_addr)
//{
//    T_APP_BR_LINK *p_link;

//    p_link = app_find_br_link(bd_addr);
//    if (p_link)
//    {
//        p_link->acl_link_in_sniffmode_flg = true;
//    }
//}

void bt_policy_set_radio(void)
{
    uint8_t num_bud = app_bt_bond_get_active_num_by_type(T_DEVICE_TYPE_EAR);
    uint8_t num_phone =  app_bt_bond_get_active_num_by_type(T_DEVICE_TYPE_PHONE);;
    APP_PRINT_INFO3("bt_policy_set_radio old mode = 0x%x, num_bud=%d, num_phone=%d, ", radio_mode,
                    num_bud, num_phone);
    if (num_bud)
    {
        if (num_phone)
        {
            radio_mode = BT_DEVICE_MODE_IDLE;
        }
        else
        {
            radio_mode = BT_DEVICE_MODE_DISCOVERABLE_CONNECTABLE;
        }
    }
    else
    {
        if (num_phone)
        {
            radio_mode = BT_DEVICE_MODE_CONNECTABLE;
        }
        else
        {
            radio_mode = BT_DEVICE_MODE_DISCOVERABLE_CONNECTABLE;
        }
    }
    APP_PRINT_INFO1("bt_policy_set_adio new mode = 0x%x", radio_mode);
    bt_device_mode_set(radio_mode);
}

static bool bt_connected_add_node(uint8_t *bd_addr, uint8_t *id)
{
    T_APP_BR_LINK *p_link;

    p_link = app_find_br_link(bd_addr);
    if (p_link == NULL)
    {
        p_link = app_alloc_br_link(bd_addr);
    }
    if (p_link != NULL)
    {
        *id = p_link->id;
        return true;
    }
    else
    {
        return false;
    }
}

void app_bt_policy_discon_all_profiles_by_addr(uint8_t *bd_addr)
{
    APP_PRINT_INFO0("app_bt_policy_discon_all_profiles_by_addr start ");
    T_APP_BR_LINK *p_link = NULL;

    p_link = app_find_br_link(bd_addr);
    if (p_link != NULL)
    {
        gap_br_send_acl_disconn_req(bd_addr);
    }
}

static void app_bt_policy_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    T_APP_BR_LINK *p_link = NULL;
    APP_PRINT_INFO1("app_bt_policy_cback  event_type = 0x%x", event_type);

    switch (event_type)
    {
    case BT_EVENT_ACL_CONN_SUCCESS:
        {
            uint8_t id;
#ifndef F_APP_DSP_ONLINE_DEBUG_SUPPORT
            if (app_bt_bond_get_temp_search_device(param->acl_conn_success.bd_addr) !=
                NULL) //inquiry scan connect
            {
                if (app_bt_bond_add_search_device(param->acl_conn_success.bd_addr))
                {
                    set_connect_status_flag(true);
                    memcpy(get_connect_status_flag_addr(), default_addr, 6);
                }
            }
            else
            {
                if (app_bt_bond_get_cod_type(param->acl_conn_success.bd_addr) == T_DEVICE_TYPE_PHONE)
                {
                    reconn_cause_timeout = 0;
                    memcpy(reconn_cause_timeout_addr, default_addr, 6);
                }
                app_bt_bond_add_device(param->acl_conn_success.bd_addr, T_DEVICE_TYPE_DEFAULT);
            }
            app_bt_bond_save_device_info_to_ftl(br_db.bond_device);
#else
            set_connect_status_flag(true);
#endif
            bt_connected_add_node(param->acl_conn_success.bd_addr, &id);
            bt_policy_set_radio();
            bt_event_handle(EVENT_BT_CONN, param);
        }
        break;

    case BT_EVENT_ACL_CONN_FAIL:
        {
            APP_PRINT_INFO1("BT_EVENT_ACL_CONN_FAIL acl_conn_fail.cause =%x ",
                            param->acl_conn_fail.cause);
            linkback_active_node.is_exit = true;
            linkback_run();
            if (reconn_cause_timeout && param->acl_conn_fail.cause == (HCI_ERR | HCI_ERR_PAGE_TIMEOUT) &&
                !memcmp(reconn_cause_timeout_addr, param->acl_conn_fail.bd_addr, 6))
            {
                //users can use different policies
                if (reconn_cause_timeout < 5)
                {
                    reconn_cause_timeout ++;
                    app_create_connection(param->acl_conn_disconn.bd_addr, HFP_PROFILE_MASK);
                }
                else
                {
                    reconn_cause_timeout = 0;
                    memcpy(reconn_cause_timeout_addr, default_addr, 6);
                }
            }
            if (!get_connect_status_flag() &&
                !memcmp(get_connect_status_flag_addr(), param->acl_conn_fail.bd_addr, 6))
            {
                set_connect_status_flag(true);
                memcpy(get_connect_status_flag_addr(), default_addr, 6);
            }
            bt_event_handle(EVENT_BT_CONN_FAIL, param);
        }
        break;

    case BT_EVENT_ACL_AUTHEN_SUCCESS:
        {
            connected_node_auth_suc(param->acl_authen_success.bd_addr);
        }
        break;

    case BT_EVENT_ACL_AUTHEN_FAIL:
        {
            APP_PRINT_INFO1("BT_EVENT_ACL_AUTHEN_FAIL acl_authen_fail.cause =%x ",
                            param->acl_authen_fail.cause);
            if ((param->acl_authen_fail.cause == (HCI_ERR | HCI_ERR_AUTHEN_FAIL)) ||
                (param->acl_authen_fail.cause == (HCI_ERR | HCI_ERR_KEY_MISSING)))
            {
                if (app_bt_bond_get_cod_type(param->acl_authen_fail.bd_addr) == T_DEVICE_TYPE_EAR)
                {
                    bt_bond_delete(param->acl_authen_fail.bd_addr);// for airpods2
                    app_create_connection(param->acl_authen_fail.bd_addr, A2DP_PROFILE_MASK);
                }

            }
            else
            {
                APP_PRINT_INFO0("BT_EVENT_ACL_AUTHEN_FAIL happened");
                //app_bt_bond_inactive_device(param->acl_authen_fail.bd_addr);//add for test
            }
        }
        break;

    case BT_EVENT_ACL_CONN_DISCONN:
        {
            APP_PRINT_INFO1("BT_EVENT_ACL_CONN_DISCONN reason = %d ", param->acl_conn_disconn.cause);
            uint8_t pair_idx;
            bt_connected_del_node(param->acl_conn_disconn.bd_addr);
            /*20230607  APP_PRINT_INFO2("BT_EVENT_ACL_CONN_DISCONN reason = %d, need_create:%d",
                                        param->acl_conn_disconn.cause, get_create_connect_by_user());*/
            linkback_active_node.is_exit = true;
            linkback_run();
            if (!get_connect_status_flag() &&
                !memcmp(get_connect_status_flag_addr(), param->acl_conn_disconn.bd_addr, 6))
            {
                set_connect_status_flag(true);
                memcpy(get_connect_status_flag_addr(), default_addr, 6);
            }
            uint8_t cur_index = app_bt_bond_get_index_by_addr(param->acl_conn_disconn.bd_addr);
            if (cur_index < MAX_BOND_INFO_NUM)
            {
                if (br_db.bond_device[cur_index].used == true)
                {
                    if (bt_bond_index_get(param->acl_conn_disconn.bd_addr, &pair_idx))
                    {
                        app_bt_bond_inactive_device_by_index(cur_index);
                    }
                    else
                    {
                        app_bt_bond_del_bond_device(param->acl_conn_disconn.bd_addr);
                    }
                    app_bt_bond_save_device_info_to_ftl(br_db.bond_device);

                }
                /*20230607      if (get_create_connect_by_user())
                                {
                                    set_create_connect_by_user(false);
                                    app_create_connection(get_create_connect_by_user_addr(), A2DP_PROFILE_MASK);
                                }*/
                if (!memcmp(br_db.acl_info.acl_reconnect_addr, param->acl_conn_disconn.bd_addr, 6))
                {
                    if (br_db.bond_device[cur_index].device_type == T_DEVICE_TYPE_EAR &&
                        param->acl_conn_disconn.cause == (HCI_ERR | HCI_ERR_REMOTE_USER_TERMINATE))
                    {
                        os_timer_restart(&acl_reconnect_timer, 8000);
                    }
                }
                if (br_db.bond_device[cur_index].device_type == T_DEVICE_TYPE_EAR &&
                    param->acl_conn_disconn.cause == (HCI_ERR | HCI_ERR_PAGE_TIMEOUT))
                {
                    memcpy(br_db.acl_info.acl_reconnect_addr, param->acl_conn_disconn.bd_addr, 6);
                    os_timer_restart(&acl_reconnect_timer, 1000);
                }
                //reconnect the phone cause timeout reason 0x08
                if (br_db.bond_device[cur_index].device_type == T_DEVICE_TYPE_PHONE &&
                    param->acl_conn_disconn.cause == (HCI_ERR | HCI_ERR_CONN_TIMEOUT))
                {
                    reconn_cause_timeout = 1;
                    memcpy(reconn_cause_timeout_addr, param->acl_conn_disconn.bd_addr, 6);
                    app_create_connection(param->acl_conn_disconn.bd_addr, HFP_PROFILE_MASK);
                }
            }
            bt_event_handle(EVENT_BT_DISCONN, param);
        }
        break;

    case BT_EVENT_ACL_CONN_READY:
        {
            /* TODO not set pkt 2m in DUT Test Mode */
            bt_acl_pkt_type_set(param->acl_conn_ready.bd_addr, BT_ACL_PKT_TYPE_2M);
            T_APP_BR_LINK *p_link;
            p_link = app_find_br_link(param->acl_conn_ready.bd_addr);
            if (p_link == NULL)
            {
                return;
            }
            if (app_bt_bond_get_cod_type(param->acl_conn_ready.bd_addr) == T_DEVICE_TYPE_PHONE
                && p_link->acl_link_role != BT_LINK_ROLE_SLAVE)
            {
                bt_link_role_switch(param->acl_conn_ready.bd_addr, false);
            }
        }
        break;

    case BT_EVENT_ACL_CONN_IND:
        {
            uint8_t link_key[16];
            T_BT_LINK_KEY_TYPE type;
            p_link = app_find_br_link(param->acl_conn_ind.bd_addr);
            if (p_link != NULL)
            {
                bt_acl_conn_reject(param->acl_conn_ind.bd_addr, BT_ACL_REJECT_UNACCEPTABLE_ADDR);
            }
            else
            {
#ifdef F_APP_DSP_ONLINE_DEBUG_SUPPORT
                if ((((param->acl_conn_ind.cod & 0x1F00) >> 8) == 0x02) ||
                    (((param->acl_conn_ind.cod & 0x1F00) >> 8) == 0x01))
                {
                    APP_PRINT_INFO1("connect to phone or pc cod = 0x%x", param->acl_conn_ind.cod);
                    bt_acl_conn_accept(param->acl_conn_ind.bd_addr, BT_LINK_ROLE_SLAVE);
                    reconn_cause_timeout = 0;
                    memcpy(reconn_cause_timeout_addr, default_addr, 6);
                }
#else
                if (((param->acl_conn_ind.cod & 0x1F00) >> 8) == 0x02)

                {
                    APP_PRINT_INFO1("connect to phone cod = 0x%x", param->acl_conn_ind.cod);
                    if (app_bt_bond_get_active_num_by_type(T_DEVICE_TYPE_PHONE))
                    {
                        APP_PRINT_INFO0("exist bredr link with phone, reject new link");
                        bt_acl_conn_reject(param->acl_conn_ind.bd_addr, BT_ACL_REJECT_UNACCEPTABLE_ADDR);

                    }
                    else
                    {
                        bt_acl_conn_accept(param->acl_conn_ind.bd_addr, BT_LINK_ROLE_SLAVE);
                        reconn_cause_timeout = 0;
                        memcpy(reconn_cause_timeout_addr, default_addr, 6);
                        set_linkback_status(false);
                    }

                }
#endif
                else if (((param->acl_conn_ind.cod & 0x1F00) >> 8) == 0x04)
                {
                    APP_PRINT_INFO1("connect to audio/video cod = 0x%x", param->acl_conn_ind.cod);
                    if (!bt_bond_key_get(param->acl_conn_ind.bd_addr, link_key, (uint8_t *)&type))
                    {
                        APP_PRINT_INFO0("app_bt_policy reject no linkkey device");
                        bt_acl_conn_reject(param->acl_conn_ind.bd_addr, BT_ACL_REJECT_UNACCEPTABLE_ADDR);
                    }
                    else
                    {
                        if (app_bt_bond_get_active_num_by_type(T_DEVICE_TYPE_EAR))
                        {
                            APP_PRINT_INFO0("exist bredr link with audio/video device, reject new link");
                            bt_acl_conn_reject(param->acl_conn_ind.bd_addr, BT_ACL_REJECT_UNACCEPTABLE_ADDR);
                        }
                        else
                        {
                            bt_acl_conn_accept(param->acl_conn_ind.bd_addr, BT_LINK_ROLE_MASTER);
                            os_timer_stop(&acl_reconnect_timer);
                            memcpy(br_db.acl_info.acl_reconnect_addr, param->acl_conn_ind.bd_addr, 6);
                        }
                    }
                }
                else
                {
                    APP_PRINT_INFO1("unknown device cod = 0x%x", param->acl_conn_ind.cod);
                    bt_acl_conn_reject(param->acl_conn_ind.bd_addr, BT_ACL_REJECT_UNACCEPTABLE_ADDR);
                }
            }
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
            bt_link_pin_code_cfm(param->link_pin_code_req.bd_addr, app_cfg_nv.pin_code,
                                 app_cfg_nv.pin_code_size, true);
        }
        break;

    case BT_EVENT_SCO_CONN_CMPL:
        {
        }
        break;

    case BT_EVENT_SCO_DISCONNECTED:
        {
            bt_sniff_mode_enable(param->sco_disconnected.bd_addr, 0);
        }
        break;

    case BT_EVENT_SDP_ATTR_INFO:
        {
            linkback_handle_sdp_attr_info(param);
        }
        break;

    case BT_EVENT_SDP_DISCOV_CMPL:
        {
            linkback_handle_sdp_discov_cmpl(param);
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

    case BT_EVENT_HFP_CONN_CMPL:
        {
            T_APP_BR_LINK *p_link;
            uint32_t prof;
            p_link = app_find_br_link(param->hfp_conn_cmpl.bd_addr);

            if (p_link)
            {
                if (param->hfp_conn_cmpl.is_hfp)
                {
                    prof = HFP_PROFILE_MASK;
                }
                else
                {
                    prof = HSP_PROFILE_MASK;
                }
                b2s_connected_add_prof(param->hfp_conn_cmpl.bd_addr, prof);
                linkback_handle_profile_conn(param->hfp_conn_cmpl.bd_addr, prof);
            }

        }
        break;

    case BT_EVENT_HFP_SNIFFING_DISCONN_CMPL:
    case BT_EVENT_HFP_DISCONN_CMPL:
        {
            uint32_t prof;
            if (param->hfp_conn_cmpl.is_hfp)
            {
                prof = HFP_PROFILE_MASK;
            }
            else
            {
                prof = HSP_PROFILE_MASK;
            }
            b2s_connected_del_prof(param->hfp_disconn_cmpl.bd_addr, prof);
            if (b2s_connected_no_profs(param->hfp_disconn_cmpl.bd_addr))
            {
                p_link = app_find_br_link(param->hfp_disconn_cmpl.bd_addr);
                if (p_link != NULL)
                {
                    if (p_link->disconn_acl_flg)
                    {
                        p_link->disconn_acl_flg = false;
                        gap_br_send_acl_disconn_req(param->hfp_disconn_cmpl.bd_addr);
                    }
                }
            }
        }
        break;

    case BT_EVENT_PBAP_CONN_CMPL:
        {
            T_APP_BR_LINK *p_link;
            uint32_t prof;
            p_link = app_find_br_link(param->pbap_conn_cmpl.bd_addr);

            if (p_link)
            {
                prof = PBAP_PROFILE_MASK;
                b2s_connected_add_prof(param->pbap_conn_cmpl.bd_addr, prof);
                linkback_handle_profile_conn(param->pbap_conn_cmpl.bd_addr, prof);
            }
        }
        break;

    case BT_EVENT_PBAP_SNIFFING_DISCONN_CMPL:
    case BT_EVENT_PBAP_DISCONN_CMPL:
        {
            uint32_t prof;
            prof = PBAP_PROFILE_MASK;

            b2s_connected_del_prof(param->pbap_disconn_cmpl.bd_addr, prof);
            if (b2s_connected_no_profs(param->pbap_disconn_cmpl.bd_addr))
            {
                p_link = app_find_br_link(param->pbap_disconn_cmpl.bd_addr);
                if (p_link != NULL)
                {
                    if (p_link->disconn_acl_flg)
                    {
                        p_link->disconn_acl_flg = false;
                        gap_br_send_acl_disconn_req(param->pbap_disconn_cmpl.bd_addr);
                    }
                }
            }

        }
        break;

    case BT_EVENT_HID_CONN_CMPL:
        {
            T_APP_BR_LINK *p_link;
            uint32_t prof;
            p_link = app_find_br_link(param->hid_conn_cmpl.bd_addr);

            if (p_link)
            {
                prof = HID_PROFILE_MASK;
                b2s_connected_add_prof(param->hid_conn_cmpl.bd_addr, prof);
                linkback_handle_profile_conn(param->hid_conn_cmpl.bd_addr, prof);
            }
        }
        break;

    case BT_EVENT_HID_SNIFFING_DISCONN_CMPL:
    case BT_EVENT_HID_DISCONN_CMPL:
        {
            uint32_t prof;
            prof = HID_PROFILE_MASK;

            b2s_connected_del_prof(param->hid_disconn_cmpl.bd_addr, prof);
            if (b2s_connected_no_profs(param->hid_disconn_cmpl.bd_addr))
            {
                p_link = app_find_br_link(param->hid_disconn_cmpl.bd_addr);
                if (p_link != NULL)
                {
                    if (p_link->disconn_acl_flg)
                    {
                        p_link->disconn_acl_flg = false;
                        gap_br_send_acl_disconn_req(param->hid_disconn_cmpl.bd_addr);
                    }
                }
            }
        }
        break;

    case BT_EVENT_A2DP_CONN_CMPL:
        {
            T_APP_BR_LINK *p_link;
            uint32_t prof;
            p_link = app_find_br_link(param->a2dp_conn_cmpl.bd_addr);

            if (p_link)
            {

                app_set_active_a2dp_idx(p_link->id);
                if (!memcmp(get_connect_status_flag_addr(), param->a2dp_conn_cmpl.bd_addr, 6))
                {
                    set_connect_status_flag(true);
                    memcpy(get_connect_status_flag_addr(), default_addr, 6);
                }
                prof = A2DP_PROFILE_MASK;
                b2s_connected_add_prof(param->a2dp_conn_cmpl.bd_addr, prof);
                linkback_handle_profile_conn(param->a2dp_conn_cmpl.bd_addr, prof);

                /*20230607                T_IO_MSG set_mode_msg;
                                set_mode_msg.type = IO_MSG_TYPE_WRISTBNAD;
                                set_mode_msg.subtype = IO_MSG_SET_PLAY_MODE;

                                if (br_db.a2dp_cur_role == BT_A2DP_ROLE_SRC)
                                {
                                   set_mode_msg.u.param = MMI_SD_A2DP_SRC_SWITCH;
                                }
                                else if (br_db.a2dp_cur_role == BT_A2DP_ROLE_SNK)
                                {
                                    set_mode_msg.u.param = MMI_SD_A2DP_SNK_SWITCH;
                                }

                                app_send_msg_to_bt_task(&set_mode_msg);*/
            }
        }
        break;

    case BT_EVENT_A2DP_DISCONN_CMPL:
        {
            uint32_t prof;
            prof = A2DP_PROFILE_MASK;

            b2s_connected_del_prof(param->a2dp_disconn_cmpl.bd_addr, prof);
            if (b2s_connected_no_profs(param->a2dp_disconn_cmpl.bd_addr))
            {
                p_link = app_find_br_link(param->a2dp_disconn_cmpl.bd_addr);
                if (p_link != NULL)
                {
                    if (p_link->disconn_acl_flg)
                    {
                        p_link->disconn_acl_flg = false;
                        gap_br_send_acl_disconn_req(param->a2dp_disconn_cmpl.bd_addr);
                    }
                }
            }

            /*20230607            T_IO_MSG set_mode_msg;
                        set_mode_msg.type = IO_MSG_TYPE_WRISTBNAD;
                        set_mode_msg.subtype = IO_MSG_SET_PLAY_MODE;
                        set_mode_msg.u.param = MMI_SD_PLAYBACK_SWITCH;
                        app_send_msg_to_bt_task(&set_mode_msg);*/
            if (get_reconnect_cause())
            {
                app_create_connection(param->a2dp_stream_open_fail.bd_addr, A2DP_PROFILE_MASK);
                set_reconnect_cause(false);
            }
        }
        break;

    case BT_EVENT_AVRCP_CONN_CMPL:
        {
            T_APP_BR_LINK *p_link;
            uint32_t prof;
            p_link = app_find_br_link(param->avrcp_conn_cmpl.bd_addr);

            if (p_link)
            {
                prof = AVRCP_PROFILE_MASK;
                b2s_connected_add_prof(param->avrcp_conn_cmpl.bd_addr, prof);
                linkback_handle_profile_conn(param->avrcp_conn_cmpl.bd_addr, prof);
            }
        }
        break;

    case BT_EVENT_AVRCP_SNIFFING_DISCONN_CMPL:
    case BT_EVENT_AVRCP_DISCONN_CMPL:
        {
            uint32_t prof;
            prof = AVRCP_PROFILE_MASK;

            b2s_connected_del_prof(param->avrcp_disconn_cmpl.bd_addr, prof);
            if (b2s_connected_no_profs(param->avrcp_disconn_cmpl.bd_addr))
            {
                p_link = app_find_br_link(param->avrcp_disconn_cmpl.bd_addr);
                if (p_link != NULL)
                {
                    if (p_link->disconn_acl_flg)
                    {
                        p_link->disconn_acl_flg = false;
                        gap_br_send_acl_disconn_req(param->avrcp_disconn_cmpl.bd_addr);
                    }
                }
            }
        }
        break;

    case BT_EVENT_SPP_CONN_CMPL:
        {
            T_APP_BR_LINK *p_link;
            uint32_t prof;
            p_link = app_find_br_link(param->spp_conn_cmpl.bd_addr);

            if (p_link)
            {
                prof = SPP_PROFILE_MASK;
                b2s_connected_add_prof(param->spp_conn_cmpl.bd_addr, prof);
                linkback_handle_profile_conn(param->spp_conn_cmpl.bd_addr, prof);
            }
        }
        break;

    case BT_EVENT_SPP_SNIFFING_DISCONN_CMPL:
    case BT_EVENT_SPP_DISCONN_CMPL:
        {
            uint32_t prof;
            prof = SPP_PROFILE_MASK;

            b2s_connected_del_prof(param->spp_disconn_cmpl.bd_addr, prof);
            if (b2s_connected_no_profs(param->spp_disconn_cmpl.bd_addr))
            {
                p_link = app_find_br_link(param->spp_disconn_cmpl.bd_addr);
                if (p_link != NULL)
                {
                    if (p_link->disconn_acl_flg)
                    {
                        p_link->disconn_acl_flg = false;
                        gap_br_send_acl_disconn_req(param->spp_disconn_cmpl.bd_addr);
                    }
                }
            }
        }
        break;

    case BT_EVENT_IAP_CONN_CMPL:
        {
            T_APP_BR_LINK *p_link;
            uint32_t prof;
            p_link = app_find_br_link(param->iap_conn_cmpl.bd_addr);

            if (p_link)
            {
                prof = IAP_PROFILE_MASK;
                b2s_connected_add_prof(param->iap_conn_cmpl.bd_addr, prof);
                linkback_handle_profile_conn(param->iap_conn_cmpl.bd_addr, prof);
            }
        }
        break;

    case BT_EVENT_IAP_SNIFFING_DISCONN_CMPL:
    case BT_EVENT_IAP_DISCONN_CMPL:
        {
            uint32_t prof;
            prof = IAP_PROFILE_MASK;

            b2s_connected_del_prof(param->iap_disconn_cmpl.bd_addr, prof);
            if (b2s_connected_no_profs(param->iap_disconn_cmpl.bd_addr))
            {
                p_link = app_find_br_link(param->iap_disconn_cmpl.bd_addr);
                if (p_link != NULL)
                {
                    if (p_link->disconn_acl_flg)
                    {
                        p_link->disconn_acl_flg = false;
                        gap_br_send_acl_disconn_req(param->iap_disconn_cmpl.bd_addr);
                    }
                }
            }
        }
        break;

    case BT_EVENT_REMOTE_NAME_RSP:
        {
            /*20230607  uint16_t unicode_name[MAX_DEVICE_NAME_NUM];
                        uint8_t name_len = 0;
                        APP_PRINT_INFO1("<-- remote name rsp: name = %s", TRACE_STRING(param->remote_name_rsp.name));
                        name_len = watch_utf8_to_unicode((uint8_t *)param->remote_name_rsp.name,
                                                         strlen(param->remote_name_rsp.name), \
                                                         unicode_name, MAX_DEVICE_NAME_NUM);
                        T_APP_BOND_DEVICE *device_temp = app_bt_bond_get_device_by_addr(param->remote_name_rsp.bd_addr);
                        if (device_temp != NULL)
                        {
                            if (device_temp->device_type == T_DEVICE_TYPE_PHONE)
                            {
                                memcpy((uint8_t *)device_temp->device_name, (uint8_t *)unicode_name,
                                       sizeof(device_temp->device_name));
                                device_temp->flag = ((uint16_t)device_temp->flag & 0xFF00) | (name_len & 0x00FF);
                                app_bt_bond_save_device_info_to_ftl(br_db.bond_device);
                            }
                            else
                            {
                                APP_PRINT_INFO1("<-- error device_temp->device_type = ", device_temp->device_type);
                            }
                        }
                        else
                        {
                            app_bt_bond_add_temp_search_device_name_info(param->remote_name_rsp.bd_addr, unicode_name,
                                                                         name_len);
                            if (app_bt_bond_get_temp_search_device(param->acl_conn_success.bd_addr) !=
                                NULL) //inquiry scan connect
                            {
                                if (app_bt_bond_add_search_device(param->acl_conn_success.bd_addr))
                                {
                                    set_connect_status_flag(true);
                                    memcpy(get_connect_status_flag_addr(), default_addr, 6);
                                }
                                app_bt_bond_save_device_info_to_ftl(br_db.bond_device);
                                bt_policy_set_radio();
                            }
                        }*/
        }
        break;

    default:
        break;
    }
}

void enter_state(T_STATE state)
{
    bool state_changed = false;

    APP_PRINT_INFO2("BT status cur_state = 0x%x, new state = 0x%x ", cur_state, state);
    if (cur_state != state)
    {
        cur_state = state;
        state_changed = true;
    }

    if (state_changed)
    {
        switch (cur_state)
        {
        case STATE_STARTUP:
            {
                enter_state(STATE_STANDBY);
            }
            break;
        case STATE_STANDBY:
            {
                if (!app_bt_bond_check_exist_device_info())
                {
                    bt_device_mode_set(BT_DEVICE_MODE_DISCOVERABLE_CONNECTABLE);
                }
                else
                {
                    bt_policy_set_radio();
                }
                if (get_linkback_status())
                {
                    //enter_state(STATE_LINKBACK);
                    os_timer_restart(&reset_hfp_reconnect, 5000);
                }
            }
            break;
        case STATE_LINKBACK:
            {
                if (get_linkback_status() && app_bt_bond_linkback(0))
                {
                    set_linkback_status(false);
                    linkback_run();
                }
            }
            break;
        case STATE_CONNECTED_1:
            {
                bt_policy_set_radio();
            }
            break;
        case STATE_CONNECTED_2:
            {
                bt_device_mode_set(BT_DEVICE_MODE_IDLE);
            }
            break;
        case STATE_PAIRING:
            {

            }
            break;
        case STATE_INQUIRY:
            {

            }
            break;
        default:
            break;
        }
    }

}

void bt_event_handle(T_EVENT event, void *param)
{
    APP_PRINT_INFO2("bt_event_handle event = 0x%x, cur_state = 0x%x", event, cur_state);

    switch (event)
    {
    case EVENT_STARTUP:
        {
            linkback_todo_queue_init();
            linkback_active_node_init();
            enter_state(STATE_STARTUP);
        }
        break;
    case EVENT_BT_CONN:
        {
            if (app_bt_bond_get_active_num_by_type(T_DEVICE_TYPE_PHONE) &&
                app_bt_bond_get_active_num_by_type(T_DEVICE_TYPE_EAR))
            {
                enter_state(STATE_CONNECTED_2);
            }
            else
            {
                enter_state(STATE_CONNECTED_1);
            }
        }
        break;
    case EVENT_BT_DISCONN:
        {
            if (!app_bt_bond_get_active_num_by_type(T_DEVICE_TYPE_PHONE) &&
                !app_bt_bond_get_active_num_by_type(T_DEVICE_TYPE_EAR))
            {
                enter_state(STATE_STANDBY);
            }
            else
            {
                enter_state(STATE_CONNECTED_1);
            }
        }
        break;
    case EVENT_BT_CONN_FAIL:
        {
            if (cur_state == STATE_LINKBACK)
            {
                if (!linkback_active_node.is_valid)
                {
                    enter_state(STATE_STANDBY);
                }
            }
        }
        break;
    default:
        break;
    }
}

static void reset_hfp_reconnect_callback(void *pxTimer)
{
    enter_state(STATE_LINKBACK);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void app_bt_policy_init(void)
{
    APP_PRINT_INFO0("app_bt_policy_init");
    bt_mgr_cback_register(app_bt_policy_cback);
    app_bt_bond_load_device_info_from_ftl(br_db.bond_device);
    app_bt_bond_free_all_device();
    os_timer_create(&acl_reconnect_timer, "acl reconnect timer", 1,
                    8 * 1000, false,
                    app_acl_reconnect_callback);
    os_timer_create(&reset_hfp_reconnect, "acl reconnect timer", 1,
                    2 * 1000, false,
                    reset_hfp_reconnect_callback);
}

void app_bt_policy_disconnect(uint8_t *bd_addr, uint32_t plan_profs)
{
    linkback_profile_disconnect_start(bd_addr, plan_profs);
}

