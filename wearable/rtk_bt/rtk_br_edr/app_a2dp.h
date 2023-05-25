/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_A2DP_H_
#define _APP_A2DP_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include <stdint.h>
#include "app_msg.h"
#include "stdbool.h"

typedef enum
{
    AUDIO_A2DP_SRC_EVENT_DATA_SEND  = 0xf0,
} T_AUDIO_A2DP_SRC_EVENT;

typedef enum
{
    APP_A2DP_SRC_DISCONN      = 0x00,
    APP_A2DP_SRC_CONN         = 0x01,
    APP_A2DP_SRC_STREAM_STOP  = 0x02,
    APP_A2DP_SRC_STREAM_CONN  = 0x03,
    APP_A2DP_SRC_STREAM_START = 0x04,
} T_APP_A2DP_SRC_STATE;

/*============================================================================*
 *                              Functions
 *============================================================================*/
/**
    * @brief  a2dp module init
    * @param  void
    * @return void
    */
void app_a2dp_init(void);

void app_a2dp_src_start(void);
void app_a2dp_src_stop(void);

void app_a2dp_src_mode_init(void);
void app_a2dp_sink_mode_init(void);

void app_a2dp_src_volume_up(void);
void app_a2dp_src_volume_down(void);
void app_a2dp_set_src_volume(uint8_t volume);

void app_a2dp_src_handle_msg(T_IO_MSG *msg);

void app_a2dp_src_init(void);

bool get_reconnect_cause(void);

void set_reconnect_cause(bool flag);

void app_set_active_a2dp_idx(uint8_t idx);

uint8_t app_get_active_a2dp_idx(void);

bool app_a2dp_check_src_volume(uint8_t volume);

void app_a2dp_set_sink_volume(uint8_t volume);

/** @} */
/* End of group APP_A2DP_Exported_Functions */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_A2DP_H_ */
