
#include "tuya_ble_stdlib.h"
#include "tuya_ble_type.h"
#include "tuya_ble_heap.h"
#include "tuya_ble_mem.h"
#include "tuya_ble_api.h"
#include "tuya_ble_port.h"
#include "tuya_ble_main.h"
#include "tuya_ble_secure.h"
#include "tuya_ble_data_handler.h"
#include "tuya_ble_mutli_tsf_protocol.h"
#include "tuya_ble_storage.h"
#include "tuya_ble_sdk_version.h"
#include "tuya_ble_utils.h"
#include "tuya_ble_event.h"
#include "tuya_ble_log.h"
#include "tuya_ble_vos.h"
#include "tuya_ble_encoded_audio_buffer.h"

#if TUYA_BLE_VOS_ENABLE


static tuya_ble_vos_sys_info_t vos_sys_info =
{
    .ButtonDownFlag = 0,              // 1: the button is pushed down; 0: no button is pushed.
    .PushTalkFlag = 0,                // trig for AMA push-to-talk.
    .StreamingTimeOutCount = 0,
    .ProvideSpeechFlag = 0,           // trig for AMA conversation talk.

    .DelayedSample = 0,
    .EndingSampleCnt = 0,
    .PreRollDelayMs = AUDIO_PREROLL_TIME_MS,
};

typedef struct
{
    uint32_t recv_len;
    uint32_t recv_len_max;
    uint8_t  *recv_data;
} tuya_ble_common_recv_packet;


static  tuya_ble_common_recv_packet  audio_recv_packet;
static frm_trsmitr_proc_s audio_trsmitr_proc;

static frm_trsmitr_proc_s token_trsmitr_proc_send;
static uint8_t *p_token_send_buffer = NULL;
static uint16_t token_send_buffer_length = 0;

static  tuya_ble_common_recv_packet  token_recv_packet;
static frm_trsmitr_proc_s token_trsmitr_proc;


#define TUYA_BLE_VOS_TOKEN_DATA_MAX_SIZE  2048


typedef struct
{
    uint8_t is_token_valid;
    uint8_t token_md5[16];
    uint16_t token_length;
} tuya_ble_vos_token_data_t;

static tuya_ble_vos_token_data_t current_token;



#define  START_SPEECH_EVENT  0
#define  STOP_SPEECH_EVENT   1
#define  AUDIO_DATA_EVENT    2

static tuya_ble_speech_state_t current_speech_state = SpeechState_IDLE;

static uint32_t dialog_id = 0;

static uint16_t sn = 0;
static uint8_t audio_data_send_flag = 0;
static uint8_t ble_buffer[7 + TUYA_BLE_VOS_DATA_BUFFER_SIZE];

static uint8_t audio_process_flag = 0;


static void tuya_vos_reset_detected_flag(void)
{
    vos_sys_info.PushTalkFlag = 0;
    vos_sys_info.ProvideSpeechFlag = 0;
    vos_sys_info.StreamingTimeOutCount = 0;
}

static void tuya_vos_audio_flush_ring_buffer(void)
{
    tuya_ble_encoded_audio_buffer_init();
}


static void tuya_ble_speech_reset(void)
{
    audio_data_send_flag = 0;
    sn = 0;
}


static void tuya_ble_common_recv_packet_free(tuya_ble_common_recv_packet *p_recv_packet)
{
    if (p_recv_packet->recv_data)
    {
        tuya_ble_free(p_recv_packet->recv_data);
        p_recv_packet->recv_data = NULL;
        p_recv_packet->recv_len_max = 0;
        p_recv_packet->recv_len = 0;
    }
}


static void tuya_vos_audio_reset_flag_and_buffer(void)
{
    tuya_vos_audio_flush_ring_buffer();
    tuya_vos_reset_detected_flag();

    audio_process_flag = 0;
    tuya_ble_speech_reset();
    tuya_ble_common_recv_packet_free(&audio_recv_packet);
}


void tuya_ble_vos_disconnect_handler(void)
{
    tuya_vos_audio_reset_flag_and_buffer();

    tuya_ble_common_recv_packet_free(&audio_recv_packet);
    tuya_ble_common_recv_packet_free(&token_recv_packet);

    if (p_token_send_buffer)
    {
        tuya_ble_free(p_token_send_buffer);
    }
    token_send_buffer_length = 0;

}

static void tuya_vos_token_init(void)
{
    uint8_t *p_buffer = NULL;
    uint8_t token_md5_temp[16];

    p_buffer = tuya_ble_malloc(TUYA_BLE_VOS_TOKEN_DATA_MAX_SIZE);

    if (p_buffer)
    {
        tuya_ble_nv_read(TUYA_NV_VOS_TOKEN_START_ADDR, p_buffer, TUYA_BLE_VOS_TOKEN_DATA_MAX_SIZE);
        current_token.token_length = (p_buffer[17] << 8) + p_buffer[18];
        memcpy(current_token.token_md5, p_buffer + 1, 16);

        if ((current_token.token_length == 0) ||
            (current_token.token_length > (TUYA_BLE_VOS_TOKEN_DATA_MAX_SIZE - 18)))
        {
            TUYA_BLE_LOG_ERROR("tuya_vos_token_init->token length error, length = %d",
                               current_token.token_length);
            memset(&current_token, 0, sizeof(tuya_ble_vos_token_data_t));
            tuya_ble_free(p_buffer);
            return;
        }

        memset(token_md5_temp, 0, sizeof(token_md5_temp));
        tuya_ble_md5_crypt(p_buffer + 19, current_token.token_length, token_md5_temp);
        if (memcmp(token_md5_temp, current_token.token_md5, 16))
        {
            TUYA_BLE_LOG_ERROR("tuya_vos_token_init->token md5 check failed.");
            memset(&current_token, 0, sizeof(tuya_ble_vos_token_data_t));
            tuya_ble_free(p_buffer);
            return;
        }
        tuya_ble_free(p_buffer);
        current_token.is_token_valid = 1;

    }
    else
    {
        TUYA_BLE_LOG_ERROR("tuya_vos_token_init->buffer malloc failed.");
    }
}


void tuya_vos_token_reset(void)
{
    if (current_token.is_token_valid)
    {
        tuya_ble_nv_erase(TUYA_NV_VOS_TOKEN_START_ADDR, TUYA_NV_VOS_TOKEN_AREA_SIZE);
    }
    memset(&current_token, 0, sizeof(tuya_ble_vos_token_data_t));
}


uint32_t tuya_vos_init(void)
{
    uint32_t err_code = NRF_SUCCESS;

    memset(&audio_recv_packet, 0, sizeof(tuya_ble_common_recv_packet));

    tuya_vos_audio_reset_flag_and_buffer();

    tuya_vos_token_init();

    TUYA_BLE_LOG_DEBUG("current_token->is_token_valid = %d", current_token.is_token_valid);

    return 0;

}

uint8_t tuya_ble_audio_process_flag_get(void)
{
    return audio_process_flag;
}

static tuya_ble_speech_state_t tuya_ble_vos_speech_state_get(void)
{
    return  current_speech_state;
}


static void tuya_ble_start_speech_handler(uint32_t evt_id, void *data)
{
    tuya_vos_audio_reset_flag_and_buffer();
    audio_process_flag = 1;
    vos_sys_info.PushTalkFlag = 1;
    sn = 0;
    TUYA_BLE_LOG_DEBUG("[VoS] Push to talk!");

}


static void tuya_ble_provide_speech_handler(void)
{
    ble_buffer[0] = 0;
    ble_buffer[1] = 1;  //ack
    ble_buffer[2] = TUYA_BLE_VOS_CMD_PROVIDE_SPEECH;
    ble_buffer[3] = AUDIO_FORMAT;
    ble_buffer[4] = AUDIO_PROFILE;
    ble_buffer[5] = dialog_id >> 24;
    ble_buffer[6] = dialog_id >> 16;
    ble_buffer[7] = dialog_id >> 8;
    ble_buffer[8] = dialog_id;

    tuya_ble_commData_send(TUYA_BLE_FRM_VOS_CMD, 0, ble_buffer, 9, ENCRYPTION_MODE_SESSION_KEY);
    TUYA_BLE_LOG_DEBUG("[Vos] Send provide speech cmd ack dialog_id = %d.", dialog_id);

    tuya_vos_audio_reset_flag_and_buffer();
    audio_process_flag = 1;
    vos_sys_info.ProvideSpeechFlag = 1;
    sn = 0;
    TUYA_BLE_LOG_DEBUG("[VoS] Provide Speech ...");

}


static void tuya_ble_send_stop_speech(uint8_t ack, tuya_vos_error_code_t err_code)
{
    uint8_t buffer[8];
    uint8_t data_length = 0;

    buffer[0] = 0;
    buffer[1] = ack;
    buffer[2] = TUYA_BLE_VOS_CMD_STOP_SPEECH;
    if (ack == 0)
    {
        buffer[3] = err_code;
        buffer[4] = dialog_id >> 24;
        buffer[5] = dialog_id >> 16;
        buffer[6] = dialog_id >> 8;
        buffer[7] = dialog_id;
        data_length = 8;
    }
    else
    {
        buffer[3] = dialog_id >> 24;
        buffer[4] = dialog_id >> 16;
        buffer[5] = dialog_id >> 8;
        buffer[6] = dialog_id;
        data_length = 7;
    }

    tuya_ble_commData_send(TUYA_BLE_FRM_VOS_CMD, 0, buffer, data_length, ENCRYPTION_MODE_SESSION_KEY);

}


static void tuya_ble_stop_speech_handler(uint32_t evt_id, void *data)
{
    tuya_ble_send_stop_speech(0, TUYA_VOS_ERROR_CODE_USER_CANCELLED);

    tuya_ble_device_enter_critical();
    tuya_vos_audio_reset_flag_and_buffer();
    audio_process_flag = 0;
    tuya_ble_device_exit_critical();

    TUYA_BLE_LOG_DEBUG("[VoS] Stop speech Cmd because user cancelled,dialog_id = %d .", dialog_id);

}

tuya_ble_status_t tuya_ble_start_speech(uint8_t type)
{
    tuya_ble_custom_evt_t event;

    (void) type;

    if (0 != tuya_ble_audio_process_flag_get())
    {
        TUYA_BLE_LOG_ERROR("speech busy!");
        return TUYA_BLE_ERR_BUSY;
    }
    if (tuya_ble_connect_status_get() != BONDING_CONN)
    {
        TUYA_BLE_LOG_ERROR("Invalid State!");
        return TUYA_BLE_ERR_INVALID_STATE;
    }
    if (current_token.is_token_valid == 0)
    {
        TUYA_BLE_LOG_ERROR("Invalid Token!");
        return TUYA_BLE_ERR_INVALID_TOKEN;
    }
    event.evt_id = START_SPEECH_EVENT;
    event.custom_event_handler = (void *)tuya_ble_start_speech_handler;
    if (tuya_ble_custom_event_send(event))
    {
        TUYA_BLE_LOG_ERROR("start speech send event error!");
        return TUYA_BLE_ERR_NO_EVENT;
    }
    return TUYA_BLE_SUCCESS;
}

tuya_ble_status_t tuya_ble_stop_speech(void)
{
    tuya_ble_custom_evt_t event;

    if (tuya_ble_connect_status_get() != BONDING_CONN)
    {
        TUYA_BLE_LOG_ERROR("Invalid State!");
        return TUYA_BLE_ERR_INVALID_STATE;
    }

    event.evt_id = STOP_SPEECH_EVENT;
    event.custom_event_handler = (void *)tuya_ble_stop_speech_handler;
    if (tuya_ble_custom_event_send(event))
    {
        TUYA_BLE_LOG_ERROR("stop speech send event error!");
        return TUYA_BLE_ERR_NO_EVENT;
    }
    return TUYA_BLE_SUCCESS;
}



static void tuya_vos_codec_data_handler(void)
{
    int32_t i32CompressedLen;
    uint32_t ui32StreamLen;
    uint8_t cnt = 0;

    tuya_ble_device_enter_critical();
    ui32StreamLen = tuya_ble_get_encoded_audio_buffer_used();
    tuya_ble_device_exit_critical();

    if ((ui32StreamLen >= TUYA_BLE_VOS_DATA_BUFFER_SIZE) && (audio_data_send_flag == 0))
    {

        audio_data_send_flag = 1;
        ble_buffer[0] = 0;
        ble_buffer[1] = sn >> 8;
        ble_buffer[2] = sn;
        ble_buffer[3] = 0x00;
        ble_buffer[4] = (AUDIO_FORMAT & 0x0F);
        ble_buffer[5] = (TUYA_BLE_VOS_DATA_BUFFER_SIZE) >> 8;
        ble_buffer[6] = (TUYA_BLE_VOS_DATA_BUFFER_SIZE) & 0xFF;


        tuya_ble_encoded_audio_buffer_pop(&ble_buffer[7], TUYA_BLE_VOS_DATA_BUFFER_SIZE);


        TUYA_BLE_LOG_HEXDUMP_DEBUG("[Vos] msbc buffer data :", ble_buffer, CODEC_OUT_RING_BUFF_SIZE + 7);
        tuya_ble_commData_send(TUYA_BLE_FRM_VOS_STREAMING, 0, ble_buffer, 7 + TUYA_BLE_VOS_DATA_BUFFER_SIZE,
                               ENCRYPTION_MODE_NONE);
        sn++;

    }

    TUYA_BLE_LOG_DEBUG("streamlen = %d", ui32StreamLen);


}


static void tuya_vos_encoded_audio_data_handler(uint32_t evt_id, void *data)
{
    tuya_ble_cb_evt_param_t event;

    if (vos_sys_info.PushTalkFlag || vos_sys_info.ProvideSpeechFlag)
    {

        if (vos_sys_info.StreamingTimeOutCount < AUDIO_KWD_TIMEOUT_S * AM_SPP_SAMPLE_FREQ)
        {

            if (vos_sys_info.StreamingTimeOutCount == 0)
            {

                if (tuya_ble_connect_status_get() == BONDING_CONN)
                {

                    if (!vos_sys_info.ProvideSpeechFlag)
                    {
                        dialog_id++;
                        ble_buffer[0] = 0;
                        ble_buffer[1] = 0;
                        ble_buffer[2] = TUYA_BLE_VOS_CMD_START_SPEECH;
                        ble_buffer[3] = AUDIO_FORMAT;
                        ble_buffer[4] = AUDIO_PROFILE;
                        ble_buffer[5] = AUDIO_SUPPRESS_EARCON;
                        ble_buffer[6] = dialog_id >> 24;
                        ble_buffer[7] = dialog_id >> 16;
                        ble_buffer[8] = dialog_id >> 8;
                        ble_buffer[9] = dialog_id;
                        ble_buffer[10] = AUDIO_PLAY_VOICE;
                        ble_buffer[11] = AUDIO_RESULT_DATA_MAX_PACKET_SIZE >> 8;
                        ble_buffer[12] = AUDIO_RESULT_DATA_MAX_PACKET_SIZE & 0xFF;

                        tuya_ble_commData_send(TUYA_BLE_FRM_VOS_CMD, 0, ble_buffer, 13, ENCRYPTION_MODE_SESSION_KEY);
                        TUYA_BLE_LOG_DEBUG("[Vos] send start speech cmd dialog_id = %d.", dialog_id);

                    }
                }
                else
                {
                    TUYA_BLE_LOG_WARNING("[Vos] Not connected to Tuya App.");
                }

            }

            if (tuya_ble_connect_status_get() == BONDING_CONN)
            {
                tuya_vos_codec_data_handler();
            }
            vos_sys_info.StreamingTimeOutCount += AM_SPP_FRAME_SAMPLES;
        }
        else
        {
            TUYA_BLE_LOG_WARNING("[Vos] AUDIO kwd timeout .");

            tuya_vos_audio_reset_flag_and_buffer();
            audio_process_flag = 0;
            tuya_ble_speech_reset();

            event.evt = TUYA_BLE_CB_EVT_AVS_SPEECH_CMD_RES;
            event.avs_cmd_event_data.cmd = AVS_SPEECH_CMD;
            event.avs_cmd_event_data.cmd_data.type = AVS_SPEECH_CMD_STOP;
            event.avs_cmd_event_data.cmd_data.err_code = 8; //timeout
            if (tuya_ble_cb_event_send(&event) != 0)
            {
                TUYA_BLE_LOG_ERROR("tuya ble send stop speech cmd callback event failed error code = %d",
                                   event.avs_cmd_event_data.cmd_data.err_code);
            }
        }
    }

}


tuya_ble_status_t tuya_vos_encoded_audio_data_send(uint8_t *p_data, uint32_t size)
{
    tuya_ble_custom_evt_t event;

    if (size != CODEC_OUT_RING_BUFF_SIZE)
    {
        TUYA_BLE_LOG_ERROR("size != CODEC_OUT_RING_BUFF_SIZE");
        return TUYA_BLE_ERR_INVALID_LENGTH;
    }

    if (vos_sys_info.PushTalkFlag || vos_sys_info.ProvideSpeechFlag)
    {

        tuya_ble_encoded_audio_buffer_push(p_data, size);

        event.evt_id = AUDIO_DATA_EVENT;
        event.custom_event_handler = (void *)tuya_vos_encoded_audio_data_handler;
        if (tuya_ble_custom_event_send(event))
        {
            TUYA_BLE_LOG_ERROR("[Vos] tuya_vos_encoded_audio_data_send failed.");
            return TUYA_BLE_ERR_BUSY;
        }

    }

    return TUYA_BLE_SUCCESS;
}


static void tuya_ble_send_endpoint_speech(uint8_t ack)
{
    uint8_t buffer[7];

    buffer[0] = 0;
    buffer[1] = ack;
    buffer[2] = TUYA_BLE_VOS_CMD_ENDPOINT_SPEECH;
    buffer[3] = dialog_id >> 24;
    buffer[4] = dialog_id >> 16;
    buffer[5] = dialog_id >> 8;
    buffer[6] = dialog_id;

    tuya_ble_commData_send(TUYA_BLE_FRM_VOS_CMD, 0, buffer, 7, ENCRYPTION_MODE_SESSION_KEY);

}


static void tuya_ble_send_audio_data(uint16_t send_sn)
{
    uint32_t ui32StreamLen = 0;

    tuya_ble_device_enter_critical();
    ui32StreamLen = tuya_ble_get_encoded_audio_buffer_used();
    tuya_ble_device_exit_critical();

    if (ui32StreamLen >= TUYA_BLE_VOS_DATA_BUFFER_SIZE)
    {

        ble_buffer[0] = 0;
        ble_buffer[1] = sn >> 8;
        ble_buffer[2] = sn;
        ble_buffer[3] = 0x00;
        ble_buffer[4] = (AUDIO_FORMAT & 0x0F);
        ble_buffer[5] = (TUYA_BLE_VOS_DATA_BUFFER_SIZE) >> 8;
        ble_buffer[6] = (TUYA_BLE_VOS_DATA_BUFFER_SIZE) & 0xFF;

        tuya_ble_encoded_audio_buffer_pop(&ble_buffer[7], TUYA_BLE_VOS_DATA_BUFFER_SIZE);

        tuya_ble_commData_send(TUYA_BLE_FRM_VOS_STREAMING, 0, ble_buffer, 7 + TUYA_BLE_VOS_DATA_BUFFER_SIZE,
                               ENCRYPTION_MODE_NONE);
        sn++;

    }
    else
    {
        if (audio_data_send_flag == 1)
        {
            audio_data_send_flag = 0;
        }
    }

}


static void tuya_ble_handle_speech_cmd(uint8_t *p_recv_data, uint32_t recv_data_len)
{
    uint8_t cmd = p_recv_data[2];
    uint8_t is_ack = p_recv_data[1];
    uint32_t dialog_id_temp;
    uint8_t status;
    uint8_t speech_state;
    uint8_t speech_state_update_app = 0;
    tuya_ble_cb_evt_param_t event;

    switch (cmd)
    {
    case TUYA_BLE_VOS_CMD_START_SPEECH:
        if (is_ack)
        {
            if (recv_data_len != 8)
            {
                TUYA_BLE_LOG_ERROR("ERROR,invalid start speech cmd ack parameters.");
                tuya_ble_device_enter_critical();
                tuya_vos_audio_reset_flag_and_buffer();
                audio_process_flag = 0;
                tuya_ble_device_exit_critical();
            }
            else
            {
                dialog_id_temp = (p_recv_data[3] << 24) + (p_recv_data[4] << 16) + (p_recv_data[5] << 8) +
                                 p_recv_data[6];
                status = p_recv_data[7];
                TUYA_BLE_LOG_DEBUG("received start speech cmd ack dialog id = %d, status = %d.", dialog_id_temp,
                                   status);

                event.evt = TUYA_BLE_CB_EVT_AVS_SPEECH_CMD_RES;
                event.avs_cmd_event_data.cmd = AVS_SPEECH_CMD_RESPONSE;
                event.avs_cmd_event_data.res_data.type = AVS_SPEECH_CMD_RES_START;
                event.avs_cmd_event_data.res_data.status = status;
                if ((dialog_id_temp != dialog_id) || (status != 0))
                {
                    TUYA_BLE_LOG_ERROR("Start Speech Error.");
                    tuya_ble_device_enter_critical();
                    tuya_vos_audio_reset_flag_and_buffer();
                    audio_process_flag = 0;
                    tuya_ble_device_exit_critical();
                }
                else
                {
                    TUYA_BLE_LOG_DEBUG("Start Speech Succeed.");
                    sn = 0;
                }

                if (tuya_ble_cb_event_send(&event) != 0)
                {
                    TUYA_BLE_LOG_ERROR("tuya ble send start speech cmd response callback event failed.");
                }

            }
        }
        else
        {
            TUYA_BLE_LOG_ERROR("ERROR,received start speech cmd with not ack.");
            tuya_ble_device_enter_critical();
            tuya_vos_audio_reset_flag_and_buffer();
            audio_process_flag = 0;
            tuya_ble_device_exit_critical();
        }
        break;
    case TUYA_BLE_VOS_CMD_PROVIDE_SPEECH:
        if (is_ack)
        {
            TUYA_BLE_LOG_ERROR("ERROR,received provide speech cmd with ack.");
        }
        else
        {
            tuya_ble_provide_speech_handler();
            event.evt = TUYA_BLE_CB_EVT_AVS_SPEECH_CMD_RES;
            event.avs_cmd_event_data.cmd = AVS_SPEECH_CMD;
            event.avs_cmd_event_data.cmd_data.type = AVS_SPEECH_CMD_PROVIDE;
            event.avs_cmd_event_data.cmd_data.err_code = 0;
            if (tuya_ble_cb_event_send(&event) != 0)
            {
                TUYA_BLE_LOG_ERROR("tuya ble send provide speech cmd callback event failed.");
            }
        }
        break;
    case TUYA_BLE_VOS_CMD_ENDPOINT_SPEECH:

        if (is_ack)
        {
            TUYA_BLE_LOG_ERROR("ERROR,received endpoint speech ack.");
        }
        else
        {

            dialog_id_temp = (p_recv_data[3] << 24) + (p_recv_data[4] << 16) + (p_recv_data[5] << 8) +
                             p_recv_data[6];
            if (dialog_id_temp == dialog_id)
            {
                TUYA_BLE_LOG_DEBUG("Received Endpoint Speech Cmd ,dialog id = %d.", dialog_id_temp);
                tuya_ble_send_endpoint_speech(1);

                tuya_vos_audio_reset_flag_and_buffer();

                event.evt = TUYA_BLE_CB_EVT_AVS_SPEECH_CMD_RES;
                event.avs_cmd_event_data.cmd = AVS_SPEECH_CMD;
                event.avs_cmd_event_data.cmd_data.type = AVS_SPEECH_CMD_ENDPOINT;
                event.avs_cmd_event_data.cmd_data.err_code = 0;
                if (tuya_ble_cb_event_send(&event) != 0)
                {
                    TUYA_BLE_LOG_ERROR("tuya ble send endpoint speech cmd callback event failed.");
                }

            }
            else
            {
                TUYA_BLE_LOG_WARNING("Received Endpoint Speech Cmd with invaild id,dialog id = %d.",
                                     dialog_id_temp);
            }


        }
        break;

    case TUYA_BLE_VOS_CMD_STOP_SPEECH:  //  stop speech cmd
        if (is_ack)
        {
            TUYA_BLE_LOG_DEBUG("Received stop speech ack.");
            event.evt = TUYA_BLE_CB_EVT_AVS_SPEECH_CMD_RES;
            event.avs_cmd_event_data.cmd = AVS_SPEECH_CMD_RESPONSE;
            event.avs_cmd_event_data.res_data.type = AVS_SPEECH_CMD_RES_STOP;
            event.avs_cmd_event_data.res_data.status = 0;
            if (tuya_ble_cb_event_send(&event) != 0)
            {
                TUYA_BLE_LOG_ERROR("tuya ble send stop speech cmd response callback event failed.");
            }
        }
        else
        {
            dialog_id_temp = (p_recv_data[4] << 24) + (p_recv_data[5] << 16) + (p_recv_data[6] << 8) +
                             p_recv_data[7];
            if (dialog_id_temp == dialog_id)
            {
                TUYA_BLE_LOG_DEBUG("Received stop speech cmd dialog_id = %d .", dialog_id_temp);
                tuya_ble_device_enter_critical();
                tuya_vos_audio_reset_flag_and_buffer();
                audio_process_flag = 0;
                tuya_ble_device_exit_critical();
                tuya_ble_send_stop_speech(1, 0);

                event.evt = TUYA_BLE_CB_EVT_AVS_SPEECH_CMD_RES;
                event.avs_cmd_event_data.cmd = AVS_SPEECH_CMD;
                event.avs_cmd_event_data.cmd_data.type = AVS_SPEECH_CMD_STOP;
                event.avs_cmd_event_data.cmd_data.err_code = p_recv_data[3];
                if (tuya_ble_cb_event_send(&event) != 0)
                {
                    TUYA_BLE_LOG_ERROR("tuya ble send stop speech cmd callback event failed.");
                }

            }
            else
            {
                TUYA_BLE_LOG_WARNING("Received stop speech cmd dialog_id = %d ,invaild id .", dialog_id_temp);
            }


        }
        break;

    case TUYA_BLE_VOS_CMD_SPEECH_STATE:
        speech_state_update_app = 1;
        speech_state = p_recv_data[3];
        current_speech_state = speech_state;
        if (speech_state == SpeechState_IDLE)
        {
            TUYA_BLE_LOG_DEBUG("Received Speech State : SpeechState_IDLE");
            if (audio_process_flag)
            {
                audio_process_flag = 0;
            }
        }
        else if (speech_state == SpeechState_LISTENING)
        {
            TUYA_BLE_LOG_DEBUG("Received Speech State : SpeechState_LISTENING");

        }
        else if (speech_state == SpeechState_PROCESSING)
        {
            TUYA_BLE_LOG_DEBUG("Received Speech State : SpeechState_PROCESSING");
        }
        else if (speech_state == SpeechState_SPEAKING)
        {
            TUYA_BLE_LOG_DEBUG("Received Speech State : SpeechState_SPEAKING");
        }
        else
        {
            TUYA_BLE_LOG_DEBUG("Received Speech State : Unknown State");
            tuya_vos_audio_reset_flag_and_buffer();
            audio_process_flag = 0;
            speech_state_update_app = 0;
        }
        if (speech_state_update_app)
        {
            event.evt = TUYA_BLE_CB_EVT_AVS_SPEECH_STATE;
            event.speech_state_data.state = speech_state;
            if (tuya_ble_cb_event_send(&event) != 0)
            {
                TUYA_BLE_LOG_ERROR("tuya ble send speech state callback event failed.");
            }
        }
        break;

    default:
        break;
    }

}


static void tuya_ble_handle_speech_streaming(uint8_t *p_recv_data, uint32_t recv_data_len)
{
    uint16_t sn = (p_recv_data[1] << 8) + p_recv_data[2];

    if (p_recv_data[5] == 0)
    {
        tuya_ble_send_audio_data(sn + 1);
    }
    else
    {
        TUYA_BLE_LOG_ERROR("ERROR,received send audio data status error.");
    }

}


static uint32_t vos_data_unpack(frm_trsmitr_proc_s *p_trsmitr_proc,
                                tuya_ble_common_recv_packet *p_recv_packet, uint8_t *buf, uint32_t len)
{
    static uint32_t offset = 0;
    mtp_ret ret;

    ret = trsmitr_recv_pkg_decode(p_trsmitr_proc, buf, len);
    if (MTP_OK != ret && MTP_TRSMITR_CONTINUE != ret)
    {
        p_recv_packet->recv_len_max = 0;
        p_recv_packet->recv_len = 0;
        if (p_recv_packet->recv_data)
        {
            tuya_ble_free(p_recv_packet->recv_data);
            p_recv_packet->recv_data = NULL;
        }

        return 3;  //error
    }

    if (FRM_PKG_FIRST == p_trsmitr_proc->pkg_desc)
    {
        if (p_recv_packet->recv_data)
        {
            tuya_ble_free(p_recv_packet->recv_data);
            p_recv_packet->recv_data = NULL;
        }
        p_recv_packet->recv_len_max = get_trsmitr_frame_total_len(p_trsmitr_proc);
        if ((p_recv_packet->recv_len_max > AUDIO_RESULT_DATA_MAX_SIZE) ||
            (p_recv_packet->recv_len_max == 0))
        {
            p_recv_packet->recv_len_max = 0;
            p_recv_packet->recv_len = 0;
            TUYA_BLE_LOG_ERROR("vos total size [%d ]error.", p_recv_packet->recv_len_max);
            return 3;  //invalid total length
        }
        p_recv_packet->recv_len = 0;
        p_recv_packet->recv_data = tuya_ble_malloc(p_recv_packet->recv_len_max + (4 -
                                                                                  (p_recv_packet->recv_len_max & 0x0003))); //Ensure that the length is a multiple of 4.
        if (p_recv_packet->recv_data == NULL)
        {
            TUYA_BLE_LOG_ERROR("vos_data_unpack malloc failed.");
            return 3;   //malloc failed.
        }
        memset(p_recv_packet->recv_data, 0, p_recv_packet->recv_len_max);
        offset = 0;
    }

    if ((offset + get_trsmitr_subpkg_len(p_trsmitr_proc)) <= p_recv_packet->recv_len_max)
    {
        if (p_recv_packet->recv_data)
        {
            memcpy(p_recv_packet->recv_data + offset, get_trsmitr_subpkg(p_trsmitr_proc),
                   get_trsmitr_subpkg_len(p_trsmitr_proc));
            offset += get_trsmitr_subpkg_len(p_trsmitr_proc);
            p_recv_packet->recv_len = offset;
        }
        else
        {
            TUYA_BLE_LOG_ERROR("vos_data_unpack error.");
            p_recv_packet->recv_len_max = 0;
            p_recv_packet->recv_len = 0;
            return 2;  //current packet error.
        }
    }
    else
    {
        ret = MTP_INVALID_PARAM;
        TUYA_BLE_LOG_ERROR("vos_data_unpack[] error:MTP_INVALID_PARAM");
        tuya_ble_common_recv_packet_free(p_recv_packet);
        return 3;
    }

    if (ret == MTP_OK)
    {
        offset = 0;
        TUYA_BLE_LOG_DEBUG("vos_data_unpack[%d]", p_recv_packet->recv_len);

        return 0;
    }
    else if (ret == MTP_TRSMITR_CONTINUE)
    {
        return 1;
    }
    else
    {
        return 3;//
    }

}



static void tuya_ble_vos_handle_common_speech_response_data(uint8_t *p_recv_data,
                                                            uint32_t recv_data_len)
{
    uint16_t data_length = recv_data_len;
    tuya_ble_cb_evt_param_t event;
    uint8_t *p_data_buffer = NULL;


    event.evt = TUYA_BLE_CB_EVT_AVS_SPEECH_STATE;
    event.speech_state_data.state = SpeechState_Result_Data;
    p_data_buffer = (uint8_t *)tuya_ble_malloc(data_length);
    if (p_data_buffer == NULL)
    {
        TUYA_BLE_LOG_ERROR("speech result data malloc failed.");
    }
    else
    {
        memcpy(p_data_buffer, p_recv_data, data_length);
        event.speech_state_data.data_length = data_length;
        event.speech_state_data.p_data = p_data_buffer;
        if (tuya_ble_cb_event_send(&event) != 0)
        {
            tuya_ble_free(p_data_buffer);
            TUYA_BLE_LOG_ERROR("tuya ble send speech result data callback event failed.");
        }
    }
}


static void tuya_ble_vos_handle_weather_response_data(uint8_t *p_recv_data, uint32_t recv_data_len)
{
    uint16_t data_length = recv_data_len;
    tuya_ble_cb_evt_param_t event;
    uint8_t *p_data_buffer = NULL;
    tuya_vos_weather_data_type_t *weather_data = NULL;
    uint16_t index_src = 0;
    uint16_t index_dest = 0;
    uint16_t data_buffer_size = 0;
    uint8_t days = 0, i = 0;

    data_buffer_size = sizeof(tuya_vos_weather_data_type_t) + data_length;

    p_data_buffer = (uint8_t *)tuya_ble_malloc(data_buffer_size);

    if (p_data_buffer == NULL)
    {
        TUYA_BLE_LOG_ERROR("[Vos]weather data malloc failed.");
    }
    else
    {
        TUYA_BLE_LOG_DEBUG("[Vos]Weather data malloc buffer size = %d", data_buffer_size);
        index_src = 0;
        index_dest = sizeof(tuya_vos_weather_data_type_t);
        weather_data = (tuya_vos_weather_data_type_t *)p_data_buffer;

        weather_data->main_title_length = p_recv_data[index_src];
        index_src++;
        if (((index_dest + weather_data->main_title_length) <= data_buffer_size) &&
            ((index_src + weather_data->main_title_length) <= data_length))
        {
            memcpy(p_data_buffer + index_dest, p_recv_data + index_src, weather_data->main_title_length);
            weather_data->p_main_title = p_data_buffer + index_dest;
            index_dest += weather_data->main_title_length;
            index_src += weather_data->main_title_length;
        }
        else
        {
            TUYA_BLE_LOG_ERROR("[Vos] Received weather main_title_length error,main_title_length = %d",
                               weather_data->main_title_length);
            tuya_ble_free(p_data_buffer);
            return;
        }

        weather_data->sub_title_length = p_recv_data[index_src];
        index_src++;

        if (((index_dest + weather_data->sub_title_length) <= data_buffer_size) &&
            ((index_src + weather_data->sub_title_length) <= data_length))
        {
            memcpy(p_data_buffer + index_dest, p_recv_data + index_src, weather_data->sub_title_length);
            weather_data->p_sub_title = p_data_buffer + index_dest;
            index_dest += weather_data->sub_title_length;
            index_src += weather_data->sub_title_length;
        }
        else
        {
            TUYA_BLE_LOG_ERROR("[Vos] Received weather sub_title_length error,sub_title_length = %d",
                               weather_data->sub_title_length);
            tuya_ble_free(p_data_buffer);
            return;
        }

        weather_data->temperature_unit = p_recv_data[index_src];
        index_src++;
        weather_data->weather_days = p_recv_data[index_src];
        index_src++;

        if ((weather_data->weather_days < 1) || (weather_data->weather_days > 8))
        {
            TUYA_BLE_LOG_ERROR("[Vos] Received weather days error,weather days = %d",
                               weather_data->weather_days);
            tuya_ble_free(p_data_buffer);
            return;
        }

        if ((weather_data->temperature_unit != TUYA_TEMPERATURE_UNIT_CELSIUS) &&
            (weather_data->temperature_unit != TUYA_TEMPERATURE_UNIT_FAHRENHEIT))
        {
            TUYA_BLE_LOG_ERROR("[Vos] Received weather temperature unit error,unit = %d",
                               weather_data->temperature_unit);
            tuya_ble_free(p_data_buffer);
            return;
        }

        weather_data->query_day_weather_data.length = (p_recv_data[index_src] << 8) + p_recv_data[index_src
                                                      + 1];
        index_src += 2;
        weather_data->query_day_weather_data.flag = p_recv_data[index_src];
        index_src++;

        weather_data->query_day_weather_data.weather_type = (p_recv_data[index_src] << 8) +
                                                            p_recv_data[index_src + 1];
        index_src += 2;
        weather_data->query_day_weather_data.temperature_current = (p_recv_data[index_src] << 8) +
                                                                   p_recv_data[index_src + 1];
        index_src += 2;
        weather_data->query_day_weather_data.temperature_min = (p_recv_data[index_src] << 8) +
                                                               p_recv_data[index_src + 1];
        index_src += 2;
        weather_data->query_day_weather_data.temperature_max = (p_recv_data[index_src] << 8) +
                                                               p_recv_data[index_src + 1];
        index_src += 2;

        weather_data->query_day_weather_data.tts_length = p_recv_data[index_src];
        index_src++;
        if (((index_dest + weather_data->query_day_weather_data.tts_length) <= data_buffer_size) &&
            ((index_src + weather_data->query_day_weather_data.tts_length) <= data_length))
        {
            memcpy(p_data_buffer + index_dest, p_recv_data + index_src,
                   weather_data->query_day_weather_data.tts_length);
            weather_data->query_day_weather_data.p_tts = p_data_buffer + index_dest;
            index_dest += weather_data->query_day_weather_data.tts_length;
            index_src += weather_data->query_day_weather_data.tts_length;
        }
        else
        {
            TUYA_BLE_LOG_ERROR("[Vos] Received weather data tts string length error,tts_length = %d",
                               weather_data->query_day_weather_data.tts_length);
            tuya_ble_free(p_data_buffer);
            return;
        }

        weather_data->query_day_weather_data.weather_description_length = p_recv_data[index_src];
        index_src++;
        if (((index_dest + weather_data->query_day_weather_data.weather_description_length) <=
             data_buffer_size) &&
            ((index_src + weather_data->query_day_weather_data.weather_description_length) <= data_length))
        {
            memcpy(p_data_buffer + index_dest, p_recv_data + index_src,
                   weather_data->query_day_weather_data.weather_description_length);
            weather_data->query_day_weather_data.p_weather_description = p_data_buffer + index_dest;
            index_dest += weather_data->query_day_weather_data.weather_description_length;
            index_src += weather_data->query_day_weather_data.weather_description_length;
        }
        else
        {
            TUYA_BLE_LOG_ERROR("[Vos] Received query day weather data weather_description_length error,weather_description_length = %d",
                               weather_data->query_day_weather_data.weather_description_length);
            tuya_ble_free(p_data_buffer);
            return;
        }


        days = weather_data->weather_days - 1;

        for (i = 0; i < days; i++)
        {
            weather_data->daily_weather_data[i].length = p_recv_data[index_src];
            index_src += 1;

            weather_data->daily_weather_data[i].weather_type = (p_recv_data[index_src] << 8) +
                                                               p_recv_data[index_src + 1];
            index_src += 2;
            weather_data->daily_weather_data[i].temperature_min = (p_recv_data[index_src] << 8) +
                                                                  p_recv_data[index_src + 1];
            index_src += 2;
            weather_data->daily_weather_data[i].temperature_max = (p_recv_data[index_src] << 8) +
                                                                  p_recv_data[index_src + 1];
            index_src += 2;

            weather_data->daily_weather_data[i].date_length = p_recv_data[index_src];
            index_src += 1;

            if (((index_dest + weather_data->daily_weather_data[i].date_length) <= data_buffer_size) &&
                ((index_src + weather_data->daily_weather_data[i].date_length) <= data_length))
            {
                memcpy(p_data_buffer + index_dest, p_recv_data + index_src,
                       weather_data->daily_weather_data[i].date_length);
                weather_data->daily_weather_data[i].p_date = p_data_buffer + index_dest;
                index_dest += weather_data->daily_weather_data[i].date_length;
                index_src += weather_data->daily_weather_data[i].date_length;
            }
            else
            {
                TUYA_BLE_LOG_ERROR("[Vos] Received weather daily data length error,date_length = %d",
                                   weather_data->daily_weather_data[i].date_length);
                tuya_ble_free(p_data_buffer);
                return;
            }

            weather_data->daily_weather_data[i].day_length = p_recv_data[index_src];
            index_src += 1;

            if (((index_dest + weather_data->daily_weather_data[i].day_length) <= data_buffer_size) &&
                ((index_src + weather_data->daily_weather_data[i].day_length) <= data_length))
            {
                memcpy(p_data_buffer + index_dest, p_recv_data + index_src,
                       weather_data->daily_weather_data[i].day_length);
                weather_data->daily_weather_data[i].p_day = p_data_buffer + index_dest;
                index_dest += weather_data->daily_weather_data[i].day_length;
                index_src += weather_data->daily_weather_data[i].day_length;
            }
            else
            {
                TUYA_BLE_LOG_ERROR("[Vos] Received weather daily data length error,day_length = %d",
                                   weather_data->daily_weather_data[i].day_length);
                tuya_ble_free(p_data_buffer);
                return;
            }

            weather_data->daily_weather_data[i].weather_description_length = p_recv_data[index_src];
            index_src++;
            if (((index_dest + weather_data->daily_weather_data[i].weather_description_length) <=
                 data_buffer_size) &&
                ((index_src + weather_data->daily_weather_data[i].weather_description_length) <= data_length))
            {
                memcpy(p_data_buffer + index_dest, p_recv_data + index_src,
                       weather_data->daily_weather_data[i].weather_description_length);
                weather_data->daily_weather_data[i].p_weather_description = p_data_buffer + index_dest;
                index_dest += weather_data->daily_weather_data[i].weather_description_length;
                index_src += weather_data->daily_weather_data[i].weather_description_length;
            }
            else
            {
                TUYA_BLE_LOG_ERROR("[Vos] Received weather daily data weather_description_length error,weather_description_length = %d",
                                   weather_data->daily_weather_data[i].weather_description_length);
                tuya_ble_free(p_data_buffer);
                return;
            }

        }

        event.evt = TUYA_BLE_CB_EVT_AVS_SPEECH_WEATHER_DATA;
        event.avs_weather_data.p_weather_data = weather_data;
        if (tuya_ble_cb_event_send(&event) != 0)
        {
            tuya_ble_free(p_data_buffer);
            TUYA_BLE_LOG_ERROR("[Vos]tuya ble send avs weather data callback event failed.");
        }

    }


}


static void tuya_ble_vos_handle_list_response_data(uint8_t *p_recv_data, uint32_t recv_data_len)
{
    uint16_t data_length = recv_data_len;
    tuya_ble_cb_evt_param_t event;
    uint8_t *p_data_buffer = NULL;
    uint16_t data_buffer_size = 0;
    tuya_vos_list_data_type_t *list_data = NULL;
    uint16_t index_src = 0;
    uint16_t index_dest = 0;
    uint8_t i = 0;


    data_buffer_size = sizeof(tuya_vos_list_data_type_t) + data_length;

    p_data_buffer = (uint8_t *)tuya_ble_malloc(data_buffer_size);

    if (p_data_buffer == NULL)
    {
        TUYA_BLE_LOG_ERROR("[Vos]List data malloc failed.");
    }
    else
    {
        index_src = 0;
        index_dest = sizeof(tuya_vos_list_data_type_t);
        list_data = (tuya_vos_list_data_type_t *)p_data_buffer;

        list_data->main_title_length = p_recv_data[index_src];
        index_src++;

        if (((index_dest + list_data->main_title_length) <= data_buffer_size) &&
            ((index_src + list_data->main_title_length) <= data_length))
        {
            memcpy(p_data_buffer + index_dest, p_recv_data + index_src, list_data->main_title_length);
            list_data->p_main_title = p_data_buffer + index_dest;
            index_dest += list_data->main_title_length;
            index_src += list_data->main_title_length;
        }
        else
        {
            TUYA_BLE_LOG_ERROR("[Vos] Received List data main_title_length error,main_title_length = %d",
                               list_data->main_title_length);
            tuya_ble_free(p_data_buffer);
            return;
        }

        list_data->sub_title_length = p_recv_data[index_src];
        index_src++;
        if (((index_dest + list_data->sub_title_length) <= data_buffer_size) &&
            ((index_src + list_data->sub_title_length) <= data_length))
        {
            memcpy(p_data_buffer + index_dest, p_recv_data + index_src, list_data->sub_title_length);
            list_data->p_sub_title = p_data_buffer + index_dest;
            index_dest += list_data->sub_title_length;
            index_src += list_data->sub_title_length;
        }
        else
        {
            TUYA_BLE_LOG_ERROR("[Vos] Received List data sub_title_length error,sub_title_length = %d",
                               list_data->sub_title_length);
            tuya_ble_free(p_data_buffer);
            return;
        }

        list_data->list_number = p_recv_data[index_src];
        index_src++;

        for (i = 0; i < list_data->list_number; i++)
        {
            list_data->list_content_data[i].left_text_length = p_recv_data[index_src];
            index_src += 1;
            if (((index_dest + list_data->list_content_data[i].left_text_length) <= data_buffer_size) &&
                ((index_src + list_data->list_content_data[i].left_text_length) <= data_length))
            {
                memcpy(p_data_buffer + index_dest, p_recv_data + index_src,
                       list_data->list_content_data[i].left_text_length);
                list_data->list_content_data[i].p_left_text = p_data_buffer + index_dest;
                index_dest += list_data->list_content_data[i].left_text_length;
                index_src += list_data->list_content_data[i].left_text_length;
            }
            else
            {
                TUYA_BLE_LOG_ERROR("[Vos] Received List data list_content_data left text length error,list_content_data[%d].left_text_length = %d",
                                   i, list_data->list_content_data[i].left_text_length);
                tuya_ble_free(p_data_buffer);
                return;
            }

            list_data->list_content_data[i].right_text_length = p_recv_data[index_src];
            index_src += 1;
            if (((index_dest + list_data->list_content_data[i].right_text_length) <= data_buffer_size) &&
                ((index_src + list_data->list_content_data[i].right_text_length) <= data_length))
            {
                memcpy(p_data_buffer + index_dest, p_recv_data + index_src,
                       list_data->list_content_data[i].right_text_length);
                list_data->list_content_data[i].p_right_text = p_data_buffer + index_dest;
                index_dest += list_data->list_content_data[i].right_text_length;
                index_src += list_data->list_content_data[i].right_text_length;
            }
            else
            {
                TUYA_BLE_LOG_ERROR("[Vos] Received List data list_content_data right text length error,list_content_data[%d].right_text_length = %d",
                                   i, list_data->list_content_data[i].right_text_length);
                tuya_ble_free(p_data_buffer);
                return;
            }
        }

        event.evt = TUYA_BLE_CB_EVT_AVS_SPEECH_LIST_DATA;
        event.avs_list_data.p_list_data = list_data;
        if (tuya_ble_cb_event_send(&event) != 0)
        {
            tuya_ble_free(p_data_buffer);
            TUYA_BLE_LOG_ERROR("[Vos]tuya ble send avs list data callback event failed.");
        }

    }


}


static void tuya_ble_handle_speech_response(uint8_t *p_recv_data, uint32_t recv_data_len)
{
    uint8_t buffer[13];
    uint32_t err_code = 0;
    uint8_t type = 0;
    uint16_t data_length;

    err_code = vos_data_unpack(&audio_trsmitr_proc, &audio_recv_packet, p_recv_data, recv_data_len);

    buffer[0] = err_code;
    buffer[1] = audio_trsmitr_proc.subpkg_num >> 8;
    buffer[2] = audio_trsmitr_proc.subpkg_num;
    buffer[3] = audio_trsmitr_proc.subpkg_len >> 8;
    buffer[4] = audio_trsmitr_proc.subpkg_len;
    buffer[5] = audio_recv_packet.recv_len >> 24;
    buffer[6] = audio_recv_packet.recv_len >> 16;
    buffer[7] = audio_recv_packet.recv_len >> 8;
    buffer[8] = audio_recv_packet.recv_len;
    buffer[9] = audio_recv_packet.recv_len_max >> 24;
    buffer[10] = audio_recv_packet.recv_len_max >> 16;
    buffer[11] = audio_recv_packet.recv_len_max >> 8;
    buffer[12] = audio_recv_packet.recv_len_max;

    tuya_ble_commData_send(TUYA_BLE_FRM_VOS_RESPONSE, 0, buffer, 13, ENCRYPTION_MODE_SESSION_KEY);


    if (err_code == 0)
    {
        type = audio_recv_packet.recv_data[1];

        data_length = (audio_recv_packet.recv_data[2] << 8) + audio_recv_packet.recv_data[3];

        TUYA_BLE_LOG_DEBUG("[Vos] Received vos response data type = %d, data length = %d ", type,
                           data_length);
        TUYA_BLE_LOG_HEXDUMP_DEBUG("[Vos] Response data :", &audio_recv_packet.recv_data[4], data_length);

        switch (type)
        {
        case 0:
            tuya_ble_vos_handle_common_speech_response_data(audio_recv_packet.recv_data + 4, data_length);
            break;
        case 1:
            tuya_ble_vos_handle_weather_response_data(audio_recv_packet.recv_data + 4, data_length);
            break;
        case 2:
            tuya_ble_vos_handle_list_response_data(audio_recv_packet.recv_data + 4, data_length);
            break;
        default:
            break;

        }

        if (audio_recv_packet.recv_data)
        {
            tuya_ble_free(audio_recv_packet.recv_data);
            audio_recv_packet.recv_data = NULL;
            audio_recv_packet.recv_len_max = 0;
            audio_recv_packet.recv_len = 0;
        }

    }
    else
    {
        return;
    }

}

static void tuya_ble_vos_token_send(void)
{
    mtp_ret ret;
    uint16_t send_len;
    uint16_t current_send_token_packet_number = 0;
    uint8_t *p_buf;

    if (token_send_buffer_length == 0)
    {
        TUYA_BLE_LOG_ERROR("[Vos] token data send error,because token_send_buffer_length is 0.");
        return;
    }

    ret = trsmitr_send_pkg_encode_with_packet_length(&token_trsmitr_proc_send,
                                                     AUDIO_RESULT_DATA_MAX_PACKET_SIZE, TUYA_BLE_PROTOCOL_VERSION_HIGN,
                                                     p_token_send_buffer + 16, token_send_buffer_length);

    if (MTP_OK != ret && MTP_TRSMITR_CONTINUE != ret)
    {
        if (p_token_send_buffer)
        {
            tuya_ble_free(p_token_send_buffer);
        }
        token_send_buffer_length = 0;
        return ;
    }
    send_len = get_trsmitr_subpkg_len(&token_trsmitr_proc_send);
    p_buf = get_trsmitr_subpkg(&token_trsmitr_proc_send);

    if (ret == MTP_TRSMITR_CONTINUE)
    {
        current_send_token_packet_number = token_trsmitr_proc_send.subpkg_num - 1;
    }
    else
    {
        current_send_token_packet_number = token_trsmitr_proc_send.subpkg_num;
    }
    TUYA_BLE_LOG_DEBUG("current send token packet number = %d, packet length = %d, total length = %d",
                       current_send_token_packet_number, token_trsmitr_proc_send.subpkg_len,
                       token_trsmitr_proc_send.total);
    tuya_ble_commData_send(TUYA_BLE_FRM_VOS_TOKEN_SEND, 0, p_buf, send_len,
                           ENCRYPTION_MODE_SESSION_KEY);

    if (MTP_OK == ret) //Last Packet
    {
        if (p_token_send_buffer)
        {
            tuya_ble_free(p_token_send_buffer);
        }
        token_send_buffer_length = 0;
        TUYA_BLE_LOG_DEBUG("[Vos] token data send succeed.");
    }
}


static void tuya_ble_handle_vos_token_send_response(uint8_t *p_recv_data, uint32_t recv_data_len)
{
    uint8_t status = p_recv_data[0];
    if (status == 0) //send complete
    {
        TUYA_BLE_LOG_DEBUG("token data send succeed. total length = %d .", token_trsmitr_proc_send.total);
        if (p_token_send_buffer)
        {
            tuya_ble_free(p_token_send_buffer);
        }
        token_send_buffer_length = 0;
    }
    else if (status == 1) //send next packet
    {
        tuya_ble_vos_token_send();
    }
    else   //error
    {
        if (p_token_send_buffer)
        {
            tuya_ble_free(p_token_send_buffer);
        }
        token_send_buffer_length = 0;
        tuya_ble_gap_disconnect();
    }


}


static void tuya_ble_handle_vos_token_reading(uint8_t *p_recv_data, uint32_t recv_data_len)
{
    uint8_t buffer[18];

    if ((p_recv_data[0] != 0) || (recv_data_len != 2))
    {
        return;
    }

    buffer[0] = 0;
    buffer[1] = (current_token.is_token_valid ? 0 : 1);
    memcpy(buffer + 2, current_token.token_md5, 16);

    tuya_ble_commData_send(TUYA_BLE_FRM_VOS_TOKEN_READING_RESP, 0, buffer, 18,
                           ENCRYPTION_MODE_SESSION_KEY);

    if ((p_recv_data[1] == 1) && (current_token.is_token_valid)) // token data request
    {
        trsmitr_init(&token_trsmitr_proc_send);
        if (p_token_send_buffer)
        {
            tuya_ble_free(p_token_send_buffer);
        }

        p_token_send_buffer = tuya_ble_malloc(current_token.token_length + 3 + 20);

        if (p_token_send_buffer)
        {
            tuya_ble_nv_read(TUYA_NV_VOS_TOKEN_START_ADDR, p_token_send_buffer,
                             current_token.token_length + 20);
            p_token_send_buffer[16] = 0; //version
            p_token_send_buffer[17] = current_token.token_length >> 8;
            p_token_send_buffer[18] = current_token.token_length;
            token_send_buffer_length = current_token.token_length + 3;
            tuya_ble_vos_token_send();
        }
        else //if malloc failed, not send token data.
        {

        }


    }

}


static void tuya_ble_handle_vos_token_write(uint8_t *p_recv_data, uint32_t recv_data_len)
{
    uint8_t buffer[13];
    uint32_t err_code = 0;
    uint16_t token_length_temp = 0;
    uint8_t  version;
    uint8_t token_md5_temp[16];

    err_code = vos_data_unpack(&token_trsmitr_proc, &token_recv_packet, p_recv_data, recv_data_len);

    buffer[0] = err_code;
    buffer[1] = token_trsmitr_proc.subpkg_num >> 8;
    buffer[2] = token_trsmitr_proc.subpkg_num;
    buffer[3] = token_trsmitr_proc.subpkg_len >> 8;
    buffer[4] = token_trsmitr_proc.subpkg_len;
    buffer[5] = token_recv_packet.recv_len >> 24;
    buffer[6] = token_recv_packet.recv_len >> 16;
    buffer[7] = token_recv_packet.recv_len >> 8;
    buffer[8] = token_recv_packet.recv_len;
    buffer[9] = token_recv_packet.recv_len_max >> 24;
    buffer[10] = token_recv_packet.recv_len_max >> 16;
    buffer[11] = token_recv_packet.recv_len_max >> 8;
    buffer[12] = token_recv_packet.recv_len_max;

    if (err_code)
    {
        tuya_ble_commData_send(TUYA_BLE_FRM_VOS_TOKEN_WRITE_RESP, 0, buffer, 13,
                               ENCRYPTION_MODE_SESSION_KEY);
    }
    else
    {
        version = token_recv_packet.recv_data[0];
        if (version == 0)
        {
            current_token.token_length = (token_recv_packet.recv_data[17] << 8) +
                                         token_recv_packet.recv_data[18];
            memcpy(current_token.token_md5, token_recv_packet.recv_data + 1, 16);
            if (current_token.token_length > (token_recv_packet.recv_len_max - 19))
            {
                TUYA_BLE_LOG_ERROR("token data write length error!");
                current_token.is_token_valid = 0;
                goto token_write_exit;
            }

            memset(token_md5_temp, 0, sizeof(token_md5_temp));
            tuya_ble_md5_crypt(token_recv_packet.recv_data + 19, current_token.token_length, token_md5_temp);
            if (memcmp(token_md5_temp, current_token.token_md5, 16))
            {
                TUYA_BLE_LOG_ERROR("token data write md5 check error!");
                current_token.is_token_valid = 0;
                goto token_write_exit;
            }

            if (tuya_ble_nv_erase(TUYA_NV_VOS_TOKEN_START_ADDR,
                                  TUYA_NV_VOS_TOKEN_AREA_SIZE) == TUYA_BLE_SUCCESS)
            {
                if (tuya_ble_nv_write(TUYA_NV_VOS_TOKEN_START_ADDR, token_recv_packet.recv_data,
                                      token_recv_packet.recv_len_max + (4 - (token_recv_packet.recv_len_max & 0x0003))) ==
                    TUYA_BLE_SUCCESS)
                {
                    current_token.is_token_valid = 1;
                    TUYA_BLE_LOG_DEBUG("[VOS]token data received ,total length = %d", current_token.token_length);
                    TUYA_BLE_LOG_HEXDUMP_DEBUG("[Vos]token data md5 :", current_token.token_md5, 16);

                }
                else
                {
                    TUYA_BLE_LOG_ERROR("token data flash write error!");
                    current_token.is_token_valid = 0;
                    goto token_write_exit;
                }
            }
            else
            {
                TUYA_BLE_LOG_ERROR("token data flash erase error!");
                current_token.is_token_valid = 0;
                goto token_write_exit;
            }

token_write_exit:
            if (current_token.is_token_valid == 1)
            {
                buffer[0] = 0;
            }
            else
            {
                buffer[0] = 3;
                current_token.token_length = 0;
                memset(current_token.token_md5, 0, 16);
                TUYA_BLE_LOG_ERROR("token data received but write flash error!");
            }

            tuya_ble_commData_send(TUYA_BLE_FRM_VOS_TOKEN_WRITE_RESP, 0, buffer, 13,
                                   ENCRYPTION_MODE_SESSION_KEY);
        }

        tuya_ble_common_recv_packet_free(&token_recv_packet);

    }

}


static void tuya_ble_handle_vos_common_set(uint8_t *p_recv_data, uint32_t recv_data_len)
{
    uint8_t buffer[10];
    uint8_t sub_cmd ;
    tuya_ble_cb_evt_param_t event;

    if (p_recv_data[0] != 0)
    {
        return;
    }

    sub_cmd = p_recv_data[1];

    switch (sub_cmd)
    {
    case 0:
        current_token.is_token_valid = 0;
        memset(current_token.token_md5, 0, 16);
        current_token.token_length = 0;
        TUYA_BLE_LOG_DEBUG("[Vos] Received clear token cmd.");
        break;
    case 1:
        if ((p_recv_data[2] == 3) && ((p_recv_data[3] == 1) || (p_recv_data[3] == 2)))
        {
            event.evt = TUYA_BLE_CB_EVT_AVS_NOTIFICATIONS_INDICATOR;
            event.avs_notifications_data.cmd = (p_recv_data[3] == 1) ? TUYA_AVS_NOTIFICATIONS_INDICATOR_SET :
                                               TUYA_AVS_NOTIFICATIONS_INDICATOR_CLEAR;
            if (event.avs_notifications_data.cmd == TUYA_AVS_NOTIFICATIONS_INDICATOR_SET)
            {
                event.avs_notifications_data.persist_visual_indicator = p_recv_data[4];
                event.avs_notifications_data.play_audio_indicator = p_recv_data[5];
            }
            if (tuya_ble_cb_event_send(&event) != 0)
            {
                TUYA_BLE_LOG_ERROR("[Vos]tuya ble send avs notifications data callback event failed.");
            }
        }
        else
        {
            TUYA_BLE_LOG_ERROR("[Vos]Received avs notifications cmd data error. data length = %d , notify cmd = %d , persist_visual_indicator = %d , play_audio_indicator = %d ",
                               p_recv_data[2], p_recv_data[3], p_recv_data[4], p_recv_data[5]);
        }
        break;
    default:
        break;

    }
    buffer[0] = 0;
    buffer[1] = sub_cmd;
    buffer[2] = 0;
    buffer[3] = 0;

    tuya_ble_commData_send(TUYA_BLE_FRM_VOS_COMMON_SET_RESP, 0, buffer, 4, ENCRYPTION_MODE_SESSION_KEY);

}


static void tuya_ble_handle_vos_timer_setting(uint8_t *p_recv_data, uint32_t recv_data_len)
{
    tuya_vos_timer_set_data_t *p_timer_set_data;
    tuya_vos_timer_cancel_data_t *p_timer_cancel_data;
    uint8_t *p_data_buffer = NULL;
    tuya_ble_cb_evt_param_t event;

    if ((p_recv_data[0] != 0) || (recv_data_len < 19))
    {
        return;
    }

    if ((p_recv_data[1] != 0) && (p_recv_data[1] != 1))
    {
        return;
    }

    if ((p_recv_data[2] != TUYA_VOS_TIMER_TIMER) && (p_recv_data[2] != TUYA_VOS_TIMER_ALARM) &&
        (p_recv_data[2] != TUYA_VOS_TIMER_REMINDER) && (p_recv_data[2] != 0xFF))
    {
        return;
    }

    if (p_recv_data[1] == 0)
    {
        p_data_buffer = (uint8_t *)tuya_ble_malloc(sizeof(tuya_vos_timer_set_data_t) + recv_data_len);
    }
    else
    {
        p_data_buffer = (uint8_t *)tuya_ble_malloc(sizeof(tuya_vos_timer_cancel_data_t) + recv_data_len);
    }

    if (p_data_buffer == NULL)
    {
        TUYA_BLE_LOG_ERROR("[Vos]Timer data malloc failed.");
    }
    else
    {
        if (p_recv_data[1] == 0) //set cmd
        {
            p_timer_set_data = (tuya_vos_timer_set_data_t *)p_data_buffer;

            p_timer_set_data->type = p_recv_data[2];
            memcpy(p_timer_set_data->token_md5, p_recv_data + 3, 16);

            p_timer_set_data->time = (p_recv_data[19] << 24) + (p_recv_data[20] << 16) +
                                     (p_recv_data[21] << 8) + p_recv_data[22];
            p_timer_set_data->loop_count = p_recv_data[23];
            p_timer_set_data->loop_count_pause_time = (p_recv_data[24] << 24) + (p_recv_data[25] << 16) +
                                                      (p_recv_data[26] << 8) + p_recv_data[27];
            p_timer_set_data->reminder_text_length = (p_recv_data[28] << 8) + p_recv_data[29];
            if (p_timer_set_data->reminder_text_length > 0)
            {
                p_timer_set_data->p_reminder_text = &p_data_buffer[sizeof(tuya_vos_timer_set_data_t)];
                memcpy(p_timer_set_data->p_reminder_text, p_recv_data + 30, p_timer_set_data->reminder_text_length);
            }

            event.evt = TUYA_BLE_CB_EVT_AVS_SPEECH_TIMER_SET_DATA;
            event.avs_timer_set_data.p_timer_set_data = p_timer_set_data;
        }
        else
        {
            p_timer_cancel_data = (tuya_vos_timer_cancel_data_t *)p_data_buffer;

            p_timer_cancel_data->type = p_recv_data[2];
            memcpy(p_timer_cancel_data->token_md5, p_recv_data + 3, 16);

            event.evt = TUYA_BLE_CB_EVT_AVS_SPEECH_TIMER_CANCEL_DATA;
            event.avs_timer_cancel_data.p_timer_cancel_data = p_timer_cancel_data;
        }

        if (tuya_ble_cb_event_send(&event) != 0)
        {
            tuya_ble_free(p_data_buffer);
            TUYA_BLE_LOG_ERROR("[Vos]tuya ble send avs timer data callback event failed.");
        }

    }

}


static void tuya_ble_vos_timer_response_handler(int32_t evt_id, void *p_data)
{
    uint8_t buffer[20];
    tuya_avs_timer_response_data_t *p_timer_response = (tuya_avs_timer_response_data_t *)p_data;

    buffer[0] = 0; //version
    buffer[1] = p_timer_response->cmd;
    buffer[2] = p_timer_response->type;
    memcpy(buffer + 3, p_timer_response->token_md5, 16);
    buffer[19] = p_timer_response->status;

    tuya_ble_free((uint8_t *)p_timer_response);

    tuya_ble_commData_send(TUYA_BLE_FRM_VOS_TIMER_SETTING_RESP, 0, buffer, 20,
                           ENCRYPTION_MODE_SESSION_KEY);

}


tuya_ble_status_t tuya_ble_vos_timer_response(tuya_vos_timer_cmd_t cmd, tuya_vos_timer_type_t type,
                                              uint8_t *p_token_md5, uint8_t status)
{
    tuya_ble_custom_evt_t event;
    tuya_avs_timer_response_data_t *p_timer_response = NULL;

    if (tuya_ble_connect_status_get() != BONDING_CONN)
    {
        TUYA_BLE_LOG_ERROR("Invalid State!");
        return TUYA_BLE_ERR_INVALID_STATE;
    }
    if (current_token.is_token_valid == 0)
    {
        TUYA_BLE_LOG_ERROR("Invalid Token!");
        return TUYA_BLE_ERR_INVALID_TOKEN;
    }

    p_timer_response = (tuya_avs_timer_response_data_t *)tuya_ble_malloc(sizeof(
                                                                             tuya_avs_timer_response_data_t));
    if (p_timer_response == NULL)
    {
        TUYA_BLE_LOG_ERROR("[vos] timer response data buffer malloc failed.");
        return TUYA_BLE_ERR_NO_MEM;
    }
    else
    {
        p_timer_response->cmd = cmd;
        p_timer_response->type = type;
        memcpy(p_timer_response->token_md5, p_token_md5, 16);
        p_timer_response->status = status;
    }

    event.evt_id = 0;  //res
    event.data = p_timer_response;
    event.custom_event_handler = (void *)tuya_ble_vos_timer_response_handler;
    if (tuya_ble_custom_event_send(event))
    {
        tuya_ble_free((uint8_t *)p_timer_response);
        TUYA_BLE_LOG_ERROR("[vos]vos timer response event send failed!");
        return TUYA_BLE_ERR_NO_EVENT;
    }
    return TUYA_BLE_SUCCESS;
}


void tuya_ble_handle_speech_req(uint16_t cmd, uint8_t *p_recv_data, uint32_t recv_data_len)
{
    uint16_t data_len;
    tuya_ble_cb_evt_param_t event;
    uint8_t err_code = 0;

    data_len = (p_recv_data[11] << 8) + p_recv_data[12];

    if (data_len == 0)
    {
        return;
    }
    else
    {

    }

    switch (cmd)
    {
    case TUYA_BLE_FRM_VOS_CMD:
        tuya_ble_handle_speech_cmd((p_recv_data + 13), data_len);
        break;

    case TUYA_BLE_FRM_VOS_STREAMING:
        tuya_ble_handle_speech_streaming((p_recv_data + 13), data_len);
        break;

    case TUYA_BLE_FRM_VOS_RESPONSE:
        tuya_ble_handle_speech_response((p_recv_data + 13), data_len);
        break;
    case TUYA_BLE_FRM_VOS_TIMER_SETTING:
        tuya_ble_handle_vos_timer_setting((p_recv_data + 13), data_len);
        break;
    case TUYA_BLE_FRM_VOS_TOKEN_READING:
        tuya_ble_handle_vos_token_reading((p_recv_data + 13), data_len);
        break;
    case TUYA_BLE_FRM_VOS_TOKEN_SEND_RESP:
        tuya_ble_handle_vos_token_send_response((p_recv_data + 13), data_len);
        break;
    case TUYA_BLE_FRM_VOS_TOKEN_WRITE:
        tuya_ble_handle_vos_token_write((p_recv_data + 13), data_len);
        break;
    case TUYA_BLE_FRM_VOS_COMMON_SET:
        tuya_ble_handle_vos_common_set((p_recv_data + 13), data_len);
        break;
    default:

        break;
    }

}

#endif

