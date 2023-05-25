/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_BT_POLICY_H_
#define _APP_BT_POLICY_H_

#include <stdint.h>
#include <stdbool.h>
#include "btm.h"
#include "app_linkback.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup APP_BT_POLICY App BT Policy
  * @brief App BLE Device
  * @{
  */

typedef enum
{
    BP_EVENT_STATE_CHANGED             = 0x00,
    BP_EVENT_RADIO_MODE_CHANGED        = 0x10,
    BP_EVENT_ROLE_DECIDED              = 0x20,
    BP_EVENT_BUD_CONN_START            = 0x30,
    BP_EVENT_BUD_AUTH_SUC              = 0x31,
    BP_EVENT_BUD_CONN_FAIL             = 0x32,
    BP_EVENT_BUD_AUTH_FAIL             = 0x33,
    BP_EVENT_BUD_DISCONN_NORMAL        = 0x34,
    BP_EVENT_BUD_DISCONN_LOST          = 0x35,
    BP_EVENT_BUD_REMOTE_CONN_CMPL      = 0x36,
    BP_EVENT_BUD_REMOTE_DISCONN_CMPL   = 0x37,
    BP_EVENT_BUD_LINKLOST_TIMEOUT      = 0x38,
    BP_EVENT_SRC_AUTH_SUC              = 0x40,
    BP_EVENT_SRC_AUTH_FAIL             = 0x41,
    BP_EVENT_SRC_DISCONN_LOST          = 0x42,
    BP_EVENT_SRC_DISCONN_NORMAL        = 0x43,
    BP_EVENT_SRC_DISCONN_ROLESWAP      = 0x44,
    BP_EVENT_PROFILE_CONN_SUC          = 0x50,
    BP_EVENT_PROFILE_DISCONN           = 0x51,
    BP_EVENT_PAIRING_MODE_TIMEOUT      = 0x60,
    BP_EVENT_DEDICATED_PAIRING         = 0x61,
} T_BP_EVENT;

typedef struct
{
    uint8_t *bd_addr;
    bool is_first_prof;
    bool is_last_prof;
    bool is_ignore;
    bool is_first_src;
    bool is_shut_down;
} T_BP_EVENT_PARAM;

typedef enum
{
    BP_STATE_IDLE                = 0x00,
    BP_STATE_FIRST_ENGAGE        = 0x01,
    BP_STATE_ENGAGE              = 0x02,
    BP_STATE_LINKBACK            = 0x03,
    BP_STATE_CONNECTED           = 0x04,
    BP_STATE_PAIRING_MODE        = 0x05,
    BP_STATE_STANDBY             = 0x06,
    BP_STATE_SECONDARY           = 0x07,
} T_BP_STATE;

typedef void (*T_BP_IND_FUN)(T_BP_EVENT event, T_BP_EVENT_PARAM *event_param);

void app_bt_policy_init(void);

void app_bt_policy_startup(T_BP_IND_FUN fun, bool at_once_trigger);
void app_bt_policy_shutdown(void);
void app_bt_policy_stop(void);

void app_bt_policy_msg_prof_conn(uint8_t *bd_addr, uint32_t prof);
void app_bt_policy_msg_prof_disconn(uint8_t *bd_addr, uint32_t prof, uint16_t cause);

void app_bt_policy_default_connect(uint8_t *bd_addr, uint32_t plan_profs, bool check_bond_flag);
void app_bt_policy_special_connect(uint8_t *bd_addr, uint32_t plan_prof,
                                   T_LINKBACK_SEARCH_PARAM *search_param);
void app_bt_policy_disconnect(uint8_t *bd_addr, uint32_t plan_profs);
void app_bt_policy_disconnect_all_link(void);

void app_bt_policy_enter_pairing_mode(bool force, bool visiable);
void app_bt_policy_exit_pairing_mode(void);

void app_bt_policy_ota_mode(bool connectable);

void app_bt_policy_start_ota_shaking(void);
void app_bt_policy_enter_ota_mode(bool connectable);

T_BP_STATE app_bt_policy_get_state(void);

T_BT_DEVICE_MODE app_bt_policy_get_radio_mode(void);

bool app_bt_policy_get_b2b_connected(void);
uint8_t app_bt_policy_get_b2s_connected_num(void);
void app_bt_policy_set_b2s_connected_num_max(uint8_t num_max);
void app_bt_policy_sync_b2s_connected(void);

bool app_bt_policy_get_first_connect_sync_default_vol_flag(void);
void app_bt_policy_set_first_connect_sync_default_vol_flag(bool flag);

/** End of APP_BT_POLICY
* @}
*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_BT_POLICY_H_ */
