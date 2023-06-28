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
#ifndef DP_NOTIFICATION_H
#define DP_NOTIFICATION_H

#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*
 *                        Header Files
*============================================================================*/
#include "stdbool.h"
#include "stdint.h"
#include "dp_wearable.h"


typedef enum
{
    NOTIFICATION,
    NOTIFICATION_CALLING,
    NOTIFICATION_SHORTMESSAGE,
    NOTIFICATION_QQ,
    NOTIFICATION_WECHAT,
} T_NC_TYPE;

void set_notification(uint8_t len, void *p_value);
void get_notification(uint8_t len, void *p_value, uint8_t msg_num);
uint8_t notification_current_num_get(void);
void notification_data_manage_init(void);

#ifdef __cplusplus
}
#endif

#endif /* DP_NOTIFICATION_H */
