/**
 * \file tuya_ble_ring_buffer.c
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
#include "tuya_ble_type.h"
#include "tuya_ble_ring_buffer.h"


tuya_ble_status_t tuya_ble_ring_buffer_init(tuya_ble_ring_buffer_t *p_ring_buffer,
                                            void *p_data_buffer, uint32_t size)
{
    if (p_ring_buffer == NULL || p_data_buffer == NULL)
    {
        return TUYA_BLE_ERR_INVALID_PARAM;
    }

    p_ring_buffer->p_buf = p_data_buffer;
    p_ring_buffer->capacity = size;
    p_ring_buffer->rd_ptr = 0;
    p_ring_buffer->wr_ptr = 0;
    p_ring_buffer->used = 0;
    p_ring_buffer->over_write = 0;

    return TUYA_BLE_SUCCESS;

}


static __TUYA_BLE_INLINE uint8_t tuya_ble_ring_buffer_empty(tuya_ble_ring_buffer_t *p_ring_buffer)
{
    uint32_t write_temp = p_ring_buffer->wr_ptr;
    uint32_t read_temp = p_ring_buffer->rd_ptr;
    uint32_t over_write_temp = p_ring_buffer->over_write;
    return (write_temp == read_temp && (!over_write_temp));
}

static __TUYA_BLE_INLINE uint8_t tuya_ble_ring_buffer_full(tuya_ble_ring_buffer_t *p_ring_buffer)
{
    uint32_t write_temp = p_ring_buffer->wr_ptr;
    uint32_t read_temp = p_ring_buffer->rd_ptr;
    uint32_t over_write_temp = p_ring_buffer->over_write;
    return (write_temp == read_temp && (over_write_temp));
}

static __TUYA_BLE_INLINE uint8_t tuya_ble_ring_buffer_over_write(tuya_ble_ring_buffer_t
                                                                 *p_ring_buffer)
{
    uint32_t write_temp = p_ring_buffer->wr_ptr;
    uint32_t read_temp = p_ring_buffer->rd_ptr;
    uint32_t over_write_temp = p_ring_buffer->over_write;
    return (write_temp != read_temp && (over_write_temp));
}


uint32_t tuya_ble_get_ring_buffer_used(tuya_ble_ring_buffer_t *p_ring_buffer)
{
    uint32_t data_length = 0;

    uint32_t write_temp = p_ring_buffer->wr_ptr;
    uint32_t read_temp = p_ring_buffer->rd_ptr;
    uint32_t capacity_temp = p_ring_buffer->capacity;

    if (tuya_ble_ring_buffer_over_write(p_ring_buffer) || tuya_ble_ring_buffer_full(p_ring_buffer))
    {
        data_length = capacity_temp;
        return data_length;
    }

    data_length = ((write_temp + capacity_temp - read_temp) % capacity_temp);

    return data_length;

}


uint32_t tuya_ble_ring_buffer_push(tuya_ble_ring_buffer_t *p_ring_buffer, void *p_src,
                                   uint32_t bytes, bool full_check)
{
    uint32_t copy_length = 0;
    uint32_t push_length = 0;
    uint32_t temp_length = 0;
    uint8_t *p_src_data;

    copy_length = bytes;
    p_src_data = (uint8_t *)p_src;

    uint32_t write_temp = p_ring_buffer->wr_ptr;
    uint32_t read_temp = p_ring_buffer->rd_ptr;
    uint32_t capacity_temp = p_ring_buffer->capacity;

    if (full_check == true)
    {
        if (tuya_ble_ring_buffer_full(p_ring_buffer))
        {
            copy_length = 0;
            push_length = 0;
            return push_length;
        }

        if (tuya_ble_ring_buffer_empty(p_ring_buffer))
        {
            if (copy_length >= capacity_temp)
            {
                p_ring_buffer->over_write = 1;
                copy_length = capacity_temp;
            }
        }
        else
        {
            if (((read_temp + capacity_temp - write_temp) % capacity_temp) <= copy_length)
            {
                p_ring_buffer->over_write = 1;
                copy_length = ((read_temp + capacity_temp - write_temp) % capacity_temp);
            }

        }

        push_length = copy_length;

        while ((write_temp + copy_length) >= capacity_temp)
        {
            temp_length = capacity_temp - write_temp;
            memcpy((void *)&p_ring_buffer->p_buf[write_temp], p_src_data, temp_length);
            write_temp = p_ring_buffer->wr_ptr = ((write_temp + temp_length) % capacity_temp);
            copy_length -= temp_length;
        }

        memcpy((void *)&p_ring_buffer->p_buf[write_temp], &p_src_data[temp_length], copy_length);
        p_ring_buffer->wr_ptr = ((write_temp + copy_length) % capacity_temp);
        write_temp = p_ring_buffer->wr_ptr;

        return push_length;
    }
    else
    {
        if (tuya_ble_ring_buffer_empty(p_ring_buffer))
        {
            if (copy_length >= capacity_temp)
            {
                p_ring_buffer->over_write = 1;
            }
        }
        else
        {
            if (((read_temp + capacity_temp - write_temp) % capacity_temp) <= copy_length)
            {
                p_ring_buffer->over_write = 1;
            }
        }

        push_length = copy_length;

        while ((write_temp + copy_length) >= capacity_temp)
        {
            temp_length = capacity_temp - write_temp;
            memcpy((void *)&p_ring_buffer->p_buf[write_temp], p_src_data, temp_length);
            write_temp = p_ring_buffer->wr_ptr = ((write_temp + temp_length) % capacity_temp);
            copy_length -= temp_length;
        }

        memcpy((void *)&p_ring_buffer->p_buf[write_temp], &p_src_data[temp_length], copy_length);
        write_temp = p_ring_buffer->wr_ptr = ((write_temp + copy_length) % capacity_temp);

        if (p_ring_buffer->over_write == 1)
        {
            p_ring_buffer->rd_ptr = (write_temp);
            read_temp = p_ring_buffer->rd_ptr;
        }

        return push_length;
    }
}


uint32_t tuya_ble_ring_buffer_pop(tuya_ble_ring_buffer_t *p_ring_buffer, void *p_dest,
                                  uint32_t bytes)
{
    uint32_t copy_length = 0;
    uint32_t temp_length = 0;
    uint32_t data_length = 0;
    uint32_t pop_length = 0;
    uint8_t *p_dest_data;

    uint32_t write_temp = p_ring_buffer->wr_ptr;
    uint32_t read_temp = p_ring_buffer->rd_ptr;
    uint32_t capacity_temp = p_ring_buffer->capacity;

    p_dest_data = (uint8_t *) p_dest;
    data_length = tuya_ble_get_ring_buffer_used(p_ring_buffer);

    if (tuya_ble_ring_buffer_over_write(p_ring_buffer))
    {
        read_temp = p_ring_buffer->rd_ptr = (write_temp);
    }

    copy_length = bytes < data_length ? bytes : data_length;

    pop_length = copy_length;

    while ((read_temp + copy_length) >= capacity_temp)
    {
        temp_length = capacity_temp - read_temp;
        memcpy(p_dest_data, (void *)&p_ring_buffer->p_buf[read_temp], temp_length);
        read_temp = p_ring_buffer->rd_ptr = ((read_temp + temp_length) % capacity_temp);
        copy_length -= temp_length;
    }

    memcpy(&p_dest_data[temp_length], (void *)&p_ring_buffer->p_buf[read_temp], copy_length);

    read_temp = p_ring_buffer->rd_ptr = ((read_temp + copy_length) % capacity_temp);

    p_ring_buffer->over_write = 0;

    return pop_length;
}


void tuya_ble_ring_buffer_flush(tuya_ble_ring_buffer_t *p_ring_buffer)
{
    p_ring_buffer->rd_ptr = 0;
    p_ring_buffer->wr_ptr = 0;
    p_ring_buffer->used = 0;
    p_ring_buffer->over_write = 0;
}


