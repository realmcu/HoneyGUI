/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_AUDIO_POLICY_H_
#define _APP_AUDIO_POLICY_H_

#include <stdint.h>
#include <stdbool.h>
#include "voice_prompt.h"
#include "audio_volume.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup APP_AUDIO App Audio
  * @brief App Audio
  * @{
  */

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_AUDIO_Exported_Functions App Audio Functions
    * @{
    */

typedef struct
{
    uint8_t     bd_addr[6];
    uint8_t     playback_volume;
    uint8_t     voice_volume;
} T_APP_AUDIO_VOLUME;

typedef enum
{
    AUDIO_COUPLE_SET                    = 0,
    AUDIO_SINGLE_SET                    = 1,
    AUDIO_SPECIFIC_SET                  = 2,
    AUDIO_COUPLE_MIC_SET                = 3,
    AUDIO_SINGLE_MIC_SET                = 4,
    AUDIO_SPECIFIC_MIC_SET              = 5,
} T_AUDIO_CHANCEL_SET;

typedef enum
{
    SCO_STOP_RECOVER_PLAY               = 0,
    SCO_INTERRUPT_A2DP_SOURCE           = 1,
    SCO_INTERRUPT_LOCAL                 = 2,
} T_SCO_INTERRUPT_PLAY_MUSIC;

/**
    * @brief  audio module init
    * @param  void
    * @return void
    */
void app_audio_init(void);

/**
    * @brief  audio set tts path by BR/EDR or BLE link.
    * @param  path: BR/EDR or BLE link
    * @return void
    */
void app_audio_set_tts_path(uint8_t path);

/**
    * @brief  audio check mic mute or not
    * @param  void
    * @return true  mic mute.
    * @return false mic unmute.
    */
uint8_t app_audio_is_mic_mute(void);

/**
    * @brief  audio clear mic mute status
    * @param  void
    * @return void
    */
void app_audio_clear_mic_mute(void);


/**
    * @brief  audio check if mic mute is allowed
    * @param  void
    * @return bool enable mic mute
    */
bool app_audio_check_mic_mute_enable(void);

/**
    * @brief  Play a tone.
    *
    * @param  index  The type of the playing tone.
    * @param  flush     Clear preceding ringtone, VP, and TTS notifications in queue.
    * @param  relay     Relay the tone to other remote identical devices.
    * @return true if is tone was played successfully, otherwise return false.
    */
bool app_audio_tone_play(uint8_t index, bool flush, bool relay);

/**
    * @brief  set mic mute status.
    *
    * @param  status  audio set mic is mute or unmute.
    * @return void.
    */
void app_audio_set_mic_mute_status(uint8_t status);

void app_audio_speaker_channel_set(uint8_t mic_spk_channel);

void app_audio_mic_channel_set(uint8_t mic_channel);

void app_audio_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path, uint8_t app_idx,
                          uint8_t *ack_pkt);

extern bool (*app_audio_notify_check)(void);

void app_a2dp_sink_volume_up(void);

void app_a2dp_sink_volume_down(void);

/** @} */ /* End of group APP_AUDIO_Exported_Functions */

/** End of APP_AUDIO
* @}
*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_AUDIO_POLICY_H_ */
