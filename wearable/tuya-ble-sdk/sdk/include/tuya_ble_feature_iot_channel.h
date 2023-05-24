/**
 * \file tuya_ble_feature_iot_channel.h
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

#ifndef TUYA_BLE_FEATURE_IOT_CHANNEL_H_
#define TUYA_BLE_FEATURE_IOT_CHANNEL_H_

#include "tuya_ble_stdlib.h"
#include "tuya_ble_type.h"


/**@brief   Request iot data sub cmd. */
typedef enum
{
    IOT_SUBCMD_REQ_SCENE_DATA = 1,  /**< request scene data. */
    IOT_SUBCMD_REQ_SCENE_CONTROL,   /**< request scene control. */

    IOT_SUBCMD_MAX,
} request_iot_data_subcmd;


/**@brief    iot data format-TLD structure. */
typedef struct
{
    uint8_t type;
    uint16_t length;
    uint8_t *data;
} tuya_ble_iot_data_tld_t;


/**@brief    iot data received packet structure. */
typedef struct
{
    uint32_t recv_len;
    uint32_t recv_len_max;
    uint8_t  *recv_data;
} tuya_ble_iot_data_recv_packet;


#ifdef __cplusplus
extern "C" {
#endif


/**@brief   Function for get the iot channel subpackage transmission max size.
 *
 *
 * @param[out] subpackage size used for iot channel data transmission.
 * @note    The size depends on the mtu value negotiated after the BLE connection is established.
 *          If the return value is 0, means dev is unbond.
 */
extern uint32_t tuya_ble_iot_channel_subpackage_maxsize_get(void);


/**
 * @brief   Function for handler iot data request response
 *
 * @details Internal use of tuya ble sdk
 *
 * @param[in] recv_data     The point of received response data
 * @param[in] recv_len      The numbers of data
 *
 * */
extern void tuya_ble_handle_iot_data_request_response(uint8_t *recv_data, uint16_t recv_len);


/**
 * @brief   Function for handler iot data received
 *
 * @details Internal use of tuya ble sdk
 *
 * @param[in] recv_data     The point of received iot data
 * @param[in] recv_len      The numbers of data
 *
 * */
extern void tuya_ble_handle_iot_data_received(uint8_t *recv_data, uint16_t recv_len);

/**
 * @brief   Function for free received iot subpackage data cache
 *
 * @details Internal use of tuya ble sdk
 *
 * */
extern void tuya_ble_iot_data_recv_packet_free(void);


#ifdef __cplusplus
}
#endif

#endif /* TUYA_BLE_FEATURE_IOT_CHANNEL_H_ */
