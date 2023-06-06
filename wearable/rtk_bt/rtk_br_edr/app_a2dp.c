/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include <string.h>
#include "trace.h"
#include "btm.h"
#include "app_br_link_util.h"
#include "bt_a2dp.h"
#include "audio_track.h"

#define A2DP_LATENCY_MS                     280
#define A2DP_LINK_NUM                      1
#define SBC_CHANNEL_MODE                   (BT_A2DP_SBC_CHANNEL_MODE_MONO|BT_A2DP_SBC_CHANNEL_MODE_DUAL_CHANNEL|BT_A2DP_SBC_CHANNEL_MODE_STEREO|BT_A2DP_SBC_CHANNEL_MODE_JOINT_STEREO)
#define SBC_BLOCK_LEN                      (BT_A2DP_SBC_BLOCK_LENGTH_4|BT_A2DP_SBC_BLOCK_LENGTH_8|BT_A2DP_SBC_BLOCK_LENGTH_12|BT_A2DP_SBC_BLOCK_LENGTH_16)
#define SBC_SUBBANDS                       (BT_A2DP_SBC_SUBBANDS_4|BT_A2DP_SBC_SUBBANDS_8)
#define SBC_ALLOCATION_METHOD              (BT_A2DP_SBC_ALLOCATION_METHOD_SNR|BT_A2DP_SBC_ALLOCATION_METHOD_LOUDNESS)
#define SBC_MIN_BITPOOL                       2
#define SBC_MAX_BITPOOL                       35


static void app_a2dp_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    T_APP_BR_LINK *p_link;

    switch (event_type)
    {
    case BT_EVENT_A2DP_CONN_IND:
        {
            APP_PRINT_INFO0(" BT_EVENT_A2DP_CONN_IND\n");
            p_link = app_find_br_link(param->a2dp_conn_ind.bd_addr);
            if (p_link != NULL)
            {
                bt_a2dp_connect_cfm(p_link->bd_addr, true);
            }
        }
        break;

    case BT_EVENT_A2DP_CONFIG_CMPL:
        {
            APP_PRINT_INFO0(" BT_EVENT_A2DP_CONFIG_CMPL\n");
        }
        break;

    case BT_EVENT_A2DP_CONN_CMPL:
        {
            APP_PRINT_INFO0(" BT_EVENT_A2DP_CONN_CMPL\n");
        }
        break;

    case BT_EVENT_A2DP_STREAM_OPEN:
        {
            APP_PRINT_INFO0(" BT_EVENT_A2DP_STREAM_OPEN\n");
        }
        break;

    case BT_EVENT_A2DP_STREAM_START_IND:
        {
            APP_PRINT_INFO0(" BT_EVENT_A2DP_STREAM_START_IND\n");
            p_link = app_find_br_link(param->a2dp_conn_ind.bd_addr);

#if 1
            T_AUDIO_FORMAT_INFO format_info = {};
            T_AUDIO_STREAM_MODE mode = AUDIO_STREAM_MODE_NORMAL;
            T_AUDIO_STREAM_USAGE stream = AUDIO_STREAM_USAGE_LOCAL;
            uint8_t volume_out = 8;
            uint8_t volume_in = 0;
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
            if (p_link->a2dp_track_handle)
            {
                audio_track_release(p_link->a2dp_track_handle);
            }
            p_link->a2dp_track_handle = audio_track_create(AUDIO_STREAM_TYPE_PLAYBACK,
                                                           mode,
                                                           stream,
                                                           format_info,
                                                           volume_out,
                                                           volume_in,
                                                           AUDIO_DEVICE_OUT_DEFAULT,
                                                           NULL,
                                                           NULL);
            if (p_link->a2dp_track_handle)
            {
                audio_track_latency_set(p_link->a2dp_track_handle, A2DP_LATENCY_MS, true);
                bt_a2dp_stream_delay_report_request(p_link->bd_addr, A2DP_LATENCY_MS);
                audio_track_start(p_link->a2dp_track_handle);
            }
#endif
            bt_a2dp_stream_start_cfm(param->a2dp_stream_start_ind.bd_addr, true);
        }
        break;

    case BT_EVENT_A2DP_STREAM_DATA_IND:
        {
            APP_PRINT_INFO0(" BT_EVENT_A2DP_STREAM_DATA_IND\n");
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
        break;

    case BT_EVENT_A2DP_STREAM_START_RSP:
        {
            APP_PRINT_INFO0(" BT_EVENT_A2DP_STREAM_START_RSP\n");
        }
        break;

    case BT_EVENT_A2DP_STREAM_STOP:
        {
            APP_PRINT_INFO0(" BT_EVENT_A2DP_STREAM_STOP\n");
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
        break;

    case BT_EVENT_A2DP_STREAM_DATA_RSP:
        {
            APP_PRINT_INFO0(" BT_EVENT_A2DP_STREAM_DATA_RSP\n");
        }
        break;

    case BT_EVENT_A2DP_STREAM_OPEN_FAIL:
        {
            APP_PRINT_INFO0(" BT_EVENT_A2DP_STREAM_OPEN_FAIL\n");
        }
        break;

    case BT_EVENT_A2DP_STREAM_CLOSE:
        {
            APP_PRINT_INFO0(" BT_EVENT_A2DP_STREAM_CLOSE\n");
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
        break;
    case BT_EVENT_A2DP_DISCONN_CMPL:
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
        break;

    default:
        {
        }
        break;
    }

}

void app_a2dp_sink_mode_init(void)
{
    if (bt_a2dp_role_set(BT_A2DP_ROLE_SNK))
    {
        a2dp_sdb_record_reinit(BT_A2DP_ROLE_SNK);

        T_BT_A2DP_STREAM_ENDPOINT sep;

        sep.codec_type = BT_A2DP_CODEC_TYPE_SBC;
        sep.u.codec_sbc.sampling_frequency_mask = BT_A2DP_SBC_SAMPLING_FREQUENCY_44_1KHZ |
                                                  BT_A2DP_SBC_SAMPLING_FREQUENCY_48KHZ;
        sep.u.codec_sbc.channel_mode_mask = SBC_CHANNEL_MODE;
        sep.u.codec_sbc.block_length_mask = SBC_BLOCK_LEN;
        sep.u.codec_sbc.subbands_mask = SBC_SUBBANDS;
        sep.u.codec_sbc.allocation_method_mask = SBC_ALLOCATION_METHOD;
        sep.u.codec_sbc.min_bitpool = SBC_MIN_BITPOOL;
        sep.u.codec_sbc.max_bitpool = SBC_MAX_BITPOOL;

        bt_a2dp_stream_endpoint_add(sep);
    }
    else
    {
        APP_PRINT_INFO0("app_a2dp_sink_mode_init set a2dp sink role fail");
    }
}

void app_a2dp_init(void)
{
    a2dp_sdb_record_reinit(BT_A2DP_ROLE_SNK);
    bt_a2dp_init(A2DP_LINK_NUM, A2DP_LATENCY_MS);
    bt_a2dp_role_set(BT_A2DP_ROLE_SNK);

    T_BT_A2DP_STREAM_ENDPOINT sep;

    sep.codec_type = BT_A2DP_CODEC_TYPE_SBC;
    sep.u.codec_sbc.sampling_frequency_mask = BT_A2DP_SBC_SAMPLING_FREQUENCY_44_1KHZ |
                                              BT_A2DP_SBC_SAMPLING_FREQUENCY_48KHZ;
    sep.u.codec_sbc.channel_mode_mask = SBC_CHANNEL_MODE;
    sep.u.codec_sbc.block_length_mask = SBC_BLOCK_LEN;
    sep.u.codec_sbc.subbands_mask = SBC_SUBBANDS;
    sep.u.codec_sbc.allocation_method_mask = SBC_ALLOCATION_METHOD;
    sep.u.codec_sbc.min_bitpool = SBC_MIN_BITPOOL;
    sep.u.codec_sbc.max_bitpool = SBC_MAX_BITPOOL;

    bt_a2dp_stream_endpoint_add(sep);

    sep.codec_type = BT_A2DP_CODEC_TYPE_AAC;
    sep.u.codec_aac.object_type_mask = BT_A2DP_AAC_OBJECT_TYPE_MPEG_2_AAC_LC;
    sep.u.codec_aac.sampling_frequency_mask = BT_A2DP_AAC_SAMPLING_FREQUENCY_44_1KHZ |
                                              BT_A2DP_AAC_SAMPLING_FREQUENCY_48KHZ;
    sep.u.codec_aac.channel_number_mask = BT_A2DP_AAC_CHANNEL_NUMBER_1 | BT_A2DP_AAC_CHANNEL_NUMBER_2;
    sep.u.codec_aac.vbr_supported = true;
    sep.u.codec_aac.bit_rate = 0;

    bt_a2dp_stream_endpoint_add(sep);

    bt_mgr_cback_register(app_a2dp_bt_cback);
}




