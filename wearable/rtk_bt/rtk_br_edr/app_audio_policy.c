/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <string.h>
#include "os_mem.h"
#include "trace.h"
#include "app_timer.h"
#include "btm.h"
#include "audio.h"
#include "ringtone.h"
#include "bt_avrcp.h"
#include "bt_hfp.h"
#include "bt_bond.h"
#include "audio_volume.h"
#include "audio_passthrough.h"
#include "app_audio_policy.h"
#include "dp_br_info.h"
#include "app_report.h"
#include "app_cfg.h"
#include "app_hfp.h"
#include "eq.h"
#include "app_audio_if.h"
#include "app_mmi.h"
#include "bt_a2dp.h"
#include "app_cmd.h"
#include "audio_probe.h"
#include "app_eq.h"
#include "app_br_link_util.h"

//for CMD_AUDIO_DSP_CTRL_SEND
#define CFG_H2D_DAC_GAIN                0x0C
#define CFG_H2D_ADC_GAIN                0x0D
#define CFG_H2D_APT_DAC_GAIN            0x4C

static uint8_t is_mic_mute;

#define MAX_MIN_VOL_REPEAT_INTERVAL  1500

typedef enum
{
    APP_AUDIO_POLICY_TIMER_LONG_PRESS_REPEAT = 0x00,
} T_APP_AUDIO_POLICY_TIMER_ID;

typedef enum
{
    APP_AUDIO_POLICY_TIMER_MAX_VOL = 0x00,
    APP_AUDIO_POLICY_TIMER_MIN_VOL = 0x01,
} T_APP_AUDIO_POLICY_TIMER_CHANNEL;

static uint8_t audio_policy_timer_id = 0;

bool (*app_audio_notify_check)(void) = NULL;

void app_audio_speaker_channel(T_AUDIO_CHANCEL_SET set_mode, uint8_t spk_channel,
                               uint8_t mic_channel);

static void app_audio_policy_cback(T_AUDIO_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_AUDIO_EVENT_PARAM *param = event_buf;
    bool handle = true;
    APP_PRINT_INFO1("app_audio_policy_cback %d", event_type);
    switch (event_type)
    {
    case AUDIO_EVENT_TRACK_STATE_CHANGED:
        {
            T_APP_BR_LINK *p_link;
            T_AUDIO_STREAM_TYPE stream_type;

            if (audio_track_stream_type_get(param->track_state_changed.handle, &stream_type) == false)
            {
                APP_PRINT_INFO0("audio track stream type get fail");
                if (br_db.sco_info.sco_interrupt_a2dp == SCO_INTERRUPT_A2DP_SOURCE)
                {
                    DBG_DIRECT("Recover play11!!!");
                    br_db.sco_info.sco_interrupt_a2dp = SCO_STOP_RECOVER_PLAY;
                    app_mmi_handle_action(MMI_AV_PLAY_PAUSE);
                }
                break;
            }
            DBG_DIRECT("audio stream type = %d, state = %d", stream_type, param->track_state_changed.state);
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
                    p_link = &br_db.br_link[app_hfp_get_active_hf_index()];
#if F_APP_MUTILINK_VA_PREEMPTIVE
                    if (app_cfg_const.enable_multi_link && app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                    {
                        app_bond_set_priority(p_link->bd_addr);
                    }
#endif
                    p_link->duplicate_fst_sco_data = true;

                }
                else if (param->track_state_changed.state == AUDIO_TRACK_STATE_RELEASED)
                {
                    if (br_db.sco_info.sco_interrupt_a2dp)
                    {
                        DBG_DIRECT("Recover play!!!");
                        br_db.sco_info.sco_interrupt_a2dp = SCO_STOP_RECOVER_PLAY;
                        app_mmi_handle_action(MMI_AV_PLAY_PAUSE);
                    }
                }
            }
        }
        break;

    case AUDIO_EVENT_TRACK_DATA_IND:
        {
#if (F_APP_SPP_CAPTURE_DSP_DATA == 1)
            if ((app_cmd_dsp_capture_data_state() & DSP_CAPTURE_DATA_ENTER_SCO_MODE_MASK) == 0)
#endif
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
                    if (p_link->duplicate_fst_sco_data)
                    {
                        p_link->duplicate_fst_sco_data = false;
                        bt_sco_data_send(br_db.br_link[app_hfp_get_active_hf_index()].bd_addr, seq_num - 1, buf, read_len);
                    }
                    bt_sco_data_send(br_db.br_link[app_hfp_get_active_hf_index()].bd_addr, seq_num, buf, read_len);
                }

                os_mem_free(buf);
            }
        }
        break;

    case AUDIO_EVENT_TRACK_VOLUME_OUT_CHANGED:
        {

        }
        break;

    case AUDIO_EVENT_VOICE_PROMPT_STARTED:
        {
//20230607  br_db.tone_vp_status.state = APP_TONE_VP_STARTED;
//20230607  br_db.tone_vp_status.index = param->voice_prompt_started.index;
        }
        break;

    case AUDIO_EVENT_VOICE_PROMPT_STOPPED:
        {
//20230607  br_db.tone_vp_status.state = APP_TONE_VP_STOP;
//20230607  br_db.tone_vp_status.index = param->voice_prompt_stopped.index;
        }
        break;

    case AUDIO_EVENT_RINGTONE_STARTED:
        {
//20230607  br_db.tone_vp_status.state = APP_TONE_VP_STARTED;
//20230607  br_db.tone_vp_status.index = param->ringtone_started.index;
        }
        break;

    case AUDIO_EVENT_RINGTONE_STOPPED:
        {
//20230607  br_db.tone_vp_status.state = APP_TONE_VP_STOP;
//20230607  br_db.tone_vp_status.index = param->ringtone_stopped.index;
        }
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

static void app_audio_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    bool handle = true;

    switch (event_type)
    {
    case BT_EVENT_SCO_CONN_IND:
        {
            T_APP_BR_LINK *p_link;
            p_link = app_find_br_link(param->sco_conn_ind.bd_addr);

            if (p_link != NULL)
            {
                if (br_db.a2dp_info.audio_play_mode != MODE_APP_A2DP_SNK)
                {
//20230607          if (app_audio_get_play_status() == APP_AUDIO_STATE_PLAY)
                    {
                        app_mmi_handle_action(MMI_AV_PLAY_PAUSE);

                        /*20230607              if (br_db.audio_play_mode == MODE_APP_PLAYBACK)
                                                {
                                                    br_db.sco_interrupt_a2dp = SCO_INTERRUPT_LOCAL;
                                                    bt_sco_conn_cfm(param->sco_conn_ind.bd_addr, true);
                                                }
                                                else if (br_db.audio_play_mode == MODE_APP_A2DP_SRC)
                                                {
                                                    br_db.sco_interrupt_a2dp = SCO_INTERRUPT_A2DP_SOURCE;
                                                }*/
                    }
                    /*20230607          else
                                        {
                                            if (br_db.audio_pipe_create == false)
                                            {
                                                bt_sco_conn_cfm(param->sco_conn_ind.bd_addr, true);
                                            }
                                        }*/
                }
                else
                {
                    bt_sco_conn_cfm(param->sco_conn_ind.bd_addr, true);
                }
            }
        }
        break;
    case BT_EVENT_SCO_CONN_CMPL:
        {
            uint8_t pair_idx;
            T_AUDIO_FORMAT_INFO format_info;
            T_APP_BR_LINK *p_link;
            int8_t temp_tx_power = 7;

            bt_link_tx_power_set(param->sco_conn_cmpl.bd_addr, temp_tx_power);

            if (param->sco_conn_cmpl.cause != 0)
            {
                break;
            }

            p_link = app_find_br_link(param->sco_conn_cmpl.bd_addr);
            if (p_link == NULL)
            {
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
                format_info.attr.cvsd.chann_num = AUDIO_SBC_CHANNEL_MODE_MONO;
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

#if (F_APP_NREC_SUPPORT == 1)
            if (p_link->nrec_instance != NULL)
            {
                nrec_release(p_link->nrec_instance);
                p_link->nrec_instance = NULL;
            }
#endif

            p_link->sco_track_handle = audio_track_create(AUDIO_STREAM_TYPE_VOICE,
                                                          AUDIO_STREAM_MODE_NORMAL,
                                                          AUDIO_STREAM_USAGE_SNOOP,
                                                          format_info,
                                                          app_cfg_nv.voice_gain_level[pair_idx],
                                                          8,
                                                          AUDIO_DEVICE_OUT_DEFAULT | AUDIO_DEVICE_IN_DEFAULT,
                                                          NULL,
                                                          NULL);
            if (p_link->sco_track_handle == NULL)
            {
                break;
            }

            if (app_find_b2s_link_num() > 1)
            {
                audio_track_latency_set(p_link->sco_track_handle, 150, false);
            }
            else
            {
                audio_track_latency_set(p_link->sco_track_handle, 100, false);
            }


#if (F_APP_NREC_SUPPORT == 1)
            p_link->nrec_instance = nrec_create();
            if (p_link->nrec_instance != NULL)
            {
                nrec_enable(p_link->nrec_instance);
                audio_track_effect_attach(p_link->sco_track_handle, p_link->nrec_instance);
            }
#endif

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

#if (F_APP_NREC_SUPPORT == 1)
                if (p_link->nrec_instance != NULL)
                {
                    nrec_release(p_link->nrec_instance);
                    p_link->nrec_instance = NULL;
                }
#endif
                p_link->sco_handle = 0;
            }

            br_db.audio_info.voice_muted = 0;
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

            p_active_link = &br_db.br_link[app_hfp_get_active_hf_index()];
            if (memcmp(p_active_link->bd_addr, p_link->bd_addr, 6) != 0)
            {
                APP_PRINT_TRACE2("BT_EVENT_HFP_AG_SCO_DATA_IND: link status is not active %s %s.",
                                 TRACE_BDADDR(p_active_link->bd_addr), TRACE_BDADDR(p_link->bd_addr));
                break;
            }
#if F_APP_MUTILINK_VA_PREEMPTIVE
            else if (app_cfg_const.enable_multi_link &&
                     remote_session_role_get() == REMOTE_SESSION_ROLE_SECONDARY)
            {
                if (memcmp(p_link->bd_addr, app_db.active_hfp_addr, 6) != 0)
                {
                    APP_PRINT_TRACE2("BT_EVENT_HFP_AG_SCO_DATA_IND: link status is not active %s %s.",
                                     TRACE_BDADDR(app_db.active_hfp_addr), TRACE_BDADDR(p_link->bd_addr));
                    break;
                }
            }
#endif
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

    case BT_EVENT_HFP_CALL_STATUS:
        {
            if (param->hfp_call_status.curr_status != BT_HFP_CALL_IDLE)
            {
//20230607      if (app_audio_get_play_status() == APP_AUDIO_STATE_PLAY)
                {
                    /*20230607          if (br_db.audio_play_mode == MODE_APP_PLAYBACK)
                                        {
                                            br_db.sco_interrupt_a2dp = SCO_INTERRUPT_LOCAL;
                                        }
                                        else if (br_db.audio_play_mode == MODE_APP_A2DP_SRC)
                                        {
                                            br_db.sco_interrupt_a2dp = SCO_INTERRUPT_A2DP_SOURCE;
                                        }*/
                    app_mmi_handle_action(MMI_AV_PLAY_PAUSE);
                }
            }
            else if (param->hfp_call_status.curr_status == BT_HFP_CALL_IDLE)
            {
                if (br_db.sco_info.sco_interrupt_a2dp == SCO_INTERRUPT_LOCAL)
                {
                    br_db.sco_info.sco_interrupt_a2dp = SCO_STOP_RECOVER_PLAY;
                    app_mmi_handle_action(MMI_AV_PLAY_PAUSE);
                }
            }
        }
        break;

    case BT_EVENT_ACL_CONN_SUCCESS:
        {

        }
        break;

    case BT_EVENT_HFP_CONN_CMPL:
        {
            uint8_t pair_idx;

            bt_bond_index_get(param->hfp_conn_cmpl.bd_addr, &pair_idx);

            audio_volume_out_set(AUDIO_STREAM_TYPE_VOICE, app_cfg_nv.voice_gain_level[pair_idx]);

            if (br_db.audio_info.voice_muted)
            {
                APP_PRINT_INFO1("voice_muted line = %d", __LINE__);
                audio_volume_in_mute(AUDIO_STREAM_TYPE_VOICE);
            }
        }
        break;

    case BT_EVENT_A2DP_STREAM_START_IND:
        {
            //this case will only used in sink mode
            if (br_db.a2dp_info.audio_play_mode == MODE_APP_A2DP_SNK)
            {
                T_APP_BR_LINK *p_link;
                uint8_t pair_idx;
                p_link = app_find_br_link(param->a2dp_stream_start_ind.bd_addr);

                T_AUDIO_FORMAT_INFO format_info = {};
                T_AUDIO_STREAM_MODE mode = AUDIO_STREAM_MODE_NORMAL;
                T_AUDIO_STREAM_USAGE stream = AUDIO_STREAM_USAGE_LOCAL;

                bt_a2dp_stream_start_cfm(param->a2dp_stream_start_ind.bd_addr, true);

                if (param->a2dp_stream_start_ind.codec_type == BT_A2DP_CODEC_TYPE_SBC)
                {
                    format_info.type = AUDIO_FORMAT_TYPE_SBC;
                    switch (param->a2dp_stream_start_ind.codec_info.sbc.sampling_frequency)
                    {
                    case BT_A2DP_SBC_SAMPLING_FREQUENCY_16KHZ:
                        format_info.attr.sbc.sample_rate = 16000;
                        break;
                    case BT_A2DP_SBC_SAMPLING_FREQUENCY_32KHZ:
                        format_info.attr.sbc.sample_rate = 32000;
                        break;
                    case BT_A2DP_SBC_SAMPLING_FREQUENCY_44_1KHZ:
                        format_info.attr.sbc.sample_rate = 44100;
                        break;
                    case BT_A2DP_SBC_SAMPLING_FREQUENCY_48KHZ:
                        format_info.attr.sbc.sample_rate = 48000;
                        break;
                    }

                    switch (param->a2dp_stream_start_ind.codec_info.sbc.channel_mode)
                    {
                    case BT_A2DP_SBC_CHANNEL_MODE_MONO:
                        format_info.attr.sbc.chann_mode = AUDIO_SBC_CHANNEL_MODE_MONO;
                        break;
                    case BT_A2DP_SBC_CHANNEL_MODE_DUAL_CHANNEL:
                        format_info.attr.sbc.chann_mode = AUDIO_SBC_CHANNEL_MODE_DUAL;
                        break;
                    case BT_A2DP_SBC_CHANNEL_MODE_STEREO:
                        format_info.attr.sbc.chann_mode = AUDIO_SBC_CHANNEL_MODE_STEREO;
                        break;
                    case BT_A2DP_SBC_CHANNEL_MODE_JOINT_STEREO:
                        format_info.attr.sbc.chann_mode = AUDIO_SBC_CHANNEL_MODE_JOINT_STEREO;
                        break;
                    }
                    switch (param->a2dp_stream_start_ind.codec_info.sbc.block_length)
                    {
                    case BT_A2DP_SBC_BLOCK_LENGTH_4:
                        format_info.attr.sbc.block_length = 4;
                        break;
                    case BT_A2DP_SBC_BLOCK_LENGTH_8:
                        format_info.attr.sbc.block_length = 8;
                        break;
                    case BT_A2DP_SBC_BLOCK_LENGTH_12:
                        format_info.attr.sbc.block_length = 12;
                        break;
                    case BT_A2DP_SBC_BLOCK_LENGTH_16:
                        format_info.attr.sbc.block_length = 16;
                        break;
                    }
                    switch (param->a2dp_stream_start_ind.codec_info.sbc.subbands)
                    {
                    case BT_A2DP_SBC_SUBBANDS_4:
                        format_info.attr.sbc.subband_num = 4;
                        break;
                    case BT_A2DP_SBC_SUBBANDS_8:
                        format_info.attr.sbc.subband_num = 8;
                        break;
                    }
                    switch (param->a2dp_stream_start_ind.codec_info.sbc.allocation_method)
                    {
                    case BT_A2DP_SBC_ALLOCATION_METHOD_LOUDNESS:
                        format_info.attr.sbc.allocation_method = 0;
                        break;
                    case BT_A2DP_SBC_ALLOCATION_METHOD_SNR:
                        format_info.attr.sbc.allocation_method = 1;
                        break;
                    }

                    p_link->a2dp_codec_info.sbc.sampling_frequency = format_info.attr.sbc.sample_rate;
                    p_link->a2dp_codec_info.sbc.channel_mode = format_info.attr.sbc.chann_mode;
                    p_link->a2dp_codec_info.sbc.block_length = format_info.attr.sbc.block_length;
                    p_link->a2dp_codec_info.sbc.subbands = format_info.attr.sbc.subband_num;
                    p_link->a2dp_codec_info.sbc.allocation_method = format_info.attr.sbc.allocation_method;
                }
                else if (param->a2dp_stream_start_ind.codec_type == BT_A2DP_CODEC_TYPE_AAC)
                {
                    format_info.type = AUDIO_FORMAT_TYPE_AAC;
                    switch (param->a2dp_stream_start_ind.codec_info.aac.sampling_frequency)
                    {
                    case BT_A2DP_AAC_SAMPLING_FREQUENCY_8KHZ:
                        format_info.attr.aac.sample_rate = 8000;
                        break;
                    case BT_A2DP_AAC_SAMPLING_FREQUENCY_16KHZ:
                        format_info.attr.aac.sample_rate = 16000;
                        break;

                    case BT_A2DP_AAC_SAMPLING_FREQUENCY_44_1KHZ:
                        format_info.attr.aac.sample_rate = 44100;
                        break;

                    case BT_A2DP_AAC_SAMPLING_FREQUENCY_48KHZ:
                        format_info.attr.aac.sample_rate = 48000;
                        break;

                    case BT_A2DP_AAC_SAMPLING_FREQUENCY_96KHZ:
                        format_info.attr.aac.sample_rate = 96000;
                        break;
                    default:
                        break;
                    }

                    switch (param->a2dp_stream_start_ind.codec_info.aac.channel_number)
                    {
                    case BT_A2DP_AAC_CHANNEL_NUMBER_1:
                        format_info.attr.aac.chann_num = 1;
                        break;

                    case BT_A2DP_AAC_CHANNEL_NUMBER_2:
                        format_info.attr.aac.chann_num = 2;
                        break;

                    default:
                        break;
                    }
                    format_info.attr.aac.transport_format = AUDIO_AAC_TRANSPORT_FORMAT_LATM;

                    p_link->a2dp_codec_info.aac.sampling_frequency = format_info.attr.aac.sample_rate;
                    p_link->a2dp_codec_info.aac.channel_number = format_info.attr.aac.chann_num;
                }

                p_link = app_find_br_link(param->a2dp_stream_start_ind.bd_addr);
                if (p_link == NULL)
                {
                    return;
                }

                if (p_link->a2dp_track_handle)
                {
                    audio_track_release(p_link->a2dp_track_handle);
                }

                bt_bond_index_get(p_link->bd_addr, &pair_idx);
                p_link->a2dp_track_handle = audio_track_create(AUDIO_STREAM_TYPE_PLAYBACK,
                                                               mode,
                                                               stream,
                                                               format_info,
                                                               app_cfg_nv.audio_gain_level[pair_idx],
                                                               0,
                                                               AUDIO_DEVICE_OUT_DEFAULT,
                                                               NULL,
                                                               NULL);


                if (p_link->a2dp_track_handle != NULL)
                {
                    /*20230607          if ((format_info.type == AUDIO_FORMAT_TYPE_SBC && format_info.attr.sbc.sample_rate == 44100) ||
                                            (format_info.type == AUDIO_FORMAT_TYPE_AAC && format_info.attr.aac.sample_rate == 44100))
                                        {
                                            if (g_eq_44p1 != NULL)
                                            {
                                                audio_track_effect_attach(p_link->a2dp_track_handle, g_eq_44p1);
                                            }
                                        }
                                        else if ((format_info.type == AUDIO_FORMAT_TYPE_SBC && format_info.attr.sbc.sample_rate == 48000) ||
                                                 (format_info.type == AUDIO_FORMAT_TYPE_AAC && format_info.attr.aac.sample_rate == 48000))
                                        {
                                            if (g_eq_48 != NULL)
                                            {
                                                audio_track_effect_attach(p_link->a2dp_track_handle, g_eq_48);
                                            }
                                        }*/
                    audio_track_latency_set(p_link->a2dp_track_handle, A2DP_LATENCY_MS, true);
                    bt_a2dp_stream_delay_report_request(p_link->bd_addr, A2DP_LATENCY_MS);
                    audio_track_start(p_link->a2dp_track_handle);
                }

            }

        }
        break;

    case BT_EVENT_A2DP_STREAM_DATA_IND:
        {
            //this case will only used in sink mode
            if (br_db.a2dp_info.audio_play_mode == MODE_APP_A2DP_SNK)
            {
                uint16_t written_len;
                T_APP_BR_LINK *p_link;
                T_AUDIO_STREAM_STATUS status = AUDIO_STREAM_STATUS_CORRECT;
                p_link = app_find_br_link(param->a2dp_stream_data_ind.bd_addr);
                if (p_link == NULL)
                {
                    break;
                }

                audio_track_write(p_link->a2dp_track_handle, param->a2dp_stream_data_ind.bt_clock,
                                  param->a2dp_stream_data_ind.seq_num,
                                  status,
                                  param->a2dp_stream_data_ind.frame_num,
                                  param->a2dp_stream_data_ind.payload,
                                  param->a2dp_stream_data_ind.len,
                                  &written_len);
            }
        }
        break;

    case BT_EVENT_A2DP_STREAM_STOP:
        {
            //this case will only used in sink mode
            if (br_db.a2dp_info.audio_play_mode == MODE_APP_A2DP_SNK)
            {
                T_APP_BR_LINK *p_link;
                p_link = app_find_br_link(param->a2dp_stream_stop.bd_addr);
                if (p_link != NULL)
                {
                    if (p_link->a2dp_track_handle != NULL)
                    {
                        audio_track_release(p_link->a2dp_track_handle);
                        p_link->a2dp_track_handle = NULL;
                    }
                }
            }
        }
        break;

    case BT_EVENT_A2DP_STREAM_CLOSE:
        {
            //this case will only used in sink mode
            if (br_db.a2dp_info.audio_play_mode == MODE_APP_A2DP_SNK)
            {
                T_APP_BR_LINK *p_link;
                p_link = app_find_br_link(param->a2dp_stream_stop.bd_addr);
                if (p_link != NULL)
                {
                    if (p_link->a2dp_track_handle != NULL)
                    {
                        audio_track_release(p_link->a2dp_track_handle);
                        p_link->a2dp_track_handle = NULL;
                    }
                }
            }
        }
        break;

    case BT_EVENT_A2DP_DISCONN_CMPL:
        {
            //this case will only used in sink mode
            if (br_db.a2dp_info.audio_play_mode == MODE_APP_A2DP_SNK)
            {
                T_APP_BR_LINK *p_link;
                p_link = app_find_br_link(param->a2dp_stream_stop.bd_addr);
                if (p_link != NULL)
                {
                    if (p_link->a2dp_track_handle != NULL)
                    {
                        audio_track_release(p_link->a2dp_track_handle);
                        p_link->a2dp_track_handle = NULL;
                    }
                }
            }
        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_audio_bt_cback: event_type 0x%04x", event_type);
    }
}

static void app_audio_policy_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    switch (timer_evt)
    {
    case APP_AUDIO_POLICY_TIMER_LONG_PRESS_REPEAT:
        {

        }
        break;

    default:
        break;
    }
}

void app_audio_init(void)
{
    APP_PRINT_INFO0("app_audio_init");
    audio_mgr_cback_register(app_audio_policy_cback);
    bt_mgr_cback_register(app_audio_bt_cback);
    app_timer_reg_cb(app_audio_policy_timeout_cb, &audio_policy_timer_id);

    app_audio_route_gain_init();
//20230607  app_eq_init();

    voice_prompt_language_set((T_VOICE_PROMPT_LANGUAGE_ID)app_cfg_nv.voice_prompt_language);

    audio_volume_out_max_set(AUDIO_STREAM_TYPE_PLAYBACK, app_cfg_volume.playback_volume_max);
    audio_volume_out_max_set(AUDIO_STREAM_TYPE_VOICE, app_cfg_volume.voice_out_volume_max);
    audio_volume_in_max_set(AUDIO_STREAM_TYPE_VOICE, app_cfg_volume.voice_volume_in_max);

    audio_volume_out_min_set(AUDIO_STREAM_TYPE_PLAYBACK, app_cfg_volume.playback_volume_min);
    audio_volume_out_min_set(AUDIO_STREAM_TYPE_VOICE, app_cfg_volume.voice_out_volume_min);
    audio_volume_in_min_set(AUDIO_STREAM_TYPE_VOICE, app_cfg_volume.voice_volume_in_min);

    audio_volume_out_set(AUDIO_STREAM_TYPE_PLAYBACK, app_cfg_volume.playback_volume_default);
    audio_volume_out_set(AUDIO_STREAM_TYPE_VOICE, app_cfg_volume.voice_out_volume_default);
    audio_volume_in_set(AUDIO_STREAM_TYPE_VOICE, app_cfg_volume.voice_volume_in_default);

    voice_prompt_volume_max_set(app_cfg_volume.voice_prompt_volume_max);
    voice_prompt_volume_min_set(app_cfg_volume.voice_prompt_volume_min);
    voice_prompt_volume_set(app_cfg_volume.voice_prompt_volume_default);

    ringtone_volume_max_set(app_cfg_volume.ringtone_volume_max);
    ringtone_volume_min_set(app_cfg_volume.ringtone_volume_min);
    ringtone_volume_set(app_cfg_volume.ringtone_volume_default);

    audio_volume_out_channel_set(AUDIO_VOLUME_CHANNEL_LEFT_RIGHT_MIX);
    APP_PRINT_INFO0("app_audio_init1");
}

uint8_t app_audio_is_mic_mute(void)
{
    return is_mic_mute;
}

void app_audio_set_mic_mute_status(uint8_t status)
{
    is_mic_mute = status;
}

void app_a2dp_sink_volume_up(void)
{
    uint8_t max_volume = 0;
    uint8_t curr_volume = 0;
    uint8_t active_idx;
    uint8_t pair_idx;
    uint8_t level;
    active_idx = app_get_active_a2dp_idx();
    if (bt_bond_index_get(br_db.br_link[active_idx].bd_addr, &pair_idx) == false)
    {
        APP_PRINT_ERROR0("app_volume_up: find active a2dp pair_index fail");
        return;
    }
    curr_volume = app_cfg_nv.audio_gain_level[pair_idx];
    max_volume = app_cfg_volume.playback_volume_max;

    if (curr_volume < max_volume)
    {
        curr_volume++;
    }
    else
    {
        curr_volume = max_volume;
    }

    level = (curr_volume * 0x7F + app_cfg_volume.playback_volume_max / 2) /
            app_cfg_volume.playback_volume_max;
    app_cfg_nv.audio_gain_level[pair_idx] = curr_volume;
    audio_track_volume_out_set(br_db.br_link[active_idx].a2dp_track_handle, curr_volume);
    bt_avrcp_volume_change_req(br_db.br_link[active_idx].bd_addr, level);
}

void app_a2dp_sink_volume_down(void)
{
    uint8_t min_volume = 0;
    uint8_t curr_volume = 0;
    uint8_t active_idx;
    uint8_t pair_idx;
    uint8_t level;
    active_idx = app_get_active_a2dp_idx();
    if (bt_bond_index_get(br_db.br_link[active_idx].bd_addr, &pair_idx) == false)
    {
        APP_PRINT_ERROR0("app_volume_up: find active a2dp pair_index fail");
        return;
    }
    curr_volume = app_cfg_nv.audio_gain_level[pair_idx];
    min_volume = app_cfg_volume.playback_volume_min;

    if (curr_volume > min_volume)
    {
        curr_volume--;
    }
    else
    {
        curr_volume = min_volume;
    }

    level = (curr_volume * 0x7F + app_cfg_volume.playback_volume_max / 2) /
            app_cfg_volume.playback_volume_max;
    app_cfg_nv.audio_gain_level[pair_idx] = curr_volume;
    audio_track_volume_out_set(br_db.br_link[active_idx].a2dp_track_handle, curr_volume);
    bt_avrcp_volume_change_req(br_db.br_link[active_idx].bd_addr, level);
}

bool app_audio_check_mic_mute_enable(void)
{
    uint8_t i;
    bool enable_mic_mute = false;

    for (i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (br_db.br_link[i].sco_handle != 0)
        {
            enable_mic_mute = true;
            break;
        }
        else
        {
            enable_mic_mute = false;
        }
    }

    return enable_mic_mute;
}

/*20230607 bool app_audio_tone_play(uint8_t index, bool flush, bool relay)
{
    bool ret = false;

    APP_PRINT_TRACE3("app_audio_tone_play: 0x%02x,state=%d,index=%d",
                     index,
                     br_db.tone_vp_status.state,
                     br_db.tone_vp_status.index);
    if (app_audio_notify_check != NULL)
    {
        if (app_audio_notify_check())
        {
            if (index < VOICE_PROMPT_INDEX)
            {
                if ((br_db.tone_vp_status.state == APP_TONE_VP_STARTED) &&
                    (br_db.tone_vp_status.index == index))
                {
                    APP_PRINT_TRACE2("app_audio_tone_play: must ignore same tone_index=%d, cur_index=%d",
                                     index,
                                     br_db.tone_vp_status.index);
                    return ret;
                }
            }
            else
            {
                if ((br_db.tone_vp_status.state == APP_TONE_VP_STARTED) &&
                    (br_db.tone_vp_status.index == (index - VOICE_PROMPT_INDEX)))
                {
                    APP_PRINT_TRACE2("app_audio_tone_play: must ignore same vp_index=%d, cur_index=%d",
                                     index,
                                     br_db.tone_vp_status.index);
                    return ret;
                }
            }
        }
    }
    if (index < VOICE_PROMPT_INDEX)
    {
        if (flush)
        {
            ;//ringtone_cancel(index);//need owner fix
        }
        ret = ringtone_play(index, relay);
    }
    else if (index < TONE_INVALID_INDEX)
    {
        if (flush)
        {
            ;//voice_prompt_cancel(index - VOICE_PROMPT_INDEX);//need owner fix
        }
        ret = voice_prompt_play(index - VOICE_PROMPT_INDEX,
                                (T_VOICE_PROMPT_LANGUAGE_ID)app_cfg_nv.voice_prompt_language,
                                relay);
    }

    return ret;
}*/


void app_audio_speaker_channel(T_AUDIO_CHANCEL_SET set_mode, uint8_t spk_channel,
                               uint8_t mic_channel)
{
    uint32_t mic_speaker_channel = 0;
    switch (set_mode)
    {
    case AUDIO_COUPLE_SET:
    case AUDIO_COUPLE_MIC_SET:
        {
            mic_speaker_channel = app_cfg_const.couple_speaker_channel & 0x0F;
            if (set_mode == AUDIO_COUPLE_MIC_SET)
            {
                mic_speaker_channel |= mic_channel << 8;
            }
            else
            {
                mic_speaker_channel |= app_cfg_const.mic_channel << 8;
            }
        }
        break;

    case AUDIO_SINGLE_SET:
    case AUDIO_SINGLE_MIC_SET:
        {
            mic_speaker_channel = app_cfg_const.solo_speaker_channel;
            if (set_mode == AUDIO_SINGLE_MIC_SET)
            {
                mic_speaker_channel |= mic_channel << 8;
            }
            else
            {
                mic_speaker_channel |= app_cfg_const.mic_channel << 8;
            }
        }
        break;

    case AUDIO_SPECIFIC_SET:
    case AUDIO_SPECIFIC_MIC_SET:
        {
            mic_speaker_channel = spk_channel;
            if (set_mode == AUDIO_SPECIFIC_MIC_SET)
            {
                mic_speaker_channel |= mic_channel << 8;
            }
            else
            {
                mic_speaker_channel |= app_cfg_const.mic_channel << 8;
            }
        }
        break;

    default:
        break;
    }
    app_cfg_nv.cur_spk_channel = mic_speaker_channel;
    app_cfg_nv.cur_mic = mic_speaker_channel >> 8;
    AUDIO_PRINT_TRACE4("app_audio_speaker_channel: set_mode = %d, couple_speaker_channel = %d, mic_speaker_channel = %x",
                       set_mode,
                       app_cfg_const.couple_speaker_channel,
                       app_cfg_const.solo_speaker_channel,
                       mic_speaker_channel);
    audio_volume_out_channel_set((T_AUDIO_VOLUME_CHANNEL_MASK)mic_speaker_channel);
}


void app_audio_speaker_channel_set(uint8_t spk_channel)
{
    app_audio_speaker_channel(AUDIO_SPECIFIC_SET, spk_channel, app_cfg_nv.cur_mic);
}

void app_audio_mic_channel_set(uint8_t mic_channel)
{
    app_audio_speaker_channel(AUDIO_SPECIFIC_MIC_SET, app_cfg_nv.cur_spk_channel, mic_channel);
}


/*20230607
void app_audio_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path, uint8_t app_idx,
                          uint8_t *ack_pkt)
{
    APP_PRINT_INFO1("app_audio_cmd_handle: %s", TRACE_BINARY(cmd_len, cmd_ptr));
    uint16_t cmd_id = (uint16_t)(cmd_ptr[0] | (cmd_ptr[1] << 8));

    switch (cmd_id)
    {
    case CMD_AUDIO_DSP_CTRL_SEND:
        {
            uint8_t *buf;
            T_AUDIO_STREAM_TYPE stream_type;
            T_DSP_TOOL_GAIN_LEVEL_DATA *gain_level_data;
            bool send_cmd_flag = true;

            buf = malloc(cmd_len - 2);
            if (buf == NULL)
            {
                return;
            }

            memcpy(buf, &cmd_ptr[2], (cmd_len - 2));

            gain_level_data = (T_DSP_TOOL_GAIN_LEVEL_DATA *)buf;

            send_cmd_flag = app_cmd_spp_capture_audio_dsp_ctrl_send_handler(&cmd_ptr[0], cmd_len, cmd_path,
                                                                            app_idx, &ack_pkt[0], send_cmd_flag);

            app_cmd_set_event_ack(cmd_path, app_idx, ack_pkt);

            if (gain_level_data->cmd_id == CFG_H2D_DAC_GAIN ||
                gain_level_data->cmd_id == CFG_H2D_ADC_GAIN ||
                gain_level_data->cmd_id == CFG_H2D_APT_DAC_GAIN)
            {
                switch (gain_level_data->category)
                {
                case AUDIO_CATEGORY_TONE:
                case AUDIO_CATEGORY_VP:
                case AUDIO_CATEGORY_AUDIO:
                    {
                        stream_type = AUDIO_STREAM_TYPE_PLAYBACK;
                    }
                    break;

                case AUDIO_CATEGORY_APT:
                case AUDIO_CATEGORY_LLAPT:
                case AUDIO_CATEGORY_ANC:
                case AUDIO_CATEGORY_ANALOG:
                case AUDIO_CATEGORY_VOICE:
                    {
                        stream_type = AUDIO_STREAM_TYPE_VOICE;
                    }
                    break;

                case AUDIO_CATEGORY_VAD:
                case AUDIO_CATEGORY_RECORD:
                    {
                        stream_type = AUDIO_STREAM_TYPE_RECORD;
                    }
                    break;

                default:
                    {
                        stream_type = AUDIO_STREAM_TYPE_PLAYBACK;
                    }
                    break;
                }

                if (gain_level_data->cmd_id == CFG_H2D_DAC_GAIN)
                {
                    app_audio_route_dac_gain_set((T_AUDIO_CATEGORY)gain_level_data->category, gain_level_data->level,
                                                 gain_level_data->gain);
                    audio_volume_out_set(stream_type, gain_level_data->level);
                }
//                else if (gain_level_data->cmd_id == CFG_H2D_APT_DAC_GAIN)
//                {
//                    app_audio_route_dac_gain_set((T_AUDIO_CATEGORY)gain_level_data->category, gain_level_data->level,
//                                                 gain_level_data->gain);
//                    app_cfg_nv.apt_volume_out_level = gain_level_data->level;
//                    audio_passthrough_volume_out_set(gain_level_data->level);
//                }
                else
                {
                    app_audio_route_adc_gain_set((T_AUDIO_CATEGORY)gain_level_data->category, gain_level_data->level,
                                                 gain_level_data->gain);
                    if (((T_AUDIO_CATEGORY)gain_level_data->category != AUDIO_CATEGORY_APT) &&
                        ((T_AUDIO_CATEGORY)gain_level_data->category != AUDIO_CATEGORY_LLAPT))
                    {
                        audio_volume_in_set(stream_type, gain_level_data->level);
                    }
                    else
                    {
                         audio_passthrough_volume_in_set(gain_level_data->level);
                    }
                }
            }

            if (send_cmd_flag)
            {
                audio_probe_dsp_send(buf, cmd_len - 2);
            }
            free(buf);
        }
        break;

    case CMD_AUDIO_CODEC_CTRL_SEND:
        {
            uint8_t *buf;

            buf = malloc(cmd_len - 2);
            if (buf == NULL)
            {
                return;
            }

            memcpy(buf, &cmd_ptr[2], (cmd_len - 2));

            app_cmd_set_event_ack(cmd_path, app_idx, ack_pkt);
            audio_probe_codec_send(buf, cmd_len - 2);
            free(buf);
        }
        break;

    default:
        break;
    }
}
*/

