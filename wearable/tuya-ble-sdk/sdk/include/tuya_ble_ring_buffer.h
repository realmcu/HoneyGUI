/**
 * \file tuya_ble_ring_buffer.h
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

#ifndef TUYA_BLE_RING_BUFFER_H_
#define TUYA_BLE_RING_BUFFER_H_

#include "tuya_ble_stdlib.h"
#include "tuya_ble_type.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
    volatile uint8_t *p_buf;
    volatile uint32_t capacity;
    volatile uint32_t rd_ptr;
    volatile uint32_t wr_ptr;
    volatile uint32_t used;
    volatile uint32_t over_write;
} tuya_ble_ring_buffer_t;


tuya_ble_status_t tuya_ble_ring_buffer_init(tuya_ble_ring_buffer_t *p_ring_buffer,
                                            void *p_data_buffer, uint32_t size);

uint32_t tuya_ble_ring_buffer_push(tuya_ble_ring_buffer_t *p_ring_buffer, void *p_src,
                                   uint32_t bytes, bool full_check);

uint32_t tuya_ble_ring_buffer_pop(tuya_ble_ring_buffer_t *p_ring_buffer, void *p_dest,
                                  uint32_t bytes);

uint32_t tuya_ble_get_ring_buffer_used(tuya_ble_ring_buffer_t *p_ring_buffer);

void tuya_ble_ring_buffer_flush(tuya_ble_ring_buffer_t *p_ring_buffer);


#ifdef __cplusplus
}
#endif

#endif //TUYA_BLE_RING_BUFFER_H_

