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
#ifndef DP_BR_INFO_H
#define DP_BR_INFO_H

#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*
 *                        Header Files
*============================================================================*/
#include "stdbool.h"
#include "stdint.h"
#include "app_br_link_util.h"
#include "app_a2dp.h"
#include "app_bond.h"

typedef enum
{
    MODE_NONE                = 0x0,
    MODE_APP_PLAYBACK        = 0x11,
    MODE_APP_A2DP_SRC        = 0x22,
    MODE_APP_A2DP_SNK        = 0x33,
} T_APP_AUDIO_MODE;

typedef struct
{
    uint8_t                     first_hf_index;
    uint8_t                     last_hf_index;
} T_HF_INFO;

typedef struct
{
    uint8_t                     wait_resume_a2dp_idx;
    uint8_t                     update_active_a2dp_idx;
    T_APP_AUDIO_MODE            audio_play_mode;
    T_BT_A2DP_ROLE              a2dp_cur_role;
    uint8_t                     a2dp_sink_addr[6];
    T_APP_A2DP_SRC_STATE        a2dp_src_state;
} T_A2DP_INFO;

typedef struct
{
    bool                        playback_muted;
    bool                        voice_muted;
    bool                        audio_pipe_create;
    bool                        volume_change_flag;
} T_AUDIO_INFO;

typedef struct
{
    uint8_t                     sco_interrupt_a2dp;
} T_SCO_INFO;

typedef struct
{
    uint8_t                     acl_reconnect_addr[6];
} T_ACL_INFO;

typedef struct
{
    T_APP_BR_LINK               br_link[MAX_BR_LINK_NUM];
    T_HF_INFO                   hf_info;
    T_A2DP_INFO                 a2dp_info;
    T_APP_BOND_DEVICE           bond_device[MAX_BOND_INFO_NUM];
    T_AUDIO_INFO                audio_info;
    T_SCO_INFO                  sco_info;
    T_ACL_INFO                  acl_info;
} T_BR_DB;

extern T_BR_DB br_db;

#ifdef __cplusplus
}
#endif

#endif /* DP_BR_INFO_H */
