/**
 * \file tuya_ble_feature_ext_module.c
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
#include "tuya_ble_feature_ext_module.h"


#if (TUYA_BLE_PROTOCOL_VERSION_HIGN >= 4 && TUYA_BLE_PROTOCOL_VERSION_LOW >= 4)

#if (TUYA_BLE_FEATURE_EXT_MODULE_ENABLE != 0)

static frm_trsmitr_proc_s em_active_trsmitr_proc;

static tuya_ble_em_active_data_recv_packet em_active_data_recv_packet =
{
    .recv_len = 0,
    .recv_len_max = 0,
    .recv_data = NULL,
};

static void tuya_ble_handle_ext_module_info_query_response_vet(int32_t evt_id, void *data);
static void tuya_ble_handle_ext_module_active_info(uint8_t *p_data, uint16_t data_len);


void tuya_ble_handle_ext_module_dev_info_query_req(uint8_t *recv_data, uint16_t recv_len)
{
    tuya_ble_cb_evt_param_t event;

    event.evt = TUYA_BLE_CB_EVT_QUERY_EXT_MODULE_DEV_INFO;

    if (tuya_ble_cb_event_send(&event) != 0)
    {
        TUYA_BLE_LOG_ERROR("tuya ble send cb event [query em info] failed.");
    }
}


tuya_ble_status_t tuya_ble_ext_module_info_asynchronous_response(uint8_t *p_data, uint32_t len)
{
    uint8_t *p_buffer = NULL;
    tuya_ble_query_em_info_response_t *p_sched_data = NULL;
    tuya_ble_custom_evt_t custom_evt;

    if (len > TUYA_BLE_SEND_MAX_DATA_LEN)
    {
        return TUYA_BLE_ERR_INVALID_LENGTH;
    }

    p_buffer = tuya_ble_malloc(sizeof(tuya_ble_query_em_info_response_t) + len);
    if (p_buffer == NULL)
    {
        return TUYA_BLE_ERR_NO_MEM;
    }

    p_sched_data = (tuya_ble_query_em_info_response_t *)p_buffer;
    p_sched_data->data_len = len;
    p_sched_data->p_data = p_buffer + sizeof(tuya_ble_query_em_info_response_t);
    memcpy(p_sched_data->p_data, p_data, len);

    custom_evt.evt_id = 0;//TUYA_BLE_EVT_QUERY_EM_INFO_RESPONSE;
    custom_evt.data = p_sched_data;
    custom_evt.custom_event_handler = tuya_ble_handle_ext_module_info_query_response_vet;

    if (tuya_ble_custom_event_send(custom_evt) != 0)
    {
        TUYA_BLE_LOG_ERROR("tuya_custom_event_send ext module info query response error");

        tuya_ble_free((uint8_t *)p_buffer);
        return TUYA_BLE_ERR_NO_EVENT;
    }

    return TUYA_BLE_SUCCESS;
}


static void tuya_ble_handle_ext_module_info_query_response_vet(int32_t evt_id, void *data)
{
    tuya_ble_query_em_info_response_t *evt = (tuya_ble_query_em_info_response_t *)data;

    uint8_t encry_mode = 0;

    encry_mode = (tuya_ble_pair_rand_valid_get()) ? (ENCRYPTION_MODE_SESSION_KEY) :
                 (ENCRYPTION_MODE_KEY_4);

    tuya_ble_commData_send(FRM_EM_DEV_INFO_QUERY_RESP, 0, evt->p_data, evt->data_len, encry_mode);

    if (data != NULL)
    {
        tuya_ble_free(data);
    }
}


void tuya_ble_em_active_data_recv_packet_free(void)
{
    if (em_active_data_recv_packet.recv_data)
    {
        tuya_ble_free(em_active_data_recv_packet.recv_data);

        em_active_data_recv_packet.recv_data = NULL;
        em_active_data_recv_packet.recv_len_max = 0;
        em_active_data_recv_packet.recv_len = 0;
    }
}


static uint32_t tuya_ble_em_active_data_unpack(uint8_t *buf, uint32_t len)
{
    static uint32_t offset = 0;
    mtp_ret ret;

    ret = trsmitr_recv_pkg_decode(&em_active_trsmitr_proc, buf, len);

    if (MTP_OK != ret && MTP_TRSMITR_CONTINUE != ret)
    {
        em_active_data_recv_packet.recv_len_max = 0;
        em_active_data_recv_packet.recv_len = 0;

        if (em_active_data_recv_packet.recv_data)
        {
            tuya_ble_free(em_active_data_recv_packet.recv_data);
            em_active_data_recv_packet.recv_data = NULL;
        }

        return 1;
    }

    if (FRM_PKG_FIRST == em_active_trsmitr_proc.pkg_desc)
    {
        if (em_active_data_recv_packet.recv_data)
        {
            tuya_ble_free(em_active_data_recv_packet.recv_data);
            em_active_data_recv_packet.recv_data = NULL;
        }
        em_active_data_recv_packet.recv_len_max = get_trsmitr_frame_total_len(&em_active_trsmitr_proc);

        if ((em_active_data_recv_packet.recv_len_max > TUYA_BLE_AIR_FRAME_MAX) ||
            (em_active_data_recv_packet.recv_len_max == 0))
        {
            em_active_data_recv_packet.recv_len_max = 0;
            em_active_data_recv_packet.recv_len = 0;
            TUYA_BLE_LOG_ERROR("em_active_data_unpack total size=[%d] error.",
                               em_active_data_recv_packet.recv_len_max);
            return 2;
        }

        em_active_data_recv_packet.recv_len = 0;
        em_active_data_recv_packet.recv_data = tuya_ble_malloc(em_active_data_recv_packet.recv_len_max);
        if (em_active_data_recv_packet.recv_data == NULL)
        {
            TUYA_BLE_LOG_ERROR("em_active_data_unpack malloc failed.");
            return 2;
        }
        memset(em_active_data_recv_packet.recv_data, 0, em_active_data_recv_packet.recv_len_max);
        offset = 0;
    }

    if ((offset + get_trsmitr_subpkg_len(&em_active_trsmitr_proc)) <=
        em_active_data_recv_packet.recv_len_max)
    {
        if (em_active_data_recv_packet.recv_data)
        {
            memcpy(em_active_data_recv_packet.recv_data + offset, get_trsmitr_subpkg(&em_active_trsmitr_proc),
                   get_trsmitr_subpkg_len(&em_active_trsmitr_proc));
            offset += get_trsmitr_subpkg_len(&em_active_trsmitr_proc);
            em_active_data_recv_packet.recv_len = offset;
        }
        else
        {
            TUYA_BLE_LOG_ERROR("em_active_data_unpack error.");
            em_active_data_recv_packet.recv_len_max = 0;
            em_active_data_recv_packet.recv_len = 0;
            return 2;
        }
    }
    else
    {
        ret = MTP_INVALID_PARAM;
        TUYA_BLE_LOG_ERROR("em_active_data_unpack[] error:MTP_INVALID_PARAM");
        tuya_ble_em_active_data_recv_packet_free();

        return 2;
    }

    if (ret == MTP_OK)
    {
        offset = 0;
        TUYA_BLE_LOG_DEBUG("em_active_data_unpack ok, total len=[%d]", em_active_data_recv_packet.recv_len);
        return 0;
    }

    return 3;
}


void tuya_ble_handle_ext_module_active_data_received(uint8_t *recv_data, uint16_t recv_len)
{
    uint8_t p_buf[13], buf_len = 0;
    uint16_t data_len = 0;
    uint32_t ack_sn = 0;

    ack_sn  = recv_data[1] << 24;
    ack_sn += recv_data[2] << 16;
    ack_sn += recv_data[3] << 8;
    ack_sn += recv_data[4];

    data_len = ((recv_data[11] << 8) | recv_data[12]);
    TUYA_BLE_LOG_HEXDUMP_DEBUG("received em active data subpackag: ", recv_data + 13, data_len);

    /* received em ative data unpack */
    uint32_t unpack_ret = tuya_ble_em_active_data_unpack(recv_data + 13, data_len);
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

    p_buf[buf_len++] = (uint8_t)(get_trsmitr_frame_seq(&em_active_trsmitr_proc) >>
                                 8);  //!<subpackage sn
    p_buf[buf_len++] = (uint8_t)(get_trsmitr_frame_seq(&em_active_trsmitr_proc));
    p_buf[buf_len++] = (uint8_t)(get_trsmitr_subpkg_len(&em_active_trsmitr_proc) >>
                                 8); //!<subpackage len
    p_buf[buf_len++] = (uint8_t)(get_trsmitr_subpkg_len(&em_active_trsmitr_proc));
    p_buf[buf_len++] = (uint8_t)(em_active_data_recv_packet.recv_len >>
                                 24);            //!<received len
    p_buf[buf_len++] = (uint8_t)(em_active_data_recv_packet.recv_len >> 16);
    p_buf[buf_len++] = (uint8_t)(em_active_data_recv_packet.recv_len >> 8);
    p_buf[buf_len++] = (uint8_t)(em_active_data_recv_packet.recv_len);
    p_buf[buf_len++] = (uint8_t)(em_active_data_recv_packet.recv_len_max >>
                                 24);        //!<package total len
    p_buf[buf_len++] = (uint8_t)(em_active_data_recv_packet.recv_len_max >> 16);
    p_buf[buf_len++] = (uint8_t)(em_active_data_recv_packet.recv_len_max >> 8);
    p_buf[buf_len++] = (uint8_t)(em_active_data_recv_packet.recv_len_max);

    tuya_ble_commData_send(FRM_EM_ACTIVE_INFO_RECEIVED_RESP, ack_sn, p_buf, buf_len,
                           ENCRYPTION_MODE_SESSION_KEY);

    if (unpack_ret == 0)
    {
        tuya_ble_handle_ext_module_active_info((uint8_t *)em_active_data_recv_packet.recv_data,
                                               em_active_data_recv_packet.recv_len);

        tuya_ble_em_active_data_recv_packet_free();
        return;
    }
}


static void tuya_ble_handle_ext_module_active_info(uint8_t *p_data, uint16_t data_len)
{
    tuya_ble_cb_evt_param_t event;
    uint8_t *ble_cb_evt_buffer = NULL;

    if (em_active_trsmitr_proc.version < 2)
    {
        TUYA_BLE_LOG_ERROR("em trsmitr_proc version not compatibility!");
        return;
    }

    if ((data_len < 5) || (data_len > TUYA_BLE_AIR_FRAME_MAX))
    {
        TUYA_BLE_LOG_ERROR("received em active data, length err= %d", data_len);
        return;
    }

    TUYA_BLE_LOG_HEXDUMP_INFO("received em active data", p_data, data_len);

    /* callback event to application */
    ble_cb_evt_buffer = (uint8_t *)tuya_ble_malloc(data_len);
    if (ble_cb_evt_buffer == NULL)
    {
        TUYA_BLE_LOG_ERROR("ble_cb_evt_buffer malloc failed.");
        return;
    }
    else
    {
        memset(ble_cb_evt_buffer, 0, data_len);
        memcpy(ble_cb_evt_buffer, p_data, data_len);
    }

    event.evt = TUYA_BLE_CB_EVT_EXT_MODULE_ACTIVE_INFO_RECEIVED;
    event.ext_module_active_data.data_len   = data_len;
    event.ext_module_active_data.p_data     = ble_cb_evt_buffer;

    if (tuya_ble_cb_event_send(&event) != 0)
    {
        tuya_ble_free(ble_cb_evt_buffer);
        TUYA_BLE_LOG_ERROR("tuya ble send cb event[em active info] failed.");
        return;
    }
}
#endif // (TUYA_BLE_FEATURE_EXT_MODULE_ENABLE != 0) 

#endif // (TUYA_BLE_PROTOCOL_VERSION_HIGN >= 4 && TUYA_BLE_PROTOCOL_VERSION_LOW >= 4)
