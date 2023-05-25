/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _BT_GAP_CB_H_
#define _BT_GAP_CB_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include "stdbool.h"
#include "btm.h"


void app_gap_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len);
void app_gap_common_callback(uint8_t cb_type, void *p_cb_data);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _BT_GAP_CB_H_ */
