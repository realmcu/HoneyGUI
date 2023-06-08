/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_MMI_H_
#define _APP_MMI_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup APP_MMI App MMI
  * @brief App MMI
  * @{
  */

/**  @brief  action ID parameter for @ref app_mmi_handle_action */
typedef enum
{
    MMI_NULL = 0x00,

    //HFP MMI
    MMI_HF_END_OUTGOING_CALL = 0x02,
    MMI_HF_ANSWER_CALL = 0x03,
    MMI_HF_REJECT_CALL = 0x04,
    MMI_HF_END_ACTIVE_CALL = 0x05,

    MMI_HF_INITIATE_VOICE_DIAL = 0x09,
    MMI_HF_CANCEL_VOICE_DIAL = 0x0A,
    MMI_HF_LAST_NUMBER_REDIAL = 0x0B,
    MMI_HF_SWITCH_TO_SECOND_CALL = 0x0C,
    MMI_HF_TRANSFER_CALL = 0X0D,
    MMI_HF_QUERY_CURRENT_CALL_LIST = 0x0E,
    MMI_HF_JOIN_TWO_CALLS = 0x0F,
    MMI_HF_RELEASE_HELD_OR_WAITING_CALL = 0x10,
    MMI_HF_RELEASE_ACTIVE_CALL_ACCEPT_HELD_OR_WAITING_CALL = 0x11,

    //AV MMI
    MMI_AV_PLAY_PAUSE = 0x32,
    MMI_AV_STOP = 0x33,
    MMI_AV_FWD = 0x34,
    MMI_AV_BWD = 0x35,
    MMI_AV_FASTFORWARD = 0x36,
    MMI_AV_REWIND = 0x37,
    MMI_AV_FASTFORWARD_STOP = 0x3B,
    MMI_AV_REWIND_STOP = 0x3C,

    //DEV MMI
    MMI_DEV_MIC_MUTE_UNMUTE = 0X06, //From av or hf mmi,keep it's original value
    MMI_DEV_MIC_MUTE = 0x07,
    MMI_DEV_MIC_UNMUTE = 0x08,
    MMI_DEV_MIC_VOL_UP = 0x15,
    MMI_DEV_MIC_VOL_DOWN = 0x16,
    MMI_DEV_SPK_VOL_UP = 0x30,
    MMI_DEV_SPK_VOL_DOWN = 0x31,
    MMI_DEV_SPK_MUTE = 0x3D,
    MMI_DEV_SPK_UNMUTE = 0X3E,

    MMI_DEV_ENTER_PAIRING_MODE = 0x51,
    MMI_DEV_EXIT_PAIRING_MODE = 0x52,
    MMI_DEV_LINK_LAST_DEVICE = 0x53,

    MMI_DEV_POWER_ON = 0x54,
    MMI_DEV_POWER_OFF = 0x56,

    MMI_DEV_FACTORY_RESET = 0x58,
    MMI_DEV_DISCONNECT_ALL_LINK = 0x59,
    MMI_DEV_NFC_DETECT = 0x5a,
    MMI_DEV_APP_SYNC_ACTION = 0x5b,

    MMI_AUDIO_EQ_NEXT = 0x61,
    MMI_AUDIO_EQ_PREVIOUS = 0x62,
    MMI_AUDIO_APT = 0x65,
    MMI_DEV_NEXT_VOICE_PROMPT_LANGUAGE = 0x66,

    MMI_OUTPUT_INDICATION1_TOGGLE = 0x67,
    MMI_OUTPUT_INDICATION2_TOGGLE = 0x68,
    MMI_OUTPUT_INDICATION3_TOGGLE = 0x69,

    MMI_AUDIO_EQ_SWITCH = 0x6B,
    MMI_DEV_FORCE_ENTER_PAIRING_MODE = 0x6C,
    MMI_APT_EQ_SWITCH = 0x6E,

    MMI_AUDIO_SWITCH_CHANNEL = 0X74,

    MMI_DEV_GAMING_MODE_SWITCH = 0x80,

    MMI_AUDIO_ANC_ON = 0x81,
    MMI_AUDIO_APT_ON = 0x82,
    MMI_AUDIO_ANC_APT_ALL_OFF = 0x83,

    MMI_AUDIO_VAD_OPEN = 0x87,
    MMI_AUDIO_VAD_CLOSE = 0x88,

    MMI_AUDIO_APT_VOL_UP = 0x92,
    MMI_AUDIO_APT_VOL_DOWN = 0x93,
    MMI_AUDIO_APT_VOICE_FOCUS = 0x94,

    MMI_OTA_OVER_BLE_START = 0xA0,
    MMI_DUT_TEST_MODE = 0xA1,
    MMI_ALEXA_WAKEWORD = 0xB0,
    MMI_XIAOAI_WAKEWORD = 0xB2,

    MMI_SD_PLAYBACK_SWITCH              = 0xC0,
    MMI_SD_AUDIO_FS_PLAY_PAUSE          = 0xC1,
    MMI_SD_AUDIO_FS_FWD                 = 0xC2,
    MMI_SD_AUDIO_FS_BWD                 = 0xC3,
    MMI_SD_AUDIO_FS_FWD_PLAYLIST        = 0xC4,
    MMI_SD_AUDIO_FS_BWD_PLAYLIST        = 0xC5,
    MMI_SD_AUDIO_FS_STOP                = 0xC6,
    MMI_SD_A2DP_SRC_SWITCH              = 0xC7,
    MMI_SD_A2DP_SNK_SWITCH              = 0xC8,

    MMI_ANC_ON_OFF = 0xD0,
    MMI_LISTENING_MODE_CYCLE = 0xD1,
    MMI_ANC_CYCLE = 0xD2,

    MMI_RECORD_START = 0xE0,
    MMI_RECORD_STOP  = 0xE1,
    MMI_RECORD_PLAY_START  = 0xE2,
    MMI_RECORD_PLAY_STOP  = 0xE3,

    MMI_START_ROLESWAP = 0xF0,
    MMI_ENTER_SINGLE_MODE = 0xF1,

    MMI_TOTAL
} T_MMI_ACTION;

/**
    * @brief  App need to process key message are handle in this function.
    *         User trigger the specific key process, then bbpro would send le advertising data.
    * @param  action key event
    * @return void
    */
void app_mmi_handle_action(uint8_t action);

/** End of APP_MMI
* @}
*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_MMI_H_ */
