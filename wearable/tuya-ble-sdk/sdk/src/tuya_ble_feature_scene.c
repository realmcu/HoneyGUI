/**
 * \file tuya_ble_feature_scene.c
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
#include "tuya_ble_feature_scene.h"
#include "tuya_ble_feature_iot_channel.h"

#if ( (TUYA_BLE_FEATURE_IOT_CHANNEL_ENABLE != 0) && (TUYA_BLE_FEATURE_SCENE_ENABLE != 0) )

static void tuya_ble_handle_scene_request_evt(int32_t evt_id, void *data);

tuya_ble_status_t tuya_ble_feature_scene_request(tuya_ble_request_scene_t *req_data)
{
    int ret = 0;
    uint32_t subpackage_maxsize;
    tuya_ble_custom_evt_t custom_evt;
    uint8_t *p_buffer = NULL;
    tuya_ble_scene_req_data_t *p_sched_data = NULL;

    uint8_t *ble_evt_buffer = NULL;
    uint16_t ble_evt_buffer_len = 0;

    if (req_data->scene_cmd >= REQUEST_SCENE_CMD_MAX)
    {
        TUYA_BLE_LOG_ERROR("request scene cmd err, this sdk version unsupport");
        return TUYA_BLE_ERR_INVALID_PARAM;
    }

    ble_evt_buffer = (uint8_t *)tuya_ble_malloc(sizeof(tuya_ble_request_scene_t));
    if (ble_evt_buffer == NULL)
    {
        return TUYA_BLE_ERR_NO_MEM;
    }

    subpackage_maxsize = tuya_ble_iot_channel_subpackage_maxsize_get();
    ble_evt_buffer[0] = (uint8_t)(subpackage_maxsize >> 8);
    ble_evt_buffer[1] = (uint8_t)(subpackage_maxsize);
    ble_evt_buffer[2] = (uint8_t)(req_data->scene_cmd >> 8);
    ble_evt_buffer[3] = (uint8_t)(req_data->scene_cmd);

    switch (req_data->scene_cmd)
    {
    case REQUEST_SCENE_DATA:
        /* request scene data */
        if ((req_data->scene_data.nums > SUPPORT_REQUEST_SCENE_MAX_NUMS) || \
            (req_data->scene_data.name_unicode_length == 0) || \
            (req_data->scene_data.name_unicode_length % 2) != 0)
        {
            TUYA_BLE_LOG_ERROR("request scene list data err, nums=%d name_unicode_len==%d", \
                               req_data->scene_data.nums, req_data->scene_data.name_unicode_length);
            ret = -1;
        }
        else
        {
            /* the data format is tld: type + length + data */
            ble_evt_buffer[4] = 0x00;
            ble_evt_buffer[5] = 0x00;
            ble_evt_buffer[6] = 0x08;
            ble_evt_buffer[7] = req_data->scene_data.nums;
            ble_evt_buffer[8] = (uint8_t)(req_data->scene_data.name_unicode_length >> 8);
            ble_evt_buffer[9] = (uint8_t)(req_data->scene_data.name_unicode_length);
            ble_evt_buffer[10] = 0x00;
            ble_evt_buffer[11] = (uint8_t)(req_data->scene_data.check_code >> 24);
            ble_evt_buffer[12] = (uint8_t)(req_data->scene_data.check_code >> 16);
            ble_evt_buffer[13] = (uint8_t)(req_data->scene_data.check_code >> 8);
            ble_evt_buffer[14] = (uint8_t)(req_data->scene_data.check_code);

            ble_evt_buffer_len = 15;
        }
        break;

    case REQUEST_SCENE_CONTROL:
        /* request scene control */
        if (req_data->scene_control.scene_id_length > SCENE_ID_MAX_LENGTH)
        {
            TUYA_BLE_LOG_ERROR("request scene control err, id_length=%d",
                               req_data->scene_control.scene_id_length);
            ret = -1;
        }
        else
        {
            /* the data format is tld: type + length + data */
            ble_evt_buffer[4] = 0x00;
            ble_evt_buffer[5] = (uint8_t)((1 + req_data->scene_control.scene_id_length) >> 8);
            ble_evt_buffer[6] = (uint8_t)(1 + req_data->scene_control.scene_id_length);
            ble_evt_buffer[7] = req_data->scene_control.scene_id_length;
            memcpy(&ble_evt_buffer[8], req_data->scene_control.scene_id,
                   req_data->scene_control.scene_id_length);

            ble_evt_buffer_len = (8 + req_data->scene_control.scene_id_length);
        }
        break;

    default:
        TUYA_BLE_LOG_ERROR("unknow request scene cmd = %d", req_data->scene_cmd);
        ret = -1;
        break;
    }

    if (ret)
    {
        tuya_ble_free(ble_evt_buffer);
        return TUYA_BLE_ERR_INVALID_PARAM;
    }

    TUYA_BLE_LOG_HEXDUMP_DEBUG("request scene data :", ble_evt_buffer, ble_evt_buffer_len);

    p_buffer = tuya_ble_malloc(sizeof(tuya_ble_scene_req_data_t) + ble_evt_buffer_len);
    if (p_buffer == NULL)
    {
        tuya_ble_free(ble_evt_buffer);
        return TUYA_BLE_ERR_NO_MEM;
    }

    p_sched_data = (tuya_ble_scene_req_data_t *)p_buffer;
    p_sched_data->data_len = ble_evt_buffer_len;
    p_sched_data->p_data = p_buffer + sizeof(tuya_ble_scene_req_data_t);
    memcpy(p_sched_data->p_data, ble_evt_buffer, ble_evt_buffer_len);

    tuya_ble_free(ble_evt_buffer);

    custom_evt.evt_id = TUYA_BLE_EVT_SCENE_DATA_REQ;
    custom_evt.data = p_sched_data;
    custom_evt.custom_event_handler = tuya_ble_handle_scene_request_evt;

    if (tuya_ble_custom_event_send(custom_evt) != 0)
    {
        TUYA_BLE_LOG_ERROR("tuya_custom_event_send scene req error");

        tuya_ble_free((uint8_t *)p_buffer);
        return TUYA_BLE_ERR_NO_EVENT;
    }

    return TUYA_BLE_SUCCESS;
}


static void tuya_ble_handle_scene_request_evt(int32_t evt_id, void *data)
{
    tuya_ble_scene_req_data_t *evt = (tuya_ble_scene_req_data_t *)data;

    uint8_t encry_mode = 0;

    encry_mode = (tuya_ble_pair_rand_valid_get()) ? (ENCRYPTION_MODE_SESSION_KEY) :
                 (ENCRYPTION_MODE_KEY_4);

    tuya_ble_commData_send(FRM_IOT_DATA_REQUEST, 0, evt->p_data, evt->data_len, encry_mode);

    if (data != NULL)
    {
        tuya_ble_free(data);
    }
}


void tuya_ble_handle_scene_request_response(uint8_t *recv_data, uint16_t recv_len)
{
    tuya_ble_cb_evt_param_t event;
    uint16_t data_len = ((recv_data[11] << 8) | recv_data[12]);

    if (data_len != 6 && data_len != 10)
    {
        TUYA_BLE_LOG_ERROR("tuya_ble_handle_scene_request_response- invalid data len received.");
        return;
    }

    event.evt = TUYA_BLE_CB_EVT_SCENE_REQ_RESPONSE;
    event.scene_req_response_data.scene_cmd = ((recv_data[13] << 8) | recv_data[14]);
    event.scene_req_response_data.status    = recv_data[18];
    event.scene_req_response_data.err_code  = 0;
    if (event.scene_req_response_data.status != 0)
    {
        event.scene_req_response_data.err_code = ((recv_data[19] << 24) + (recv_data[20] << 16) +
                                                  (recv_data[21] << 8) + recv_data[22]);
    }

    if (tuya_ble_cb_event_send(&event) != 0)
    {
        TUYA_BLE_LOG_ERROR("tuya_ble_handle_scene_request_response-send cb event failed.");
    }
}


void tuya_ble_handle_scene_data_received(uint8_t *p_data, uint16_t data_len)
{
    uint16_t sub_cmd;
    uint8_t tld_type;
    uint16_t tld_len;
    uint8_t *tld_data = NULL;
    tuya_ble_cb_evt_param_t event;
    uint8_t *ble_cb_evt_buffer;
    uint16_t buffer_len;
    uint32_t err_code, check_code;

    memset(&event, 0, sizeof(tuya_ble_cb_evt_param_t));

    /* iot sub cmd */
    sub_cmd = (p_data[0] << 8) | p_data[1];

    /* parse tld content */
    tld_type = p_data[2];
    tld_len = (p_data[3] << 8) | p_data[4];
    tld_data = p_data + 5;

    err_code = ((tld_data[1] << 24) + (tld_data[2] << 16) + (tld_data[3] << 8) + tld_data[4]);

    switch (sub_cmd)
    {
    case REQUEST_SCENE_DATA:
        if (tld_type == 0x00)
        {
            event.evt = TUYA_BLE_CB_EVT_SCENE_DATA_RECEIVED;
            event.scene_data_received_data.status = tld_data[0];
            event.scene_data_received_data.err_code = err_code;

            event.scene_data_received_data.need_update = 0;
            event.scene_data_received_data.check_code = 0;

            if (event.scene_data_received_data.status == 0)
            {
                event.scene_data_received_data.need_update = tld_data[5];

                if (event.scene_data_received_data.need_update)
                {
                    check_code = ((tld_data[6] << 24) + (tld_data[7] << 16) + (tld_data[8] << 8) + tld_data[9]);
                    event.scene_data_received_data.check_code = check_code;
                }
            }

            if (event.scene_data_received_data.status == 0 && event.scene_data_received_data.need_update &&
                tld_len > 10)
            {
                ble_cb_evt_buffer = (uint8_t *)tuya_ble_malloc(tld_len - 10);
                if (ble_cb_evt_buffer == NULL)
                {
                    TUYA_BLE_LOG_ERROR("scene_data_received, callback buf malloc failed.");
                    return;
                }
                else
                {
                    buffer_len = (tld_len - 10);
                    memcpy(ble_cb_evt_buffer, &tld_data[10], buffer_len);

                    event.scene_data_received_data.p_data = ble_cb_evt_buffer;
                    event.scene_data_received_data.data_len = buffer_len;
                }
            }
            else
            {
                event.scene_data_received_data.p_data = NULL;
                event.scene_data_received_data.data_len = 0;
            }

            if (tuya_ble_cb_event_send(&event) != 0)
            {
                tuya_ble_free(ble_cb_evt_buffer);
                TUYA_BLE_LOG_ERROR("scene_data_received-send cb event failed.");
            }
        }
        break;

    case REQUEST_SCENE_CONTROL:
        if (tld_type == 0x00 && tld_len >= 5)
        {
            event.evt = TUYA_BLE_CB_EVT_SCENE_CTRL_RESULT_RECEIVED;
            event.scene_ctrl_received_data.status   = tld_data[0];
            event.scene_ctrl_received_data.err_code = err_code;

            uint8_t scene_id_length = tld_data[5];

            ble_cb_evt_buffer = (uint8_t *)tuya_ble_malloc(scene_id_length);
            if (ble_cb_evt_buffer == NULL)
            {
                TUYA_BLE_LOG_ERROR("scene_ctrl_result_received, callback buf malloc failed.");
                return;
            }
            else
            {
                memcpy(ble_cb_evt_buffer, &tld_data[6], scene_id_length);

                event.scene_ctrl_received_data.scene_id_len = scene_id_length;
                event.scene_ctrl_received_data.p_scene_id = ble_cb_evt_buffer;
            }

            if (tuya_ble_cb_event_send(&event) != 0)
            {
                tuya_ble_free(ble_cb_evt_buffer);
                TUYA_BLE_LOG_ERROR("scene_ctrl_result_received- send cb event failed.");
            }
        }
        break;

    default:
        break;
    }
}

#endif
