/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_CFG_H_
#define _APP_CFG_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <os_mem.h>
#include "trace.h"


#ifdef __cplusplus
extern "C" { /* __cplusplus */
#endif
//App config data
#define APP_DATA_SYNC_WORD      0xAA55
#define APP_DATA_SYNC_WORD_LEN  2
#define APP_DATA_TONE_NUM_LEN   2

#define DATA_SYNC_WORD          0xAA55AA55
#define DATA_SYNC_WORD_LEN      4

#define APP_CONFIG_OFFSET       (0)
#define APP_CONFIG_SIZE         1024

#define APP_LED_OFFSET          (APP_CONFIG_OFFSET + APP_CONFIG_SIZE)
#define APP_LED_SIZE            512

#define SYS_CONFIG_OFFSET       (APP_LED_OFFSET + APP_LED_SIZE)
#define SYS_CONFIG_SIZE         1024

#define EXTEND_APP_CONFIG_OFFSET  (SYS_CONFIG_OFFSET + SYS_CONFIG_SIZE)
#define EXTEND_APP_CONFIG_SIZE    128

#define TONE_DATA_OFFSET        (4096)//Rsv 4K for APP parameter for better flash control
#define TONE_DATA_SIZE          3072
#define VOICE_PROMPT_OFFSET     (TONE_DATA_OFFSET + TONE_DATA_SIZE)
#define VOICE_PROMPT_SIZE       137 * 1024

//FTL
#define APP_RW_DATA_ADDR        3072
#define APP_RW_DATA_SIZE        360

#define FACTORY_RESET_OFFSET    140

//xm adp adv count store offset
#define APP_BOND_DEVICE_INFO_ADDR           (APP_RW_DATA_ADDR + APP_RW_DATA_SIZE)
#define APP_BOND_DEVICE_INFO_SIZE           (416)

#define MFB_KEY_SUPPORT                 0
#define UNPLUG_POWERON_RESET_ENABLE     0
#define APP_AVP_INIT                    0
#define APP_GAMING_MODE_SUPPORT         0
#define APP_CAP_TOUCH_SUPPORT           0

#define TRANSPORT_BIT_LE                0x01
#define TRANSPORT_BIT_SPP               0x02
#define TRANSPORT_BIT_IAP               0x04

#define VOICE_PROMPT_INDEX              0x80
#define TONE_INVALID_INDEX              0xFF

#define A2DP_FLUSH_TIME                     0x60 // 0x80 //0x140
#define A2DP_INTERVAL                       0x30 // default 0x48
#define A2DP_CTRL_RSVD_SLOT                 0x08   // default 6
#define A2DP_FLUSH_TIME_LOW_LATENCY         0x32
#define A2DP_INTERVAL_LOW_LATENCY           0x20
#define A2DP_CTRL_RSVD_SLOT_LOW_LATENCY     0x0A

#define AAC_LOW_LATENCY_MS                  60
#define SBC_LOW_LATENCY_MS                  60
#define A2DP_LATENCY_MS                     280

#define PLAYBACK_POOL_SIZE                  (10*1024)
#define VOICE_POOL_SIZE                     (2*1024)
#define RECORD_POOL_SIZE                    (1*1024)
#define NOTIFICATION_POOL_SIZE              (3*1024)

enum
{
    SHORT = 0,
    LONG  = 1,
};

enum
{
    KEY0 = 0,
    KEY1 = 1,
    KEY2 = 2,
    KEY3 = 3,
    KEY4 = 4,
    KEY5 = 5,
    KEY6 = 6,
    KEY7 = 7
};

enum
{
    HY_KEY0 = 0,
    HY_KEY1 = 1,
    HY_KEY2 = 2,
    HY_KEY3 = 3,
    HY_KEY4 = 4,
    HY_KEY5 = 5,
    HY_KEY6 = 6,
    HY_KEY7 = 7
};

typedef enum
{
    DSP_OUTPUT_LOG_NONE = 0x0,          //!< no DSP log.
    DSP_OUTPUT_LOG_BY_UART = 0x1,       //!< DSP log by uart directly.
    DSP_OUTPUT_LOG_BY_MCU = 0x2,        //!< DSP log by MCU.
} T_DSP_OUTPUT_LOG;

/** @defgroup APP_CFG App Cfg
  * @brief App Cfg
  * @{
  */

/*============================================================================*
 *                              Types
 *============================================================================*/
/** @defgroup APP_CFG_Exported_Types App Cfg Types
    * @{
    */
/** @brief  Read/Write configurations for inbox audio application  */
typedef struct
{
    uint32_t sync_word;
    uint32_t length;
} T_APP_CFG_NV_HDR;

typedef struct
{
    T_APP_CFG_NV_HDR hdr;

    //offset: 0x08
    uint8_t device_name_legacy[40];
    uint8_t device_name_le[40];

    //offset: 0x58
    uint8_t bud_sn[16];
    uint8_t le_random_addr[6];
    uint8_t bud_local_addr[6];
    uint8_t bud_peer_addr[6];
    uint8_t bud_peer_factory_addr[6];

    //offset: 0x80
    uint8_t bud_role;
    uint8_t single_tone_tx_gain;
    uint8_t single_tone_channel_num;
    uint8_t reserved;
    uint32_t single_tone_timeout_val;

    //offset: 0x88
    uint8_t app_is_power_on : 1;
    uint8_t factory_reset_done : 1;
    uint8_t adp_factory_reset_power_on : 1;
    uint8_t app_status_rsv : 5;
    uint8_t eq_index;
    uint8_t audio_effect_type;
    uint8_t dual_audio_effect_gaming_type;

    uint8_t audio_gain_level[8];
    uint8_t voice_gain_level[8];

    //offset: 0x9C
    uint8_t voice_prompt_language;
    uint8_t cur_spk_channel;
    uint8_t cur_mic;
    uint8_t pin_code_size;
    uint8_t pin_code[8];
} T_APP_CFG_NV;

/** @brief  Read only configurations for inbox audio application */
typedef struct
{
    uint32_t sync_word;

    //Peripheral
    //dmic,sport,micbias,ext_amp?pinmux?framework T_SYS_CFG_CONST????
    uint8_t ext_amp_pinmux;
    uint8_t ext_amp_boost_pinmux;

    //Miscellaneous
    uint8_t device_name_legacy_default[40];
    uint8_t device_name_le_default[40];
    uint8_t class_of_device[3];
    uint8_t max_legacy_multilink_devices;
    uint16_t hfp_brsf_capability;

    uint32_t supported_profile_mask;
    uint16_t a2dp_codec_type_sbc : 1;
    uint16_t a2dp_codec_type_aac : 1;
    uint16_t a2dp_codec_type_ldac : 1;
    uint16_t a2dp_codec_type_rsv : 13;
    uint8_t link_scenario;
    uint8_t voice_prompt_language;
    uint8_t mic_channel;
    uint8_t couple_speaker_channel;
    uint8_t solo_speaker_channel;

    //dsp_option0
    uint8_t enable_ctrl_ext_amp : 1;
    uint8_t enable_ext_amp_high_active : 1;
    uint8_t enable_ringtone_mixing : 1;
    uint8_t enable_vp_mixing : 1;
    uint8_t dsp_option_rsv_bits : 4;

    //dsp_option1
    uint8_t dsp_log_pin : 6;
    uint8_t dsp_log_output_select : 2;

    //Profile A2DP codec settings + profile link number: Total 36 bytes(offset = 680)
    //SBC settings
    uint8_t sbc_sampling_frequency;
    uint8_t sbc_channel_mode;
    uint8_t sbc_block_length;
    uint8_t sbc_subbands;
    uint8_t sbc_allocation_method;
    uint8_t sbc_min_bitpool;
    uint8_t sbc_max_bitpool;

    //AAC settings
    uint16_t aac_sampling_frequency;
    uint8_t  aac_object_type;
    uint8_t  aac_channel_number;
    uint8_t  aac_vbr_supported;
    uint8_t  aac_reserved[3];
    uint32_t aac_bit_rate;

    //LDAC settings
    int32_t ldac_supported_reserved1;
    int32_t ldac_supported_reserved2;

    //Profile link
    uint8_t a2dp_link_number;
    uint8_t hfp_link_number;
    uint8_t spp_link_number;
    uint8_t pbap_link_number;
    uint8_t avrcp_link_number;
    uint8_t profile_rsv[3];
} T_APP_CFG_CONST;

typedef struct
{
    uint8_t apt_volume_min;
    uint8_t apt_volume_max;
    uint8_t playback_volume_min;
    uint8_t playback_volume_max;
    uint8_t playback_volume_default;
    uint8_t voice_out_volume_min;
    uint8_t voice_out_volume_max;
    uint8_t voice_out_volume_default;
    uint8_t voice_volume_in_min;
    uint8_t voice_volume_in_max;
    uint8_t voice_volume_in_default;
    uint8_t record_volume_min;
    uint8_t record_volume_max;
    uint8_t record_volume_default;
    uint8_t voice_prompt_volume_min;
    uint8_t voice_prompt_volume_max;
    uint8_t voice_prompt_volume_default;
    uint8_t ringtone_volume_min;
    uint8_t ringtone_volume_max;
    uint8_t ringtone_volume_default;
} T_APP_CFG_VOLUME;

typedef struct
{
    uint8_t data_uart_tx_pinmux;
    uint8_t data_uart_rx_pinmux;
    uint8_t tx_wake_up_pinmux;
    uint8_t rx_wake_up_pinmux;

    uint8_t enable_data_uart;
    uint8_t enable_tx_wake_up;
    uint8_t enable_rx_wake_up;

    uint8_t data_uart_baud_rate;

    uint8_t dt_resend_num;
    uint8_t enable_embedded_cmd;
    uint8_t enable_dsp_capture_data_by_spp;

    uint8_t report_uart_event_only_once;
} T_CONSOLE_CFG_CONST;

typedef struct
{
    uint8_t support_sink;
    uint8_t support_local_source;
} T_APP_FUNCTION_CONFIG;


/** @brief in/out chargebox detect method for power on off function*/
enum
{
    DISABLE_DETECT = 0,
    ADAPTOR_DETECT = 1,
    GPIO_DETECT = 2,
};
/** End of APP_CFG_Exported_Types
    * @}
    */

extern T_APP_CFG_CONST app_cfg_const;

extern T_APP_CFG_NV app_cfg_nv;

extern T_APP_CFG_VOLUME app_cfg_volume;

extern const T_CONSOLE_CFG_CONST console_cfg_const;

extern T_APP_FUNCTION_CONFIG  app_audio_cfg;

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_CFG_Exported_Functions App Cfg Functions
    * @{
    */
/**
    * @brief  store config parameters to ftl
    * @param  void
    * @return ftl result
    */
uint32_t app_cfg_store(void);

/**
    * @brief  reset config parameters
    * @param  void
    * @return ftl result
    */
uint32_t app_cfg_reset(void);

/**
    * @brief  load config parameters from ftl
    * @param  void
    * @return void
    */
void app_cfg_load(void);

/**
    * @brief  load extend config parameters from ftl
    * @param  void
    * @return void
    */
void app_extend_cfg_load(void);

/**
    * @brief  config module init
    * @param  void
    * @return void
    */
void app_cfg_init(void);

/** @} */ /* End of group APP_CFG_Exported_Functions */

/** End of APP_CFG
* @}
*/


#ifdef __cplusplus
} /* __cplusplus */
#endif

#endif /*_AU_CFG_H_*/
