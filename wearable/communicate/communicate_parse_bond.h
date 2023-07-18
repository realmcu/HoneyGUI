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

#ifndef __COMMUNICATE_PARSE_BOND_H__
#define __COMMUNICATE_PARSE_BOND_H__


#ifdef __cplusplus
extern "C" {
#endif



#include <stdbool.h>
#include "stdint.h"


#define  USER_ID_LENGTH         (32)


typedef enum
{
    KEY_BOND_REQUEST        = 0x01,
    KEY_BOND_RESPOSE        = 0x02,
    KEY_LOGIN_REQUEST       = 0x03,
    KEY_LOGIN_RESPONSE      = 0x04,
    KEY_UNBOND              = 0x05,
    KEY_SUPER_BOND          = 0x06,
    KEY_SUPER_BOND_RESPONSE = 0x07,
} BOND_KEY;




void resolve_private_bond_command(uint8_t key, const uint8_t *pValue, uint16_t length);



#ifdef __cplusplus
}
#endif

#endif //__COMMUNICATE_PARSE_BOND_H_