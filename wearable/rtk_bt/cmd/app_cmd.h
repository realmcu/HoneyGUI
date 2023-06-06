/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_CMD_H_
#define _APP_CMD_H_

#include <stdint.h>
#include <stdbool.h>

#include "app_report.h"
#include "dp_br_info.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_CMD App Cmd
  * @brief App Cmd
  * @{
  */

/*============================================================================*
 *                              Macros
 *============================================================================*/
/** @defgroup APP_CMD_Exported_Macros App Cmd Macros
   * @{
   */

#define CMD_SET_VER_MAJOR                   0x01
#define CMD_SET_VER_MINOR                   0x07
#define EQ_SPEC_VER_MAJOR                   0x02
#define EQ_SPEC_VER_MINOR                   0x00
#define EQ_SPEC_VER_MAJOR_OLD               0x01    /* for support old dsp config */
#define EQ_SPEC_VER_MINOR_OLD               0x00    /* for support old dsp config */
#define CMD_SYNC_BYTE                       0xAA

//align dsp_driver.h/codec_driver.h define
#define APP_MIC_SEL_DMIC_1                  0x00
#define APP_MIC_SEL_DMIC_2                  0x01
#define APP_MIC_SEL_AMIC_1                  0x02
#define APP_MIC_SEL_AMIC_2                  0x03
#define APP_MIC_SEL_AMIC_3                  0x04
#define APP_MIC_SEL_DISABLE                 0x07

#define CHECK_IS_LCH        (app_cfg_const.bud_side == DEVICE_BUD_SIDE_LEFT)
#define CHECK_IS_RCH        (app_cfg_const.bud_side == DEVICE_BUD_SIDE_RIGHT)

#define INVALID_VALUE       0xFF
#define L_CH_PRIMARY        0x01
#define R_CH_PRIMARY        0x02

#define L_CH_BATT_LEVEL     CHECK_IS_LCH ? app_db.local_batt_level : (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) ? app_db.remote_batt_level : 0
#define R_CH_BATT_LEVEL     CHECK_IS_RCH ? app_db.local_batt_level : (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) ? app_db.remote_batt_level : 0

//for CMD_AUDIO_DSP_CTRL_SEND to capture dsp data
#define VENDOR_SPP_CAPTURE_DSP_LOG      0x01
#define VENDOR_SPP_CAPTURE_DSP_RWA_DATA 0x02
//#define H2D_CMD_DSP_DAC_ADC_DATA_TO_MCU 0x1F
#define H2D_SPPCAPTURE_SET              0x0F01
#define CHANGE_MODE_EXIST               0x00
#define CHANGE_MODE_TO_SCO              0x01
#define DSP_CAPTURE_DATA_START_MASK                 0x01
#define DSP_CAPTURE_DATA_SWAP_TO_MASTER             0x02
#define DSP_CAPTURE_DATA_ENTER_SCO_MODE_MASK        0x04
#define DSP_CAPTURE_DATA_CHANGE_MODE_TO_SCO_MASK    0x08
#define DSP_CAPTURE_RAW_DATA_EXECUTING              0x10
#define DSP_CAPTURE_DATA_LOG_EXECUTING              0x20

/* for 8753BFN */
#if F_APP_DEVICE_CMD_SUPPORT
//for CMD_INQUIRY
#define START_INQUIRY                           0x00
#define STOP_INQUIRY                            0x01
#define NORMAL_INQUIRY                          0x00
#define PERIODIC_INQUIRY                        0x01
#define MAX_INQUIRY_TIME                        0x30

//for CMD_SERVICES_SEARCH
#define START_SERVICES_SEARCH                   0x00
#define STOP_SERVICES_SEARCH                    0x01

//for CMD_PAIR_REPLY and CMD_SSP_CONFIRMATION
#define ACCEPT_PAIRING_REQ                      0x00
#define REJECT_PAIRING_REQ                      0x01

//for CMD_SET_PAIRING_CONTROL
#define ENABLE_AUTO_ACCEPT_ACL_ACF_REQ          0x00
#define ENABLE_AUTO_REJECT_ACL_ACF_REQ          0x01
#define FORWARD_ACL_ACF_REQ_TO_HOST             0x02

//for CMD_GET_REMOTE_DEV_ATTR_INFO
#define GET_AVRCP_ATTR_INFO                     0x00
#define GET_PBAP_ATTR_INFO                      0x01
#endif

#if F_APP_AVRCP_CMD_SUPPORT
#define ALL_ELEMENT_ATTR                        0x00
#define MAX_NUM_OF_ELEMENT_ATTR                 0x07
#endif

//for CMD_GET_LINK_KEY
#define GET_ALL_LINK_KEY                        0x00
#define GET_SPECIAL_ADDR_LINK_KEY               0x01
#define GET_PRORITY_LINK_KEY                    0x02

//for CMD_SET_AND_READ_DLPS
#define SET_DLPS_DISABLE                        0x00
#define SET_DLPS_ENABLE                         0x01
#define GET_DLPS_STATUS                         0x02
// end of for 8753BFN

// for command handler
#define PARAM_START_POINT                       2
#define COMMAND_ID_LENGTH                       2

/** End of APP_CMD_Exported_Macros
    * @}
    */
/*============================================================================*
 *                              Types
 *============================================================================*/
/** @defgroup APP_CMD_Exported_Types App Cmd Types
  * @{
  */
/**  @brief  embedded uart, spp or le vendor command type.
  *    <b> Only <b> valid when BT SOC connects to external MCU via data uart, spp or le.
  *    refer to SDK audio sample code for definition
  */
typedef enum
{
    CMD_ACK                             = 0x0000,
    CMD_BT_READ_PAIRED_RECORD           = 0x0001,
    CMD_BT_CREATE_CONNECTION            = 0x0002,
    CMD_BT_DISCONNECT                   = 0x0003,
    CMD_MMI                             = 0x0004,
    CMD_LEGACY_DATA_TRANSFER            = 0x0005,
    CMD_ASSIGN_BUFFER_SIZE              = 0x0006,
    CMD_BT_READ_LINK_INFO               = 0x0007,
    CMD_TONE_GEN                        = 0x0008,
    CMD_BT_GET_REMOTE_NAME              = 0x0009,
    CMD_BT_IAP_LAUNCH_APP               = 0x000A,
    CMD_TTS                             = 0x000B,
    CMD_INFO_REQ                        = 0x000C,

    CMD_DAC_GAIN_CTRL                   = 0x000F,
    CMD_ADC_GAIN_CTRL                   = 0x0010,
    CMD_BT_SEND_AT_CMD                  = 0x0011,
    CMD_SET_CFG                         = 0x0012,
    CMD_INDICATION                      = 0x0013,
    CMD_LINE_IN_CTRL                    = 0x0014,
    CMD_LANGUAGE_GET                    = 0x0015,
    CMD_LANGUAGE_SET                    = 0x0016,
    CMD_GET_CFG_SETTING                 = 0x0017,
    CMD_GET_STATUS                      = 0x0018,  // used to get part of bud info
    CMD_SUPPORT_MULTILINK               = 0x0019,

    CMD_BT_HFP_DIAL_WITH_NUMBER         = 0x001B,
    CMD_GET_BD_ADDR                     = 0x001C,
    CMD_STRING_MODE                     = 0x001E,
    CMD_SET_VP_VOLUME                   = 0x001F,

    CMD_SET_AND_READ_DLPS               = 0x0020,
    CMD_GET_BUD_INFO                    = 0x0021,  // used to get complete bud info

    CMD_XM_SET_MODE                     = 0x0030,
    CMD_BT_HFP_SCO_MAG                  = 0x0040,

    CMD_LE_START_ADVERTISING            = 0x0100,
    CMD_LE_STOP_ADVERTISING             = 0x0101,
    CMD_LE_DATA_TRANSFER                = 0x0102,
    CMD_LE_START_SCAN                   = 0x0103,
    CMD_LE_STOP_SCAN                    = 0x0104,
    CMD_LE_GET_RAND_ADDR                = 0x0105,
    CMD_LE_SET_DATA_LEN                 = 0x0106,
    CMD_LE_SET_PHY                      = 0x0107,
    CMD_LE_GET_REMOTE_FEATURES          = 0x0108,
    CMD_LE_START_PAIR                   = 0x0109,
    CMD_LE_GET_ALL_BONDED_DEV           = 0x010A,
    CMD_LE_GATT_SERV_CHG                = 0x010B,
    CMD_LE_IGNORE_SLAVE_LATENCY         = 0x010C,
    CMD_LE_SET_RAND_ADDR                = 0x010D,
    CMD_LE_CREATE_CONN                  = 0x010E,
    CMD_LE_ATT_MTU_EXCHANGE             = 0x010F,

    CMD_ANCS_REGISTER                   = 0x0110,
    CMD_ANCS_GET_NOTIFICATION_ATTR      = 0x0111,
    CMD_ANCS_GET_APP_ATTR               = 0x0112,
    CMD_ANCS_PERFORM_NOTIFICATION_ACTION = 0x0113,

    CMD_AMS_REGISTER                    = 0x0120,
    CMD_AMS_WRITE_REMOTE_CMD            = 0x0121,
    CMD_AMS_WRITE_ENTITY_UPD_CMD        = 0x0122,


    CMD_AUDIO_EQ_QUERY                  = 0x0200,
    CMD_AUDIO_EQ_PARAM_SET              = 0x0203,
    CMD_AUDIO_EQ_PARAM_GET              = 0x0204,
    CMD_AUDIO_EQ_INDEX_SET              = 0x0205,
    CMD_AUDIO_EQ_INDEX_GET              = 0x0206,
    CMD_AUDIO_DSP_CTRL_SEND             = 0x0207,
    CMD_AUDIO_CODEC_CTRL_SEND           = 0x0208,
    CMD_SEND_RAW_PAYLOAD                = 0x0209,
#if 0
    //supported only in cmd set version v0.0.0.1
    CMD_DSP_GET_AUDIO_EQ_SETTING_IDX    = 0x0207,
    CMD_DSP_SET_AUDIO_EQ_SETTING_IDX    = 0x0208,
    CMD_DSP_SET_APT_GAIN                = 0x0209,
#endif
    CMD_SET_VOLUME                      = 0x020A,
#if F_APP_APT_SUPPORT
    CMD_APT_EQ_INDEX_SET                = 0x020B,
    CMD_APT_EQ_INDEX_GET                = 0x020C,
#endif
    CMD_DSP_DEBUG_SIGNAL_IN             = 0x020D,   // only support BBPro2

#if F_APP_APT_SUPPORT
    CMD_SET_APT_VOLUME_OUT_LEVEL        = 0x020E,
    CMD_GET_APT_VOLUME_OUT_LEVEL        = 0x020F,
#endif

    // for equalizer page
    CMD_AUDIO_EQ_QUERY_PARAM            = 0x0210,

    CMD_SET_TONE_VOLUME_LEVEL           = 0x0211,
    CMD_GET_TONE_VOLUME_LEVEL           = 0x0212,
    CMD_DSP_TOOL_FUNCTION_ADJUSTMENT    = 0x0213,

    CMD_RESET_EQ_DATA                   = 0x0214,

#if DATA_CAPTURE_V2_SUPPORT
    CMD_DSP_CAPTURE_V2_START_STOP       = 0x0220,
    CMD_DSP_CAPTURE_SCENARIO_ENTER_EXIT = 0x0222,
#endif

    //for good test
    CMD_LED_TEST                        = 0x0300,
    CMD_CLEAR_MP_DATA                   = 0x0301,
    CMD_BT_GET_LOCAL_ADDR               = 0x0302,
    CMD_GET_LEGACY_RSSI                 = 0x0303,
    CMD_GET_RF_POWER                    = 0x0304,
    CMD_GET_CRYSTAL_TRIM                = 0x0305,
    CMD_GET_LINK_KEY                    = 0x0306,
    CMD_GET_COUNTRY_CODE                = 0x0307,
    CMD_GET_FW_VERSION                  = 0x0308,
    CMD_BT_BOND_INFO_CLEAR              = 0x0309,
    CMD_GET_ADC_VALUE_1                 = 0x030A,
    CMD_GET_ADC_VALUE_2                 = 0x030B,
    CMD_GET_UNSIZE_RAM                  = 0x030C,
    CMD_GET_FLASH_DATA                  = 0x030D,
    CMD_MIC_SWITCH                      = 0x030E,
    CMD_GET_PACKAGE_ID                  = 0x030F,
    CMD_SWITCH_TO_HCI_DOWNLOAD_MODE     = 0x0310,
    CMD_GET_PAD_VOLTAGE                 = 0x0311,
    CMD_PX318J_CALIBRATION              = 0x0312,
    CMD_READ_CODEC_REG                  = 0x0313,
    CMD_WRITE_CODEC_REG                 = 0x0314,
    CMD_GET_NUM_OF_CONNECTION           = 0x0315,
    CMD_REG_ACCESS                      = 0X0316,
    CMD_DSP_TEST_MODE                   = 0X0317,
    CMD_IQS773_CALIBRATION              = 0x0318,
#if F_APP_DEVICE_CMD_SUPPORT
    CMD_INQUIRY                         = 0x0321,
    CMD_SERVICES_SEARCH                 = 0x0322,
    CMD_SET_PAIRING_CONTROL             = 0x0323,
    CMD_SET_PIN_CODE                    = 0x0324,
    CMD_GET_PIN_CODE                    = 0x0325,
    CMD_PAIR_REPLY                      = 0x0326,
    CMD_SSP_CONFIRMATION                = 0x0327,
    CMD_GET_CONNECTED_DEV_ID            = 0x0328,
    CMD_GET_REMOTE_DEV_ATTR_INFO        = 0x0329,
#endif

    CMD_RF_XTAK_K                       = 0x032A,
    CMD_RF_XTAL_K_GET_RESULT            = 0x032B,

    //0x0400 ~ 0x04FF reserved for profile
#if F_APP_HFP_CMD_SUPPORT
    CMD_SEND_DTMF                       = 0x0400,
    CMD_GET_OPERATOR                    = 0x0401,
    CMD_GET_SUBSCRIBER_NUM              = 0x0402,
    CMD_SEND_VGM                        = 0x0403,
    CMD_SEND_VGS                        = 0x0404,
#endif

#if F_APP_AVRCP_CMD_SUPPORT
    CMD_AVRCP_LIST_SETTING_ATTR         = 0x0500,
    CMD_AVRCP_LIST_SETTING_VALUE        = 0x0501,
    CMD_AVRCP_GET_CURRENT_VALUE         = 0x0502,
    CMD_AVRCP_SET_VALUE                 = 0x0503,
    CMD_AVRCP_ABORT_DATA_TRANSFER       = 0x0504,
    CMD_AVRCP_SET_ABSOLUTE_VOLUME       = 0x0505,
    CMD_AVRCP_GET_PLAY_STATUS           = 0x0506,
    CMD_AVRCP_GET_ELEMENT_ATTR          = 0x0507,
#endif

#if F_APP_PBAP_CMD_SUPPORT
    CMD_PBAP_DOWNLOAD                   = 0x0420,
    CMD_PBAP_DOWNLOAD_CONTROL           = 0x0421,
    CMD_PBAP_DOWNLOAD_GET_SIZE          = 0x0422,
#endif

    CMD_OTA_DEV_INFO                    = 0x0600,
    CMD_OTA_IMG_VER                     = 0x0601,
    CMD_OTA_START                       = 0x0602,
    CMD_OTA_PACKET                      = 0x0603,
    CMD_OTA_VALID                       = 0x0604,
    CMD_OTA_RESET                       = 0x0605,
    CMD_OTA_ACTIVE_RESET                = 0x0606,
    CMD_OTA_BUFFER_CHECK_ENABLE         = 0x0607,
    CMD_OTA_BUFFER_CHECK                = 0x0608,
    CMD_OTA_IMG_INFO                    = 0x0609,
    CMD_OTA_SECTION_SIZE                = 0x060A,
    CMD_OTA_DEV_EXTRA_INFO              = 0x060B,
    CMD_OTA_PROTOCOL_TYPE               = 0x060C,
    CMD_OTA_GET_RELEASE_VER             = 0x060D,
    CMD_OTA_INACTIVE_BANK_VER           = 0x060E,
    CMD_OTA_COPY_IMG                    = 0x060F,
    CMD_OTA_CHECK_SHA256                = 0x0610,
    CMD_OTA_ROLESWAP                    = 0x0611,
    CMD_OTA_TEST_EN                     = 0x0612,

#if (F_APP_LOCAL_PLAYBACK_SUPPORT == 1)
    /* only support BBPro2 */
    CMD_PLAYBACK_QUERY_INFO                 = 0x0680,
    CMD_PLAYBACK_GET_LIST_DATA              = 0x0681,
    CMD_PLAYBACK_TRANS_START                = 0x0682,
    CMD_PLAYBACK_TRANS_CONTINUE             = 0x0683,
    CMD_PLAYBACK_REPORT_BUFFER_CHECK        = 0x0684,
    CMD_PLAYBACK_VALID_SONG                 = 0x0685,
    CMD_PLAYBACK_TRIGGER_ROLE_SWAP          = 0x0686,
    CMD_PLAYBACK_TRANS_CANCEL               = 0x0687,
    CMD_PLAYBACK_EXIT_TRANS                 = 0x0688,
    CMD_PLAYBACK_PERMANENT_DELETE_SONG      = 0x0689,
    CMD_PLAYBACK_PLAYLIST_ADD_SONG          = 0x068A,
    CMD_PLAYBACK_PLAYLIST_DELETE_SONG       = 0x068B,
    CMD_PLAYBACK_PERMANENT_DELETE_ALL_SONG  = 0x068C,
#endif

    CMD_GET_SUPPORTED_MMI_LIST          = 0x0700,
    CMD_GET_SUPPORTED_CLICK_TYPE        = 0x0701,
    CMD_GET_SUPPORTED_CALL_STATUS       = 0x0702,
    CMD_GET_KEY_MMI_MAP                 = 0x0703,
    CMD_SET_KEY_MMI_MAP                 = 0x0704,

    CMD_RESET_KEY_MMI_MAP               = 0x0707,

    CMD_GET_RWS_KEY_MMI_MAP             = 0x0708,
    CMD_SET_RWS_KEY_MMI_MAP             = 0x0709,
    CMD_RESET_RWS_KEY_MMI_MAP           = 0x070A,

    CMD_VENDOR_SEPC                     = 0x0800, //It has been reserved for vendor customer A, please dont't use this value.

    CMD_DFU_START                       = 0x0900,

    //for customize
    CMD_RSV1                            = 0x0A00,
    CMD_RSV2                            = 0x0A01,
    CMD_RSV3                            = 0x0A02,
    CMD_SET_MERIDIAN_SOUND_EFFECT_MODE  = 0x0A03,
    CMD_LG_CUSTOMIZED_FEATURE           = 0x0A04,
    CMD_CUSTOMIZED_SITRON_FEATURE       = 0x0A05,
    CMD_JSA_CALIBRATION                 = 0x0A06,
    CMD_MIC_MP_VERIFY_BY_HFP            = 0x0A07,
    CMD_GET_DSP_CONFIG_GAIN             = 0x0A08,
    CMD_CUSTOMIZED_TOZO_FEATURE         = 0x0A09,
    CMD_RSV4                            = 0x0A0A,
    CMD_IO_PIN_PULL_HIGH                = 0x0A0B,
    CMD_ENTER_BAT_OFF_MODE              = 0x0A0C,

    //for HCI command
    CMD_ANC_VENDOR_COMMAND              = 0x0B00,
    CMD_WDG_RESET                       = 0x0B01,
    CMD_DUAL_MIC_MP_VERIFY              = 0x0B02,

    CMD_SOUND_PRESS_CALIBRATION         = 0x0B10,
    CMD_CAP_TOUCH_CTL                   = 0x0B11,
    CMD_GET_IMU_SENSOR_DATA             = 0x0B12,

    //for ANC command
    CMD_ANC_TEST_MODE                   = 0x0C00,
    CMD_ANC_WRITE_GAIN                  = 0x0C01,
    CMD_ANC_READ_GAIN                   = 0x0C02,
    CMD_ANC_BURN_GAIN                   = 0x0C03,
    CMD_ANC_COMPARE                     = 0x0C04,
    CMD_ANC_GEN_TONE                    = 0x0C05,
    CMD_ANC_CONFIG_DATA_LOG             = 0x0C06,
    CMD_ANC_READ_DATA_LOG               = 0x0C07,
    CMD_ANC_READ_MIC_CONFIG             = 0x0C08,
    CMD_ANC_READ_SPEAKER_CHANNEL        = 0x0C09,
    CMD_ANC_READ_REGISTER               = 0x0C0A,
    CMD_ANC_WRITE_REGISTER              = 0x0C0B,
    CMD_ANC_LLAPT_WRITE_GAIN            = 0x0C0C,
    CMD_ANC_LLAPT_READ_GAIN             = 0x0C0D,
    CMD_ANC_LLAPT_BURN_GAIN             = 0x0C0E,
    CMD_ANC_LLAPT_FEATURE_CONTROL       = 0x0C0F,

    CMD_ANC_QUERY                       = 0x0C20,
    CMD_ANC_ENABLE_DISABLE              = 0x0C21,
    CMD_LLAPT_QUERY                     = 0x0C22,
    CMD_LLAPT_ENABLE_DISABLE            = 0x0C23,

    CMD_RAMP_GET_INFO                   = 0x0C26,
    CMD_RAMP_BURN_PARA_START            = 0x0C27,
    CMD_RAMP_BURN_PARA_CONTINUE         = 0x0C28,
    CMD_RAMP_BURN_GRP_INFO              = 0x0C29,
    CMD_RAMP_MULTI_DEVICE_APPLY         = 0x0C2A,

    CMD_LISTENING_MODE_CYCLE_SET        = 0x0C2B,
    CMD_LISTENING_MODE_CYCLE_GET        = 0x0C2C,

    CMD_VENDOR_SPP_COMMAND              = 0x0C2D,

    CMD_APT_VOLUME_INFO                 = 0x0C2E,
    CMD_APT_VOLUME_SET                  = 0x0C2F,
    CMD_APT_VOLUME_STATUS               = 0x0C30,
    CMD_LLAPT_BRIGHTNESS_INFO           = 0x0C31,
    CMD_LLAPT_BRIGHTNESS_SET            = 0x0C32,
    CMD_LLAPT_BRIGHTNESS_STATUS         = 0x0C33,
    CMD_LLAPT_SCENARIO_CHOOSE_INFO      = 0x0C36,
    CMD_LLAPT_SCENARIO_CHOOSE_TRY       = 0x0C37,
    CMD_LLAPT_SCENARIO_CHOOSE_RESULT    = 0x0C38,
    CMD_APT_GET_POWER_ON_DELAY_TIME     = 0x0C39,
    CMD_APT_SET_POWER_ON_DELAY_TIME     = 0x0C3A,
    CMD_LISTENING_STATE_SET             = 0x0C3B,
    CMD_LISTENING_STATE_STATUS          = 0x0C3C,

    // ADSP PARA
    CMD_ANC_GET_ADSP_INFO               = 0x0C40,
    CMD_ANC_SEND_ADSP_PARA_START        = 0x0C41,
    CMD_ANC_SEND_ADSP_PARA_CONTINUE     = 0x0C42,
    CMD_ANC_ENABLE_DISABLE_ADAPTIVE_ANC = 0x0C43,

    CMD_ANC_SCENARIO_CHOOSE_INFO        = 0x0C44,
    CMD_ANC_SCENARIO_CHOOSE_TRY         = 0x0C45,
    CMD_ANC_SCENARIO_CHOOSE_RESULT      = 0x0C46,

    // OTA Tooling section
    CMD_OTA_TOOLING_PARKING             = 0x0D00,
    CMD_MEMORY_DUMP                     = 0x0D22,

#if F_APP_SAIYAN_MODE
    CMD_SAIYAN_GAIN_CTL                 = 0x0D29,
#endif

    CMD_GET_LOW_LATENCY_MODE_STATUS     = 0x0E01,
    CMD_GET_EAR_DETECTION_STATUS        = 0x0E02,
    CMD_GET_GAMING_MODE_EQ_INDEX        = 0x0E03,
    CMD_SET_LOW_LATENCY_LEVEL           = 0x0E04,

    CMD_MP_TEST                         = 0x0F00,
    CMD_FORCE_ENGAGE                    = 0x0F01,
    CMD_AGING_TEST_CONTROL              = 0x0F02,

#if F_APP_DURIAN_SUPPORT
    CMD_AVP_LD_EN                       = 0x1100,
    CMD_AVP_ANC_CYCLE_SET               = 0x1101,
    CMD_AVP_CLICK_SET                   = 0x1102,
    CMD_AVP_CONTROL_SET                 = 0x1103,
    CMD_AVP_ANC_SETTINGS                = 0x1104,
#endif

#if (F_APP_HEARABLE_SUPPORT == 1)
    /* only support BBPro2 */
    CMD_HA_SET_PARAM                    = 0x2000,
    CMD_HA_VER_REQ                      = 0x2001,
    CMD_HA_ACCESS_EFFECT_INDEX          = 0x2002,
    CMD_HA_ACCESS_ON_OFF                = 0x2003,
    CMD_HA_GET_TOOL_EXTEND_REQ          = 0x2004,
    CMD_HA_SET_DSP_PARAM                = 0x2005,
    CMD_HA_ACCESS_PROGRAM_ID            = 0x2006,
    CMD_HA_SET_NR_PARAM                 = 0x2007,
    CMD_HA_GET_PROGRAM_NUM              = 0x2008,
    CMD_HA_ACCESS_PROGRAM_NAME          = 0x2009,
    CMD_HA_SET_OVP_PARAM                = 0x200a,
    CMD_HA_SET_BEAMFORMING_PARAM        = 0x200b,
#endif

#if F_APP_SS_SUPPORT
    CMD_SS_REQ                          = 0x2200,
#endif

    CMD_GCSS_ADD                = 0x3000,
    CMD_GCSS_DEL                = 0x3001,
    CMD_GCSS_INFO               = 0x3002,
    CMD_GCSS_DATA_TRANSFER      = 0x3003,
    CMD_GCSS_READ_RSP           = 0x3004,
    CMD_GCSS_WRITE_RSP          = 0x3005,

    CMD_GCSC_SERV_DISCOVER_ALL           = 0x3050,
    CMD_GCSC_SERV_DISCOVER_UUID128       = 0x3051,
    CMD_GCSC_SERV_DISCOVER_UUID16        = 0x3052,
    CMD_GCSC_CHAR_DISCOVER_ALL           = 0x3053,
    CMD_GCSC_CHAR_DISCOVER_UUID128       = 0x3054,
    CMD_GCSC_CHAR_DISCOVER_UUID16        = 0x3055,
    CMD_GCSC_DESC_DISCOVER_ALL           = 0x3056,
    CMD_GCSC_READ                        = 0x3057,
    CMD_GCSC_READ_BY_UUID128             = 0x3058,
    CMD_GCSC_READ_BY_UUID16              = 0x3059,
    CMD_GCSC_IND_CFM                     = 0x305A,
    CMD_GCSC_WRITE                       = 0x305B,
    CMD_GCSC_DISCOVER_ALL                = 0x305C,


    /* customer customer command */
    CMD_XM_ENTER_HCI_MODE               = 0x8000,
    CMD_XM_DEVICE_REBOOT                = 0x8001,
    CMD_XM_MMI                          = 0x8004,
    CMD_XM_BT_SET_ADDR                  = 0x8005,
    CMD_XM_SPP_DATA_TRANSFER            = 0x8006,
    CMD_XM_GATT_DATA_TRANSFER           = 0x8007,
    CMD_XM_GET_FACTORY_RESET            = 0x8008,
    CMD_XM_SNIFF_MODE_CTRL              = 0x8009,
    CMD_XM_SET_XTAL                     = 0x8010,
    CMD_XM_SET_GAIN_K                   = 0x8011,
    CMD_XM_SET_FLATNESS_K               = 0x8012,
    CMD_XM_USER_CFM_REQ                 = 0x8013,
    CMD_XM_EXIT_SNIFF                   = 0x8014,
    CMD_XM_ENTER_SNIFF                  = 0x8015,
#if (F_APP_WATCH_CUSTOMER_CONN_MANAGER_SUPPORT == 1)
    CMD_XM_CREATE_EAR_INFO              = 0x8016,
    CMD_XM_GET_ALL_CONNECT_INFO         = 0x8017,
#endif
    CMD_XM_BT_SDP_RECORD_ADD            = 0x8018,
    CMD_XM_BT_SDP_RECORD_DEL            = 0x8019,

    CMD_XM_BT_SDP_RECORD_CONN           = 0x8020,
    CMD_XM_BT_SDP_RECORD_DISC           = 0x8021,
    CMD_XM_BT_SDP_RECORD_DATA_SEND      = 0x8022,
    CMD_XM_BT_SDP_RECORD_REG            = 0x8023,
    CMD_XM_BT_SDP_RECORD_CONN_CFM       = 0x8024,
    CMD_XM_BT_SDP_RECORD_DISCOVER       = 0x8025,
    CMD_XM_BT_READ_LINK_KEY             = 0x8026,
    CMD_XM_BT_CREATE_VOICE_TRACK        = 0x8027,
    CMD_XM_BT_AVRCP_GET_ELE_ATTR_RSP    = 0x8028,
    CMD_XM_BT_AVRCP_GET_ABS_SUPPORTED   = 0x8029,
    CMD_XM_BT_SET_ADDR_APP              = 0x8030,
    CMD_XM_LE_START_ADVERTISING         = 0x8100,
    CMD_XM_LE_STOP_ADVERTISING          = 0x8101,
    CMD_XM_LE_DISC                      = 0x8102,
    CMD_XM_LE_CONN_PARAM_UPDATE         = 0x8103,
    CMD_XM_LE_ADV_DATA_UPDATE           = 0x8104,
    CMD_XM_LE_SCAN_RSP_DATA_UPDATE      = 0x8105,
    CMD_XM_LE_ADV_INTVAL_UPDATE         = 0x8106,
    CMD_XM_LE_ATT_GET_MTU               = 0x8107,
    CMD_XM_LE_GET_ADDR                  = 0x8108,
    CMD_XM_LE_USER_CFM_REQ              = 0x8109,
    CMD_XM_LE_BOND_DEL                  = 0x810A,
    CMD_XM_LE_ADV_CREATE                = 0x810B,
    CMD_XM_LE_BOND_DEL_ALL              = 0x810C,
    CMD_XM_LE_ADV_PARAM_UPD             = 0x810D,
    CMD_XM_LE_GET_PHY                   = 0x810E,

    CMD_XM_MUSIC                        = 0x8500,
    CMD_XM_TTS                          = 0x8501,
    CMD_XM_START_RECORD                 = 0x8502,
    CMD_XM_STOP_RECORD                  = 0x8503,
    CMD_XM_START_RECORD_PLAY            = 0x8504,
    CMD_XM_STOP_RECORD_PLAY             = 0x8505,
    CMD_XM_RECORD_PLAY_DATA             = 0x8506,
    CMD_XM_RECORD_THEN_PLAY_VP          = 0x8507,
    CMD_XM_SET_VOLUME_LEVEL             = 0x8508,
    CMD_XM_SET_A2DP_VOLUME_LEVEL        = 0x8509,
    CMD_XM_AUDIO_TEST_GET_PA_ID         = 0x8600,
    CMD_XM_AUDIO_TEST_BYPASS            = 0x8601,
    CMD_XM_SET_PA_MODE                  = 0x8700,
    CMD_XM_DISABLE_VERBOSE_LOG          = 0x8F00,
    CMD_XM_TEST                         = 0x8F01,
    CMD_XM_SET_CPU_FREQ                 = 0x8F02,
    CMD_XM_CRASH_DUMP                   = 0x8F03,
} T_CMD_ID;

/** @brief  packet type for legacy transfer*/
typedef enum t_pkt_type
{
    PKT_TYPE_SINGLE = 0x00,
    PKT_TYPE_START = 0x01,
    PKT_TYPE_CONT = 0x02,
    PKT_TYPE_END = 0x03
} T_PKT_TYPE;

typedef enum
{
    BUD_TYPE_SINGLE      = 0x00,
    BUD_TYPE_RWS         = 0x01,
} T_BUD_TYPE;

typedef enum
{
    APP_REMOTE_MSG_CMD_GET_FW_VERSION           = 0x00,
    APP_REMOTE_MSG_CMD_REPORT_FW_VERSION        = 0x01,
    APP_REMOTE_MSG_CMD_GET_OTA_FW_VERSION       = 0x02,
    APP_REMOTE_MSG_CMD_REPORT_OTA_FW_VERSION    = 0x03,
    APP_REMOTE_MSG_DSP_DEBUG_SIGNAL_IN_SYNC     = 0x04,    // only support BBPro2

    APP_REMOTE_MSG_SYNC_EQ_CTRL_BY_SRC          = 0x05,
    APP_REMOTE_MSG_SYNC_RAW_PAYLOAD             = 0x06,
    APP_REMOTE_MSG_CMD_TOTAL                    = 0x07,
} T_CMD_REMOTE_MSG;

//for CMD_SET_CFG
typedef enum
{
    CFG_SET_LE_NAME                  = 0x00,
    CFG_SET_LEGACY_NAME              = 0x01,
    CFG_SET_AUDIO_LATENCY            = 0x02,
    CFG_SET_SUPPORT_CODEC            = 0x03,
    CFG_SET_SERIAL_ID                = 0x04,
    CFG_SET_SERIAL_SINGLE_ID         = 0x05,
    CFG_SET_HFP_REPORT_BATT          = 0x06,
} T_CMD_SET_CFG_TYPE;

//for CMD_GET_CFG_SETTING
typedef enum
{
    CFG_GET_LE_NAME                  = 0x00,
    CFG_GET_LEGACY_NAME              = 0x01,
    CFG_GET_IC_NAME                  = 0x02,
    CFG_GET_COMPANY_ID_AND_MODEL_ID  = 0x03,
    CFG_GET_SERIAL_ID                = 0x04,
    CFG_GET_SERIAL_LEFT_SINGLE_ID    = 0x05,
    CFG_GET_SERIAL_RIGHT_SINGLE_ID   = 0x06,
    CFG_GET_MAX                      = 0x07,
} T_CMD_GET_CFG_TYPE;

/**  @brief CMD Set Info Request type. */
typedef enum
{
    CMD_SET_INFO_TYPE_VERSION = 0x00,
    CMD_INFO_GET_CAPABILITY   = 0x01,
} T_CMD_SET_INFO_TYPE;

typedef enum
{
    GET_STATUS_RWS_STATE                  = 0x00,
    GET_STATUS_RWS_CHANNEL                = 0x01,
    GET_STATUS_BATTERY_STATUS             = 0x02,
    GET_STATUS_APT_STATUS                 = 0x03,
    GET_STATUS_APP_STATE                  = 0x04,
    GET_STATUS_BUD_ROLE                   = 0x05,
    GET_STATUS_APT_NR_STATUS              = 0x06,
    GET_STATUS_APT_VOL                    = 0x07,
    GET_STATUS_LOCK_BUTTON                = 0x08,
    GET_STATUS_FIND_ME                    = 0x09,
    GET_STATUS_ANC_STATUS                 = 0x0A,
    GET_STATUS_LLAPT_STATUS               = 0x0B,
    GET_STATUS_RWS_DEFAULT_CHANNEL        = 0x0C,
    GET_STATUS_RWS_BUD_SIDE               = 0x0D,
    GET_STATUS_RWS_SYNC_APT_VOL           = 0x0E,
    GET_STATUS_FACTORY_RESET_STATUS       = 0x0F,
    GET_STATUS_AUTO_REJECT_CONN_REQ_STATUS = 0x10,
    GET_STATUS_RADIO_MODE                 = 0x11,
    GET_STATUS_SCO_STATUS                 = 0x12,
    GET_STATUS_MIC_MUTE_STATUS            = 0x13,

    /* for LG (BBLite D-cut) */
    GET_STATUS_BUD_ROLE_FOR_LG            = 0xA0,
    GET_STATUS_VOLUME                     = 0xA1,
    GET_STATUS_MERIDIAN_SOUND_EFFECT_MODE = 0xA2,
    GET_STATUS_LIGHT_SENSOR               = 0xA3,

#if F_APP_DURIAN_SUPPORT
    GET_STATUS_AVP_RWS_VER                = 0xA4,
    GET_STATUS_AVP_MULTILINK_ON_OFF       = 0xA5,
#endif
} T_CMD_GET_STATUS_TYPE;

typedef struct
{
    // Byte 0
    uint8_t snk_support_get_set_le_name : 1;
    uint8_t snk_support_get_set_br_name : 1;
    uint8_t snk_support_get_set_vp_language : 1;
    uint8_t snk_support_get_battery_info : 1;
    uint8_t snk_support_ota : 1;
    uint8_t snk_support_change_channel : 1;
    uint8_t snk_support_rsv1 : 2;

    // Byte 1
    uint8_t snk_support_tts : 1;
    uint8_t snk_support_get_set_rws_state : 1;
    uint8_t snk_support_get_set_apt_state : 1; //bit 10
    uint8_t snk_support_get_set_eq_state : 1;
    uint8_t snk_support_get_set_vad_state : 1;
    uint8_t snk_support_get_set_anc_state : 1;
    uint8_t snk_support_get_set_llapt_state : 1; //bit 14
    uint8_t snk_support_get_set_listening_mode_cycle : 1;

    // Byte 2
    uint8_t snk_support_llapt_brightness : 1;
    uint8_t snk_support_anc_eq : 1;
    uint8_t snk_support_apt_eq : 1;
    uint8_t snk_support_tone_volume_adjustment : 1;
    uint8_t snk_support_apt_eq_adjust_separate : 1; //bit20
    uint8_t snk_support_multilink_support : 1;
    uint8_t snk_support_avp : 1; //bit22
    uint8_t snk_support_nr_on_off : 1;

    // Byte 3
    uint8_t snk_support_llapt_scenario_choose : 1;
    uint8_t snk_support_ear_detection : 1;
    uint8_t snk_support_power_on_delay_apply_apt_on : 1;
    uint8_t snk_support_phone_set_anc_eq : 1; //bit 27
    uint8_t snk_support_new_report_bud_status_flow : 1;
    uint8_t snk_support_new_report_listening_status : 1;
    uint8_t snk_support_apt_volume_force_adjust_sync : 1; //bit 30
    uint8_t snk_support_reset_key_remap : 1;

    // Byte 4
    uint8_t snk_support_ansc : 1;
    uint8_t snk_support_vibrator : 1;
    uint8_t snk_support_change_mfb_func : 1;
    uint8_t snk_support_gaming_mode : 1;
    uint8_t snk_support_gaming_mode_eq : 1;
    uint8_t snk_support_key_remap : 1;
    uint8_t snk_support_HA: 1;
    uint8_t snk_support_local_playback : 1;  //bit39

    // Byte 5
    uint8_t snk_support_rsv8_1 : 2;
    uint8_t snk_support_anc_scenario_choose : 1;
    uint8_t snk_support_rws_key_remap : 1;
    uint8_t snk_support_user_eq : 1;
    uint8_t snk_support_reset_key_map_by_bud : 1;
    uint8_t snk_support_get_set_serial_id : 1;
    uint8_t snk_support_rsv8_3 : 1;

    //byte 6
    uint8_t snk_support_spp_2 : 1;
    uint8_t snk_support_anc_apt_coexist : 1; //bit 49
    uint8_t snk_support_spatial_audio : 1;
    uint8_t snk_support_ui_ota_version : 1;
    uint8_t snk_support_anc_apt_scenario_separate : 1; //bit 52
    uint8_t snk_support_3bin_scenario: 1; //bit 53
    uint8_t snk_support_rsv6_2 : 2;

} T_SNK_CAPABILITY;

/**  @brief  cmd set status to phone
  */
typedef enum
{
    CMD_SET_STATUS_COMPLETE = 0x00,
    CMD_SET_STATUS_DISALLOW = 0x01,
    CMD_SET_STATUS_UNKNOW_CMD = 0x02,
    CMD_SET_STATUS_PARAMETER_ERROR = 0x03,
    CMD_SET_STATUS_BUSY = 0x04,
    CMD_SET_STATUS_PROCESS_FAIL = 0x05,
    CMD_SET_STATUS_ONE_WIRE_EXTEND = 0x06,
    CMD_SET_STATUS_TTS_REQ = 0x07,
    CMD_SET_STATUS_MUSIC_REQ = 0x08,
    CMD_SET_STATUS_VERSION_INCOMPATIBLE = 0x09,
    CMD_SET_STATUS_SCENARIO_ERROR = 0x0A,
    CMD_SET_STATUS_SPP = 0x0B,
    CMD_SET_STATUS_SPP_ERROR = 0x0C,
    CMD_SET_STATUS_GATT = 0x11,
    CMD_SET_STATUS_GATT_ERROR = 0x12,

} T_CMD_SET_STATUS;

typedef struct
{
    uint32_t flash_data_start_addr_tmp;
    uint32_t flash_data_start_addr;
    uint32_t flash_data_size;
    uint8_t flash_data_type;
} T_FLASH_DATA;

typedef union
{
    uint8_t d8[16];
    struct
    {
        uint32_t ver_major: 4;      //!< major version
        uint32_t ver_minor: 8;      //!< minor version
        uint32_t ver_revision: 15;  //!< revision version
        uint32_t ver_reserved: 5;   //!< reserved
        uint32_t ver_commitid;      //!< git commit id
        uint8_t customer_name[8];   //!< customer name
    };
} T_PATCH_IMG_VER_FORMAT;

typedef union
{
    uint32_t version;
    struct
    {
        uint32_t ver_major: 8;      //!< major version
        uint32_t ver_minor: 8;      //!< minor version
        uint32_t ver_revision: 8;   //!< revision version
        uint32_t ver_reserved: 8;   //!< reserved
    };
} T_APP_UI_IMG_VER_FORMAT;

typedef union
{
    uint8_t version[4];
    struct
    {
        uint8_t cmd_set_ver_major;
        uint8_t cmd_set_ver_minor;
        uint8_t eq_spec_ver_major;
        uint8_t eq_spec_ver_minor;
    };
} T_SRC_SUPPORT_VER_FORMAT;

typedef enum
{
    LE_RSSI,
    LEGACY_RSSI
} T_RSSI_TYPE;

typedef enum
{
    REG_ACCESS_READ,
    REG_ACCESS_WRITE,
} T_CMD_REG_ACCESS_ACTION;

typedef enum
{
    REG_ACCESS_TYPE_AON,
    REG_ACCESS_TYPE_AON2B,
    REG_ACCESS_TYPE_DIRECT,
} T_CMD_REG_ACCESS_TYPE;

typedef enum
{
    DSP_TOOL_FUNCTION_BRIGHTNESS = 0x0000,
} T_CMD_DSP_TOOL_FUNCTION_TYPE;

typedef void (*P_APP_CMD_PARSE_CBACK)(uint8_t msg_type, uint8_t *buf, uint16_t len);

typedef enum
{
    APP_CMD_MODULE_TYPE_NONE              = 0x00,
    APP_CMD_MODULE_TYPE_AUDIO_POLICY      = 0x01,
    APP_CMD_MODULE_TYPE_HA                = 0x02,
} T_APP_CMD_MODULE_TYPE;

typedef struct t_app_cmd_parse_cback_item
{
    struct t_app_cmd_parse_cback_item   *p_next;
    P_APP_CMD_PARSE_CBACK               parse_cback;
    uint16_t                            module_total_len;
    T_APP_CMD_MODULE_TYPE               module_type;
    uint8_t                             msg_type_max;
} T_APP_CMD_PARSE_CBACK_ITEM;

/** End of APP_CMD_Exported_Types
    * @}
    */
/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_CMD_Exported_Functions App Cmd Functions
    * @{
    */
/**
    * @brief  App process uart or embedded spp vendor command.
    * @param  cmd_ptr command type
    * @param  cmd_len command length
    * @param  cmd_path command path use for distinguish uart,or le,or spp channel.
    * @param  rx_seqn recieved command sequence
    * @param  app_idx received rx command device index
    * @return void
    */
void app_handle_cmd_set(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path, uint8_t rx_seqn,
                        uint8_t app_idx);

/**
    * @brief  send event packet via br link or le link.
    * @param  event_id @ref T_EVENT_ID
    * @param  buf content in the event packet
    * @param  len buf length.
    * @return void
    */
void app_cmd_set_event_broadcast(uint16_t event_id, uint8_t *buf, uint16_t len);

#if (F_APP_OTA_TOOLING_SUPPORT == 1)
void app_cmd_ota_tooling_parking(void);
void app_cmd_stop_ota_parking_power_off(void);
#endif

#if F_APP_DEVICE_CMD_SUPPORT
bool app_cmd_get_auto_reject_conn_req_flag(void);
bool app_cmd_get_auto_accept_conn_req_flag(void);
bool app_cmd_get_report_attr_info_flag(void);
void app_cmd_set_report_attr_info_flag(bool flag);
#endif

#if (F_APP_SPP_CAPTURE_DSP_DATA == 1)
uint8_t app_cmd_dsp_capture_data_state(void);
bool app_cmd_spp_capture_executing_check(void);
bool app_cmd_spp_capture_audio_dsp_ctrl_send_handler(uint8_t *cmd_ptr, uint16_t cmd_len,
                                                     uint8_t cmd_path, uint8_t app_idx, uint8_t *ack_pkt, bool send_flag);
extern void spp_capture_ble_stop(void);
#endif
/* @brief  app cmd init
*
* @param  void
* @return none
*/
void app_cmd_init(void);
void app_cmd_set_event_ack(uint8_t cmd_path, uint8_t app_idx, uint8_t *buf);


bool app_cmd_get_tool_connect_status(void);
void app_cmd_update_eq_ctrl(uint8_t value, bool is_need_relay);
T_SRC_SUPPORT_VER_FORMAT *app_cmd_get_src_version(uint8_t cmd_path, uint8_t app_idx);
bool app_cmd_check_specific_version(T_SRC_SUPPORT_VER_FORMAT *version, uint8_t ver_major,
                                    uint8_t ver_minor);
bool app_cmd_check_src_cmd_version(uint8_t cmd_path, uint8_t app_idx);
bool app_cmd_check_src_eq_spec_version(uint8_t cmd_path, uint8_t app_idx);
bool app_cmd_cback_register(P_APP_CMD_PARSE_CBACK parse_cb,
                            T_APP_CMD_MODULE_TYPE module_type, uint8_t msg_max);
void app_cmd_set_event_ack(uint8_t cmd_path, uint8_t app_idx, uint8_t *buf);

/** @} */ /* End of group APP_CMD_Exported_Functions */
/** End of APP_CMD
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_CMD_H_ */
