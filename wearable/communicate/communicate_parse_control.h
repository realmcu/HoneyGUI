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

#ifndef __COMMUNICATE_PARSE_CONTROL_H__
#define __COMMUNICATE_PARSE_CONTROL_H__


#ifdef __cplusplus
extern "C" {
#endif



#include <stdbool.h>
#include "stdint.h"

typedef enum
{
    KEY_TAKE_PHOTO = 0X01,
    KEY_PHONE_CAMERA_STATUS = 0X11,
} CONTROL_KEY;

void resolve_Control_command(uint8_t key, const uint8_t *pValue, uint16_t length);



#ifdef __cplusplus
}
#endif

#endif //__COMMUNICATE_PARSE_CONTROL_H__