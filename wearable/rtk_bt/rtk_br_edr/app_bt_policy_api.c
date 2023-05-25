/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <string.h>
#include "trace.h"
#include "gap_timer.h"
#include "gap_le.h"
#include "btm.h"

#include "engage.h"
#include "app_cfg.h"
#include "app_main.h"
#include "app_bt_policy_api.h"
#include "app_bt_policy_int.h"
#include "app_linkback.h"
#include "app_relay.h"

extern T_LINKBACK_ACTIVE_NODE linkback_active_node;

extern T_BP_STATE bp_state;
extern T_EVENT cur_event;

extern T_BT_DEVICE_MODE radio_mode;

extern bool b2b_connected;
extern T_B2S_CONNECTED b2s_connected;
extern bool first_connect_sync_default_volume_to_src;

void app_bt_policy_startup(T_BP_IND_FUN fun, bool at_once_trigger)
{
    T_STARTUP_PARAM param;

    param.ind_fun = fun;
    param.at_once_trigger = at_once_trigger;
    state_machine(EVENT_STARTUP, &param);
}

void app_bt_policy_shutdown(void)
{
    state_machine(EVENT_SHUTDOWN, NULL);
}

void app_bt_policy_stop(void)
{
    state_machine(EVENT_STOP, NULL);
}

void app_bt_policy_msg_prof_conn(uint8_t *bd_addr, uint32_t prof)
{
    T_BT_PARAM bt_param;

    memset(&bt_param, 0, sizeof(T_BT_PARAM));

    bt_param.bd_addr = bd_addr;
    bt_param.prof = prof;

    state_machine(EVENT_PROFILE_CONN_SUC, &bt_param);
}

void app_bt_policy_msg_prof_disconn(uint8_t *bd_addr, uint32_t prof, uint16_t cause)
{
    T_BT_PARAM bt_param;

    memset(&bt_param, 0, sizeof(T_BT_PARAM));

    bt_param.bd_addr = bd_addr;
    bt_param.prof = prof;
    bt_param.cause = cause;

    state_machine(EVENT_PROFILE_DISCONN, &bt_param);
}

void app_bt_policy_enter_pairing_mode(bool force, bool visiable)
{
    T_BT_PARAM bt_param;

    memset(&bt_param, 0, sizeof(T_BT_PARAM));

    bt_param.is_force = force;
    bt_param.is_visiable = visiable;

    state_machine(EVENT_DEDICATED_ENTER_PAIRING_MODE, &bt_param);
}

void app_bt_policy_exit_pairing_mode(void)
{
    state_machine(EVENT_DEDICATED_EXIT_PAIRING_MODE, NULL);
}

void app_bt_policy_ota_mode(bool connectable)
{
    T_BT_PARAM bt_param;

    memset(&bt_param, 0, sizeof(T_BT_PARAM));

    bt_param.is_connectable = connectable;

    state_machine(EVENT_OTA_MODE, &bt_param);
}

#if (F_APP_OTA_TOOLING_SUPPORT == 1)
void app_bt_policy_start_ota_shaking(void)
{
    state_machine(EVENT_START_OTA_SHAKING, NULL);
}

void app_bt_policy_enter_ota_mode(bool connectable)
{
    T_BT_PARAM bt_param;

    memset(&bt_param, 0, sizeof(T_BT_PARAM));

    bt_param.is_connectable = connectable;

    state_machine(EVENT_ENTER_OTA_MODE, &bt_param);
}
#endif

void app_bt_policy_default_connect(uint8_t *bd_addr, uint32_t plan_profs, bool check_bond_flag)
{
    T_BT_PARAM bt_param;

    memset(&bt_param, 0, sizeof(T_BT_PARAM));

    bt_param.bd_addr = bd_addr;
    bt_param.prof = plan_profs;
    bt_param.is_special = false;
    bt_param.check_bond_flag = check_bond_flag;
    state_machine(EVENT_DEDICATED_CONNECT, &bt_param);
}

void app_bt_policy_special_connect(uint8_t *bd_addr, uint32_t plan_prof,
                                   T_LINKBACK_SEARCH_PARAM *search_param)
{
    T_BT_PARAM bt_param;

    memset(&bt_param, 0, sizeof(T_BT_PARAM));

    bt_param.bd_addr = bd_addr;
    bt_param.prof = plan_prof;
    bt_param.is_special = true;
    bt_param.search_param = search_param;
    bt_param.check_bond_flag = false;
    state_machine(EVENT_DEDICATED_CONNECT, &bt_param);
}

void app_bt_policy_disconnect(uint8_t *bd_addr, uint32_t plan_profs)
{
    T_BT_PARAM bt_param;

    memset(&bt_param, 0, sizeof(T_BT_PARAM));

    bt_param.bd_addr = bd_addr;
    bt_param.prof = plan_profs;
    state_machine(EVENT_DEDICATED_DISCONNECT, &bt_param);
}

void app_bt_policy_disconnect_all_link(void)
{
    state_machine(EVENT_DISCONNECT_ALL, NULL);
}

T_BP_STATE app_bt_policy_get_state(void)
{
    return bp_state;
}

T_BT_DEVICE_MODE app_bt_policy_get_radio_mode(void)
{
    return radio_mode;
}

bool app_bt_policy_get_b2b_connected(void)
{
    return b2b_connected;
}

uint8_t app_bt_policy_get_b2s_connected_num(void)
{
    return b2s_connected.num;
}

void app_bt_policy_set_b2s_connected_num_max(uint8_t num_max)
{
    b2s_connected.num_max = num_max;
}

void app_bt_policy_sync_b2s_connected(void)
{
    app_db.is_b2s_connected = app_bt_policy_get_b2s_connected_num();

    if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
        (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY))
    {
        remote_async_msg_relay(app_db.relay_handle, APP_REMOTE_MSG_PHONE_CONNECTED,
                               &app_db.is_b2s_connected, 1, false);
    }
}

bool app_bt_policy_get_first_connect_sync_default_vol_flag(void)
{
    return first_connect_sync_default_volume_to_src;
}

void app_bt_policy_set_first_connect_sync_default_vol_flag(bool flag)
{
    first_connect_sync_default_volume_to_src = flag;
}

