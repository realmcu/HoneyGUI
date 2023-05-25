/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include "mcu_cfg.h"
#include "dp_mcu_cfg_const.h"
#include "dp_mcu_cfg_rw.h"
#include "fmc_api.h"
#include "trace.h"


static const T_MCU_CFG_CONST mcu_cfg_default =
{
    .sync_word = MCU_CFG_CONST_DATA_SYNC_WORD,

    //Peripheral
    .ext_amp_pinmux = 0xFF,
    .ext_amp_boost_pinmux = 0xFF,

    //Miscellaneous
    .device_name_legacy_default = {"BB2 Watch"},
    .device_name_le_default = {"BB2 Watch LE"},
    .class_of_device = {0x18, 0x04, 0x24},
    .max_legacy_multilink_devices = 2,
    .hfp_brsf_capability = 0,
    .a2dp_codec_type.a2dp_codec_type_bit.a2dp_codec_type_sbc = 0,
    .a2dp_codec_type.a2dp_codec_type_bit.a2dp_codec_type_aac = 0,
    .a2dp_codec_type.a2dp_codec_type_bit.a2dp_codec_type_ldac = 0,
    .a2dp_codec_type.a2dp_codec_type_bit.a2dp_codec_type_rsv = 0,
    .supported_profile_mask = 0x0F,
    .voice_prompt_language = 0,

    //DSP-related
    .solo_speaker_channel = 0,
    //dsp_option0
    .dsp_option0.dsp_option0_bit.enable_ctrl_ext_amp = 0,
    .dsp_option0.dsp_option0_bit.enable_ext_amp_high_active = 0,
    .dsp_option0.dsp_option0_bit.enable_ringtone_mixing = 0,
    .dsp_option0.dsp_option0_bit.enable_vp_mixing = 0,

    //dsp_option1
    .dsp_option1.dsp_option1_bit.dsp_log_pin = 0,
    .dsp_option1.dsp_option1_bit.dsp_log_output_select = 0,

    //Profile A2DP codec settings + profile link number
    //SBC settings
    .sbc_sampling_frequency = 0,
    .sbc_channel_mode = 0,
    .sbc_block_length = 0,
    .sbc_subbands = 0,
    .sbc_allocation_method = 0,
    .sbc_min_bitpool = 0,
    .sbc_max_bitpool = 0,

    //AAC settings
    .aac_sampling_frequency = 0,
    .aac_object_type = 0,
    .aac_channel_number = 0,
    .aac_vbr_supported = 0,
    .aac_bit_rate = 0,

    //LDAC settings
    .ldac_supported_reserved1 = 0,
    .ldac_supported_reserved2 = 0,

    //Profile link
    .a2dp_link_number = 1,
    .hfp_link_number = 1,
    .spp_link_number = 1,
    .pbap_link_number = 1,
    .avrcp_link_number = 1,
};

static const T_MCU_CFG_RW mcu_cfg_rw_default =
{
    .mcu_cfg_rw_sync = MCU_CFG_RW_DATA_SYNC_WORD,
    .single_tone_timeout_val = 20 * 1000, //20s
    .single_tone_tx_gain = 0,
    .single_tone_channel_num = 20,
};

void mcu_cfg_const_db_data_init(T_MCU_CFG_CONST *mcu_cfg_const)
{
    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_EXT_AMP_PINMUX,
                 &mcu_cfg_const->ext_amp_pinmux, sizeof(mcu_cfg_const->ext_amp_pinmux));

    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_EXT_AMP_BOOST_PINMUX,
                 &mcu_cfg_const->ext_amp_boost_pinmux, sizeof(mcu_cfg_const->ext_amp_boost_pinmux));


    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_DEVICE_NAME_LEGACY_DEFAULT,
                 mcu_cfg_const->device_name_legacy_default, sizeof(mcu_cfg_const->device_name_legacy_default));

    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_DEVICE_NAME_LE_DEFAULT,
                 mcu_cfg_const->device_name_le_default, sizeof(mcu_cfg_const->device_name_le_default));

    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_CLASS_OF_DEVICE,
                 mcu_cfg_const->class_of_device, sizeof(mcu_cfg_const->class_of_device));

    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_MAX_LEGACY_MULTILINK_DEVICES,
                 &mcu_cfg_const->max_legacy_multilink_devices, sizeof(mcu_cfg_const->max_legacy_multilink_devices));

    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_HFP_BRSF_CAPABILITY,
                 &mcu_cfg_const->hfp_brsf_capability, sizeof(mcu_cfg_const->hfp_brsf_capability));


    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_SUPPORTED_PROFILE_MASK,
                 &mcu_cfg_const->supported_profile_mask, sizeof(mcu_cfg_const->supported_profile_mask));

    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_A2DP_CODEC_TYPE,
                 &mcu_cfg_const->a2dp_codec_type, sizeof(mcu_cfg_const->a2dp_codec_type));

    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_LINK_SCENARIO,
                 &mcu_cfg_const->link_scenario, sizeof(mcu_cfg_const->link_scenario));

    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_VOICE_PROMPT_LANGUAGE,
                 &mcu_cfg_const->voice_prompt_language, sizeof(mcu_cfg_const->voice_prompt_language));

    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_MIC_CHANNEL, &mcu_cfg_const->mic_channel,
                 sizeof(mcu_cfg_const->mic_channel));

    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_COUPLE_SPEAKER_CHANNEL,
                 &mcu_cfg_const->couple_speaker_channel, sizeof(mcu_cfg_const->couple_speaker_channel));

    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_SOLO_SPEAKER_CHANNEL,
                 &mcu_cfg_const->solo_speaker_channel, sizeof(mcu_cfg_const->solo_speaker_channel));


    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_DSP_OPTION0, &mcu_cfg_const->dsp_option0,
                 sizeof(mcu_cfg_const->dsp_option0));


    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_DSP_OPTION1, &mcu_cfg_const->dsp_option1,
                 sizeof(mcu_cfg_const->dsp_option1));


    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_SBC_SAMPLING_FREQUENCY,
                 &mcu_cfg_const->sbc_sampling_frequency, sizeof(mcu_cfg_const->sbc_sampling_frequency));

    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_SBC_CHANNEL_MODE,
                 &mcu_cfg_const->sbc_channel_mode, sizeof(mcu_cfg_const->sbc_channel_mode));

    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_SBC_BLOCK_LENGTH,
                 &mcu_cfg_const->sbc_block_length, sizeof(mcu_cfg_const->sbc_block_length));

    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_SBC_SUBBANDS, &mcu_cfg_const->sbc_subbands,
                 sizeof(mcu_cfg_const->sbc_subbands));

    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_SBC_ALLOCATION_METHOD,
                 &mcu_cfg_const->sbc_allocation_method, sizeof(mcu_cfg_const->sbc_allocation_method));

    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_SBC_MIN_BITPOOL,
                 &mcu_cfg_const->sbc_min_bitpool, sizeof(mcu_cfg_const->sbc_min_bitpool));

    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_SBC_MAX_BITPOOL,
                 &mcu_cfg_const->sbc_max_bitpool, sizeof(mcu_cfg_const->sbc_max_bitpool));


    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_AAC_SAMPLING_FREQUENCY,
                 &mcu_cfg_const->aac_sampling_frequency, sizeof(mcu_cfg_const->aac_sampling_frequency));

    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_AAC_OBJECT_TYPE,
                 &mcu_cfg_const->aac_object_type, sizeof(mcu_cfg_const->aac_object_type));

    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_AAC_CHANNEL_NUMBER,
                 &mcu_cfg_const->aac_channel_number, sizeof(mcu_cfg_const->aac_channel_number));

    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_AAC_VBR_SUPPORTED,
                 &mcu_cfg_const->aac_vbr_supported, sizeof(mcu_cfg_const->aac_vbr_supported));

    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_AAC_RESERVED, mcu_cfg_const->aac_reserved,
                 sizeof(mcu_cfg_const->aac_reserved));

    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_AAC_BIT_RATE, &mcu_cfg_const->aac_bit_rate,
                 sizeof(mcu_cfg_const->aac_bit_rate));


    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_LDAC_SUPPORTED_RESERVED1,
                 &mcu_cfg_const->ldac_supported_reserved1, sizeof(mcu_cfg_const->ldac_supported_reserved1));

    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_LDAC_SUPPORTED_RESERVED2,
                 &mcu_cfg_const->ldac_supported_reserved2, sizeof(mcu_cfg_const->ldac_supported_reserved2));


    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_A2DP_LINK_NUMBER,
                 &mcu_cfg_const->a2dp_link_number, sizeof(mcu_cfg_const->a2dp_link_number));

    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_HFP_LINK_NUMBER,
                 &mcu_cfg_const->hfp_link_number, sizeof(mcu_cfg_const->hfp_link_number));

    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_SPP_LINK_NUMBER,
                 &mcu_cfg_const->spp_link_number, sizeof(mcu_cfg_const->spp_link_number));

    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_PBAP_LINK_NUMBER,
                 &mcu_cfg_const->pbap_link_number, sizeof(mcu_cfg_const->pbap_link_number));

    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_AVRCP_LINK_NUMBER,
                 &mcu_cfg_const->avrcp_link_number, sizeof(mcu_cfg_const->ext_amp_pinmux));

    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_PROFILE_RSV, mcu_cfg_const->profile_rsv,
                 sizeof(mcu_cfg_const->profile_rsv));


    rtk_kvdb_set(KVDB_MCU_CFG_CONST_NAME, KEY_MCU_CFG_CONST_SYNC_WORD, &mcu_cfg_const->sync_word,
                 sizeof(mcu_cfg_const->sync_word));


    APP_PRINT_INFO0("mcu_cfg_const_db_data_init success\n");
    return;
}

void mcu_cfg_const_load(void)
{
    uint32_t sync_word = 0;

    T_MCU_CFG_CONST *mcu_cfg_const = malloc(sizeof(T_MCU_CFG_CONST));
    if (mcu_cfg_const == NULL)
    {
        APP_PRINT_INFO0("mcu_cfg_cosnt_load mcu_cfg_const malloc fail\n");
        return;
    }

    fmc_flash_nor_read((flash_cur_bank_img_payload_addr_get(FLASH_IMG_MCUCONFIG) +
                        MCU_CFG_CONST_OFFSET),
                       (uint8_t *)&sync_word, MCU_CFG_CONST_DATA_SYNC_WORD_LEN);
    if (sync_word == MCU_CFG_CONST_DATA_SYNC_WORD)
    {
        fmc_flash_nor_read((flash_cur_bank_img_payload_addr_get(FLASH_IMG_MCUCONFIG) +
                            MCU_CFG_CONST_OFFSET),
                           (uint8_t *)mcu_cfg_const, sizeof(T_MCU_CFG_CONST));
    }
    else
    {
        memcpy(mcu_cfg_const, &mcu_cfg_default, sizeof(T_MCU_CFG_CONST));
    }

    mcu_cfg_const_db_data_init(mcu_cfg_const);

    free(mcu_cfg_const);
}

void mcu_cfg_rw_reset(void)
{
    if (rtk_kvdb_reset_default(KVDB_MCU_CFG_RW_NAME) == false)
    {
        APP_PRINT_INFO0("rtk_kvdb_reset_default fail\n");
        return;
    }

    T_FACTORY_STATUS factory_status = {0};
    rtk_kvdb_set(KVDB_MCU_CFG_RW_NAME, KEY_MCU_CFG_RW_FACTORY_STATUS, &factory_status,
                 sizeof(factory_status));

    uint32_t sync_word = MCU_CFG_RW_DATA_SYNC_WORD;
    rtk_kvdb_set(KVDB_MCU_CFG_RW_NAME, KEY_MCU_CFG_RW_SYNC, &sync_word, sizeof(sync_word));

    uint32_t single_tone_timeout_val = mcu_cfg_rw_default.single_tone_timeout_val;
    rtk_kvdb_set(KVDB_MCU_CFG_RW_NAME, KEY_MCU_CFG_RW_SINGLE_TONE_TIMEOUT_VAL, &single_tone_timeout_val,
                 sizeof(single_tone_timeout_val));

    uint8_t single_tone_tx_gain = mcu_cfg_rw_default.single_tone_tx_gain;
    rtk_kvdb_set(KVDB_MCU_CFG_RW_NAME, KEY_MCU_CFG_RW_SINGLE_TONE_TX_GAIN, &single_tone_tx_gain,
                 sizeof(KEY_MCU_CFG_RW_SINGLE_TONE_TX_GAIN));

    uint8_t single_tone_channel_num = mcu_cfg_rw_default.single_tone_channel_num;
    rtk_kvdb_set(KVDB_MCU_CFG_RW_NAME, KEY_MCU_CFG_RW_SINGLE_TONE_CHANNEL_NUM, &single_tone_channel_num,
                 sizeof(KEY_MCU_CFG_RW_SINGLE_TONE_CHANNEL_NUM));

    uint8_t bud_peer_factory_addr[6] = {0};
    rtk_kvdb_set(KVDB_MCU_CFG_RW_NAME, KEY_MCU_CFG_RW_BUD_PEER_FACTORY_ADDR, bud_peer_factory_addr,
                 sizeof(bud_peer_factory_addr));

    uint8_t device_name_legacy_default[40];
    rtk_kvdb_get(KVDB_MCU_CFG_CONST_NAME, "mcu_cfg_const_device_name_legacy_defaul",
                 device_name_legacy_default, sizeof(device_name_legacy_default));
    rtk_kvdb_set(KVDB_MCU_CFG_RW_NAME, KEY_MCU_CFG_RW_DEVICE_NAME_LEGACY, device_name_legacy_default,
                 sizeof(device_name_legacy_default));

    uint8_t device_name_le_default[40];
    rtk_kvdb_get(KVDB_MCU_CFG_CONST_NAME, "mcu_cfg_const_device_name_le_default",
                 device_name_le_default, sizeof(device_name_le_default));
    rtk_kvdb_set(KVDB_MCU_CFG_RW_NAME, KEY_MCU_CFG_RW_DEVICE_NAME_LE, device_name_le_default,
                 sizeof(device_name_le_default));

    uint8_t bud_peer_addr[6] = {0};
    rtk_kvdb_set(KVDB_MCU_CFG_RW_NAME, "mcu_cfg_rw_bud_peer_addr", bud_peer_addr,
                 sizeof(bud_peer_addr));

    uint8_t audio_gain_level[8] = {0};
    rtk_kvdb_set(KVDB_MCU_CFG_RW_NAME, KEY_MCU_CFG_RW_AUDIO_GAIN_LEVEL, audio_gain_level,
                 sizeof(audio_gain_level));

    uint8_t vocie_gain_level[8] = {0};
    rtk_kvdb_set(KVDB_MCU_CFG_RW_NAME, KEY_MCU_CFG_RW_VOICE_GAIN_LEVEL, vocie_gain_level,
                 sizeof(vocie_gain_level));

    uint8_t voice_prompt_language;
    rtk_kvdb_get(KVDB_MCU_CFG_CONST_NAME, "mcu_cfg_const_voice_prompt_language", &voice_prompt_language,
                 sizeof(voice_prompt_language));
    rtk_kvdb_set(KVDB_MCU_CFG_RW_NAME, KEY_MCU_CFG_RW_VOICE_PROMPT_LANGUAGE, &voice_prompt_language,
                 sizeof(voice_prompt_language));

    uint8_t pin_code_size = 4;
    rtk_kvdb_set(KVDB_MCU_CFG_RW_NAME, KEY_MCU_CFG_RW_PIN_CODE_SIZE, &pin_code_size,
                 sizeof(pin_code_size));

    uint8_t pin_code[8];
    memset(pin_code, 0x30, 8);
    rtk_kvdb_set(KVDB_MCU_CFG_RW_NAME, KEY_MCU_CFG_RW_PIN_CODE, pin_code, sizeof(pin_code));

    APP_PRINT_INFO0("mcu_cfg_rw_reset success\n");
}

void mcu_cfg_rw_load(void)
{
    uint32_t mcu_cfg_rw_sync = 0;
    uint8_t mcu_cfg_rw_reserved = 0;

    rtk_kvdb_get(KVDB_MCU_CFG_RW_NAME, "mcu_cfg_rw_sync", &mcu_cfg_rw_sync, sizeof(mcu_cfg_rw_sync));

    if (mcu_cfg_rw_sync != MCU_CFG_RW_DATA_SYNC_WORD)
    {
        //Load factory reset bit first when mppgtool factory reset
        mcu_cfg_rw_reset();
    }

    APP_PRINT_INFO0("mcu_cfg_rw_load success\n");
}

void mcu_cfg_init(void)
{
    mcu_cfg_const_load();
    mcu_cfg_rw_load();
}