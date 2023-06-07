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

/** @} */
/* End of group APP_A2DP_Exported_Functions */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_A2DP_H_ */
