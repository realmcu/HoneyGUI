/**
 * \file tuya_ble_encoded_audio_buffer.h
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

#ifndef TUYA_BLE_ENCODED_AUDIO_BUFFER_H_
#define TUYA_BLE_ENCODED_AUDIO_BUFFER_H_

#include "tuya_ble_stdlib.h"
#include "tuya_ble_type.h"
#include "tuya_ble_ring_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

void tuya_ble_encoded_audio_buffer_init(void);

tuya_ble_status_t tuya_ble_encoded_audio_buffer_push(void *pvSource, uint32_t ui32Bytes);

uint32_t tuya_ble_encoded_audio_buffer_pop(void *pvDest, uint32_t ui32Bytes);

uint32_t tuya_ble_get_encoded_audio_buffer_used(void);

#ifdef __cplusplus
}
#endif

#endif //



