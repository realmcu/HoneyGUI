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


typedef struct
{
    uint8_t                     first_hf_index;
    uint8_t                     last_hf_index;
} T_HF_INFO;

typedef struct
{
    T_APP_BR_LINK               br_link[MAX_BR_LINK_NUM];
    T_HF_INFO                   hf_info;
} T_BR_DB;


#ifdef __cplusplus
}
#endif

#endif /* DP_BR_INFO_H */
