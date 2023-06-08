/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include <string.h>
#include "trace.h"
#include "btm.h"
#include "bt_a2dp.h"
#include "app_cfg.h"
#include "dp_br_info.h"
#include "app_br_link_util.h"
#include "bt_a2dp.h"
#include "audio_track.h"
#include "app_timer.h"
#include "audio_type.h"
#include "app_linkback.h"
#include "bt_task.h"
#include "os_sync.h"
#include "app_mmi.h"
#include "bt_hfp.h"
#include "bt_bond.h"
#include "os_timer.h"
#include "hw_tim.h"
#include "bt_types.h"
#include "app_sdp.h"
#include "app_a2dp_enc.h"
#include "audio_pipe.h"
#include "board.h"
#include "app_audio_if.h"
#include "app_audio_policy.h"
#include "watch_msg.h"

#define A2DP_LATENCY_MS                     280
#define A2DP_LINK_NUM                      1
#define SBC_CHANNEL_MODE                   (BT_A2DP_SBC_CHANNEL_MODE_MONO|BT_A2DP_SBC_CHANNEL_MODE_DUAL_CHANNEL|BT_A2DP_SBC_CHANNEL_MODE_STEREO|BT_A2DP_SBC_CHANNEL_MODE_JOINT_STEREO)
#define SBC_BLOCK_LEN                      (BT_A2DP_SBC_BLOCK_LENGTH_4|BT_A2DP_SBC_BLOCK_LENGTH_8|BT_A2DP_SBC_BLOCK_LENGTH_12|BT_A2DP_SBC_BLOCK_LENGTH_16)
#define SBC_SUBBANDS                       (BT_A2DP_SBC_SUBBANDS_4|BT_A2DP_SBC_SUBBANDS_8)
#define SBC_ALLOCATION_METHOD              (BT_A2DP_SBC_ALLOCATION_METHOD_SNR|BT_A2DP_SBC_ALLOCATION_METHOD_LOUDNESS)
#define SBC_MIN_BITPOOL                       2
#define SBC_MAX_BITPOOL                       35

#define A2DP_SRC_DEC_DATA_TX_PKT_NUM    4
#define A2DP_SRC_ENC_DATA_RX_PKT_NUM    6

extern T_OS_QUEUE       a2dp_enc_queue;
extern FRAME_INFO       *g_frame_info;
extern FRAME_CONTENT    *g_playback_frame;

void *p_snk_data_buf = NULL;
static T_AUDIO_PIPE_HANDLE audio_pipe_handle = NULL;
static uint8_t SRC_A2DP_STREAM_MAX_CREDITS = 8;
static uint8_t src_a2dp_credits = 8;
static uint8_t active_a2dp_index = 0;
T_HW_TIMER_HANDLE a2dp_timer_handle = NULL;
extern uint8_t default_addr[6];

static const uint16_t a2dp_gain_table[] =
{
    0x8001, 0xeb00, 0xec80, 0xee00, 0xef80, 0xf100, 0xf280, 0xf400,
    0xf580, 0xf700, 0xf880, 0xfa00, 0xfb80, 0xfd00, 0xfe80, 0x0000
};
static uint8_t  cur_pair_idx = 0;
static const uint8_t  min_gain_level = 0;
static const uint8_t  max_gain_level = 15;
static uint8_t  a2dp_src_bitpool = 0x22;
static float    a2dp_src_timer = 21.333;
static float    a2dp_sbc_time = 2.6666;
static bool     dsp_tx_ack = false;


void   *a2dp_open_stream_timer = NULL;
static bool reconnect_cause_no_resource = false;

bool get_reconnect_cause(void)
{
    return reconnect_cause_no_resource;
}

void set_reconnect_cause(bool flag)
{
    reconnect_cause_no_resource = flag;
}

static void app_a2dp_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    T_APP_BR_LINK *p_link;
    bool handle = true;

    switch (event_type)
    {
    case BT_EVENT_A2DP_CONN_IND:
        {
            APP_PRINT_INFO0(" BT_EVENT_A2DP_CONN_IND\n");
            p_link = app_find_br_link(param->a2dp_conn_ind.bd_addr);
            if (p_link == NULL)
            {
                APP_PRINT_ERROR0("app_a2dp_bt_cback: no acl link found");
                break;
            }

            bool is_a2dp_connected = false;
            for (uint8_t i = 0; i < MAX_BR_LINK_NUM; ++i)
            {
                if (br_db.br_link[i].used && (br_db.br_link[i].connected_profile & A2DP_PROFILE_MASK))
                {
                    is_a2dp_connected = true;
                    break;
                }
            }
            if (is_a2dp_connected)
            {
                APP_PRINT_INFO0("A2DP is already connected. Connection is rejected.");
                bt_a2dp_connect_cfm(p_link->bd_addr, false);
            }
            else
            {
                bt_a2dp_connect_cfm(p_link->bd_addr, true);
                memcpy(br_db.acl_info.acl_reconnect_addr, default_addr, 6);
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
            if (br_db.a2dp_info.audio_play_mode == MODE_APP_A2DP_SRC ||
                br_db.a2dp_info.audio_play_mode == MODE_APP_PLAYBACK)
            {
                br_db.a2dp_info.a2dp_src_state = APP_A2DP_SRC_CONN;
                memcpy(br_db.a2dp_info.a2dp_sink_addr, param->a2dp_conn_cmpl.bd_addr, 6);
                os_timer_restart(&a2dp_open_stream_timer, 1000);

                src_a2dp_credits = SRC_A2DP_STREAM_MAX_CREDITS;
                APP_PRINT_INFO1("reset a2dp credits = %d", src_a2dp_credits);
            }
        }
        break;

    case BT_EVENT_A2DP_STREAM_OPEN:
        {
            APP_PRINT_INFO0(" BT_EVENT_A2DP_STREAM_OPEN\n");
            if (br_db.a2dp_info.audio_play_mode == MODE_APP_A2DP_SRC)
            {
                os_timer_stop(&a2dp_open_stream_timer);
                br_db.a2dp_info.a2dp_src_state = APP_A2DP_SRC_STREAM_CONN;
                memcpy(br_db.a2dp_info.a2dp_sink_addr, param->a2dp_stream_open.bd_addr, 6);
                if (bt_bond_index_get(br_db.a2dp_info.a2dp_sink_addr, &cur_pair_idx) == false)
                {
                    APP_PRINT_ERROR0("A2dp stream open, get pair idx fail");
                }
                APP_PRINT_INFO2("pair idx = %d, vol level = %d", cur_pair_idx,
                                app_cfg_nv.audio_gain_level[cur_pair_idx]);
            }
        }
        break;

    case BT_EVENT_A2DP_STREAM_START_IND:
        {
            APP_PRINT_INFO0(" BT_EVENT_A2DP_STREAM_START_IND\n");
            if (br_db.a2dp_info.audio_play_mode == MODE_APP_A2DP_SRC)
            {
                bt_a2dp_stream_start_cfm(param->a2dp_stream_start_ind.bd_addr, true);
                br_db.a2dp_info.a2dp_src_state = APP_A2DP_SRC_STREAM_START;
                bt_sniff_mode_disable(br_db.a2dp_info.a2dp_sink_addr);
                bt_link_qos_set(br_db.a2dp_info.a2dp_sink_addr, BT_QOS_TYPE_GUARANTEED, 18);
                bt_a2dp_stream_suspend_req(param->a2dp_stream_start_ind.bd_addr);
            }
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
            if (br_db.a2dp_info.audio_play_mode == MODE_APP_A2DP_SRC)
            {
                br_db.a2dp_info.a2dp_src_state = APP_A2DP_SRC_STREAM_START;
                bt_sniff_mode_disable(br_db.a2dp_info.a2dp_sink_addr);
                bt_link_qos_set(br_db.a2dp_info.a2dp_sink_addr, BT_QOS_TYPE_GUARANTEED, 18);
//20230607      app_a2dp_src_start();
            }
        }
        break;

    case BT_EVENT_A2DP_STREAM_STOP:
        {
            APP_PRINT_INFO0(" BT_EVENT_A2DP_STREAM_STOP\n");
            if (br_db.a2dp_info.audio_play_mode == MODE_APP_A2DP_SRC)
            {
                br_db.a2dp_info.a2dp_src_state = APP_A2DP_SRC_STREAM_STOP;
                bt_sniff_mode_enable(br_db.a2dp_info.a2dp_sink_addr, 500);
                bt_link_qos_set(br_db.a2dp_info.a2dp_sink_addr, BT_QOS_TYPE_GUARANTEED, 38);
            }
        }
        break;

    case BT_EVENT_A2DP_STREAM_DATA_RSP:
        {
            APP_PRINT_INFO0(" BT_EVENT_A2DP_STREAM_DATA_RSP\n");
            if (br_db.a2dp_info.audio_play_mode == MODE_APP_A2DP_SRC)
            {
                src_a2dp_credits++;
                if (src_a2dp_credits > SRC_A2DP_STREAM_MAX_CREDITS)
                {
                    src_a2dp_credits = SRC_A2DP_STREAM_MAX_CREDITS;
                }
                APP_PRINT_INFO1("get a2dp credits = %d", src_a2dp_credits);
            }
        }
        break;

    case BT_EVENT_A2DP_STREAM_OPEN_FAIL:
        {
            APP_PRINT_INFO0(" BT_EVENT_A2DP_STREAM_OPEN_FAIL\n");
            if (br_db.a2dp_info.audio_play_mode == MODE_APP_A2DP_SRC)
            {
                p_link = app_find_br_link(param->a2dp_stream_open_fail.bd_addr);
                APP_PRINT_INFO1("a2dp_stream_open_fail.cause = %d", param->a2dp_stream_open_fail.cause);
                if (p_link != NULL)
                {
                    bt_a2dp_disconnect_req(param->a2dp_stream_open_fail.bd_addr);//linkback_profile_disconnect_start
                    if (param->a2dp_stream_open_fail.cause == (L2C_ERR | L2C_CONN_RSP_NO_RESOURCE))
                    {
                        set_reconnect_cause(true);
                    }
                }
            }
        }
        break;

    case BT_EVENT_A2DP_STREAM_CLOSE:
        {
            APP_PRINT_INFO0(" BT_EVENT_A2DP_STREAM_CLOSE\n");
            if (br_db.a2dp_info.audio_play_mode == MODE_APP_A2DP_SRC)
            {
                bt_link_qos_set(br_db.a2dp_info.a2dp_sink_addr, BT_QOS_TYPE_GUARANTEED, 38);
            }
        }
        break;
    case BT_EVENT_A2DP_DISCONN_CMPL:
        {
            APP_PRINT_INFO0(" BT_EVENT_A2DP_DISCONN_CMPL\n");
            if (br_db.a2dp_info.audio_play_mode == MODE_APP_A2DP_SRC)
            {
                br_db.a2dp_info.a2dp_src_state = APP_A2DP_SRC_DISCONN;
            }
        }
        break;

    default:
        {
            handle = false;
        }
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_a2dp_bt_cback: event_type 0x%04x", event_type);
    }
}

void app_set_active_a2dp_idx(uint8_t idx)
{
    active_a2dp_index = idx;
}

uint8_t app_get_active_a2dp_idx(void)
{
    return active_a2dp_index;
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

    /*20230607
        if (app_cfg_const.supported_profile_mask & A2DP_PROFILE_MASK)
        {
            bt_a2dp_init(app_cfg_const.a2dp_link_number, A2DP_LATENCY_MS);
            br_db.a2dp_cur_role = BT_A2DP_ROLE_SRC;
            bt_a2dp_role_set(BT_A2DP_ROLE_SRC);
            if (app_cfg_const.a2dp_codec_type_sbc)
            {
                T_BT_A2DP_STREAM_ENDPOINT sep;

                sep.codec_type = BT_A2DP_CODEC_TYPE_SBC;
                sep.u.codec_sbc.sampling_frequency_mask = BT_A2DP_SBC_SAMPLING_FREQUENCY_48KHZ;
                sep.u.codec_sbc.channel_mode_mask = app_cfg_const.sbc_channel_mode;
                sep.u.codec_sbc.block_length_mask = app_cfg_const.sbc_block_length;
                sep.u.codec_sbc.subbands_mask = app_cfg_const.sbc_subbands;
                sep.u.codec_sbc.allocation_method_mask = app_cfg_const.sbc_allocation_method;
                sep.u.codec_sbc.min_bitpool = app_cfg_const.sbc_min_bitpool;
                sep.u.codec_sbc.max_bitpool = app_cfg_const.sbc_max_bitpool;
                bt_a2dp_stream_endpoint_add(sep);
            }

            if (app_cfg_const.a2dp_codec_type_aac)
            {
                T_BT_A2DP_STREAM_ENDPOINT sep;

                sep.codec_type = BT_A2DP_CODEC_TYPE_AAC;
                sep.u.codec_aac.object_type_mask = app_cfg_const.aac_object_type;
                sep.u.codec_aac.sampling_frequency_mask = app_cfg_const.aac_sampling_frequency;
                sep.u.codec_aac.channel_number_mask = app_cfg_const.aac_channel_number;
                sep.u.codec_aac.vbr_supported = app_cfg_const.aac_vbr_supported;
                sep.u.codec_aac.bit_rate = app_cfg_const.aac_bit_rate;
                bt_a2dp_stream_endpoint_add(sep);
            }

            bt_mgr_cback_register(app_a2dp_bt_cback);
        }*/
}



