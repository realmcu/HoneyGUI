/**
 * \file tuya_ble_vos.h
 *
 * \brief
 */
/*
 *  Copyright (C) 2014-2022, Tuya Inc., All Rights Reserved
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

#ifndef TUYA_BLE_VOS_H_
#define TUYA_BLE_VOS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
 * INCLUDE
 */
#include "tuya_ble_stdlib.h"
#include "tuya_ble_type.h"
#include "tuya_ble_config.h"

#if defined(__CC_ARM)
#pragma anon_unions
#elif defined(__ICCARM__)
#pragma language = extended
#elif defined(__GNUC__)
/* anonymous unions are enabled by default */
#endif

#if TUYA_BLE_VOS_ENABLE

#define  AudioFormat_PCM_L16_16KHZ_MONO  0
#define  AudioFormat_OPUS_16KHZ_32KBPS_CBR_0_20MS  1
#define  AudioFormat_OPUS_16KHZ_16KBPS_CBR_0_20MS  2
#define  AudioFormat_MSBC   3


#define  AudioProfile_CLOSE_TALK   0
#define  AudioProfile_NEAR_FIELD   1
#define  AudioProfile_FAR_FIELD    2


#define OPUS_IN_RING_BUFF_SIZE      640
#define OPUS_IN_FRAME_SIZE          320
#define OPUS_OUT_RING_BUFF_SIZE     80

#define MSBC_IN_RING_BUFF_SIZE      240
#define MSBC_IN_FRAME_SIZE          120
#define MSBC_OUT_RING_BUFF_SIZE     57


#define AUDIO_FORMAT           AudioFormat_MSBC//AudioFormat_MSBC//AudioFormat_OPUS_16KHZ_32KBPS_CBR_0_20MS
#define AUDIO_PROFILE          AudioProfile_NEAR_FIELD
#define AUDIO_SUPPRESS_EARCON  0
#define AUDIO_PLAY_VOICE       1

#if (AUDIO_FORMAT == AudioFormat_MSBC)
#define CODEC_IN_RING_BUFF_SIZE     MSBC_IN_RING_BUFF_SIZE
#define CODEC_OUT_RING_BUFF_SIZE    MSBC_OUT_RING_BUFF_SIZE
#define AM_SPP_FRAME_SAMPLES        MSBC_IN_FRAME_SIZE
#elif (AUDIO_FORMAT == AudioFormat_OPUS_16KHZ_32KBPS_CBR_0_20MS)
#define CODEC_IN_RING_BUFF_SIZE     OPUS_IN_RING_BUFF_SIZE
#define CODEC_OUT_RING_BUFF_SIZE    OPUS_OUT_RING_BUFF_SIZE
#define AM_SPP_FRAME_SAMPLES        OPUS_IN_FRAME_SIZE
#else
#error "The current audio encoding format is not supported.!"
#endif


#define SAMPLE_8BIT               1
#define SAMPLE_16BIT              2
#define SAMPLE_32BIT              4

#define AUDIO_PREROLL_TIME_MS     500


#define AM_SPP_SAMPLE_FREQ          16000
#define AM_SPP_OUT_DATA_FORMAT      SAMPLE_16BIT

#define AUDIO_KWD_TIMEOUT_S         20//8


#define AUDIO_RESULT_DATA_MAX_SIZE           2048
#define AUDIO_RESULT_DATA_MAX_PACKET_SIZE    512


#if (AUDIO_FORMAT == AudioFormat_MSBC)
#define  TUYA_BLE_VOS_DATA_BUFFER_SIZE  (CODEC_OUT_RING_BUFF_SIZE*20)
#define  TUYA_BLE_AUDIO_BUFFER_ENCODED_NUM  300
#elif (AUDIO_FORMAT == AudioFormat_OPUS_16KHZ_32KBPS_CBR_0_20MS)
#define  TUYA_BLE_VOS_DATA_BUFFER_SIZE  (CODEC_OUT_RING_BUFF_SIZE*6)
#define  TUYA_BLE_AUDIO_BUFFER_ENCODED_NUM  50
#else
#error "The current audio encoding format is not supported.!"
#endif



typedef struct
{
    uint8_t     ButtonDownFlag;              // 1: the button is pushed down; 0: no button is pushed.
    uint8_t     PushTalkFlag;                // trig for push-to-talk.
    uint8_t     ProvideSpeechFlag;           // trig for conversation talk.

    uint32_t    StreamingTimeOutCount;      // Voice streaming timeout counter

    int32_t     DelayedSample;
    int32_t     EndingSampleCnt;
    int32_t     PreRollDelayMs;

} tuya_ble_vos_sys_info_t;

/**@brief   Macro for defining bulk data transfer protocol. */

#define TUYA_BLE_VOS_CMD_START_SPEECH      0x01    //start speech
#define TUYA_BLE_VOS_CMD_PROVIDE_SPEECH    0x02    //provide speech
#define TUYA_BLE_VOS_CMD_ENDPOINT_SPEECH   0x03    //endpoint speech
#define TUYA_BLE_VOS_CMD_STOP_SPEECH       0x04    //stop speech
#define TUYA_BLE_VOS_CMD_SPEECH_STATE      0x05    //notify speech state


typedef enum
{
    TUYA_VOS_ERROR_CODE_SUCCESS = 0,
    TUYA_VOS_ERROR_CODE_UNKNOWN = 1,
    TUYA_VOS_ERROR_CODE_INTERNAL = 2,
    TUYA_VOS_ERROR_CODE_UNSUPPORTED = 3,
    TUYA_VOS_ERROR_CODE_USER_CANCELLED = 4,
    TUYA_VOS_ERROR_CODE_NOT_FOUND = 5,
    TUYA_VOS_ERROR_CODE_INVALID = 6,
    TUYA_VOS_ERROR_CODE_BUSY = 7
} tuya_vos_error_code_t;


void tuya_ble_handle_speech_req(uint16_t cmd, uint8_t *p_recv_data, uint32_t recv_data_len);

uint32_t tuya_vos_init(void);

tuya_ble_status_t tuya_ble_start_speech(uint8_t type);

tuya_ble_status_t tuya_ble_stop_speech(void);

void tuya_ble_vos_disconnect_handler(void);

void tuya_vos_token_reset(void);

tuya_ble_status_t tuya_vos_encoded_audio_data_send(uint8_t *p_data, uint32_t size);

tuya_ble_status_t tuya_ble_vos_timer_response(tuya_vos_timer_cmd_t cmd, tuya_vos_timer_type_t type,
                                              uint8_t *p_token_md5, uint8_t status);


#endif


#ifdef __cplusplus
}
#endif

#endif // 







