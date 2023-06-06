/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_LE_LINK_UTIL_H_
#define _APP_LE_LINK_UTIL_H_

#include <stdint.h>
#include <stdbool.h>
#include "os_queue.h"
#ifdef RTL8772F
#include "tts.h"
#endif
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_LINK App Link
  * @brief App Link
  * @{
  */

/** max BLE link num */
#define MAX_BLE_LINK_NUM                2


#define MAX_ADV_SET_NUMBER              6






typedef void(*P_FUN_LE_LINK_DISC_CB)(uint8_t conn_id, uint8_t local_disc_cause,
                                     uint16_t disc_cause);

typedef struct t_le_disc_cb_entry
{
    struct t_le_disc_cb_entry  *p_next;
    P_FUN_LE_LINK_DISC_CB disc_callback;
} T_LE_DISC_CB_ENTRY;

/**  @brief  App define le link connection database */
typedef struct
{
    uint8_t             bd_addr[6];
    bool                used;
    uint8_t             id;
    bool                cmd_set_enable;
    uint8_t            *p_embedded_cmd;
#ifdef RTL8772F
    void               *tts_handle;
    uint8_t             tts_state;
    uint8_t             tts_seq;
    uint16_t            tts_data_offset;
    uint8_t            *tts_frame_buf;
    uint16_t            tts_frame_len;

    uint8_t             eq_state;
    uint8_t             eq_index;
    uint8_t             eq_type;
    uint8_t             eq_seq;
    uint16_t            eq_data_offset;
    uint8_t            *eq_data_buf;
    uint16_t            eq_data_len;
#endif
    uint16_t            embedded_cmd_len;
    uint16_t            mtu_size;
    uint8_t             state;
    uint8_t             conn_id;
    uint8_t             rx_cmd_seqn;
    uint8_t             tx_event_seqn;
    uint8_t             transmit_srv_tx_enable_fg;
    uint8_t             local_disc_cause;
    T_OS_QUEUE          disc_cb_list;
} T_APP_LE_LINK;




/**
    * @brief  find the BLE link by connected id
    * @param  conn_id BLE link id(slot)
    * @return the BLE link
    */
T_APP_LE_LINK *app_find_le_link_by_conn_id(uint8_t conn_id);

/**
    * @brief  find the BLE link by bluetooth address
    * @param  bd_addr bluetooth address
    * @return the BLE link
    */
T_APP_LE_LINK *app_find_le_link_by_addr(uint8_t *bd_addr);
#ifdef RTL8772F
/**
    * @brief  find the BLE link by tts handle
    * @param  handle tts handle
    * @return the BLE link
    */
T_APP_LE_LINK *app_find_le_link_by_tts_handle(T_TTS_HANDLE handle);
#endif
/**
    * @brief  alloc the BLE link by link id(slot)
    * @param  conn_id BLE link id(slot)
    * @return the BLE link
    */
T_APP_LE_LINK *app_alloc_le_link_by_conn_id(uint8_t conn_id);

/**
    * @brief  free the BLE link
    * @param  p_link the BLE link
    * @return true: success; false: fail
    */
bool app_free_le_link(T_APP_LE_LINK *p_link);

bool app_reg_le_link_disc_cb(uint8_t conn_id, P_FUN_LE_LINK_DISC_CB p_fun_cb);

uint8_t app_get_ble_link_num(void);


/** End of APP_LINK
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_LE_LINK_UTIL_H_ */
