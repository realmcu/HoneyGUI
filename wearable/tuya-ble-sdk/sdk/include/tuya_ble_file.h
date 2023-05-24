/**
 * \file tuya_ble_file.h
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

#ifndef __TUYA_BLE_FILE_H__
#define __TUYA_BLE_FILE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
 * INCLUDE
 */
#include "tuya_ble_stdlib.h"
#include "tuya_ble_type.h"
#include "tuya_ble_config.h"

#if TUYA_BLE_FILE_ENABLE

/**@brief   Macro for defining file transfer protocol. */
#define FRM_FILE_INFOR_REQ          0x0070 //APP->BLE
#define FRM_FILE_INFOR_RESP         0x0070 //BLE->APP
#define FRM_FILE_OFFSET_REQ         0x0071 //APP->BLE
#define FRM_FILE_OFFSET_RESP        0x0071 //BLE->APP
#define FRM_FILE_DATA_REQ           0x0072 //APP->BLE
#define FRM_FILE_DATA_RESP          0x0072 //BLE->APP
#define FRM_FILE_END_REQ            0x0073 //APP->BLE
#define FRM_FILE_END_RESP           0x0073 //BLE->APP


/**@brief   Function for tuya ble handle file request.
 *
 *
 * @param[in] cmd           Request command.
 * @param[in] p_recv_data     Pointer to the buffer with received data.
 * @param[in] recv_data_len      Length of received data.
 */
void tuya_ble_handle_file_req(uint16_t cmd, uint8_t *p_recv_data, uint32_t recv_data_len);


/**@brief   Function for respond to app requests.
 *
 *
 * @param[in] p_data   The pointer to the response data.
 */
tuya_ble_status_t tuya_ble_file_response(tuya_ble_file_response_t *p_data);

#endif

#ifdef __cplusplus
}
#endif

#endif //__TUYA_BLE_FILE_H__

