/**
*****************************************************************************************
*     Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file
   * @brief
   * @details
   * @author    carol_shen
   * @date      2021-1-11
   * @version   1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2021 Realtek Semiconductor Corporation</center></h2>
   * *************************************************************************************
  */

/*============================================================================*
  *                   Define to prevent recursive inclusion
  *============================================================================*/
#ifndef _APP_AUDIO_FS_INTERFACE_H_
#define _APP_AUDIO_FS_INTERFACE_H_

/*============================================================================*
  *                               Header Files
  *============================================================================*/
#include <stdbool.h>
#include <stdint.h>
#include "remote.h"
#include "app_playback.h"

#define PLAYLIST_PAUSE_EN   1
#define PLAYLIST_NUM        8
#define FILE_NAME_LEN       (PATH_LEN * sizeof(TCHAR))

typedef enum
{
    APP_AUDIO_STATE_STOP         = 0x00,
    APP_AUDIO_STATE_PLAY         = 0x01,
    APP_AUDIO_STATE_PAUSE        = 0x02,

    // indicate that command is on-going
    APP_AUDIO_STATE_TRY_STOPPING = 0x10,
    APP_AUDIO_STATE_TRY_PLAYING  = 0x11,
    APP_AUDIO_STATE_TRY_PAUSING  = 0x12,
} T_APP_AUDIO_STATE;

typedef enum
{
    APP_AUDIO_SUCCESS                    = 0x00,
    APP_AUDIO_READ_ERROR                 = 0x01,
    APP_AUDIO_IDX_ERROR                  = 0x02,
    APP_AUDIO_OPEN_FILE_ERROR            = 0x03,
    APP_AUDIO_CRC_ERROR                  = 0x04,
    APP_AUDIO_CLOSE_FILE_ERROR           = 0x05,
    APP_AUDIO_PLAYLIST_ERROR             = 0x06,
    APP_AUDIO_WRITE_ERROR                = 0x07,
    APP_AUDIO_TYPE_ERROR                 = 0x08,
    APP_AUDIO_MALLOC_ERROR               = 0x09,
    APP_AUDIO_PIPE_DRAIN_ERROR           = 0x0A,
    APP_AUDIO_PIPE_FILL_ERROR            = 0x0B,
    APP_AUDIO_A2DP_ENC_RX_ERROR          = 0x0C,
    APP_AUDIO_A2DP_ENC_PEEK_ERROR        = 0x0D,
    APP_AUDIO_A2DP_DATA_SEND             = 0x0E,
    APP_AUDIO_MODE_ERROR                 = 0x0F,
} T_APP_AUDIO_ERROR;

typedef enum
{
    APP_AUDIO_STOPPED_IDLE_ACTION                        = 0x00,
    APP_AUDIO_STOPPED_SWITCH_FWD_ACTION                  = 0x01,
    APP_AUDIO_STOPPED_SWITCH_BWD_ACTION                  = 0x02,
    APP_AUDIO_STOPPED_FILE_END_TO_NEXT_ACTION            = 0x03,
    APP_AUDIO_STOPPED_SWITCH_BY_NAME                     = 0x04,
    APP_AUDIO_STOPPED_SWITCH_LOCAL_PLAY                  = 0x05,
    APP_AUDIO_STOPPED_SWITCH_A2DP_SRC                    = 0x06,
    APP_AUDIO_STOPPED_SWITCH_A2DP_SNK                    = 0x07,
} T_APP_AUDIO_STOPPED_NEXT_ACTION;

typedef enum
{
    APP_AUDIO_FS_BUF_NORMAL         = 0x00,
    APP_AUDIO_FS_BUF_LOW            = 0x01,
    APP_AUDIO_FS_BUF_HIGH           = 0x02,
} T_APP_AUDIO_FS_BUF_STATE;

typedef struct
{
    T_APP_AUDIO_STATE           sd_play_state;
    T_APP_AUDIO_FS_BUF_STATE    local_buffer_state;
    T_APP_AUDIO_FS_BUF_STATE    source_buffer_state;
    uint8_t                     op_next_action;
    uint8_t                     track_state;
    uint8_t                     volume;
    uint8_t                     playlist_idx;
    uint16_t                    file_idx;
    uint16_t                    seq_num;
    uint8_t                     *file_name;
    uint16_t                    name_length;
} T_APP_AUDIO_FS_DATA;

extern T_APP_AUDIO_FS_DATA     playback_db;

uint8_t app_audio_fs_interface_init(void);

void app_sd_card_power_down_enable(void);
void app_sd_card_power_down_disable(void);

void app_sd_card_power_down(void);
void app_sd_card_power_on(void);

#endif //_APP_AUDIO_FS_INTERFACE_H_
