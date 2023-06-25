/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2022-09-06     howie  first version
 */

#include "board.h"
#include "stdlib.h"
#include "string.h"
#include "os_sched.h"
#include "os_timer.h"
#include "audio.h"
#include "ringtone.h"
#include "trace.h"
#include <app_msg.h>
#include <watch_msg.h>
#include "app_audio_fs_interface.h"
#include "app_audio_if.h"
#if (RTK_BR_TASK == 1)
#include "dp_br_info.h"
#endif

T_APP_AUDIO_ERROR app_audio_play_by_name(uint8_t *file_name, uint16_t length)
{

    if ((playback_db.sd_play_state == APP_AUDIO_STATE_PAUSE) ||
        (playback_db.sd_play_state == APP_AUDIO_STATE_STOP))
    {
        if (g_curr_song != NULL)
        {
            Mp3_FreeHandle(g_curr_song);
            g_curr_song = NULL;
        }

        EMp3Res mp3_res;
        g_curr_song = Mp3_CreateHandle((char *)file_name, &mp3_res);
        if (g_curr_song == NULL || mp3_res != MP3RES_OK)
        {
            APP_PRINT_INFO0("Play failed! May be not a mp3 file.");
            //play_next_music();
            return APP_AUDIO_OPEN_FILE_ERROR;
        }

        uint16_t res = app_audio_start();
        if (res == APP_AUDIO_SUCCESS)
        {
            playback_db.file_name = file_name;
            playback_db.name_length = length;
        }
        return (T_APP_AUDIO_ERROR)res;
    }
    else if (playback_db.sd_play_state == APP_AUDIO_STATE_PLAY)
    {
        playback_db.op_next_action = APP_AUDIO_STOPPED_SWITCH_BY_NAME;
        playback_db.file_name = file_name;
        playback_db.name_length = length;
        uint16_t res = app_audio_pause();

        if (res != APP_AUDIO_SUCCESS)
        {
            playback_db.op_next_action = APP_AUDIO_STOPPED_IDLE_ACTION;
        }
        return (T_APP_AUDIO_ERROR)res;
    }
    else  // Ignore the command because other cammand is on going
    {
        return APP_AUDIO_MODE_ERROR;
    }
}

T_APP_AUDIO_STATE app_audio_get_play_status(void)
{
    return playback_db.sd_play_state;
}

void app_volume_up(void)
{
    app_playback_volume_up();
}

void app_volume_down(void)
{
    app_playback_volume_down();
}

uint8_t app_audio_get_volume(void)
{
    return playback_db.volume;
}

uint16_t app_audio_start(void)
{
    uint16_t res = app_playback_start();
    return res;
}

uint16_t app_audio_pause(void)
{
    uint16_t res = app_playback_stop();
    return res;
}

uint16_t app_audio_stop(void)
{
    uint16_t res = APP_AUDIO_SUCCESS;

    if (app_audio_get_play_status() == APP_AUDIO_STATE_PLAY)
    {
        res = app_audio_pause();
    }
    res = app_playback_close_flie();
    playback_db.sd_play_state = APP_AUDIO_STATE_STOP;

    return res;
}

uint16_t app_audio_next(void)
{
    uint16_t res = APP_AUDIO_SUCCESS;
    //play_next_music();
    return res;
}

uint16_t app_audio_prev(void)
{
    uint16_t res = APP_AUDIO_SUCCESS;
    //play_prev_music();
    return res;
}

void app_audio_mode_switch(uint8_t new_mode)
{
    if (app_audio_get_play_status() == APP_AUDIO_STATE_PLAY)
    {
        APP_PRINT_INFO0("app_audio_mode_switch stop playing first");

        if (new_mode == MODE_APP_PLAYBACK)
        {
            playback_db.op_next_action = APP_AUDIO_STOPPED_SWITCH_LOCAL_PLAY;
        }
        app_audio_pause();

        return;
    }

    app_playback_mode_init();

    //gui_update_by_event((UI_MenuTypeDef *)&Playmode, NULL, false);
}

void app_audio_interface_init(void)
{
//    if (!app_audio_fs_interface_init())
//    {
//        app_playback_update_file_init();
    app_audio_mode_switch(MODE_APP_PLAYBACK);
//    }
}




