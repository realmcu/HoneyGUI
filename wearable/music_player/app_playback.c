/**
*********************************************************************************************************
*               Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     app_playback.c
* @brief    This file provides playback application code.
* @details
* @author   lemon hua
* @date     2020-3-20
* @version  v1.0
*********************************************************************************************************
*/
/* Includes -----------------------------------------------------------------*/
#include "trace.h"
#include "os_mem.h"
#include "os_msg.h"
#include "os_task.h"
#include "audio_track.h"
#include "audio_type.h"
#include "btm.h"
#include "bt_avrcp.h"
#include "app_cfg.h"
#include "app_timer.h"
#include "audio.h"
#include "app_playback.h"
#include "app_audio_if.h"
#include "app_eq.h"
#include "os_timer.h"

/* Defines ------------------------------------------------------------------*/
#define PLAYBACK_FRAMES_NUM_2           5   //(PLAYBACK_FRAMES_NUM_2 + PLAYBACK_LOW_LEVEL) < LATENCY pkts
#define PLAYBACK_LOW_LEVEL              60  //ms
/******PLAYBACK_UPPER_LEVEL***************/
//max bit rate 320kbps = 40kB/s
//playback pool size time = playback pool size / 40960 * 1000  ms
//if playback pool size = 10240Byte, time = 250ms, set 240ms for safe
/******PLAYBACK_UPPER_LEVEL***************/
#define PLAYBACK_UPPER_LEVEL            240 //ms
#define PLAYBACK_LATENCY                200 //ms

#define PLAYBACK_PUT_DATA_PKT_NUM                   4
// #define PLAYBACK_PUT_DATA_TIME_MS                   (PLAYBACK_PUT_DATA_PKT_NUM * 20)
//#define PLAYBACK_SINGLE_PREQ_PKTS(sf)           sf * (PLAYBACK_LATENCY + PLAYBACK_LOW_LEVEL) / 1000 / 1024 + 3
#define PLAYBACK_INTERVAL_TIMER_MS(pkt_num, sf)     pkt_num * 1000 * 1024 / sf
/* Globals ------------------------------------------------------------------*/
T_APP_AUDIO_FS_DATA     playback_db;
T_AUDIO_TRACK_HANDLE    playback_track_handle;
Mp3Hdl_t                g_curr_song;


static uint16_t g_playback_put_data_time_ms = 0;
static uint16_t g_playback_single_preq_pkts = 0;

typedef enum
{
    APP_TIMER_PLAYBACK_PUT_DATA,
} T_APP_PLAYBACK_TIMER;

static uint8_t timer_idx_playback_put_data = 0;
static uint8_t app_playback_time_id = 0;

static void app_playback_put_data_stop_timer(uint8_t pkt_num);
static void app_playback_put_data_start_timer(uint16_t time_ms);
static void *mp3_loop_timer = NULL;


/**
  * @brief Initialize FATFS file system and sd card physical layer.
  * @param   No parameter.
  * @return  void
  */
uint8_t app_playback_parameter_recfg(void)
{
    ASSERT(g_curr_song != NULL);

    T_AUDIO_FORMAT_INFO format_info;
    format_info.type = AUDIO_FORMAT_TYPE_MP3;
    format_info.attr.mp3.sample_rate = Mp3_GetSamplingFrequency_Hz(g_curr_song);
    format_info.attr.mp3.bitrate = Mp3_GetBitRate_kbps(g_curr_song);
    format_info.attr.mp3.layer = Mp3_GetLayer(g_curr_song);
    format_info.attr.mp3.version = Mp3_GetVersion(g_curr_song);
    switch (Mp3_GetChannelMode(g_curr_song))
    {
    case CHANNEL_STEREO:
    case CHANNEL_JOINT_STEREO:
    case CHANNEL_DOUBLE:
        format_info.attr.mp3.chann_mode = AUDIO_MP3_CHANNEL_MODE_DUAL;  // Stereo
        break;
    case CHANNEL_SINGLE:
        format_info.attr.mp3.chann_mode = AUDIO_MP3_CHANNEL_MODE_MONO;  // Mono
        break;
    default:
        APP_PRINT_ERROR0("a2dp_src_get_header_info_from_fs: channel not supprot");
        break;
    }

    APP_PRINT_TRACE5("app_playback_parameter_recfg: MP3, samplefrequency:%d, channel_mode:%d, "
                     "bitrate:%d, version: %d, layer:%d",
                     format_info.attr.mp3.sample_rate, format_info.attr.mp3.chann_mode,
                     format_info.attr.mp3.bitrate, format_info.attr.mp3.version,
                     format_info.attr.mp3.layer);

    g_playback_put_data_time_ms = PLAYBACK_PUT_DATA_PKT_NUM * Mp3_GetTimePerFrame_ms(g_curr_song);
    g_playback_single_preq_pkts = Mp3_GetSamplingFrequency_Hz(g_curr_song) *
                                  (PLAYBACK_LATENCY + PLAYBACK_LOW_LEVEL)
                                  / 1000 / Mp3_GetSamplePerFrame(g_curr_song);
    APP_PRINT_TRACE2("app_playback_parameter_recfg: g_playback_put_data_time_ms: %d, g_playback_single_preq_pkts: %d",
                     g_playback_put_data_time_ms, g_playback_single_preq_pkts);
    playback_track_handle = audio_track_create(AUDIO_STREAM_TYPE_PLAYBACK, //stream_type
                                               AUDIO_STREAM_MODE_NORMAL, // mode
                                               AUDIO_STREAM_USAGE_SNOOP, // usage
                                               format_info, //format_info
                                               playback_db.volume, //volume
                                               0,
                                               AUDIO_DEVICE_OUT_SPK, // device
                                               NULL,
                                               NULL);
    app_playback_get_audio_track_state_handle();

    if (playback_track_handle != NULL)
    {
        if (format_info.attr.mp3.sample_rate == 44100 && g_eq_44p1 != NULL)
        {
            //audio_track_effect_attach(playback_track_handle, g_eq_44p1);
        }
        else if (format_info.attr.mp3.sample_rate == 48000 && g_eq_48 != NULL)
        {
            //audio_track_effect_attach(playback_track_handle, g_eq_48);
        }
        audio_track_latency_set(playback_track_handle, PLAYBACK_LATENCY, true);
        audio_track_threshold_set(playback_track_handle, PLAYBACK_UPPER_LEVEL, PLAYBACK_LOW_LEVEL);
    }

    return APP_AUDIO_SUCCESS;
}

void app_playback_mode_init(void)
{
    playback_db.op_next_action = APP_AUDIO_STOPPED_IDLE_ACTION;
}

/**
  * @brief playback start function
  * @param   No parameter.
  * @return  void
  */
uint8_t app_playback_start(void)
{
    uint8_t res = APP_AUDIO_SUCCESS;

    APP_PRINT_TRACE0("app_playback_start ++");

    if ((res = app_playback_parameter_recfg()) != 0)
    {
        return res;
    }
    app_playback_volume_set();
    playback_db.sd_play_state = APP_AUDIO_STATE_PLAY;
    playback_db.op_next_action = APP_AUDIO_STOPPED_IDLE_ACTION;
    playback_db.seq_num = 0;
    playback_db.track_state = PLAYBACK_TRACK_STATE_CLEAR;
    playback_db.local_buffer_state = APP_AUDIO_FS_BUF_NORMAL;
    audio_track_start(playback_track_handle);
    //gui_update_by_event((UI_MenuTypeDef *)&Player, NULL, false);

    return res;
}

void app_playback_volume_up(void)
{
    if (playback_db.volume < app_cfg_volume.playback_volume_max)
    {
        playback_db.volume++;
    }
    else
    {
        playback_db.volume = app_cfg_volume.playback_volume_max;
    }

    APP_PRINT_TRACE2("app_playback_volume_up,volume:%d,max:%d", playback_db.volume,
                     app_cfg_volume.playback_volume_max);

    if (playback_track_handle != NULL)
    {
        audio_track_volume_out_set(playback_track_handle, playback_db.volume);
    }

    //gui_update_by_event((UI_MenuTypeDef *)&Player, NULL, false);
}

void app_playback_volume_down(void)
{
    if (playback_db.volume > app_cfg_volume.playback_volume_min)
    {
        playback_db.volume--;
    }
    else
    {
        playback_db.volume = app_cfg_volume.playback_volume_min;
    }

    APP_PRINT_TRACE2("app_playback_volume_down,volume:%d,min:%d", playback_db.volume,
                     app_cfg_volume.playback_volume_min);

    if (playback_track_handle != NULL)
    {
        audio_track_volume_out_set(playback_track_handle, playback_db.volume);
    }

    //gui_update_by_event((UI_MenuTypeDef *)&Player, NULL, false);
}

void app_playback_volume_set(void)
{
    APP_PRINT_TRACE1("app_playback_volume_set vol:%d", playback_db.volume);
    audio_track_volume_out_set(playback_track_handle, playback_db.volume);
}

/**
  * @brief playback stop function
  * @param   No parameter.
  * @return  void
  */
uint8_t app_playback_stop(void)
{
    uint8_t res = APP_AUDIO_SUCCESS;

    app_stop_timer(&timer_idx_playback_put_data);
    if (playback_db.sd_play_state == APP_AUDIO_STATE_PLAY)
    {
        playback_db.sd_play_state = APP_AUDIO_STATE_PAUSE;
    }

    if (playback_track_handle != NULL)
    {
        playback_db.track_state = PLAYBACK_TRACK_STATE_CLEAR;
        audio_track_release(playback_track_handle);
//        playback_track_handle = NULL;
    }

    //gui_update_by_event((UI_MenuTypeDef *)&Player, NULL, false);

    return res;
}

uint8_t app_playback_close_flie(void)
{
    if (g_curr_song != NULL)
    {
        Mp3_FreeHandle(g_curr_song);
        g_curr_song = NULL;

        //reset_play_music_num();
        //gui_update_by_event((UI_MenuTypeDef *)&Player, NULL, false);
        return APP_AUDIO_SUCCESS;
    }
    else
    {
        return APP_AUDIO_CLOSE_FILE_ERROR;
    }
}

void mp3_loop_timer_cb()
{
    audio_test_start_mp3();
}

void mp3_loop_timer_init()
{
    os_timer_create(&mp3_loop_timer, "mp3_loop_timer", 0, 1000, false, mp3_loop_timer_cb);
}

void app_playback_put_data(uint8_t pkt_num)
{
    uint16_t buf_level_ms;
    if (audio_track_buffer_level_get(playback_track_handle, &buf_level_ms))
    {

        APP_PRINT_TRACE2("app_playback_put_data: pkt_num: %d, buffer_level = %d ms",
                         pkt_num, buf_level_ms);
        // buf_level_after_write >= PLAYBACK_UPPER_LEVEL
        while (buf_level_ms + g_playback_put_data_time_ms / PLAYBACK_PUT_DATA_PKT_NUM * pkt_num
               >= PLAYBACK_UPPER_LEVEL)
        {
            --pkt_num;
            APP_PRINT_TRACE1("app_playback_put_data: buffer level high, pkt_num = %d", pkt_num);
        }
    }

    for (uint8_t i = 0; i < pkt_num; ++i)
    {
        uint8_t *pbuf; uint32_t buf_len; float pos_ms;
        EMp3Res res = Mp3_ReadNextFrame(g_curr_song, &pbuf, &buf_len, &pos_ms);
        if (res != MP3RES_OK) // MP3RES_FILE_ENDS
        {
            APP_PRINT_ERROR1("app_playback_put_data ERROR,RES:0x%x", res);
            app_stop_timer(&app_playback_time_id);
            APP_PRINT_WARN0("app_playback_buffer_low_handle,file end, and paly next song!!!");
            app_playback_stop();
            os_timer_start(&mp3_loop_timer);
            playback_db.op_next_action = APP_AUDIO_STOPPED_FILE_END_TO_NEXT_ACTION;
            return;
        }

        uint16_t written_len;
        playback_db.seq_num++;
        if (audio_track_write(playback_track_handle,
                              0, // timestamp,
                              playback_db.seq_num,
                              AUDIO_STREAM_STATUS_CORRECT,
                              1, // frame_num,
                              pbuf,
                              buf_len,
                              &written_len) == false)
        {
            break;
        }
    }

    if (playback_db.sd_play_state == APP_AUDIO_STATE_PLAY)
    {
        // This maybe AUDIO_EVENT_TRACK_BUFFER_HIGH event
        uint16_t time_ms = (playback_db.local_buffer_state == APP_AUDIO_FS_BUF_HIGH) ?
                           g_playback_put_data_time_ms * 2 : g_playback_put_data_time_ms;
        APP_PRINT_INFO2("time_ms = %d, g_playback_put_data_time_ms = %d", time_ms,
                        g_playback_put_data_time_ms);
        app_playback_put_data_start_timer(time_ms);
    }
    playback_db.local_buffer_state = APP_AUDIO_FS_BUF_NORMAL;
}


void app_playback_get_audio_track_state_handle(void)
{
    audio_track_state_get(playback_track_handle, (T_AUDIO_TRACK_STATE *)&playback_db.track_state);
    APP_PRINT_TRACE1("app_playback_get_audio_track_state_handle, track_state:%d",
                     playback_db.track_state);
}

static void app_playback_track_state_changed_handle(void)
{
    APP_PRINT_TRACE3("app_playback_track_state_changed_handle, track_state:%d, bud_role:%d, connect:%d",
                     playback_db.track_state, app_cfg_nv.bud_role, remote_session_state_get());
    if (AUDIO_TRACK_STATE_STARTED == playback_db.track_state)
    {
        app_playback_put_data(g_playback_single_preq_pkts);
    }
    else if (AUDIO_TRACK_STATE_RELEASED == playback_db.track_state)
    {
        playback_track_handle = NULL;
        if (playback_db.op_next_action == APP_AUDIO_STOPPED_SWITCH_BY_NAME)
        {
            playback_db.op_next_action = APP_AUDIO_STOPPED_IDLE_ACTION;
            app_audio_play_by_name(playback_db.file_name, playback_db.name_length);
        }
        else if (playback_db.op_next_action == APP_AUDIO_STOPPED_FILE_END_TO_NEXT_ACTION)
        {
            playback_db.op_next_action = APP_AUDIO_STOPPED_IDLE_ACTION;
            //play_next_music();
        }
    }
}

static void app_playback_buffer_low_handle(void)
{
    playback_db.local_buffer_state = APP_AUDIO_FS_BUF_LOW;
    app_playback_put_data_stop_timer(PLAYBACK_PUT_DATA_PKT_NUM + 2);

}

static void app_playback_buffer_high_handle(void)
{
//    app_stop_timer(&timer_idx_playback_put_data);
    playback_db.local_buffer_state = APP_AUDIO_FS_BUF_HIGH;
}


static void app_playback_audio_policy_cback(T_AUDIO_EVENT event_type, void *event_buf,
                                            uint16_t buf_len)
{
    T_AUDIO_EVENT_PARAM *param = event_buf;
    bool handle = true;

    if (param->track_state_changed.handle != playback_track_handle)
    {
        return;
    }

    switch (event_type)
    {
    case AUDIO_EVENT_TRACK_STATE_CHANGED:
        {
            playback_db.track_state = param->track_state_changed.state;
            app_playback_get_audio_track_state_handle();
            app_playback_track_state_changed_handle();
        }
        break;

    case AUDIO_EVENT_TRACK_BUFFER_LOW:
        {
            if (playback_db.sd_play_state == APP_AUDIO_STATE_PLAY)
            {
                app_playback_buffer_low_handle();
            }
        }
        break;

    case AUDIO_EVENT_TRACK_BUFFER_HIGH:
        {
            if (playback_db.sd_play_state == APP_AUDIO_STATE_PLAY)
            {
                app_playback_buffer_high_handle();
            }
        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_TRACE1("app_playback_audio_policy_cback: event_type 0x%04x", event_type);
    }
}

//////////////////////////////////////////////////////////////////////////////////////

static void app_playback_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    switch (timer_evt)
    {
    case APP_TIMER_PLAYBACK_PUT_DATA:
        {
            app_playback_put_data_stop_timer(PLAYBACK_PUT_DATA_PKT_NUM);
        }
        break;

    default:
        break;
    }
}

static void app_playback_put_data_stop_timer(uint8_t pkt_num)
{
    APP_PRINT_TRACE0("app_playback_put_data_stop_timer");

    app_stop_timer(&timer_idx_playback_put_data);
    if (playback_db.sd_play_state != APP_AUDIO_STATE_PLAY)
    {
        return;
    }
    app_playback_put_data(pkt_num);
}

static void app_playback_put_data_start_timer(uint16_t time_ms)
{
    app_start_timer(&timer_idx_playback_put_data, "playback_put_data",
                    app_playback_time_id, APP_TIMER_PLAYBACK_PUT_DATA, 0, false,
                    time_ms);
}

void app_playback_init(void)
{
    memset(&playback_db, 0, sizeof(T_APP_AUDIO_FS_DATA));
    playback_db.volume = 8;
    audio_mgr_cback_register(app_playback_audio_policy_cback);
    mp3_loop_timer_init();
    app_timer_reg_cb(app_playback_timeout_cb, &app_playback_time_id);
}
/******************* (C) COPYRIGHT 2019 Realtek Semiconductor Corporation *****END OF FILE****/
