/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_REPORT_H_
#define _APP_REPORT_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup APP_REPORT App Report
  * @brief App Report
  * @{
  */
//for EVENT_VOLUME_SYNC
#define SYNC_TYPE_HF_GAIN_LEVEL             0x00
#define SYNC_TYPE_AVRCP_UP_DN               0x01
#define SYNC_TYPE_AVRCP_ABSOLUTE_VOL        0x02

//for EVENT_CALLER_ID
#define CALLER_ID_TYPE_NUMBER               0x00
#define CALLER_ID_TYPE_NAME                 0x01

typedef enum
{
    CMD_PATH_NONE = 0x00,
    CMD_PATH_UART = 0x01,
    CMD_PATH_LE   = 0x02,
    CMD_PATH_SPP  = 0x03,
    CMD_PATH_IAP  = 0x04,
    CMD_PATH_RWS  = 0x05,
    CMD_PATH_ONE_WIRE_UART = 0x06,
} T_CMD_PATH;

typedef enum
{
    EVENT_ACK                               = 0x0000,
    EVENT_REPLY_PAIRED_RECORD               = 0x0001,
    EVENT_LEGACY_DATA_TRANSFER              = 0x0002,
    EVENT_CONNECT_STATUS                    = 0x0003,
    EVENT_DISCONNECT_STATUS                 = 0x0004,
    EVENT_CALLER_ID                         = 0x0005,
    EVENT_IAP_STATUS                        = 0x0006,
    EVENT_DEVICE_STATE                      = 0x0007,
    EVENT_REPLY_LINK_INFO                   = 0x0008,
    EVENT_REPLY_REMOTE_NAME                 = 0x0009,
    EVENT_CALL_STATUS                       = 0x000A,
    EVENT_PLAYER_STATUS                     = 0x000B,
    EVENT_FEATURE_REPORT                    = 0x000C,
    EVENT_REPORT_SPP_INFO                   = 0x000D,
    EVENT_TTS                               = 0x000E,
    EVENT_VENDOR_AT_RESULT                  = 0x0010,
    EVENT_INFO_RSP                          = 0x0011,
    EVENT_RESUME_DATA_TRANSFER              = 0x0012,
    EVENT_VOLUME_SYNC                       = 0x0013,
    EVENT_LINK_STATUS                       = 0x0014,
    EVENT_CODEC_SETTING                     = 0x0015,
    EVENT_INITIATE_CONN_DONE                = 0x0016,
    EVENT_LANGUAGE_REPORT                   = 0x0017,
    EVENT_REPORT_CFG_TYPE                   = 0x0018,
    EVENT_GET_BD_ADDR                       = 0x001A,

    EVENT_REPORT_STATUS                     = 0x0019,
    EVENT_AUDIO_VOL_CHANGE                  = 0x0020,
    EVENT_TRACK_CHANGED                     = 0x0021,

    EVENT_LE_CONNECTED                      = 0x0100,
    EVENT_LE_DISCONNECTED                   = 0x0101,
    EVENT_LE_PAIR_STATUS                    = 0x0102,
    EVENT_LE_DATA_TRANSFER                  = 0x0103,
    EVENT_LE_PUBLIC_ADDR                    = 0x0104,
    EVENT_LE_SCAN_INFO                      = 0x0105,

    EVENT_ANCS_REGISTER_COMPLETE            = 0x0110,
    EVENT_ANCS_NOTIFICATION                 = 0x0111,
    EVENT_ANCS_ACTION_RESULT                = 0x0112,

    EVENT_AUDIO_EQ_REPLY                    = 0x0200,
    EVENT_AUDIO_EQ_PARAM_REPORT             = 0x0201,
    EVENT_AUDIO_EQ_INDEX_REPORT             = 0x0202,

    EVENT_DSP_READ_PARAM                    = 0x0203,
    EVENT_DSP_REPORT_AUDIO_EQ               = 0x0204,
#if F_APP_APT_SUPPORT
    EVENT_APT_VOLUME_OUT_LEVEL              = 0x0205,
#endif
    EVENT_APT_EQ_INDEX_INFO                 = 0x0206,

    EVENT_AUDIO_DSP_CTRL_INFO               = 0x0207,
    // for equalize page
    EVENT_AUDIO_EQ_REPLY_PARAM              = 0x0208,
    EVENT_TONE_VOLUME_STATUS                = 0x0209,
    EVENT_TONE_VOLUME_LEVEL_SET             = 0x020A,
#if DATA_CAPTURE_V2_SUPPORT
    EVENT_DSP_CAPTURE_V2_START_STOP_RESULT      = 0x0220,
    EVENT_DSP_CAPTURE_V2_DATA                   = 0x0221,
    EVT_DSP_CAPTURE_SCENARIO_ENTER_EXIT_RESULT  = 0x0222,
#endif
#if (C_APP_HFP_CMD_SUPPORT == 1)
    EVENT_SCO_STATE                         = 0x0300,
    EVENT_CALL_WAITING                      = 0x0301,
    EVENT_HFP_SIGNAL                        = 0x0302,
    EVENT_HFP_ROAM                          = 0x0303,
    EVENT_HFP_SERVICE                       = 0x0304,
    EVENT_NETWORK_OPERATOR                  = 0x0305,
    EVENT_SUBSCRIBER_NUMBER                 = 0x0306,
    EVENT_CURRENT_CALLS                     = 0x0307,
    EVENT_DTMF                              = 0x0308,
    EVENT_REPORT_HFP_BATTERY                = 0x0309,
    EVENT_REPORT_AG_BRSF                    = 0x030A,
#endif

    EVENT_RF_XTAL_K                         = 0x032A,
    EVENT_RF_XTAL_K_GET_RESULT              = 0x032B,

#if (C_APP_AVRCP_CMD_SUPPORT == 1)
    EVENT_AVRCP_REPORT_LIST_SETTING_ATTR    = 0x0400,
    EVENT_AVRCP_REPORT_LIST_SETTING_VALUE   = 0x0401,
    EVENT_AVRCP_REPORT_CURRENT_VALUE        = 0x0402,
    EVENT_AVRCP_REPORT_SETTING_CHANGED      = 0x0403,
    EVENT_AVRCP_REPORT_ELEMENT_ATTR         = 0x0404,
    EVENT_AVRCP_REPORT_PLAYER_STATUS        = 0x0405,
#endif

#if (C_APP_PBAP_CMD_SUPPORT == 1)
    EVENT_PBAP_REPORT_DATA                  = 0x0500,
    EVENT_PBAP_REPORT_SESSION_STATUS        = 0x0501,
    EVENT_PBAP_DOWNLOAD_START               = 0x0502,
    EVENT_PBAP_DOWNLOAD_CMPL                = 0x0503,
    EVENT_PBAP_REPORT_SIZE                  = 0x0504,
#endif

    EVENT_OTA_DEV_INFO                      = 0x0600,
    EVENT_OTA_ACTIVE_BANK_VER               = 0x0601,
    EVENT_OTA_START                         = 0x0602,
    EVENT_OTA_PACKET                        = 0x0603,
    EVENT_OTA_VALID                         = 0x0604,
    EVENT_OTA_BUFFER_CHECK_ENABLE           = 0x0605,
    EVENT_OTA_BUFFER_CHECK                  = 0x0606,
    EVENT_OTA_IMG_INFO                      = 0x0607,
    EVENT_OTA_SECTION_SIZE                  = 0x0608,
    EVENT_OTA_DEV_EXTRA_INFO                = 0x0609,
    EVENT_OTA_PROTOCOL_TYPE                 = 0x060A,
    EVENT_OTA_ACTIVE_ACK                    = 0x060B,
    EVENT_OTA_GET_RELEASE_VER               = 0x060C,
    EVENT_OTA_INACTIVE_BANK_VER             = 0x060D,
    EVENT_OTA_COPY_IMG                      = 0x060E,
    EVENT_OTA_CHECK_SHA256                  = 0x060F,
    EVENT_OTA_ROLESWAP                      = 0x0610,

    EVENT_PLAYBACK_QUERY_INFO               = 0x0680,
    EVENT_PLAYBACK_GET_LIST_DATA            = 0x0681,
    EVENT_PLAYBACK_TRANS_START              = 0x0682,
    EVENT_PLAYBACK_REPORT_BUFFER_CHECK      = 0x0683,
    EVENT_PLAYBACK_VALID_SONG               = 0x0684,
    EVENT_PLAYBACK_TRIGGER_ROLE_SWAP        = 0x0685,
    EVENT_PLAYBACK_TRANS_CANCEL             = 0x0686,
    EVENT_PLAYBACK_EXIT_TRANS               = 0x0687,
    EVENT_PLAYBACK_PERMANENT_DELETE_SONG    = 0x0688,
    EVENT_PLAYBACK_PLAYLIST_ADD_SONG        = 0x0689,
    EVENT_PLAYBACK_PLAYLIST_DELETE_SONG     = 0x068A,
    EVENT_PLAYBACK_PERMANENT_DELETE_ALL_SONG = 0x068B,

    EVENT_REPORT_SUPPORTED_MMI_LIST         = 0x0700, //0x0700
    EVENT_REPORT_SUPPORTED_CLICK_TYPE,                //0x0701
    EVENT_REPORT_SUPPORTED_CALL_STATE,                //0x0702
    EVENT_REPORT_KEY_MMI_MAP,                         //0x0703
    EVENT_REPORT_USER_BIN_VER               = 0x0705, //0x0705
    EVENT_REPORT_PAD_VOLTAGE,                         //0x0706
    EVENT_PX318J_CALIBRATION_REPORT,

    EVENT_VENDOR_SEPC                       = 0x8800, //It has been reserved for vendor customer A, please dont't use this value.

    EVENT_LED_TEST                          = 0x0900,
    EVENT_KEY_TEST                          = 0x0901,
    EVENT_CLEAR_MP_DATA                     = 0x0902,
    EVENT_LOCAL_ADDR                        = 0x0903,
    EVENT_BT_BOND_INFO_CLEAR                = 0x0904,
    EVENT_DEVICE_ROLE                       = 0x0905,
    EVENT_CRYSTAL_TRIM                      = 0x0906,
    EVENT_LINK_KEY                          = 0x0907,
    EVENT_COUNTRY_CODE                      = 0x0908,
    EVENT_FW_VERSION                        = 0x0909,
    EVENT_GET_FLASH_BKP_DATA                = 0x090A,
    EVENT_ADC_VALUE_1                       = 0x090B,
    EVENT_ADC_VALUE_2                       = 0x090C,
    EVENT_GET_UNSIZE_RAM                    = 0x090D,
    EVENT_REPORT_FLASH_DATA                 = 0x090E,
    EVENT_MIC_SWITCH                        = 0x090F,
    EVENT_FACTORY_RESET_DONE                = 0x0910,
    EVENT_REPORT_PACKAGE_ID                 = 0x0911,

#if(C_APP_DEVICE_CMD_SUPPORT == 1)
    EVENT_BT_READY                          = 0x0920,
    EVENT_REPORT_PIN_CODE                   = 0x0921,
    EVENT_GET_LOCAL_DEV_STATE               = 0x0922,
    EVENT_INQUIRY_STATE                     = 0x0923,
    EVENT_SERVICES_SEARCH_STATE             = 0x0924,
    EVENT_PAIRING_REQUEST                   = 0x0925,
    EVENT_REPORT_SSP_NUMERIC_VALUE          = 0x0926,
    EVENT_LEGACY_RSSI                       = 0x0927,
    EVENT_REPORT_CONNECTED_DEV_ID           = 0x0928,
    EVENT_INQUIRY_RESULT                    = 0x0929,
    EVENT_STABLE_STATE                      = 0x092A,
    EVENT_REPORT_REMOTE_DEV_ATTR_INFO       = 0x092B,
#endif

    //for customize
    EVENT_JSA_CAL_RESULT                    = 0x0A02,
    EVENT_SITRON_CUSTOMIZED_FEATURE_REPORT  = 0x0A06,
    EVENT_MIC_MP_VERIFY_BY_HFP              = 0x0A07,
    EVENT_GET_DSP_CONFIG_GAIN               = 0x0A08,
    EVENT_CUSTOMIZED_TOZO_FEATURE           = 0x0A09,
    EVENT_RSV                               = 0x0A0A,
    EVENT_IO_PIN_PULL_HIGH                  = 0x0A0B,

    //for HCI command
    EVENT_ANC_VENDOR_REPORT                 = 0x0B00,
    EVENT_WDG_RESET                         = 0x0B01,
    EVENT_DUAL_MIC_MP_VERIFY                = 0x0B02,

    EVENT_SOUND_PRESS_CALIBRATION           = 0x0B10,

    // ANC
    EVENT_ANC_TEST_MODE                     = 0x0C00,
    EVENT_ANC_WRITE_GAIN,
    EVENT_ANC_READ_GAIN,
    EVENT_ANC_BURN_GAIN,
    EVENT_ANC_COMPARE,
    EVENT_ANC_GEN_TONE,
    EVENT_ANC_CONFIG_DATA_LOG,
    EVENT_ANC_READ_DATA_LOG,
    EVENT_ANC_READ_MIC_CONFIG,
    EVENT_ANC_READ_SPEAKER_CHANNEL,
    EVENT_ANC_READ_REGISTER,                //0x0C0A
    EVENT_ANC_WRITE_REGISTER,
    EVENT_ANC_LLAPT_WRITE_GAIN              = 0x0C0C,
    EVENT_ANC_LLAPT_READ_GAIN               = 0x0C0D,
    EVENT_ANC_LLAPT_BURN_GAIN               = 0x0C0E,
    EVENT_ANC_LLAPT_FEATURE_CONTROL         = 0x0C0F,

    EVENT_ANC_QUERY                         = 0x0C20,
    EVENT_ANC_ENABLE_DISABLE                = 0x0C21,
    EVENT_LLAPT_QUERY                       = 0x0C22,
    EVENT_LLAPT_ENABLE_DISABLE              = 0x0C23,

    EVENT_RAMP_GET_INFO                     = 0x0C26,
    EVENT_RAMP_BURN_PARA_START              = 0x0C27,
    EVENT_RAMP_BURN_PARA_CONTINUE           = 0x0C28,
    EVENT_RAMP_BURN_GRP_INFO                = 0x0C29,
    EVENT_RAMP_MULTI_DEVICE_APPLY           = 0x0C2A,

    EVENT_LISTENING_MODE_CYCLE_SET          = 0x0C2B,
    EVENT_LISTENING_MODE_CYCLE_GET          = 0x0C2C,
    EVENT_CUSTOMIZE_SENSOR_ACK              = 0x0C2D,

    EVENT_APT_VOLUME_INFO                   = 0x0C2E,
    EVENT_APT_VOLUME_SET                    = 0x0C2F,
    EVENT_APT_VOLUME_STATUS                 = 0x0C30,
    EVENT_LLAPT_BRIGHTNESS_INFO             = 0x0C31,
    EVENT_LLAPT_BRIGHTNESS_SET              = 0x0C32,
    EVENT_LLAPT_BRIGHTNESS_STATUS           = 0x0C33,
    EVENT_LLAPT_SCENARIO_CHOOSE_INFO        = 0x0C36,
    EVENT_LLAPT_SCENARIO_CHOOSE_TRY         = 0x0C37,
    EVENT_LLAPT_SCENARIO_CHOOSE_RESULT      = 0x0C38,
    EVENT_APT_POWER_ON_DELAY_TIME           = 0x0C39,


    // OTA Tooling
    EVENT_OTA_TOOLING_PARKING               = 0x0D00,

    EVENT_LOW_LATENCY_STATUS = 0x0E00,      //0x0E00
    EVENT_EAR_DETECTION_STATUS = 0x0E01,    //0x0E01
    EVENT_LOW_LATENCY_LEVEL_SET             = 0x0E02,

    EVENT_MP_TEST_RESULT                    = 0x0F00,

    EVENT_RECORDING_DATA                    = 0x100B,

#if (F_APP_HEARABLE_SUPPORT == 1)
    EVENT_HA_VER_INFO                       = 0x2000,
    EVENT_HA_EFFECT_INDEX_INFO,
    EVENT_HA_GET_ON_OFF_INFO,
    EVENT_HA_GET_TOOL_EXTEND_INFO,
    EVENT_HA_ANC_APT_INFO,
#endif

    EVENT_TOTAL
} T_EVENT_ID;

//for EVENT_CALLER_ID
#define CALLER_ID_TYPE_NUMBER           0x00
#define CALLER_ID_TYPE_NAME             0x01

/**
    * @brief  report app event by spp/le/uart
    * @param  cmd_path br/le/uart
    * @param  event_id event id
    * @param  app_index BR/LE link index
    * @param  data event data
    * @param  len  event len
    * @return void
    */
void app_report_event(uint8_t cmd_path, uint16_t event_id, uint8_t app_index, uint8_t *data,
                      uint16_t len);

/** End of APP_REPORT
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_REPORT_H_ */
