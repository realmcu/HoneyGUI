/*
* Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
*/

#include <string.h>
#include "trace.h"
#include "bt_bond.h"
#include "bt_a2dp.h"
#include "bt_avrcp.h"
#include "bt_hfp.h"
#include "bt_rdtp.h"
#include "bt_iap.h"
#include "bt_pbap.h"
#include "bt_spp.h"
#include "gap_le.h"
#include "engage.h"
#include "app_cfg.h"
#include "app_linkback.h"
#include "app_br_link_util.h"
#include "app_bond.h"
#include "app_bt_policy_int.h"
#include "dp_br_info.h"


const uint32_t prof_arr[] =
{
    A2DP_PROFILE_MASK,
    AVRCP_PROFILE_MASK,
    HFP_PROFILE_MASK,
    HSP_PROFILE_MASK,
    DID_PROFILE_MASK,
    SPP_PROFILE_MASK,
    IAP_PROFILE_MASK,
    PBAP_PROFILE_MASK,
};

const uint32_t prof_relation[3][2] =
{
    {A2DP_PROFILE_MASK, AVRCP_PROFILE_MASK},
    {HFP_PROFILE_MASK, PBAP_PROFILE_MASK},
    {HSP_PROFILE_MASK, PBAP_PROFILE_MASK},
};



static T_LINKBACK_NODE_ITEM linkback_node_item[6] = {0};
static T_LINKBACK_TODO_QUEUE linkback_todo_queue = {0};
T_LINKBACK_ACTIVE_NODE linkback_active_node = {0};
extern uint8_t default_addr[6];

void active_node(void)
{
    ENGAGE_PRINT_TRACE2("active_node: bd_addr %s, total retry time %d s",
                        TRACE_BDADDR(linkback_active_node.linkback_node.bd_addr), linkback_active_node.retry_timeout);

    ENGAGE_PRINT_TRACE4("active_node: is_valid %d, is_exit %d, is_force %d, is_sdp_ok %d",
                        linkback_active_node.is_valid, linkback_active_node.is_exit,
                        linkback_active_node.linkback_node.is_force, linkback_active_node.is_sdp_ok);

    ENGAGE_PRINT_TRACE4("active_node: plan_profs 0x%08x, remain_profs 0x%08x, doing_prof 0x%08x, prof_retry_cnt %d",
                        linkback_active_node.linkback_node.plan_profs, linkback_active_node.remain_profs,
                        linkback_active_node.doing_prof, linkback_active_node.prof_retry_cnt);
}


bool linkback_profile_search_start(uint8_t *bd_addr, uint32_t prof, bool is_special,
                                   T_LINKBACK_SEARCH_PARAM *param)
{
    bool ret = true;
    T_GAP_UUID_DATA uuid;
    T_GAP_UUID_TYPE uuid_type = GAP_UUID16;

    ENGAGE_PRINT_TRACE2("linkback_profile_search_start: bd_addr %s, prof 0x%08x", TRACE_BDADDR(bd_addr),
                        prof);

    if (DID_PROFILE_MASK == prof)
    {
        if (gap_br_start_did_discov(bd_addr) != GAP_CAUSE_SUCCESS)
        {
            ret = false;
        }

        return ret;
    }

    switch (prof)
    {
    case A2DP_PROFILE_MASK:
        {
            if (is_special)
            {
                if (param->is_source)
                {
                    uuid.uuid_16 = UUID_AUDIO_SOURCE;
                }
                else
                {
                    uuid.uuid_16 = UUID_AUDIO_SINK;
                }
            }
            else
            {
                uuid.uuid_16 = UUID_AUDIO_SOURCE;
            }
        }
        break;

    case AVRCP_PROFILE_MASK:
        {
            if (is_special)
            {
                if (param->is_target)
                {
                    uuid.uuid_16 = UUID_AV_REMOTE_CONTROL_TARGET;
                }
                else
                {
                    uuid.uuid_16 = UUID_AV_REMOTE_CONTROL_CONTROLLER;
                }
            }
            else
            {
                uuid.uuid_16 = UUID_AV_REMOTE_CONTROL_TARGET;
            }
        }
        break;

    case HFP_PROFILE_MASK:
        {
            uuid.uuid_16 = UUID_HANDSFREE_AUDIO_GATEWAY;
        }
        break;

    case HSP_PROFILE_MASK:
        {
            uuid.uuid_16 = UUID_HEADSET_AUDIO_GATEWAY;
        }
        break;

    case SPP_PROFILE_MASK:
        {
            uuid.uuid_16 = UUID_RFCOMM;
        }
        break;

    case IAP_PROFILE_MASK:
        {
            uint8_t iap_service_class[16] = {0x00, 0x00, 0x00, 0x00, 0xde, 0xca, 0xfa, 0xde,
                                             0xde, 0xca, 0xde, 0xaf, 0xde, 0xca, 0xca, 0xfe
                                            };
            uuid_type = GAP_UUID128;
            memcpy(&uuid.uuid_128[0], iap_service_class, 16);
        }
        break;

    case PBAP_PROFILE_MASK:
        {
            uuid.uuid_16 = UUID_PBAP;
        }
        break;

    default:
        {
            ret = false;
        }
        break;
    }

    if (ret)
    {
        if (gap_br_start_sdp_discov(bd_addr, uuid_type, uuid) != GAP_CAUSE_SUCCESS)
        {
            ret = false;
        }
    }

    return ret;
}

bool linkback_profile_connect_start(uint8_t *bd_addr, uint32_t prof, T_LINKBACK_CONN_PARAM *param)
{
    bool ret = true;

    ENGAGE_PRINT_TRACE2("linkback_profile_connect_start: bd_addr %s, prof 0x%08x",
                        TRACE_BDADDR(bd_addr), prof);

    switch (prof)
    {
    case A2DP_PROFILE_MASK:
        ret = bt_a2dp_connect_req(bd_addr, param->protocol_version);
        break;

    case AVRCP_PROFILE_MASK:
        ret = bt_avrcp_connect_req(bd_addr);
        break;

    case HFP_PROFILE_MASK:
        ret = bt_hfp_connect_req(bd_addr, param->server_channel, true);
        break;

    case HSP_PROFILE_MASK:
        ret = bt_hfp_connect_req(bd_addr, param->server_channel, false);
        break;

    case SPP_PROFILE_MASK:
        ret = bt_spp_connect_req(bd_addr, param->server_channel, 1012, 7, param->local_server_chann);
        break;

    case IAP_PROFILE_MASK:
        ret = bt_iap_connect_req(bd_addr, param->server_channel, 1012, 7);
        break;

    case PBAP_PROFILE_MASK:
        ret = bt_pbap_connect_req(bd_addr, param->server_channel, param->feature);
        break;

    default:
        ret = false;
        break;
    }

    return ret;
}

void linkback_profile_disconnect_start(uint8_t *bd_addr, uint32_t profs)
{
    ENGAGE_PRINT_TRACE2("linkback_profile_disconnect_start: bd_addr %s, prof 0x%08x",
                        TRACE_BDADDR(bd_addr), profs);

    if (0 != profs)
    {
        if (profs & A2DP_PROFILE_MASK)
        {
            bt_a2dp_disconnect_req(bd_addr);
        }

        if (profs & AVRCP_PROFILE_MASK)
        {
            bt_avrcp_disconnect_req(bd_addr);
        }

        if (profs & (HFP_PROFILE_MASK | HSP_PROFILE_MASK))
        {
            bt_hfp_disconnect_req(bd_addr);
        }

        if (profs & SPP_PROFILE_MASK)
        {
            bt_spp_disconnect_all_req(bd_addr);
        }

        if (profs & IAP_PROFILE_MASK)
        {
            bt_iap_disconnect_req(bd_addr);
        }

        if (profs & PBAP_PROFILE_MASK)
        {
            bt_pbap_disconnect_req(bd_addr);
        }

        if (profs & RDTP_PROFILE_MASK)
        {
            bt_rdtp_disconnect_req(bd_addr);
        }
    }
    else
    {
        gap_br_send_acl_disconn_req(bd_addr);
    }
}

void linkback_todo_queue_init(void)
{
    memset(linkback_node_item, 0, sizeof(linkback_node_item));

    linkback_todo_queue.head = NULL;
    linkback_todo_queue.tail = NULL;
}

T_LINKBACK_NODE_ITEM *linkback_todo_queue_malloc_node_item(void)
{
    uint32_t i;
    T_LINKBACK_NODE_ITEM *p_item = NULL;

    for (i = 0; i < sizeof(linkback_node_item) / sizeof(linkback_node_item[0]); i++)
    {
        if (!linkback_node_item[i].is_used)
        {
            linkback_node_item[i].is_used = true;
            p_item = &linkback_node_item[i];
            break;
        }
    }

    return p_item;
}

void linkback_todo_queue_free_node_item(T_LINKBACK_NODE_ITEM *p_item)
{
    if (p_item != NULL)
    {
        p_item->is_used = false;
    }
}

void linkback_todo_queue_all_node(void)
{
    int i;
    T_LINKBACK_NODE_ITEM *p_item;

    i = 0;
    p_item = (T_LINKBACK_NODE_ITEM *)linkback_todo_queue.head;
    while (p_item != NULL)
    {
        ENGAGE_PRINT_TRACE4("linkback_todo_queue_all_node: item_no %d, bd_addr %s, plan_profs 0x%x, is_force %d",
                            ++i, TRACE_BDADDR(p_item->linkback_node.bd_addr), p_item->linkback_node.plan_profs,
                            p_item->linkback_node.is_force);

        p_item = p_item->next;
    }
}

void linkback_todo_queue_insert_normal_node(uint8_t *bd_addr, uint32_t plan_profs,
                                            uint16_t retry_timeout)
{

    T_LINKBACK_NODE_ITEM *p_item;

    p_item = linkback_todo_queue_malloc_node_item();
    if (p_item != NULL)
    {
        memcpy(p_item->linkback_node.bd_addr, bd_addr, 6);
        p_item->linkback_node.plan_profs = plan_profs;
        p_item->linkback_node.is_force = false;
        p_item->linkback_node.is_special = true;
        p_item->linkback_node.retry_timeout = retry_timeout;
        if (br_db.a2dp_info.audio_play_mode == MODE_APP_A2DP_SNK)
        {
            p_item->linkback_node.search_param.is_source = true;
        }
        else
        {
            p_item->linkback_node.search_param.is_source = false;
        }

        p_item->next = NULL;
        if (linkback_todo_queue.head == NULL)
        {
            linkback_todo_queue.head = p_item;
        }
        else
        {
            linkback_todo_queue.tail->next = p_item;
        }
        linkback_todo_queue.tail = p_item;
    }

    ENGAGE_PRINT_TRACE1("linkback_todo_queue_insert_normal_node: p_item = 0x%p", p_item);

    linkback_todo_queue_all_node();
}

void linkback_todo_queue_insert_force_node(uint8_t *bd_addr, uint32_t plan_profs,
                                           bool is_special, T_LINKBACK_SEARCH_PARAM *search_param, bool check_bond_flag,
                                           uint16_t retry_timeout)
{
    T_LINKBACK_NODE_ITEM *p_item;

    p_item = linkback_todo_queue_malloc_node_item();
    if (p_item != NULL)
    {
        memcpy(p_item->linkback_node.bd_addr, bd_addr, 6);
        p_item->linkback_node.plan_profs = plan_profs;
        p_item->linkback_node.is_force = true;
        p_item->linkback_node.is_special = is_special;
        p_item->linkback_node.retry_timeout = retry_timeout;
        memcpy(&p_item->linkback_node.search_param, search_param, sizeof(T_LINKBACK_SEARCH_PARAM));
        p_item->linkback_node.check_bond_flag = check_bond_flag;

        if (linkback_todo_queue.head == NULL)
        {
            linkback_todo_queue.tail = p_item;
            p_item->next = NULL;
        }
        else
        {
            p_item->next = linkback_todo_queue.head;
        }
        linkback_todo_queue.head = p_item;
    }

    ENGAGE_PRINT_TRACE1("linkback_todo_queue_insert_force_node: p_item = 0x%p", p_item);

    linkback_todo_queue_all_node();
}

bool linkback_todo_queue_take_first_node(T_LINKBACK_NODE *node)
{
    bool ret = false;
    T_LINKBACK_NODE_ITEM *p_item;

    p_item = (T_LINKBACK_NODE_ITEM *)linkback_todo_queue.head;
    if (p_item != NULL)
    {
        memcpy(node, &p_item->linkback_node, sizeof(T_LINKBACK_NODE));

        if (linkback_todo_queue.head->next == NULL)
        {
            linkback_todo_queue.head = linkback_todo_queue.tail = NULL;
        }
        else
        {
            linkback_todo_queue.head = linkback_todo_queue.head->next;
        }

        linkback_todo_queue_free_node_item(p_item);
        ret = true;
    }

    ENGAGE_PRINT_TRACE1("linkback_todo_queue_take_first_node: p_item = 0x%p", p_item);

    linkback_todo_queue_all_node();

    return ret;
}

void linkback_todo_queue_remove_plan_profs(uint8_t *bd_addr, uint32_t plan_profs)
{
    T_LINKBACK_NODE_ITEM *p_item;

    p_item = (T_LINKBACK_NODE_ITEM *)linkback_todo_queue.head;
    while (p_item != NULL)
    {
        if (!memcmp(p_item->linkback_node.bd_addr, bd_addr, 6))
        {
            p_item->linkback_node.plan_profs &= ~plan_profs;
        }
        p_item = p_item->next;
    }
}

void linkback_todo_queue_delete_all_node(void)
{
    T_LINKBACK_NODE_ITEM *p_item;

    p_item = (T_LINKBACK_NODE_ITEM *)linkback_todo_queue.head;
    while (p_item != NULL)
    {
        linkback_todo_queue_free_node_item(p_item);
        p_item = p_item->next;
    }

    linkback_todo_queue.head = NULL;
    linkback_todo_queue.tail = NULL;
}

void linkback_active_node_init(void)
{
    memset(&linkback_active_node, 0, sizeof(linkback_active_node));
}

void linkback_active_node_load_doing_prof(void)
{
    uint32_t i;

    linkback_active_node.doing_prof = 0;

    for (i = 0; i < sizeof(prof_arr) / sizeof(prof_arr[0]); i++)
    {
        if (prof_arr[i] & linkback_active_node.remain_profs)
        {
            linkback_active_node.doing_prof = prof_arr[i];
            break;
        }
    }
}

void linkback_active_node_load(T_LINKBACK_NODE *node)
{
    ENGAGE_PRINT_TRACE0("linkback_active_node_load");

    memset(&linkback_active_node, 0, sizeof(linkback_active_node));

    if (node->plan_profs)
    {
        linkback_active_node.is_valid = true;
        linkback_active_node.is_exit = false;
        memcpy(&linkback_active_node.linkback_node, node, sizeof(T_LINKBACK_NODE));
        linkback_active_node.retry_timeout = node->retry_timeout;
        linkback_active_node.remain_profs = node->plan_profs;
        linkback_active_node.is_sdp_ok = false;
        linkback_active_node.prof_retry_cnt = 0;

        linkback_active_node_load_doing_prof();

        active_node();
    }
}

void linkback_active_node_step_suc_adjust_remain_profs(void)
{
    ENGAGE_PRINT_TRACE0("linkback_active_node_step_suc_adjust_remain_profs");

    linkback_active_node.remain_profs &= ~linkback_active_node.doing_prof;
    linkback_active_node.is_sdp_ok = false;
    linkback_active_node.prof_retry_cnt = 0;
    memset(&linkback_active_node.linkback_conn_param, 0, sizeof(T_LINKBACK_CONN_PARAM));

    linkback_active_node_load_doing_prof();

    active_node();
}

void linkback_active_node_step_fail_adjust_remain_profs(void)
{
    uint32_t i;

    ENGAGE_PRINT_TRACE0("linkback_active_node_step_fail_adjust_remain_profs");

    linkback_active_node.is_sdp_ok = false;

    if (++linkback_active_node.prof_retry_cnt > 2)
    {
        APP_PRINT_INFO1("linkback_active_node.prof_retry_cnt = %d", linkback_active_node.prof_retry_cnt);
        linkback_active_node.remain_profs &= ~linkback_active_node.doing_prof;
        linkback_active_node.prof_retry_cnt = 0;
        memset(&linkback_active_node.linkback_conn_param, 0, sizeof(T_LINKBACK_CONN_PARAM));

        for (i = 0; i < sizeof(prof_relation) / sizeof(prof_relation[0]); i++)
        {
            if (prof_relation[i][0] == linkback_active_node.doing_prof)
            {
                linkback_active_node.remain_profs &= ~prof_relation[i][1];
            }
        }

        linkback_active_node_load_doing_prof();
    }
    APP_PRINT_INFO1("111linkback_active_node.prof_retry_cnt = %d", linkback_active_node.prof_retry_cnt);

    active_node();
}

void linkback_active_node_src_conn_fail_adjust_remain_profs(void)
{
    ENGAGE_PRINT_TRACE0("linkback_active_node_src_conn_fail_adjust_remain_profs");

    linkback_active_node.remain_profs = linkback_active_node.linkback_node.plan_profs;
    linkback_active_node.is_sdp_ok = false;
    linkback_active_node.prof_retry_cnt = 0;
    memset(&linkback_active_node.linkback_conn_param, 0, sizeof(T_LINKBACK_CONN_PARAM));

    linkback_active_node_load_doing_prof();

    active_node();
}

void linkback_active_node_remain_profs_add(uint32_t profs, bool check_bond_flag)
{
    ENGAGE_PRINT_TRACE0("linkback_active_node_remain_profs_add");

    linkback_active_node.remain_profs |= profs;
    linkback_active_node.linkback_node.check_bond_flag = check_bond_flag;

    active_node();
}

void linkback_active_node_remain_profs_sub(uint32_t profs)
{
    ENGAGE_PRINT_TRACE0("linkback_active_node_remain_profs_sub");

    linkback_active_node.remain_profs &= ~profs;

    active_node();
}

bool linkback_active_node_judge_cur_conn_addr(uint8_t *bd_addr)
{
    bool ret = false;

    if (linkback_active_node.is_valid)
    {
        if (!memcmp(linkback_active_node.linkback_node.bd_addr, bd_addr, 6))
        {
            ret = true;
        }
    }

    return ret;
}

bool linkback_active_node_judge_cur_conn_prof(uint8_t *bd_addr, uint32_t prof)
{
    bool ret = false;

    if (linkback_active_node_judge_cur_conn_addr(bd_addr))
    {
        if (linkback_active_node.doing_prof == prof &&
            linkback_active_node.is_sdp_ok)
        {
            ret = true;
        }
    }

    return ret;
}

bool linkback_load_bond_list(uint16_t retry_timeout)
{
    bool    ret = false;
    uint8_t i;
    uint8_t bd_addr[6];
    uint8_t bond_num;
    uint32_t bond_flag;
    uint32_t plan_profs;

    linkback_todo_queue_delete_all_node();
    bond_num = bt_bond_num_get();

    APP_PRINT_INFO1("linkback_load_bond_list bond_num = %d", bond_num);
    //insert phone to linkback queue
    for (i = 1; i <= bond_num; i++)
    {
        bond_flag = 0;
        if (bt_bond_addr_get(i, bd_addr))
        {
            bt_bond_flag_get(bd_addr, &bond_flag);
            if (bond_flag & (BOND_FLAG_HFP | BOND_FLAG_HSP))
            {
                plan_profs = (HFP_PROFILE_MASK | HSP_PROFILE_MASK);
                linkback_todo_queue_insert_normal_node(bd_addr, plan_profs, retry_timeout);
                ret = true;
            }
        }
    }
    //insert buds to linkback queue
    for (i = 1; i <= bond_num; i++)
    {
        bond_flag = 0;
        if (bt_bond_addr_get(i, bd_addr))
        {
            bt_bond_flag_get(bd_addr, &bond_flag);
            if ((bond_flag & BOND_FLAG_A2DP) && ((bond_flag & (BOND_FLAG_HFP | BOND_FLAG_HSP)) == 0))
            {
                plan_profs = (A2DP_PROFILE_MASK | AVRCP_PROFILE_MASK);
                linkback_todo_queue_insert_normal_node(bd_addr, plan_profs, retry_timeout);
                ret = true;
            }
        }
    }

    linkback_todo_queue_all_node();

    return ret;
}

bool check_bond_smart_phone(void)
{
    bool    ret = false;
    uint8_t i;
    uint8_t bd_addr[6];
    uint8_t bond_num;
    uint32_t bond_flag;

    bond_num = bt_bond_num_get();

    for (i = 1; i <= bond_num; i++)
    {
        bond_flag = 0;
        if (bt_bond_addr_get(i, bd_addr))
        {
            bt_bond_flag_get(bd_addr, &bond_flag);
            if (bond_flag & (BOND_FLAG_HFP | BOND_FLAG_HSP))
            {
                ret = true;
                break;
            }
        }
    }

    return ret;
}

bool check_bond_buds(void)
{
    bool    ret = false;
    uint8_t i;
    uint8_t bd_addr[6];
    uint8_t bond_num;
    uint32_t bond_flag;

    bond_num = bt_bond_num_get();

    for (i = 1; i <= bond_num; i++)
    {
        bond_flag = 0;
        if (bt_bond_addr_get(i, bd_addr))
        {
            bt_bond_flag_get(bd_addr, &bond_flag);
            if (bond_flag & BOND_FLAG_A2DP)
            {
                ret = true;
                break;
            }
        }
    }

    return ret;
}


bool linkback_check_bond_flag(uint8_t *bd_addr, uint32_t prof)
{
    uint8_t i;
    uint8_t addr[6];
    uint8_t bond_num;
    uint32_t bond_flag;
    uint32_t bond_profs;

    bond_num = bt_bond_num_get();
    for (i = 1; i <= bond_num; i++)
    {
        bond_flag = 0;
        if (bt_bond_addr_get(i, addr))
        {
            if (!memcmp(bd_addr, addr, 6))
            {
                bt_bond_flag_get(bd_addr, &bond_flag);
                bond_profs = get_profs_by_bond_flag(bond_flag);
                if (prof & bond_profs)
                {
                    return true;
                }
            }
        }
    }

    return false;
}


bool check_br_link_connected_profile(uint8_t *bd_addr, uint32_t *profs)
{
    T_APP_BR_LINK *p_link;

    p_link = app_find_br_link(bd_addr);
    if (p_link != NULL)
    {
        if (profs != NULL)
        {
            *profs = p_link->connected_profile;
        }
        return true;
    }
    else
    {
        return false;
    }
}

void linkback_run(void)
{
    T_LINKBACK_NODE node;
    uint32_t profs;

    ENGAGE_PRINT_TRACE0("linkback_run: start");

RETRY:
    if (!linkback_active_node.is_valid)
    {
        if (linkback_todo_queue_take_first_node(&node))
        {
            linkback_active_node_load(&node);

            if (linkback_active_node.is_valid)
            {
//                gap_stop_timer(&timer_handle_linkback);
//                gap_start_timer(&timer_handle_linkback, "linkback", timer_queue_id,
//                                TIMER_ID_LINKBACK_TIMEOUT, 0, linkback_active_node.retry_timeout * 1000);
            }
        }
    }

    if (!linkback_active_node.is_valid)
    {
        ENGAGE_PRINT_TRACE0("linkback_run: have no valid node, finish");

        goto EXIT;
    }

    if (linkback_active_node.is_exit)
    {
        linkback_active_node.is_valid = false;

        goto RETRY;
    }

    if (0 == linkback_active_node.remain_profs)
    {
        linkback_active_node.is_valid = false;

        goto RETRY;
    }

    if (check_br_link_connected_profile(linkback_active_node.linkback_node.bd_addr, &profs))
    {
        if (profs & linkback_active_node.doing_prof)
        {
            ENGAGE_PRINT_TRACE1("linkback_run: prof 0x%08x, already connected",
                                linkback_active_node.doing_prof);
            if (linkback_active_node.doing_prof == A2DP_PROFILE_MASK)
            {
                set_connect_status_flag(true);
                memcpy(get_connect_status_flag_addr(), default_addr, 6);
            }
            linkback_active_node_step_suc_adjust_remain_profs();

            goto RETRY;
        }
        else
        {
            goto LINKBACK;
        }
    }
    else
    {
        if (linkback_active_node.linkback_node.is_force)
        {
            goto LINKBACK;
        }
        else
        {
//            if (b2s_connected_is_full())
//            {
//                ENGAGE_PRINT_TRACE0("linkback_run: b2s is full, abort this node");

//                linkback_active_node.is_valid = false;

//                goto RETRY;
//            }
//            else
            {
                goto LINKBACK;
            }
        }
    }

LINKBACK:

    if (linkback_active_node.linkback_node.check_bond_flag)
    {
        if (!linkback_check_bond_flag(linkback_active_node.linkback_node.bd_addr,
                                      linkback_active_node.doing_prof))
        {
            linkback_active_node_step_fail_adjust_remain_profs();

            goto RETRY;
        }
    }

    if (!linkback_profile_search_start(linkback_active_node.linkback_node.bd_addr,
                                       linkback_active_node.doing_prof, linkback_active_node.linkback_node.is_special,
                                       &linkback_active_node.linkback_node.search_param))
    {
        linkback_active_node_step_fail_adjust_remain_profs();

        goto RETRY;
    }
    else
    {
        ENGAGE_PRINT_TRACE0("linkback_run: wait search result");

        goto EXIT;
    }

EXIT:
    return;
}

void linkback_handle_sdp_attr_info(T_BT_EVENT_PARAM *param)
{
    linkback_active_node.linkback_conn_param.protocol_version =
        param->sdp_attr_info.info.protocol_version;
    linkback_active_node.linkback_conn_param.server_channel =
        param->sdp_attr_info.info.server_channel;
    linkback_active_node.linkback_conn_param.feature =
        param->sdp_attr_info.info.supported_feat;

    linkback_active_node.is_sdp_ok = true;
}

void linkback_handle_sdp_discov_cmpl(T_BT_EVENT_PARAM *param)
{
    APP_PRINT_INFO1("check connect profile = %d",
                    check_br_link_connected_profile(param->sdp_discov_cmpl.bd_addr, NULL));
    APP_PRINT_INFO1("param->sdp_discov_cmpl.cause = 0x%x", param->sdp_discov_cmpl.cause);
    if ((!check_br_link_connected_profile(param->sdp_discov_cmpl.bd_addr, NULL)) ||
        (param->sdp_discov_cmpl.cause == (HCI_ERR | HCI_ERR_REMOTE_USER_TERMINATE)) ||
        (param->sdp_discov_cmpl.cause == (HCI_ERR | HCI_ERR_CONN_TIMEOUT)) ||
        (param->sdp_discov_cmpl.cause == (HCI_ERR | HCI_ERR_LMP_RESPONSE_TIMEOUT)))
    {
        ENGAGE_PRINT_TRACE0("state_afe_linkback_event_handle: wait EVENT_SRC_CONN_FAIL");
        if (linkback_active_node.is_valid)
        {
            linkback_active_node.is_valid = false;
            linkback_run();
        }
    }
    else
    {
        APP_PRINT_INFO1("linkback_active_node.is_sdp_ok = %d", linkback_active_node.is_sdp_ok);
        if (linkback_active_node.is_sdp_ok)
        {
            if (linkback_profile_connect_start(linkback_active_node.linkback_node.bd_addr,
                                               linkback_active_node.doing_prof, &linkback_active_node.linkback_conn_param))
            {
                if (PBAP_PROFILE_MASK == linkback_active_node.doing_prof)
                {
                    linkback_active_node_step_suc_adjust_remain_profs();
                    linkback_run();
                }
            }
            else
            {
                linkback_active_node_step_fail_adjust_remain_profs();
                linkback_run();
            }
        }
        else
        {
            linkback_active_node_step_fail_adjust_remain_profs();
            linkback_run();
        }
    }
}

void linkback_handle_profile_conn(uint8_t *bd_addr, uint32_t prof)
{
    if (linkback_active_node_judge_cur_conn_prof(bd_addr, prof))
    {
        linkback_active_node_step_suc_adjust_remain_profs();
        linkback_run();
    }
}


void app_create_connection(uint8_t *bd_addr, uint32_t profile_mask)
{
    linkback_todo_queue_insert_normal_node(bd_addr, profile_mask, 0);
    linkback_run();
}
