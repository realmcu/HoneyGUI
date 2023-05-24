/**
 * \file tuya_ble_feature_iot_channel.c
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
#include "tuya_ble_heap.h"
#include "tuya_ble_mem.h"
#include "tuya_ble_api.h"
#include "tuya_ble_port.h"
#include "tuya_ble_main.h"
#include "tuya_ble_internal_config.h"
#include "tuya_ble_data_handler.h"
#include "tuya_ble_mutli_tsf_protocol.h"
#include "tuya_ble_utils.h"
#include "tuya_ble_secure.h"
#include "tuya_ble_main.h"
#include "tuya_ble_storage.h"
#include "tuya_ble_unix_time.h"
#include "tuya_ble_log.h"
#include "tuya_ble_gatt_send_queue.h"
#include "tuya_ble_feature_iot_channel.h"

#if ( TUYA_BLE_FEATURE_IOT_CHANNEL_ENABLE != 0 )

#if ( TUYA_BLE_FEATURE_SCENE_ENABLE != 0 )
#include "tuya_ble_feature_scene.h"
#endif

static frm_trsmitr_proc_s iot_trsmitr_proc;

static tuya_ble_iot_data_recv_packet iot_recv_packet =
{
    .recv_len = 0,
    .recv_len_max = 0,
    .recv_data = NULL,
};


static void tuya_ble_iot_data_dispatch(uint8_t *p_iot_data, uint16_t iot_data_len);

uint32_t tuya_ble_iot_channel_subpackage_maxsize_get(void)
{
    uint32_t mtu_length = 0;
    uint32_t subpackage_size = 0;

    if (tuya_ble_connect_status_get() != BONDING_CONN)
    {
        subpackage_size = 0;
    }
    else
    {
        mtu_length = tuya_ble_send_packet_data_length_get();

#if 0
        if (mtu_length < 100)
        {
            subpackage_size = 256;
        }
        else if ((mtu_length >= 100) && (mtu_length < 150))
        {
            subpackage_size = 512;
        }
        else
        {
            subpackage_size = 512;
        }
#endif

        subpackage_size = mtu_length;
    }

    TUYA_BLE_LOG_DEBUG("iot channel subpackage maxsize =[%d]", subpackage_size);
    return subpackage_size;
}


void tuya_ble_iot_data_recv_packet_free(void)
{
    if (iot_recv_packet.recv_data)
    {
        tuya_ble_free(iot_recv_packet.recv_data);

        iot_recv_packet.recv_data = NULL;
        iot_recv_packet.recv_len_max = 0;
        iot_recv_packet.recv_len = 0;
    }
}


static uint32_t tuya_ble_iot_data_unpack(uint8_t *buf, uint32_t len)
{
    static uint32_t offset = 0;
    mtp_ret ret;

    ret = trsmitr_recv_pkg_decode(&iot_trsmitr_proc, buf, len);

    if (MTP_OK != ret && MTP_TRSMITR_CONTINUE != ret)
    {
        iot_recv_packet.recv_len_max = 0;
        iot_recv_packet.recv_len = 0;

        if (iot_recv_packet.recv_data)
        {
            tuya_ble_free(iot_recv_packet.recv_data);
            iot_recv_packet.recv_data = NULL;
        }

        return 1;
    }

    if (FRM_PKG_FIRST == iot_trsmitr_proc.pkg_desc)
    {
        if (iot_recv_packet.recv_data)
        {
            tuya_ble_free(iot_recv_packet.recv_data);
            iot_recv_packet.recv_data = NULL;
        }
        iot_recv_packet.recv_len_max = get_trsmitr_frame_total_len(&iot_trsmitr_proc);

        if ((iot_recv_packet.recv_len_max > TUYA_BLE_AIR_FRAME_MAX) || (iot_recv_packet.recv_len_max == 0))
        {
            iot_recv_packet.recv_len_max = 0;
            iot_recv_packet.recv_len = 0;
            TUYA_BLE_LOG_ERROR("iot_data_unpack total size=[%d] error.", iot_recv_packet.recv_len_max);
            return 2;
        }

        iot_recv_packet.recv_len = 0;
        iot_recv_packet.recv_data = tuya_ble_malloc(iot_recv_packet.recv_len_max);
        if (iot_recv_packet.recv_data == NULL)
        {
            TUYA_BLE_LOG_ERROR("iot_data_unpack malloc failed.");
            return 2;
        }
        memset(iot_recv_packet.recv_data, 0, iot_recv_packet.recv_len_max);
        offset = 0;
    }

    if ((offset + get_trsmitr_subpkg_len(&iot_trsmitr_proc)) <= iot_recv_packet.recv_len_max)
    {
        if (iot_recv_packet.recv_data)
        {
            memcpy(iot_recv_packet.recv_data + offset, get_trsmitr_subpkg(&iot_trsmitr_proc),
                   get_trsmitr_subpkg_len(&iot_trsmitr_proc));
            offset += get_trsmitr_subpkg_len(&iot_trsmitr_proc);
            iot_recv_packet.recv_len = offset;
        }
        else
        {
            TUYA_BLE_LOG_ERROR("iot_data_unpack error.");
            iot_recv_packet.recv_len_max = 0;
            iot_recv_packet.recv_len = 0;
            return 2;
        }
    }
    else
    {
        ret = MTP_INVALID_PARAM;
        TUYA_BLE_LOG_ERROR("iot_data_unpack[] error:MTP_INVALID_PARAM");
        tuya_ble_iot_data_recv_packet_free();

        return 2;
    }

    if (ret == MTP_OK)
    {
        offset = 0;
        TUYA_BLE_LOG_DEBUG("iot_data_unpack[%d]", iot_recv_packet.recv_len);
        return 0;
    }

    return 3;
}


void tuya_ble_handle_iot_data_request_response(uint8_t *recv_data, uint16_t recv_len)
{
    uint16_t sub_cmd = (recv_data[13] << 8) | recv_data[14];

    if (sub_cmd >= IOT_SUBCMD_MAX)
    {
        TUYA_BLE_LOG_ERROR("tuya_ble_handle_iot_data_request_response, err sub_cmd=%d", sub_cmd);
        return;
    }

    switch (sub_cmd)
    {
#if ( TUYA_BLE_FEATURE_SCENE_ENABLE != 0 )
    case IOT_SUBCMD_REQ_SCENE_DATA:
    case IOT_SUBCMD_REQ_SCENE_CONTROL:
        tuya_ble_handle_scene_request_response(recv_data, recv_len);
        break;
#endif

    default:
        break;
    }
}


void tuya_ble_handle_iot_data_received(uint8_t *recv_data, uint16_t recv_len)
{
    uint8_t p_buf[13], buf_len = 0;
    uint16_t data_len = 0;
    uint32_t ack_sn = 0;

    ack_sn  = recv_data[1] << 24;
    ack_sn += recv_data[2] << 16;
    ack_sn += recv_data[3] << 8;
    ack_sn += recv_data[4];

    data_len = ((recv_data[11] << 8) | recv_data[12]);
    TUYA_BLE_LOG_HEXDUMP_DEBUG("received iot subpackag: ", recv_data + 13, data_len);

    /* received iot data unpack */
    uint32_t unpack_ret = tuya_ble_iot_data_unpack(recv_data + 13, data_len);
    if (unpack_ret == 0)
    {
        p_buf[buf_len++] = 0x00;    //!<unpack success
    }
    else if (unpack_ret == 1 || unpack_ret == 2)
    {
        p_buf[buf_len++] = 0x02;    //!<unpack err
    }
    else if (unpack_ret == 3)
    {
        p_buf[buf_len++] = 0x01;    //!<subpack, continue
    }

    p_buf[buf_len++] = (uint8_t)(get_trsmitr_frame_seq(&iot_trsmitr_proc) >> 8);    //!<subpackage sn
    p_buf[buf_len++] = (uint8_t)(get_trsmitr_frame_seq(&iot_trsmitr_proc));
    p_buf[buf_len++] = (uint8_t)(get_trsmitr_subpkg_len(&iot_trsmitr_proc) >> 8);   //!<subpackage len
    p_buf[buf_len++] = (uint8_t)(get_trsmitr_subpkg_len(&iot_trsmitr_proc));
    p_buf[buf_len++] = (uint8_t)(iot_recv_packet.recv_len >>
                                 24);                   //!<received len
    p_buf[buf_len++] = (uint8_t)(iot_recv_packet.recv_len >> 16);
    p_buf[buf_len++] = (uint8_t)(iot_recv_packet.recv_len >> 8);
    p_buf[buf_len++] = (uint8_t)(iot_recv_packet.recv_len);
    p_buf[buf_len++] = (uint8_t)(iot_recv_packet.recv_len_max >>
                                 24);               //!<package total len
    p_buf[buf_len++] = (uint8_t)(iot_recv_packet.recv_len_max >> 16);
    p_buf[buf_len++] = (uint8_t)(iot_recv_packet.recv_len_max >> 8);
    p_buf[buf_len++] = (uint8_t)(iot_recv_packet.recv_len_max);

    tuya_ble_commData_send(FRM_IOT_DATA_RECEIVED_RESP, ack_sn, p_buf, buf_len,
                           ENCRYPTION_MODE_SESSION_KEY);

    if (unpack_ret == 0)
    {
        tuya_ble_iot_data_dispatch((uint8_t *)iot_recv_packet.recv_data, iot_recv_packet.recv_len);

        tuya_ble_iot_data_recv_packet_free();
        return;
    }
}


static void tuya_ble_iot_data_dispatch(uint8_t *p_iot_data, uint16_t iot_data_len)
{
    uint16_t sub_cmd;

    if (iot_trsmitr_proc.version < 2)
    {
        TUYA_BLE_LOG_ERROR("iot trsmitr_proc version not compatibility!");
        return;
    }

    sub_cmd = (p_iot_data[0] << 8) | p_iot_data[1];
    if (sub_cmd >= IOT_SUBCMD_MAX)
    {
        TUYA_BLE_LOG_ERROR("received iot data, cmd err= %d", sub_cmd);
        return;
    }
    else if ((iot_data_len < 5) || (iot_data_len > TUYA_BLE_AIR_FRAME_MAX))
    {
        TUYA_BLE_LOG_ERROR("received iot data, length err= %d", iot_data_len);
        return;
    }

    TUYA_BLE_LOG_HEXDUMP_INFO("received iot data", p_iot_data, iot_data_len);

    switch (sub_cmd)
    {
#if ( TUYA_BLE_FEATURE_SCENE_ENABLE != 0 )
    case IOT_SUBCMD_REQ_SCENE_DATA:
    case IOT_SUBCMD_REQ_SCENE_CONTROL:
        tuya_ble_handle_scene_data_received(p_iot_data, iot_data_len);
        break;
#endif

    default:
        break;
    }
}

#endif

