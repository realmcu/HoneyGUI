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

#ifndef __COMMUNICATE_PARSE_NAVIGATION_H__
#define __COMMUNICATE_PARSE_NAVIGATION_H__


#ifdef __cplusplus
extern "C" {
#endif



#include <stdbool.h>
#include "stdint.h"

typedef enum
{
    KEY_NAVIGTOR_DATA   = 0x01,
    KEY_NAVIGTOR_STATE  = 0x02,

} NAVIGATOR_KEY;

typedef enum
{
    NAVI_IDLE    = 0x00,
    NAVI_START   = 0x01,
    NAVI_PAUSE   = 0x02,

} NAVI_STATE;

typedef enum
{
    MESSGAE_INFO_QQ         = 0x01,
    MESSGAE_INFO_WECHAT     = 0x02,
    MESSGAE_INFO_MESSAGE    = 0x03,

} MESSGAE_INFO;

#define TURN_LEFT    2
#define TURN_RIGHT   3
#define LEFT_FRONT   4
#define RIGHT_FRONT  5
#define LEFT_BACK    6
#define RIGHT_BACK   7
#define LEFT_TURN_AROUND   8
#define GO_STRAIGHT  9

typedef void(*P_FUNC_NAVI_CB)(uint16_t len, void *value);
typedef void(*P_FUNC_NAVI_TYPE_CB)(uint8_t TYPE);

void resolve_Navigation_command(uint8_t key, const uint8_t *pValue, uint16_t length);
void Navi_register_cb(P_FUNC_NAVI_CB cb);
void Navi_type_register_cb(P_FUNC_NAVI_TYPE_CB cb);

#ifdef __cplusplus
}
#endif

#endif //__COMMUNICATE_PARSE_HEALTH_H_