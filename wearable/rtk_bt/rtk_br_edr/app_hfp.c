/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <string.h>
#include "trace.h"
#include "btm.h"
#include "app_link_util.h"
#include "bt_hfp.h"
#include "app_sdp.h"
#include "app_hfp.h"
#include "dp_app.h"
#include "dp_br_info.h"
#include "audio.h"
#include "os_mem.h"
#include "dp_br_info.h"
#include "app_cfg.h"
#include "dp_rtk_db.h"
#ifndef __RTTHREAD__
#include "wristband_sdk_config.h"
#endif


static uint8_t active_hf_index = 0;

void app_hfp_volume_up(void)
{

}

void app_hfp_volume_down(void)
{

}

void app_hfp_update_call_status(void)
{

}


static void app_hfp_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    bool handle = true;
    T_APP_BR_LINK *p_link;

    APP_PRINT_INFO1("app_hfp_bt_cback: 0x%04x", event_type);

    switch (event_type)
    {
    case BT_EVENT_HFP_CONN_IND:
        {
            p_link = app_find_br_link(param->hfp_conn_ind.bd_addr);
            if (p_link == NULL)
            {
                APP_PRINT_ERROR0("app_hfp_bt_cback: no acl link found");
                return;
            }

            bt_hfp_connect_cfm(p_link->bd_addr, true);
        }
        break;

    case BT_EVENT_HFP_CONN_CMPL:
        {
            T_APP_BR_LINK *p_link;

            p_link = app_find_br_link(param->hfp_conn_cmpl.bd_addr);
            if (p_link != NULL)
            {
                uint8_t link_number;
                uint8_t pair_idx = 0;

                p_link->call_id_type_check = true;
                p_link->call_id_type_num = false;

                bt_bond_index_get(p_link->bd_addr, &pair_idx);

                uint8_t voice_gain_level[8];
                rtk_kvdb_get(KVDB_MCU_CFG_RW_NAME, KEY_MCU_CFG_RW_VOICE_GAIN_LEVEL, voice_gain_level,
                             sizeof(voice_gain_level));

                bt_hfp_speaker_gain_level_report(p_link->bd_addr, voice_gain_level[pair_idx]);
                bt_hfp_microphone_gain_level_report(p_link->bd_addr, 0x0a);

                bt_hfp_batt_level_report(p_link->bd_addr, app_db.local_batt_level);

                link_number = app_connected_profile_link_num(HFP_PROFILE_MASK | HSP_PROFILE_MASK);
                if (link_number == 1)
                {
                    app_hfp_set_active_hf_index(p_link->bd_addr);
                    app_bond_set_priority(p_link->bd_addr);
                }
                if (br_db.br_link[br_db.hf_info.first_hf_index].app_hf_state == APP_HF_STATE_STANDBY)
                {
                    br_db.hf_info.first_hf_index = p_link->id;
                }
                else
                {
                    br_db.hf_info.last_hf_index = p_link->id;
                }
                p_link->app_hf_state = APP_HF_STATE_CONNECTED;
            }
        }
        break;

    case BT_EVENT_HFP_CALL_STATUS:
        {

        }
        break;

    case BT_EVENT_HFP_SERVICE_STATUS:
        {

        }
        break;

    case BT_EVENT_HFP_CALL_WAITING_IND:
    case BT_EVENT_HFP_CALLER_ID_IND:
        {

        }
        break;

    case BT_EVENT_HFP_RING_ALERT:
        {

        }
        break;

    case BT_EVENT_HFP_SPK_VOLUME_CHANGED:
        {

        }
        break;

    case BT_EVENT_HFP_MIC_VOLUME_CHANGED:
        {

        }
        break;

    case BT_EVENT_HFP_DISCONN_CMPL:
        {


        }
        break;
    case BT_EVENT_SCO_CONN_IND:
        {
            T_APP_BR_LINK *p_link;
            p_link = app_find_br_link(param->sco_conn_ind.bd_addr);

            if (p_link != NULL)
            {
                bt_sco_conn_cfm(param->sco_conn_ind.bd_addr, true);
            }
        }
        break;

    case BT_EVENT_SCO_CONN_CMPL:
        {
            APP_PRINT_INFO0("BT_EVENT_SCO_CONN_CMPL");
            uint8_t pair_idx;
            T_AUDIO_FORMAT_INFO format_info;
            T_APP_BR_LINK *p_link;
            int8_t temp_tx_power = 7;

            bt_link_tx_power_set(param->sco_conn_cmpl.bd_addr, temp_tx_power);

            if (param->sco_conn_cmpl.cause != 0)
            {
                APP_PRINT_INFO1("param->sco_conn_cmpl.cause 0x%x", param->sco_conn_cmpl.cause);
                break;
            }

            p_link = app_find_br_link(param->sco_conn_cmpl.bd_addr);
            if (p_link == NULL)
            {
                APP_PRINT_INFO0("p_link == NULL");
                break;
            }

            p_link->sco_handle = param->sco_conn_cmpl.handle;

            if (app_find_sco_conn_num() == 1)
            {
                bt_sco_link_switch(param->sco_conn_cmpl.bd_addr);
            }
            else
            {
                APP_PRINT_TRACE2("app_multilink_bt_cback: active sco link %s, current link %s",
                                 TRACE_BDADDR(br_db.br_link[app_hfp_get_active_hf_index()].bd_addr),
                                 TRACE_BDADDR(param->sco_conn_cmpl.bd_addr));
                bt_sco_link_switch(br_db.br_link[app_hfp_get_active_hf_index()].bd_addr);

            }

            p_link->sco_seq_num = 0;

            bt_bond_index_get(param->sco_conn_cmpl.bd_addr, &pair_idx);

            if (param->sco_conn_cmpl.air_mode == 3) /**< Air mode transparent data. */
            {
                format_info.type = AUDIO_FORMAT_TYPE_MSBC;
                format_info.attr.msbc.chann_mode = AUDIO_SBC_CHANNEL_MODE_MONO;
                format_info.attr.msbc.sample_rate = 16000;
                format_info.attr.msbc.bitpool = 26;
                format_info.attr.msbc.allocation_method = 0;
                format_info.attr.msbc.subband_num = 8;
                format_info.attr.msbc.block_length = 15;
            }
            else if (param->sco_conn_cmpl.air_mode == 2) /**< Air mode CVSD. */
            {
                format_info.type = AUDIO_FORMAT_TYPE_CVSD;
                format_info.attr.cvsd.chann_num = AUDIO_SBC_CHANNEL_MODE_DUAL;
                format_info.attr.cvsd.sample_rate = 8000;
                if (param->sco_conn_cmpl.rx_pkt_len == 30)
                {
                    format_info.attr.cvsd.frame_duration = AUDIO_CVSD_FRAME_DURATION_3_75_MS;
                }
                else
                {
                    format_info.attr.cvsd.frame_duration = AUDIO_CVSD_FRAME_DURATION_7_5_MS;
                }
            }

            if (p_link->sco_track_handle != NULL)
            {
                audio_track_release(p_link->sco_track_handle);
                p_link->sco_track_handle = NULL;
            }

            p_link->sco_track_handle = audio_track_create(AUDIO_STREAM_TYPE_VOICE,
                                                          AUDIO_STREAM_MODE_NORMAL,
                                                          AUDIO_STREAM_USAGE_SNOOP,
                                                          format_info,
                                                          12,
                                                          8,
                                                          AUDIO_DEVICE_OUT_DEFAULT | AUDIO_DEVICE_IN_DEFAULT,
                                                          NULL,
                                                          NULL);
            if (p_link->sco_track_handle == NULL)
            {
                APP_PRINT_INFO0("p_link->sco_track_handle == NULL");
                break;
            }


            audio_track_latency_set(p_link->sco_track_handle, 150, false);

            p_link = &br_db.br_link[app_hfp_get_active_hf_index()];

            APP_PRINT_INFO0("voice audio track start");
            audio_track_start(p_link->sco_track_handle);
        }
        break;



    case BT_EVENT_SCO_DISCONNECTED:
        {
            T_APP_BR_LINK *p_link;

            p_link = app_find_br_link(param->sco_disconnected.bd_addr);

            bt_link_tx_power_set(param->sco_disconnected.bd_addr, 0);

            if (p_link != NULL)
            {
                if (p_link->sco_track_handle != NULL)
                {
                    audio_track_release(p_link->sco_track_handle);
                    p_link->sco_track_handle = NULL;
                }
            }
        }
        break;

    case BT_EVENT_SCO_DATA_IND:
        {
            uint16_t written_len;
            T_APP_BR_LINK *p_link;
            T_APP_BR_LINK *p_active_link;
            T_AUDIO_STREAM_STATUS status;

            p_link = app_find_br_link(param->sco_data_ind.bd_addr);
            if (p_link == NULL)
            {
                break;
            }


            p_link->sco_seq_num++;

            APP_PRINT_TRACE2("BT_EVENT_HFP_AG_SCO_DATA_IND:len %u status %u",
                             param->sco_data_ind.length, param->sco_data_ind.status);
            if (param->sco_data_ind.status == BT_SCO_PKT_STATUS_OK)
            {
                status = AUDIO_STREAM_STATUS_CORRECT;
            }
            else
            {
                status = AUDIO_STREAM_STATUS_LOST;
            }

            audio_track_write(p_link->sco_track_handle, param->sco_data_ind.bt_clk,
                              p_link->sco_seq_num,
                              status,
                              1,
                              param->sco_data_ind.p_data,
                              param->sco_data_ind.length,
                              &written_len);
        }
        break;


    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_hfp_bt_cback: event_type 0x%04x", event_type);
    }
}

static void app_audio_hfp_cback(T_AUDIO_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_AUDIO_EVENT_PARAM *param = event_buf;
    bool handle = true;

    switch (event_type)
    {
    case AUDIO_EVENT_TRACK_STATE_CHANGED:
        {
            T_APP_BR_LINK *p_link;
            T_AUDIO_STREAM_TYPE stream_type;

            if (audio_track_stream_type_get(param->track_state_changed.handle, &stream_type) == false)
            {
                APP_PRINT_ERROR0("audio track stream type get fail");
                break;
            }
            APP_PRINT_INFO2("audio stream type = %d, state = %d", stream_type,
                            param->track_state_changed.state);
            if (stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
            {
                if (param->track_state_changed.state == AUDIO_TRACK_STATE_STARTED)
                {

                }
                else if (param->track_state_changed.state == AUDIO_TRACK_STATE_STOPPED ||
                         param->track_state_changed.state == AUDIO_TRACK_STATE_PAUSED)
                {

                }
                else if (param->track_state_changed.state == AUDIO_TRACK_STATE_CREATED)
                {

                }
            }
            else if (stream_type == AUDIO_STREAM_TYPE_VOICE)
            {
                if (param->track_state_changed.state == AUDIO_TRACK_STATE_STOPPED ||
                    param->track_state_changed.state == AUDIO_TRACK_STATE_PAUSED)
                {

                }
                else if (param->track_state_changed.state == AUDIO_TRACK_STATE_STARTED ||
                         param->track_state_changed.state == AUDIO_TRACK_STATE_RESTARTED)
                {


                }
                else if (param->track_state_changed.state == AUDIO_TRACK_STATE_RELEASED)
                {

                }
            }
        }
        break;

    case AUDIO_EVENT_TRACK_DATA_IND:
        {

            T_APP_BR_LINK *p_link;
            p_link = app_find_br_link(br_db.br_link[app_hfp_get_active_hf_index()].bd_addr);
            if (p_link == NULL)
            {
                break;
            }

            APP_PRINT_TRACE1("app_audio_policy_cback: data ind len %u", param->track_data_ind.len);
            uint32_t timestamp;
            uint16_t seq_num;
            uint8_t frame_num;
            uint16_t read_len;
            uint8_t *buf;
            T_AUDIO_STREAM_STATUS status;

            buf = os_mem_alloc(RAM_TYPE_DATA_ON, param->track_data_ind.len);

            if (buf == NULL)
            {
                return;
            }

            if (audio_track_read(p_link->sco_track_handle,
                                 &timestamp,
                                 &seq_num,
                                 &status,
                                 &frame_num,
                                 buf,
                                 param->track_data_ind.len,
                                 &read_len) == true)
            {
                APP_PRINT_INFO5("audio_track_read timestamp: %d, seq num: %d, frame num: %d, read length = %d, duplicate = %d",
                                timestamp, seq_num, frame_num, read_len, p_link->duplicate_fst_sco_data);
                APP_PRINT_INFO1("read mic data = %b", TRACE_BINARY(read_len, buf));

                bt_sco_data_send(br_db.br_link[app_hfp_get_active_hf_index()].bd_addr, seq_num, buf, read_len);
            }

            os_mem_free(buf);
        }

        break;

    case AUDIO_EVENT_TRACK_VOLUME_OUT_CHANGED:

        break;

    case AUDIO_EVENT_VOICE_PROMPT_STARTED:

        break;

    case AUDIO_EVENT_VOICE_PROMPT_STOPPED:

        break;

    case AUDIO_EVENT_RINGTONE_STARTED:

        break;

    case AUDIO_EVENT_RINGTONE_STOPPED:

        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_audio_policy_cback: event_type 0x%04x", event_type);
    }
}


uint8_t app_hfp_get_active_hf_index(void)
{
    return active_hf_index;
}

bool app_hfp_set_active_hf_index(uint8_t *bd_addr)
{
    T_APP_BR_LINK *p_link;

    p_link = app_find_br_link(bd_addr);
    if (p_link)
    {
        active_hf_index = p_link->id;
        return bt_sco_link_switch(bd_addr);
    }
    return false;
}

bool app_hfp_sco_is_connected(void)
{
    return true;
}


void app_hfp_init(void)
{
#if 0
    uint16_t hfp_brsf_capability;
    rtk_kvdb_get("mcu_cfg_const_hfp_brsf_capability", &hfp_brsf_capability,
                 sizeof(hfp_brsf_capability));
    bt_hfp_init(MAX_BR_LINK_NUM, RFC_HFP_CHANN_NUM, RFC_HSP_CHANN_NUM, hfp_brsf_capability);
    DBG_DIRECT("rtk_kvdb_get mcu_cfg_const_hfp_brsf_capability");
#else
    app_cfg_const.hfp_brsf_capability = 0x2BF;
    bt_hfp_init(MAX_BR_LINK_NUM, RFC_HFP_CHANN_NUM, RFC_HSP_CHANN_NUM,
                app_cfg_const.hfp_brsf_capability);
#endif
//    bt_hfp_init(MAX_BR_LINK_NUM, RFC_HFP_CHANN_NUM, RFC_HSP_CHANN_NUM,
//                0);
    bt_mgr_cback_register(app_hfp_bt_cback);
    audio_mgr_cback_register(app_audio_hfp_cback);

}





