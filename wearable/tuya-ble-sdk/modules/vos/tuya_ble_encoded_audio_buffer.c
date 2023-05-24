/**
 * \file tuya_ble_encoded_audio_buffer.c
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

#include "tuya_ble_stdlib.h"
#include "tuya_ble_encoded_audio_buffer.h"
#include "tuya_ble_type.h"
#include "tuya_ble_vos.h"
#include "tuya_ble_log.h"

#define TUYA_BLE_AUDIO_BUFFER_ENCODED_SIZE  (TUYA_BLE_AUDIO_BUFFER_ENCODED_NUM*CODEC_OUT_RING_BUFF_SIZE)

static uint8_t audio_buffer_encoded[TUYA_BLE_AUDIO_BUFFER_ENCODED_SIZE];

static tuya_ble_ring_buffer_t encoded_audio_buffer;

void tuya_ble_encoded_audio_buffer_init(void)
{
    tuya_ble_ring_buffer_init(&encoded_audio_buffer, (void *) audio_buffer_encoded,
                              TUYA_BLE_AUDIO_BUFFER_ENCODED_SIZE);
}


tuya_ble_status_t tuya_ble_encoded_audio_buffer_push(void *pvSource, uint32_t ui32Bytes)
{
    tuya_ble_device_enter_critical();
    tuya_ble_ring_buffer_push(&encoded_audio_buffer, pvSource, ui32Bytes, false);
    tuya_ble_device_exit_critical();
    return TUYA_BLE_SUCCESS;

}


uint32_t tuya_ble_encoded_audio_buffer_pop(void *pvDest, uint32_t ui32Bytes)
{
    uint32_t size = 0;
    tuya_ble_device_enter_critical();
    size = tuya_ble_ring_buffer_pop(&encoded_audio_buffer, pvDest, ui32Bytes);
    tuya_ble_device_exit_critical();
    return size;
}


uint32_t tuya_ble_get_encoded_audio_buffer_used(void)
{
    return tuya_ble_get_ring_buffer_used(&encoded_audio_buffer);
}



