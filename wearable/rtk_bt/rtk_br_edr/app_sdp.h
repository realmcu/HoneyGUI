/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_SDP_H_
#define _APP_SDP_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>
#include "bt_a2dp.h"

/** @defgroup APP_SDP App Sdp
  * @brief App Sdp
  * @{
  */

#define RFC_HFP_CHANN_NUM               1
#define RFC_HSP_CHANN_NUM               2
#define RFC_IAP_CHANN_NUM               3
#define RFC_SPP_CHANN_NUM               3
#define RFC_GFPS_CHANN_NUM              7
#define RFC_SPECIFIC2_CHANN_NUM         8
#define RFC_PBAP_CHANN_NUM              10
#define RFC_SPP_XIAOAI_CHANN_NUM        11
#define RFC_SPP_AMA_CHANN_NUM           12
#define RFC_SPP_BISTO_CTRL_CHANN_NUM    13
#define RFC_SPP_BISTO_AU_CHANN_NUM      14

extern const uint8_t *avp1_sdp_record;
extern const uint8_t *avp2_sdp_record;
extern const uint8_t *avp3_sdp_record;

void a2dp_sdb_record_reinit(T_BT_A2DP_ROLE new_a2dp_role);
/**
    * @brief  sdp module init
    * @param  void
    * @return void
    */
void app_sdp_init(void);


/** End of APP_SDP
* @}
*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_SDP_H_ */
