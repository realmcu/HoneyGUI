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
#ifndef _DP_MCU_CFG_RW_H_
#define _DP_MCU_CFG_RW_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*
 *                        Header Files
*============================================================================*/
#include "stdbool.h"
#include "stdint.h"


#define KVDB_MCU_CFG_RW_NAME "mcu_cfg_rw"

#define MCU_CFG_RW_DATA_SYNC_WORD          0xAA55AA55

/*mcu_cfg_rw key*/
#define KEY_MCU_CFG_RW_SYNC   "mcu_cfg_rw_sync"

#define KEY_MCU_CFG_RW_DEVICE_NAME_LEGACY   "mcu_cfg_rw_device_name_legacy"
#define KEY_MCU_CFG_RW_DEVICE_NAME_LE   "mcu_cfg_rw_device_name_le"

#define KEY_MCU_CFG_RW_BUD_SN   "mcu_cfg_rw_bud_sn"
#define KEY_MCU_CFG_RW_LE_RANDOM_ADDR   "mcu_cfg_rw_le_random_addr"
#define KEY_MCU_CFG_RW_BUD_LOCAL_ADDR   "mcu_cfg_rw_bud_local_addr"
#define KEY_MCU_CFG_RW_BUD_PEER_ADDR   "mcu_cfg_rw_bud_peer_addr"
#define KEY_MCU_CFG_RW_BUD_PEER_FACTORY_ADDR   "mcu_cfg_rw_bud_peer_factory_addr"

#define KEY_MCU_CFG_RW_BUD_ROLE   "mcu_cfg_rw_bud_role"
#define KEY_MCU_CFG_RW_SINGLE_TONE_TX_GAIN   "mcu_cfg_rw_single_tone_tx_gain"
#define KEY_MCU_CFG_RW_SINGLE_TONE_CHANNEL_NUM   "mcu_cfg_rw_single_tone_channel_num"
#define KEY_MCU_CFG_RW_RESERVED   "mcu_cfg_rw_reserved"
#define KEY_MCU_CFG_RW_SINGLE_TONE_TIMEOUT_VAL   "mcu_cfg_rw_single_tone_timeout_val"

#define KEY_MCU_CFG_RW_FACTORY_STATUS   "mcu_cfg_rw_factory_status"
#define KEY_MCU_CFG_RW_EQ_INDEX   "mcu_cfg_rw_eq_index"
#define KEY_MCU_CFG_RW_AUDIO_EFFECT_TYPE   "mcu_cfg_rw_audio_effect_type"
#define KEY_MCU_CFG_RW_DUAL_AUDIO_EFFECT_GAMING_TYPE   "mcu_cfg_rw_dual_audio_effect_gaming_type"

#define KEY_MCU_CFG_RW_AUDIO_GAIN_LEVEL   "mcu_cfg_rw_audio_gain_level"
#define KEY_MCU_CFG_RW_VOICE_GAIN_LEVEL   "mcu_cfg_rw_voice_gain_level"

#define KEY_MCU_CFG_RW_VOICE_PROMPT_LANGUAGE   "mcu_cfg_rw_voice_prompt_language"
#define KEY_MCU_CFG_RW_CUR_SPK_CHANNEL   "mcu_cfg_rw_cur_spk_channel"
#define KEY_MCU_CFG_RW_CUR_MIC   "mcu_cfg_rw_cur_mic"
#define KEY_MCU_CFG_RW_PIN_CODE_SIZE   "mcu_cfg_rw_pin_code_size"
#define KEY_MCU_CFG_RW_PIN_CODE   "mcu_cfg_rw_pin_code"
/*mcu_cfg_rw key*/


typedef union
{
    uint8_t factory_status_data;
    struct
    {
        uint8_t app_is_power_on : 1;
        uint8_t factory_reset_done : 1;
        uint8_t adp_factory_reset_power_on : 1;
        uint8_t app_status_rsv : 5;
    } factory_status_bit;
} T_FACTORY_STATUS;

typedef struct
{
    uint32_t mcu_cfg_rw_sync;

    uint8_t device_name_legacy[40];
    uint8_t device_name_le[40];

    uint8_t bud_sn[16];
    uint8_t le_random_addr[6];
    uint8_t bud_local_addr[6];
    uint8_t bud_peer_addr[6];
    uint8_t bud_peer_factory_addr[6];

    uint8_t bud_role;
    uint8_t single_tone_tx_gain;
    uint8_t single_tone_channel_num;
    uint8_t reserved;
    uint32_t single_tone_timeout_val;

    T_FACTORY_STATUS factory_status;

    uint8_t eq_index;
    uint8_t audio_effect_type;
    uint8_t dual_audio_effect_gaming_type;

    uint8_t audio_gain_level[8];
    uint8_t voice_gain_level[8];

    uint8_t voice_prompt_language;
    uint8_t cur_spk_channel;
    uint8_t cur_mic;
    uint8_t pin_code_size;
    uint8_t pin_code[8];
} T_MCU_CFG_RW;


int dp_mcu_cfg_rw_flashDB_init(void);
void mcu_cfg_rw_load(void);
#ifdef __cplusplus
}
#endif

#endif /* _DP_MCU_CFG_RW_H_ */
