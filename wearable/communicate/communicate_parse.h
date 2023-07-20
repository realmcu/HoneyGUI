/**
*********************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file         communicate_protocol.c
* @brief
* @details
* @author
* @date
* @version   v0.1
* *********************************************************************************************************
*/

#ifndef __COMMUNICATE_PARSE_H__
#define __COMMUNICATE_PARSE_H__


#ifdef __cplusplus
extern "C" {
#endif



#include <stdbool.h>
#include "stdint.h"
#include "communicate_parse_setting.h"
#include "communicate_parse_bond.h"
#include "communicate_parse_control.h"
#include "communicate_parse_health.h"
#include "communicate_parse_notify.h"


/* Command ID */
typedef enum
{
    FIRMWARE_UPDATE_CMD_ID      = 0x01,
    SET_CONFIG_COMMAND_ID       = 0x02,
    BOND_COMMAND_ID             = 0x03,
    NOTIFY_COMMAND_ID           = 0x04,
    HEALTH_DATA_COMMAND_ID      = 0x05,
    FACTORY_TEST_COMMAND_ID     = 0x06,
    CONTROL_COMMAND_ID          = 0X07,
    WEATHER_INFORMATION_ID      = 0X0b,
    BLUETOOTH_LOG_COMMAND_ID    = 0x0a,
    NAVIGATION_INFORMATION_ID   = 0x0c,
    SEND_HRS_RAW_DATA           = 0x11,
    GET_STACK_DUMP              = 0x10,
    TEST_FLASH_READ_WRITE       = 0xFE,
    TEST_COMMAND_ID             = 0xFF,
} WRISTBAND_COMMUNICATE_COMMAND;

typedef enum
{
    QQ_NOTIFY_BIT                   = 0x01,     //BIT 0
    WECHAT_NOTIFY_BIT               = 0x01 << 1, //BIT 1
    SHOTMESSAGE_NOTIFY_BIT          = 0x01 << 2, //BIT 2
    LINE_NOTIFY_BIT                 = 0x01 << 3, //BIT 3
} Incomming_Message_Notify_Bit_t;


bool L2_frame_resolve(uint8_t *pData, uint16_t length);



#ifdef __cplusplus
}
#endif

#endif //__COMMUNICATE_PARSE_H__