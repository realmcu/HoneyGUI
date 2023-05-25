/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      datebse.h
* @brief
* @details
* @author
* @date
* @version
* *********************************************************************************************************
*/

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef _DP_MCU_CFG_CONST_H_
#define _DP_MCU_CFG_CONST_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*
 *                        Header Files
*============================================================================*/
#include "stdbool.h"
#include "stdint.h"


#define KVDB_MCU_CFG_CONST_NAME "mcu_cfg_const"

#define MCU_CFG_CONST_DATA_SYNC_WORD          0xAA55AA55
#define MCU_CFG_CONST_DATA_SYNC_WORD_LEN      4

#define MCU_CFG_CONST_OFFSET       (0)
#define MCU_CFG_CONST_SIZE         1024

/*mcu_cfg_const key*/
#define KEY_MCU_CFG_CONST_SYNC_WORD   "mcu_cfg_const_sync_word"

//Periphera
//dmic,sport,micbias,ext_amp?pinmux?framework T_SYS_CFG_CONST????
#define KEY_MCU_CFG_CONST_EXT_AMP_PINMUX   "mcu_cfg_const_ext_amp_pinmux"
#define KEY_MCU_CFG_CONST_EXT_AMP_BOOST_PINMUX   "mcu_cfg_const_ext_amp_boost_pinmux"

//Miscellaneou
#define KEY_MCU_CFG_CONST_DEVICE_NAME_LEGACY_DEFAULT   "mcu_cfg_const_device_name_legacy_defaul"
#define KEY_MCU_CFG_CONST_DEVICE_NAME_LE_DEFAULT   "mcu_cfg_const_device_name_le_default"
#define KEY_MCU_CFG_CONST_CLASS_OF_DEVICE   "mcu_cfg_const_class_of_device"
#define KEY_MCU_CFG_CONST_MAX_LEGACY_MULTILINK_DEVICES   "mcu_cfg_const_max_legacy_multilink_device"
#define KEY_MCU_CFG_CONST_HFP_BRSF_CAPABILITY   "mcu_cfg_const_hfp_brsf_capability"

#define KEY_MCU_CFG_CONST_SUPPORTED_PROFILE_MASK   "mcu_cfg_const_supported_profile_mask"
#define KEY_MCU_CFG_CONST_A2DP_CODEC_TYPE   "mcu_cfg_const_a2dp_codec_type"

#define KEY_MCU_CFG_CONST_LINK_SCENARIO   "mcu_cfg_const_link_scenario"
#define KEY_MCU_CFG_CONST_VOICE_PROMPT_LANGUAGE   "mcu_cfg_const_voice_prompt_language"
#define KEY_MCU_CFG_CONST_MIC_CHANNEL   "mcu_cfg_const_mic_channel"
#define KEY_MCU_CFG_CONST_COUPLE_SPEAKER_CHANNEL   "mcu_cfg_const_couple_speaker_channel"
#define KEY_MCU_CFG_CONST_SOLO_SPEAKER_CHANNEL   "mcu_cfg_const_solo_speaker_channel"

//dsp_option
#define KEY_MCU_CFG_CONST_DSP_OPTION0   "mcu_cfg_const_dsp_option0"

//dsp_option
#define KEY_MCU_CFG_CONST_DSP_OPTION1   "mcu_cfg_const_dsp_option1"

//Profile A2DP codec settings + profile link number: Total 6
//SBC setting
#define KEY_MCU_CFG_CONST_SBC_SAMPLING_FREQUENCY   "mcu_cfg_const_sbc_sampling_frequency"
#define KEY_MCU_CFG_CONST_SBC_CHANNEL_MODE   "mcu_cfg_const_sbc_channel_mode"
#define KEY_MCU_CFG_CONST_SBC_BLOCK_LENGTH   "mcu_cfg_const_sbc_block_length"
#define KEY_MCU_CFG_CONST_SBC_SUBBANDS   "mcu_cfg_const_sbc_subbands"
#define KEY_MCU_CFG_CONST_SBC_ALLOCATION_METHOD   "mcu_cfg_const_sbc_allocation_method"
#define KEY_MCU_CFG_CONST_SBC_MIN_BITPOOL   "mcu_cfg_const_sbc_min_bitpool"
#define KEY_MCU_CFG_CONST_SBC_MAX_BITPOOL   "mcu_cfg_const_sbc_max_bitpool"

//AAC setting
#define KEY_MCU_CFG_CONST_AAC_SAMPLING_FREQUENCY   "mcu_cfg_const_aac_sampling_frequency"
#define KEY_MCU_CFG_CONST_AAC_OBJECT_TYPE   "mcu_cfg_const_aac_object_type"
#define KEY_MCU_CFG_CONST_AAC_CHANNEL_NUMBER   "mcu_cfg_const_aac_channel_number"
#define KEY_MCU_CFG_CONST_AAC_VBR_SUPPORTED   "mcu_cfg_const_aac_vbr_supported"
#define KEY_MCU_CFG_CONST_AAC_RESERVED   "mcu_cfg_const_aac_reserved"
#define KEY_MCU_CFG_CONST_AAC_BIT_RATE   "mcu_cfg_const_aac_bit_rate"

//LDAC setting
#define KEY_MCU_CFG_CONST_LDAC_SUPPORTED_RESERVED1   "mcu_cfg_const_ldac_supported_reserved1"
#define KEY_MCU_CFG_CONST_LDAC_SUPPORTED_RESERVED2   "mcu_cfg_const_ldac_supported_reserved2"

//Profile link
#define KEY_MCU_CFG_CONST_A2DP_LINK_NUMBER   "mcu_cfg_const_a2dp_link_number"
#define KEY_MCU_CFG_CONST_HFP_LINK_NUMBER   "mcu_cfg_const_hfp_link_number"
#define KEY_MCU_CFG_CONST_SPP_LINK_NUMBER   "mcu_cfg_const_spp_link_number"
#define KEY_MCU_CFG_CONST_PBAP_LINK_NUMBER   "mcu_cfg_const_pbap_link_number"
#define KEY_MCU_CFG_CONST_AVRCP_LINK_NUMBER   "mcu_cfg_const_avrcp_link_number"
#define KEY_MCU_CFG_CONST_PROFILE_RSV   "mcu_cfg_const_profile_rsv"
/*mcu_cfg_const key*/


typedef union
{
    uint16_t a2dp_codec_type_data;
    struct
    {
        uint16_t a2dp_codec_type_sbc : 1;
        uint16_t a2dp_codec_type_aac : 1;
        uint16_t a2dp_codec_type_ldac : 1;
        uint16_t a2dp_codec_type_rsv : 13;
    } a2dp_codec_type_bit;
} T_A2DP_CODEC_TYPE;

typedef union
{
    uint8_t dsp_option0_data;
    struct
    {
        uint8_t enable_ctrl_ext_amp : 1;
        uint8_t enable_ext_amp_high_active : 1;
        uint8_t enable_ringtone_mixing : 1;
        uint8_t enable_vp_mixing : 1;
        uint8_t dsp_option_rsv_bits : 4;
    } dsp_option0_bit;
} T_DSP_OPTION0;

typedef union
{
    uint8_t dsp_option1_data;
    struct
    {
        uint8_t dsp_log_pin : 6;
        uint8_t dsp_log_output_select : 2;
    } dsp_option1_bit;
} T_DSP_OPTION1;

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
    T_A2DP_CODEC_TYPE a2dp_codec_type;
    uint8_t link_scenario;
    uint8_t voice_prompt_language;
    uint8_t mic_channel;
    uint8_t couple_speaker_channel;
    uint8_t solo_speaker_channel;

    //dsp_option0
    T_DSP_OPTION0 dsp_option0;

    //dsp_option1
    T_DSP_OPTION1 dsp_option1;

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
} T_MCU_CFG_CONST;


int dp_mcu_cfg_rw_flashDB_init(void);
void mcu_cfg_const_load(void);
#ifdef __cplusplus
}
#endif

#endif /* _DP_MCU_CFG_CONST_H_ */
