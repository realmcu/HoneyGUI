/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include "ftl.h"
#include "patch_header_check.h"
#include "rtl876x.h"
#include "section.h"
#include "app_cfg.h"
#include "bt_hfp.h"
#include "fmc_api.h"


void rwcback(void *status);



RAM_TEXT_SECTION
void rwcback(void *status)
{
    APP_PRINT_TRACE1("sd rw_cback status =%d", (*(uint16_t *)status));
}
static const T_APP_CFG_CONST app_cfg_default =
{
    .sync_word = DATA_SYNC_WORD,

    //Peripheral
    .ext_amp_pinmux = 0xFF,
    .ext_amp_boost_pinmux = 0xFF,

    //Miscellaneous
    .device_name_legacy_default = {"BB2 Watch"},
    .device_name_le_default = {"BB2 Watch LE"},
    .class_of_device = {0x18, 0x04, 0x24},
    .max_legacy_multilink_devices = 2,
    .hfp_brsf_capability = 0,
    .a2dp_codec_type_sbc = 0,
    .a2dp_codec_type_aac = 0,
    .a2dp_codec_type_ldac = 0,
    .a2dp_codec_type_rsv = 0,
    .supported_profile_mask = 0x0F,
    .voice_prompt_language = 0,

    //DSP-related
    .solo_speaker_channel = 0,
    //dsp_option0
    .enable_ctrl_ext_amp = 0,
    .enable_ext_amp_high_active = 0,
    .enable_ringtone_mixing = 0,
    .enable_vp_mixing = 0,

    //dsp_option1
    .dsp_log_pin = 0,
    .dsp_log_output_select = 0,

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

static const T_APP_CFG_NV app_cfg_rw_default =
{
    .hdr.sync_word = DATA_SYNC_WORD,
    .hdr.length = sizeof(T_APP_CFG_NV),
    .single_tone_timeout_val = 20 * 1000, //20s
    .single_tone_tx_gain = 0,
    .single_tone_channel_num = 20,
};

T_APP_CFG_VOLUME app_cfg_volume =    //load from dsp data
{
    .apt_volume_min = 0,
    .apt_volume_max = 15,
    .playback_volume_min = 0,
    .playback_volume_max = 15,
    .playback_volume_default = 8,
    .voice_out_volume_min = 0,
    .voice_out_volume_max = 15,
    .voice_out_volume_default = 8,
    .voice_volume_in_min = 0,
    .voice_volume_in_max = 15,
    .voice_volume_in_default = 8,
    .record_volume_min = 0,
    .record_volume_max = 15,
    .record_volume_default = 12,
    .voice_prompt_volume_min = 0,
    .voice_prompt_volume_max = 15,
    .voice_prompt_volume_default = 8,
    .ringtone_volume_min = 0,
    .ringtone_volume_max = 15,
    .ringtone_volume_default = 8,
};

const T_CONSOLE_CFG_CONST console_cfg_const =
{
    .data_uart_tx_pinmux = P3_1,
    .data_uart_rx_pinmux = P3_0,
    .tx_wake_up_pinmux = 0xFF,
    .rx_wake_up_pinmux = 0xFF,

    .enable_data_uart = 1,
    .enable_tx_wake_up = 0,
    .enable_rx_wake_up = 0,

    .data_uart_baud_rate = 0,

    .dt_resend_num = 3,
    .enable_embedded_cmd = true,
    .enable_dsp_capture_data_by_spp = false,

    .report_uart_event_only_once = 0,
};

T_APP_CFG_CONST app_cfg_const;
T_APP_CFG_NV app_cfg_nv;

uint32_t app_cfg_reset(void)
{
    uint8_t temp_bd_addr[6];
    uint8_t temp_reset_power_on;
    uint8_t temp_reset_done;

    app_cfg_nv.adp_factory_reset_power_on = 0;
    temp_reset_power_on = app_cfg_nv.adp_factory_reset_power_on;
    temp_reset_done = app_cfg_nv.factory_reset_done;

    memcpy(temp_bd_addr, app_cfg_nv.bud_peer_factory_addr, 6);
    memcpy(&app_cfg_nv, &app_cfg_rw_default, sizeof(T_APP_CFG_NV));

    app_cfg_nv.adp_factory_reset_power_on = temp_reset_power_on;
    app_cfg_nv.factory_reset_done = temp_reset_done;
    memcpy(app_cfg_nv.bud_peer_factory_addr, temp_bd_addr, 6);

    memcpy(&app_cfg_nv.device_name_legacy[0], &app_cfg_const.device_name_legacy_default[0], 40);
    memcpy(&app_cfg_nv.device_name_le[0], &app_cfg_const.device_name_le_default[0], 40);

    //app_cfg_nv.bud_role = REMOTE_SESSION_ROLE_SINGLE;
    memset(app_cfg_nv.bud_peer_addr, 0, 6);
    //memcpy(app_cfg_nv.bud_local_addr, app_db.factory_addr, 6);
    app_cfg_nv.app_is_power_on = 0;

    memset(&app_cfg_nv.audio_gain_level[0], 8, 8);//default audio volume for different bond device
    memset(&app_cfg_nv.voice_gain_level[0], 8, 8);//default voice volume for different bond device

    app_cfg_nv.voice_prompt_language = app_cfg_const.voice_prompt_language;

    app_cfg_nv.pin_code_size = 4;
    memset(&app_cfg_nv.pin_code[0], 0x30, 8);

    return ftl_save_to_storage(&app_cfg_nv, APP_RW_DATA_ADDR, sizeof(T_APP_CFG_NV));
}

void app_cfg_load(void)
{
    uint32_t sync_word = 0;

    fmc_flash_nor_read((flash_cur_bank_img_payload_addr_get(FLASH_IMG_MCUCONFIG) +
                        APP_CONFIG_OFFSET),
                       (uint8_t *)&sync_word, DATA_SYNC_WORD_LEN);
    if (sync_word == DATA_SYNC_WORD)
    {
        fmc_flash_nor_read((flash_cur_bank_img_payload_addr_get(FLASH_IMG_MCUCONFIG) +
                            APP_CONFIG_OFFSET),
                           (uint8_t *)&app_cfg_const, sizeof(T_APP_CFG_CONST));
    }
    else
    {
        memcpy(&app_cfg_const, &app_cfg_default, sizeof(T_APP_CFG_CONST));
    }

#if 0//todo by howie
    app_dsp_cfg_init();
    app_dsp_cfg_init_gain_level();
#endif

    //read-write data
    ftl_load_from_storage(&app_cfg_nv.hdr, APP_RW_DATA_ADDR, sizeof(app_cfg_nv.hdr));

    if (app_cfg_nv.hdr.sync_word != DATA_SYNC_WORD)
    {
        //Load factory reset bit first when mppgtool factory reset
        if (app_cfg_nv.hdr.length == 0)
        {
            ftl_load_from_storage(&app_cfg_nv.reserved, APP_RW_DATA_ADDR + FACTORY_RESET_OFFSET, 4);
        }

        app_cfg_reset();
    }
    else
    {
        uint32_t load_len = app_cfg_nv.hdr.length;

        if (load_len > sizeof(T_APP_CFG_NV))
        {
            APP_PRINT_ERROR0("app_cfg_load, error");
        }
        else
        {
            uint32_t res = ftl_load_from_storage(&app_cfg_nv, APP_RW_DATA_ADDR, load_len);

            if (res == 0)
            {

                if (load_len < sizeof(T_APP_CFG_NV))
                {
                    uint8_t *p_dst = ((uint8_t *)&app_cfg_nv) + load_len;
                    uint8_t *p_src = ((uint8_t *)&app_cfg_rw_default) + load_len;
                    memcpy(p_dst, p_src, sizeof(T_APP_CFG_NV) - load_len);
                }
                app_cfg_nv.hdr.length = sizeof(T_APP_CFG_NV);
            }
            else
            {
                app_cfg_reset();
            }

        }

    }
}

uint32_t app_cfg_store(void)
{
    return ftl_save_to_storage(&app_cfg_nv, APP_RW_DATA_ADDR, sizeof(T_APP_CFG_NV));
}

void app_cfg_init(void)
{
    //sd_app_config_init(sd_card_cfg);
    app_cfg_load();
}

