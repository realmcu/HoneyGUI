/**
 * \file tuya_ble_feature_ext_module.h
 *
 * \brief
 */
/*
 *  Copyright (C) 2014-2019, Tuya Inc., All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of tuya ble sdk
 */

#ifndef TUYA_BLE_FEATURE_EXT_MODULE_H_
#define TUYA_BLE_FEATURE_EXT_MODULE_H_

#include "tuya_ble_stdlib.h"
#include "tuya_ble_type.h"

/**@brief   Macro for defining extern module transfer protocol. */
#define FRM_EM_DEV_INFO_QUERY_REQ           0x0050     //APP->BLE
#define FRM_EM_DEV_INFO_QUERY_RESP          0x0050     //BLE->APP
#define FRM_EM_ACTIVE_INFO_RECEIVED         0x0051     //APP->BLE
#define FRM_EM_ACTIVE_INFO_RECEIVED_RESP    0x0051     //BLE->APP


/**@brief    ext module active data received packet structure. */
typedef struct
{
    uint32_t recv_len;
    uint32_t recv_len_max;
    uint8_t  *recv_data;
} tuya_ble_em_active_data_recv_packet;


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief   Function for asynchronous response extern module dev info
 *
 * @details
 *
* @param[in] p_data     The point of ext module dev info. The data format is as follows:
 *  --------------------------------------------------------------------------------------------------------
 * |         1byte         |      1byte      |    1byte     |    1+2+n bytes    |   ...     |   1+2+n bytes |
 * |     Include_EM_Info   |     EM_Type     |    N_TLD     |       TLD1        |   ...     |     TLDn      |
 *  --------------------------------------------------------------------------------------------------------
 *  Include_EM_Info£º
 *          0x01 - include em info
 *
 *  EM_Type:
 *          0x01 - NB module
 *          0x02 - WiFi module
 *          0x03 - Cat.x module
 *          0x04 - zigbee module
 *
 *  N_TLD: the number of subsequent TLDs
 *
 *  TLD: type[1byte] + length[2bytes] + data[nbytes]
 *  ---------------------------------------------
 *  |         TYPE=1 em dev info                |
 *  ---------------------------------------------
 *  |   type    |   length  |      data         |
 *  |   0x01    |     N     |   dev info json   |
 *  ---------------------------------------------
 *
 *  ---------------------------------------------
 *  |         TYPE=2 em dev insert state        |
 *  ---------------------------------------------
 *  |   type    |   length  |         data      |
 *  |   0x02    |   0x01    |  0-pop; 1-insert  |
 *  ---------------------------------------------
 *
 *  ---------------------------------------------
 *  |         TYPE=3 communicate priority       |
 *  ---------------------------------------------
 *  |   type    |   length  |       data        |
 *  |           |           |      LAN=0        |
 *  |           |           |      MQTT=1       |
 *  |           |           |      HTTP=2       |
 *  |   0x03    |    0x02   |      BLE=3        |
 *  |           |           |      SIGMESH=4    |
 *  |           |           |      TUYAMESH=5   |
 *  |           |           |      BEACON=6     |
 *  ---------------------------------------------
 *
 *  ---------------------------------------------
 *  |         TYPE=5 em dev binding state       |
 *  ---------------------------------------------
 *  |   type    |   length  |         data      |
 *  |           |           |    0-unbinding    |
 *  |   0x05    |   0x01    |    1-binding      |
 *  ---------------------------------------------
 *
 * @param[in] len       The length of ext module dev info
 *
 * */
extern tuya_ble_status_t tuya_ble_ext_module_info_asynchronous_response(uint8_t *p_data,
                                                                        uint32_t len);


/**
 * @brief   Function for handler app query extern module dev info when connected.
 *
 * @details Internal use of tuya ble sdk
 *
 * @param[in] recv_data     The point of recvived scene data
 * @param[in] recv_len      The length of data
 *
 * */
extern void tuya_ble_handle_ext_module_dev_info_query_req(uint8_t *recv_data, uint16_t recv_len);


/**
 * @brief   Function for free received iot subpackage data cache
 *
 * @details Internal use of tuya ble sdk
 *
 * */
extern void tuya_ble_em_active_data_recv_packet_free(void);


/**
 * @brief   Function for handler extern module active data received
 *
 * @details Internal use of tuya ble sdk
 *
 * @param[in] recv_data     The point of received data
 * @param[in] recv_len      The length of data
 *
 * */
extern void tuya_ble_handle_ext_module_active_data_received(uint8_t *recv_data, uint16_t recv_len);


#ifdef __cplusplus
}
#endif

#endif /* TUYA_BLE_FEATURE_EXT_MODULE_H_ */
