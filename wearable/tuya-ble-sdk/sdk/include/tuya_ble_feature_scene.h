/**
 * \file tuya_ble_feature_scene.h
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

#ifndef TUYA_BLE_FEATURE_SCENE_H_
#define TUYA_BLE_FEATURE_SCENE_H_

#include "tuya_ble_stdlib.h"
#include "tuya_ble_type.h"

#define SCENE_ID_MAX_LENGTH             ( 24 )

#define SUPPORT_REQUEST_SCENE_MAX_NUMS  ( 100 )


/**@brief   Request scene type. */
typedef enum
{
    REQUEST_SCENE_DATA = 1,         /**< request scene list data. */
    REQUEST_SCENE_CONTROL,          /**< request scene control. */

    REQUEST_SCENE_CMD_MAX,
} request_scene_cmd;


/**@brief   Request scene list data structure. */
typedef struct
{
    uint8_t nums;                   /**< request scene numbers, rang from [1-SUPPORT_REQUEST_SCENE_MAX_NUMS]. */
    uint16_t name_unicode_length;   /**< request scene name length, The encoding format is unicode, It must be a multiple of 2. */
    uint32_t check_code;            /**< scene check code, algorithm-crc32. */
} request_scene_list_data_t;


/**@brief   Request scene control structure. */
typedef struct
{
    uint8_t scene_id_length;                /**< scene id length. */
    uint8_t scene_id[SCENE_ID_MAX_LENGTH];  /**< scene id, Max length equal to SCENE_ID_MAX_LENGTH. */
} request_scene_control_t;


/**@brief   Request scene cmd structure. */
typedef struct
{
    request_scene_cmd scene_cmd;                    /**< request scene cmd. */

    union
    {
        request_scene_list_data_t   scene_data;     /**< request scene list data info. */
        request_scene_control_t     scene_control;  /**< request scene ctrl info. */
    };
} tuya_ble_request_scene_t;


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief  How to use tuya_ble_feature_scene component function, steps are:
 *
 * Step1:   Add tuya_ble_feature_iot_channel.c and tuya_ble_feature_scene.c file into project.
 *
 * Step2:   Define TUYA_BLE_FEATURE_IOT_CHANNEL_ENABLE and TUYA_BLE_FEATURE_SCENE_ENABLE macro in the <custom_tuya_ble_config.h> file, such as:
 *
 * @code
        // If it is 1, the sdk support common iot data channel
        #define TUYA_BLE_FEATURE_IOT_CHANNEL_ENABLE     ( 1 )

        #if ( TUYA_BLE_FEATURE_IOT_CHANNEL_ENABLE != 0 )
        // If it is 1, the sdk support scene list data request and control
        #define TUYA_BLE_FEATURE_SCENE_ENABLE           ( 1 )
        #endif
 * @endcode
 *
 * Step3:   In tuya_cb_handler() function add callback events, The following is the code how to process the callback events.
 *
 * @code
        case TUYA_BLE_CB_EVT_SCENE_REQ_RESPONSE:
            TUYA_APP_LOG_INFO("received scene request response result code=[%d], err_code=[%d] sub_cmd=[%d]", \
                                event->scene_req_response_data.status, \
                                event->scene_req_response_data.err_code, \
                                event->scene_req_response_data.scene_cmd);
            break;

        case TUYA_BLE_CB_EVT_SCENE_DATA_RECEIVED:
            TUYA_APP_LOG_INFO("received scene data result, status=[%d] err_code=[%d] need_update=[%d] check_code=[0x%X]", \
                                event->scene_data_received_data.status, \
                                event->scene_data_received_data.err_code, \
                                event->scene_data_received_data.need_update, \
                                event->scene_data_received_data.check_code);

            if (event->scene_data_received_data.status == 0 && event->scene_data_received_data.need_update)
            {
                // TODO .. update scene check code.

                if (event->scene_data_received_data.data_len != 0)
                {
                    uint8_t *iot_scene_object;
                    uint16_t scene_id_len, scene_name_len;
                    uint16_t object_len = 0;

                    for (;;)
                    {
                        iot_scene_object = (uint8_t *)(event->scene_data_received_data.p_data + object_len);

                        scene_id_len = iot_scene_object[0];
                        scene_name_len = (iot_scene_object[1+scene_id_len] << 8) + iot_scene_object[2+scene_id_len];

                        TUYA_APP_LOG_HEXDUMP_DEBUG("scene id :", &iot_scene_object[1], scene_id_len);
                        TUYA_APP_LOG_HEXDUMP_DEBUG("scene name unicode :", &iot_scene_object[3+scene_id_len], scene_name_len);

                        object_len += (3 + scene_id_len + scene_name_len);
                        if (object_len >= event->scene_data_received_data.data_len)
                            break;
                    }
                }
            }
            break;

        case TUYA_BLE_CB_EVT_SCENE_CTRL_RESULT_RECEIVED:
            TUYA_APP_LOG_INFO("received scene ctrl result, status=[%d] err_code=[%d]", \
                                event->scene_ctrl_received_data.status, \
                                event->scene_ctrl_received_data.err_code);
            TUYA_APP_LOG_HEXDUMP_DEBUG("scene ctrl id :", event->scene_ctrl_received_data.p_scene_id, event->scene_ctrl_received_data.scene_id_len);
            break;
 * @endcode
 *
 * Step4:   Developers could call tuya_ble_feature_scene_request(tuya_ble_request_scene_t *req_data) api to request scene data,
 *          input parameter see the struct of tuya_ble_request_scene_t for details. For example request scene list data:
 *
 * @code
        tuya_ble_request_scene_t req_scene_data;

        req_scene_data.cmd_type = REQUEST_SCENE_DATA;
        req_scene_data.scene_data.nums = 1;
        req_scene_data.scene_data.name_unicode_length = 10;
        req_scene_data.scene_data.check_code = 0;

        tuya_ble_feature_scene_request(&req_scene_data);
 * @endcode
 *
 */


/**
 * @brief   Function for request scene
 *
 * @param[in] req_data  For details, see the struct of tuya_ble_request_scene_t
 *
 * @retval  TUYA_BLE_ERR_INVALID_PARAM  The provided parameters are not valid.
 * @retval  TUYA_BLE_ERR_NO_MEM         If no memory is available to accept the operation.
 * @retval  TUYA_BLE_ERR_NO_EVENT       If iot data request message send failed.
 * @retval  TUYA_BLE_SUCCESS            Successful.
 * */
extern tuya_ble_status_t tuya_ble_feature_scene_request(tuya_ble_request_scene_t *req_data);


/**
 * @brief   Function for handler scene request response
 *
 * @details Internal use of tuya ble sdk
 *
 * @param[in] recv_data     The point of received response data
 * @param[in] recv_len      The numbers of data
 *
 * */
extern void tuya_ble_handle_scene_request_response(uint8_t *recv_data, uint16_t recv_len);


/**
 * @brief   Function for handler scene data received
 *
 * @details Internal use of tuya ble sdk
 *
 * @param[in] recv_data     The point of recvived scene data
 * @param[in] recv_len      The length of data
 *
 * */
void tuya_ble_handle_scene_data_received(uint8_t *p_data, uint16_t data_len);


#ifdef __cplusplus
}
#endif

#endif /* TUYA_BLE_FEATURE_SCENE_H_ */
